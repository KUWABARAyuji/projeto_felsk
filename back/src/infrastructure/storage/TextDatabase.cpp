#include "infrastructure/storage/TextDatabase.hpp"
#include "domain/model/StatusContrato.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace infrastructure::storage {
namespace {

std::string escapeField(const std::string& value) {
    std::string out;
    for (char c : value) {
        if (c == '\\') out += "\\b";
        else if (c == '|') out += "\\p";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') {}
        else out += c;
    }
    return out;
}

std::vector<std::string> splitEscaped(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool escaped = false;
    for (char c : line) {
        if (escaped) {
            if (c == 'n') current += '\n';
            else if (c == 'p') current += '|';
            else if (c == 'b') current += '\\';
            else current += c;
            escaped = false;
            continue;
        }
        if (c == '\\') {
            escaped = true;
            continue;
        }
        if (c == '|') {
            fields.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    fields.push_back(current);
    return fields;
}

int toInt(const std::string& value, int fallback = 0) {
    if (value.empty()) return fallback;
    return std::stoi(value);
}

double toDouble(const std::string& value, double fallback = 0.0) {
    if (value.empty()) return fallback;
    return std::stod(value);
}

DatabaseState estadoInicial() {
    DatabaseState state;
    state.nextClienteId = 3;
    state.nextTrabalhadorId = 3;
    state.nextContratoId = 1;

    state.clientes.emplace_back(1, "Ana Lima", "47-99001-1111", "111.222.333-44", "Rua das Flores, 10");
    state.clientes.emplace_back(2, "Bruno Reis", "47-99002-2222", "555.666.777-88", "Av. Brasil, 200");

    domain::model::Trabalhador carlos(1, "Carlos Souza", "47-98001-0001", "100.200.300-01", 4.8, 4);
    carlos.adicionarHabilidade(domain::model::Habilidade("Eletricista", "Instalacoes eletricas", 80.0));
    carlos.adicionarHabilidade(domain::model::Habilidade("Pedreiro", "Alvenaria e pequenos reparos", 60.0));

    domain::model::Trabalhador davi(2, "Davi Melo", "47-98002-0002", "200.300.400-02", 4.6, 3);
    davi.adicionarHabilidade(domain::model::Habilidade("Encanador", "Reparos hidraulicos", 70.0));
    davi.adicionarHabilidade(domain::model::Habilidade("Jardineiro", "Paisagismo e limpeza externa", 50.0));

    state.trabalhadores.push_back(carlos);
    state.trabalhadores.push_back(davi);
    return state;
}

void recalcularIds(DatabaseState& state) {
    for (const auto& c : state.clientes) state.nextClienteId = std::max(state.nextClienteId, c.id() + 1);
    for (const auto& t : state.trabalhadores) state.nextTrabalhadorId = std::max(state.nextTrabalhadorId, t.id() + 1);
    for (const auto& c : state.contratos) state.nextContratoId = std::max(state.nextContratoId, c.id() + 1);
}

} // namespace

TextDatabase::TextDatabase(std::string caminhoArquivo) : caminhoArquivo_(std::move(caminhoArquivo)) {
    criarArquivoInicialSeNecessario();
}

const std::string& TextDatabase::caminhoArquivo() const { return caminhoArquivo_; }

void TextDatabase::criarArquivoInicialSeNecessario() const {
    std::lock_guard<std::mutex> lock(mutex_);
    const std::filesystem::path path(caminhoArquivo_);
    if (path.has_parent_path()) std::filesystem::create_directories(path.parent_path());
    if (!std::filesystem::exists(path) || std::filesystem::is_empty(path)) {
        salvarUnsafe(estadoInicial());
    }
}

DatabaseState TextDatabase::carregarUnsafe() const {
    std::ifstream file(caminhoArquivo_);
    if (!file.is_open()) throw std::runtime_error("Nao foi possivel abrir o banco TXT.");

    DatabaseState state;
    std::string section;
    std::string line;
    std::vector<std::tuple<int, domain::model::Habilidade>> habilidadesPendentes;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (line.front() == '[' && line.back() == ']') {
            section = line;
            continue;
        }
        if (section.empty()) {
            auto pos = line.find('=');
            if (pos == std::string::npos) continue;
            const std::string key = line.substr(0, pos);
            const std::string value = line.substr(pos + 1);
            if (key == "NEXT_CLIENTE_ID") state.nextClienteId = toInt(value, 1);
            else if (key == "NEXT_TRABALHADOR_ID") state.nextTrabalhadorId = toInt(value, 1);
            else if (key == "NEXT_CONTRATO_ID") state.nextContratoId = toInt(value, 1);
            continue;
        }

        auto f = splitEscaped(line);
        try {
            if (section == "[CLIENTES]" && f.size() >= 5) {
                state.clientes.emplace_back(toInt(f[0]), f[1], f[2], f[3], f[4]);
            } else if (section == "[TRABALHADORES]" && f.size() >= 6) {
                state.trabalhadores.emplace_back(toInt(f[0]), f[1], f[2], f[3], toDouble(f[4]), toInt(f[5]));
            } else if (section == "[HABILIDADES]" && f.size() >= 4) {
                habilidadesPendentes.emplace_back(toInt(f[0]), domain::model::Habilidade(f[1], f[2], toDouble(f[3])));
            } else if (section == "[CONTRATOS]" && f.size() >= 11) {
                state.contratos.emplace_back(toInt(f[0]), toInt(f[1]), f[2], toInt(f[3]), f[4], f[5],
                                             domain::model::Data::fromIsoString(f[6]), toInt(f[7]), toDouble(f[8]),
                                             f[9], domain::model::statusContratoFromString(f[10]));
            }
        } catch (const std::exception&) {
            // Ignora linha quebrada para nao derrubar o sistema inteiro.
        }
    }

    for (const auto& [trabalhadorId, habilidade] : habilidadesPendentes) {
        for (auto& trabalhador : state.trabalhadores) {
            if (trabalhador.id() == trabalhadorId && !trabalhador.possuiHabilidade(habilidade.nome())) {
                trabalhador.adicionarHabilidade(habilidade);
            }
        }
    }

    recalcularIds(state);
    return state;
}

void TextDatabase::salvarUnsafe(const DatabaseState& state) const {
    const std::filesystem::path path(caminhoArquivo_);
    if (path.has_parent_path()) std::filesystem::create_directories(path.parent_path());

    const std::string tempPath = caminhoArquivo_ + ".tmp";
    std::ofstream file(tempPath, std::ios::trunc);
    if (!file.is_open()) throw std::runtime_error("Nao foi possivel salvar o banco TXT.");

    file << "# Banco TXT do sistema Marido de Aluguel\n";
    file << "# Edite com cuidado. Campos usam | e caracteres especiais sao escapados.\n";
    file << "NEXT_CLIENTE_ID=" << state.nextClienteId << "\n";
    file << "NEXT_TRABALHADOR_ID=" << state.nextTrabalhadorId << "\n";
    file << "NEXT_CONTRATO_ID=" << state.nextContratoId << "\n\n";

    file << "[CLIENTES]\n";
    for (const auto& c : state.clientes) {
        file << c.id() << '|' << escapeField(c.nome()) << '|' << escapeField(c.telefone()) << '|'
             << escapeField(c.cpf()) << '|' << escapeField(c.endereco()) << "\n";
    }

    file << "\n[TRABALHADORES]\n";
    for (const auto& t : state.trabalhadores) {
        file << t.id() << '|' << escapeField(t.nome()) << '|' << escapeField(t.telefone()) << '|'
             << escapeField(t.cpf()) << '|' << t.avaliacaoMedia() << '|' << t.totalAvaliacoes() << "\n";
    }

    file << "\n[HABILIDADES]\n";
    for (const auto& t : state.trabalhadores) {
        for (const auto& h : t.habilidades()) {
            file << t.id() << '|' << escapeField(h.nome()) << '|' << escapeField(h.descricao()) << '|'
                 << h.valorHora() << "\n";
        }
    }

    file << "\n[CONTRATOS]\n";
    for (const auto& c : state.contratos) {
        file << c.id() << '|' << c.clienteId() << '|' << escapeField(c.clienteNome()) << '|'
             << c.trabalhadorId() << '|' << escapeField(c.trabalhadorNome()) << '|'
             << escapeField(c.habilidade()) << '|' << c.data().toIsoString() << '|'
             << c.horas() << '|' << c.valorTotal() << '|' << escapeField(c.descricao()) << '|'
             << domain::model::statusContratoToString(c.status()) << "\n";
    }

    file.close();
    std::filesystem::rename(tempPath, caminhoArquivo_);
}

std::vector<domain::model::Cliente> TextDatabase::listarClientes() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return carregarUnsafe().clientes;
}

std::optional<domain::model::Cliente> TextDatabase::buscarClientePorId(int id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto state = carregarUnsafe();
    for (const auto& cliente : state.clientes) if (cliente.id() == id) return cliente;
    return std::nullopt;
}

domain::model::Cliente TextDatabase::salvarCliente(const domain::model::Cliente& cliente) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto state = carregarUnsafe();
    domain::model::Cliente novo(state.nextClienteId++, cliente.nome(), cliente.telefone(), cliente.cpf(), cliente.endereco());
    state.clientes.push_back(novo);
    salvarUnsafe(state);
    return novo;
}

bool TextDatabase::atualizarCliente(const domain::model::Cliente& cliente) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto state = carregarUnsafe();
    for (auto& item : state.clientes) {
        if (item.id() == cliente.id()) {
            item = cliente;
            salvarUnsafe(state);
            return true;
        }
    }
    return false;
}

bool TextDatabase::removerCliente(int id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto state = carregarUnsafe();
    const auto before = state.clientes.size();
    state.clientes.erase(std::remove_if(state.clientes.begin(), state.clientes.end(), [&](const auto& c) { return c.id() == id; }), state.clientes.end());
    if (state.clientes.size() == before) return false;
    salvarUnsafe(state);
    return true;
}

std::vector<domain::model::Trabalhador> TextDatabase::listarTrabalhadores() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return carregarUnsafe().trabalhadores;
}

std::optional<domain::model::Trabalhador> TextDatabase::buscarTrabalhadorPorId(int id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto state = carregarUnsafe();
    for (const auto& trabalhador : state.trabalhadores) if (trabalhador.id() == id) return trabalhador;
    return std::nullopt;
}

domain::model::Trabalhador TextDatabase::salvarTrabalhador(const domain::model::Trabalhador& trabalhador) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto state = carregarUnsafe();
    domain::model::Trabalhador novo(state.nextTrabalhadorId++, trabalhador.nome(), trabalhador.telefone(), trabalhador.cpf(),
                                    trabalhador.avaliacaoMedia(), trabalhador.totalAvaliacoes());
    for (const auto& habilidade : trabalhador.habilidades()) novo.adicionarHabilidade(habilidade);
    state.trabalhadores.push_back(novo);
    salvarUnsafe(state);
    return novo;
}

bool TextDatabase::atualizarTrabalhador(const domain::model::Trabalhador& trabalhador) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto state = carregarUnsafe();
    for (auto& item : state.trabalhadores) {
        if (item.id() == trabalhador.id()) {
            item = trabalhador;
            salvarUnsafe(state);
            return true;
        }
    }
    return false;
}

bool TextDatabase::removerTrabalhador(int id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto state = carregarUnsafe();
    const auto before = state.trabalhadores.size();
    state.trabalhadores.erase(std::remove_if(state.trabalhadores.begin(), state.trabalhadores.end(), [&](const auto& t) { return t.id() == id; }), state.trabalhadores.end());
    if (state.trabalhadores.size() == before) return false;
    salvarUnsafe(state);
    return true;
}

std::vector<domain::model::Contrato> TextDatabase::listarContratos() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return carregarUnsafe().contratos;
}

std::optional<domain::model::Contrato> TextDatabase::buscarContratoPorId(int id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto state = carregarUnsafe();
    for (const auto& contrato : state.contratos) if (contrato.id() == id) return contrato;
    return std::nullopt;
}

domain::model::Contrato TextDatabase::salvarContrato(const domain::model::Contrato& contrato) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto state = carregarUnsafe();
    domain::model::Contrato novo(state.nextContratoId++, contrato.clienteId(), contrato.clienteNome(), contrato.trabalhadorId(),
                                  contrato.trabalhadorNome(), contrato.habilidade(), contrato.data(), contrato.horas(),
                                  contrato.valorTotal(), contrato.descricao(), contrato.status());
    state.contratos.push_back(novo);
    salvarUnsafe(state);
    return novo;
}

bool TextDatabase::atualizarContrato(const domain::model::Contrato& contrato) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto state = carregarUnsafe();
    for (auto& item : state.contratos) {
        if (item.id() == contrato.id()) {
            item = contrato;
            salvarUnsafe(state);
            return true;
        }
    }
    return false;
}

bool TextDatabase::removerContrato(int id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto state = carregarUnsafe();
    const auto before = state.contratos.size();
    state.contratos.erase(std::remove_if(state.contratos.begin(), state.contratos.end(), [&](const auto& c) { return c.id() == id; }), state.contratos.end());
    if (state.contratos.size() == before) return false;
    salvarUnsafe(state);
    return true;
}

bool TextDatabase::existeContratoConfirmadoNaData(int trabalhadorId, const domain::model::Data& data, int contratoIgnoradoId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto state = carregarUnsafe();
    for (const auto& contrato : state.contratos) {
        if (contrato.id() == contratoIgnoradoId) continue;
        if (contrato.trabalhadorId() == trabalhadorId && contrato.data() == data &&
            contrato.status() == domain::model::StatusContrato::CONFIRMADO) {
            return true;
        }
    }
    return false;
}

}

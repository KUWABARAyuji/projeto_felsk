#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    using SocketHandle = SOCKET;
    static constexpr SocketHandle INVALID_SOCKET_HANDLE = INVALID_SOCKET;
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
    using SocketHandle = int;
    static constexpr SocketHandle INVALID_SOCKET_HANDLE = -1;
#endif

// ============================================================
//  Modelos principais do sistema
// ============================================================

struct Cliente {
    int id{};
    std::string nome;
    std::string telefone;
    std::string cpf;
    std::string endereco;
};

struct Habilidade {
    std::string nome;
    std::string descricao;
    double valorHora{};
};

struct Trabalhador {
    int id{};
    std::string nome;
    std::string telefone;
    std::string cpf;
    double avaliacaoMedia{};
    int totalAvaliacoes{};
    std::vector<Habilidade> habilidades;
};

struct Contrato {
    int id{};
    int clienteId{};
    std::string clienteNome;
    int trabalhadorId{};
    std::string trabalhadorNome;
    std::string habilidade;
    std::string data;
    int horas{};
    double valorTotal{};
    std::string descricao;
    std::string status{"PENDENTE"};
};

struct BancoDados {
    int proximoClienteId{1};
    int proximoTrabalhadorId{1};
    int proximoContratoId{1};
    std::vector<Cliente> clientes;
    std::vector<Trabalhador> trabalhadores;
    std::vector<Contrato> contratos;
};

// ============================================================
//  Funcoes auxiliares gerais
// ============================================================

std::string trim(const std::string& valor) {
    size_t inicio = 0;
    while (inicio < valor.size() && std::isspace(static_cast<unsigned char>(valor[inicio]))) inicio++;

    size_t fim = valor.size();
    while (fim > inicio && std::isspace(static_cast<unsigned char>(valor[fim - 1]))) fim--;

    return valor.substr(inicio, fim - inicio);
}

std::vector<std::string> split(const std::string& texto, char separador) {
    std::vector<std::string> partes;
    std::stringstream ss(texto);
    std::string item;

    while (std::getline(ss, item, separador)) {
        partes.push_back(item);
    }

    return partes;
}

std::string limparCampoTxt(std::string valor) {
    std::replace(valor.begin(), valor.end(), '|', ' ');
    std::replace(valor.begin(), valor.end(), '\n', ' ');
    std::replace(valor.begin(), valor.end(), '\r', ' ');
    return trim(valor);
}

int paraInt(const std::string& valor, int padrao = 0) {
    try {
        if (trim(valor).empty()) return padrao;
        return std::stoi(valor);
    } catch (...) {
        return padrao;
    }
}

double paraDouble(const std::string& valor, double padrao = 0.0) {
    try {
        if (trim(valor).empty()) return padrao;
        return std::stod(valor);
    } catch (...) {
        return padrao;
    }
}

std::string envStringOrDefault(const char* nome, const std::string& padrao) {
    const char* valor = std::getenv(nome);
    if (valor == nullptr || std::string(valor).empty()) return padrao;
    return valor;
}

int envIntOrDefault(const char* nome, int padrao) {
    const char* valor = std::getenv(nome);
    if (valor == nullptr || std::string(valor).empty()) return padrao;
    return std::stoi(valor);
}

// ============================================================
//  JSON simples para entrada e saida
// ============================================================

std::string jsonEscape(const std::string& valor) {
    std::ostringstream out;

    for (unsigned char c : valor) {
        switch (c) {
            case '"': out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\b': out << "\\b"; break;
            case '\f': out << "\\f"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default:
                if (c < 0x20) {
                    out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                } else {
                    out << c;
                }
        }
    }

    return out.str();
}

std::string jsonString(const std::string& valor) {
    return "\"" + jsonEscape(valor) + "\"";
}

std::optional<size_t> encontrarChaveJson(const std::string& corpo, const std::string& chave) {
    const std::string busca = "\"" + chave + "\"";
    size_t pos = corpo.find(busca);
    if (pos == std::string::npos) return std::nullopt;

    pos = corpo.find(':', pos + busca.size());
    if (pos == std::string::npos) return std::nullopt;

    pos++;
    while (pos < corpo.size() && std::isspace(static_cast<unsigned char>(corpo[pos]))) pos++;

    return pos;
}

std::optional<std::string> jsonGetString(const std::string& corpo, const std::string& chave) {
    auto inicio = encontrarChaveJson(corpo, chave);
    if (!inicio.has_value() || *inicio >= corpo.size() || corpo[*inicio] != '"') return std::nullopt;

    std::string valor;
    bool escapado = false;

    for (size_t i = *inicio + 1; i < corpo.size(); ++i) {
        char c = corpo[i];

        if (escapado) {
            switch (c) {
                case 'n': valor += '\n'; break;
                case 'r': valor += '\r'; break;
                case 't': valor += '\t'; break;
                case 'b': valor += '\b'; break;
                case 'f': valor += '\f'; break;
                default: valor += c; break;
            }
            escapado = false;
        } else if (c == '\\') {
            escapado = true;
        } else if (c == '"') {
            return valor;
        } else {
            valor += c;
        }
    }

    return std::nullopt;
}

std::optional<int> jsonGetInt(const std::string& corpo, const std::string& chave) {
    auto inicio = encontrarChaveJson(corpo, chave);
    if (!inicio.has_value()) return std::nullopt;

    size_t fim = *inicio;
    while (fim < corpo.size() && (std::isdigit(static_cast<unsigned char>(corpo[fim])) || corpo[fim] == '-')) fim++;

    if (fim == *inicio) return std::nullopt;
    return std::stoi(corpo.substr(*inicio, fim - *inicio));
}

std::optional<double> jsonGetDouble(const std::string& corpo, const std::string& chave) {
    auto inicio = encontrarChaveJson(corpo, chave);
    if (!inicio.has_value()) return std::nullopt;

    size_t fim = *inicio;
    while (fim < corpo.size()) {
        char c = corpo[fim];
        if (!(std::isdigit(static_cast<unsigned char>(c)) || c == '-' || c == '+' || c == '.' || c == 'e' || c == 'E')) break;
        fim++;
    }

    if (fim == *inicio) return std::nullopt;
    return std::stod(corpo.substr(*inicio, fim - *inicio));
}

std::string jsonErro(const std::string& mensagem) {
    return "{\"erro\":" + jsonString(mensagem) + "}";
}

std::string clienteJson(const Cliente& c) {
    std::ostringstream out;
    out << "{\"id\":" << c.id
        << ",\"nome\":" << jsonString(c.nome)
        << ",\"telefone\":" << jsonString(c.telefone)
        << ",\"cpf\":" << jsonString(c.cpf)
        << ",\"endereco\":" << jsonString(c.endereco)
        << "}";
    return out.str();
}

std::string habilidadeJson(const Habilidade& h) {
    std::ostringstream out;
    out << "{\"nome\":" << jsonString(h.nome)
        << ",\"descricao\":" << jsonString(h.descricao)
        << ",\"valorHora\":" << h.valorHora
        << "}";
    return out.str();
}

std::string trabalhadorJson(const Trabalhador& t) {
    std::ostringstream out;
    out << "{\"id\":" << t.id
        << ",\"nome\":" << jsonString(t.nome)
        << ",\"telefone\":" << jsonString(t.telefone)
        << ",\"cpf\":" << jsonString(t.cpf)
        << ",\"avaliacaoMedia\":" << t.avaliacaoMedia
        << ",\"totalAvaliacoes\":" << t.totalAvaliacoes
        << ",\"habilidades\":[";

    for (size_t i = 0; i < t.habilidades.size(); ++i) {
        if (i > 0) out << ',';
        out << habilidadeJson(t.habilidades[i]);
    }

    out << "]}";
    return out.str();
}

std::string contratoJson(const Contrato& c) {
    std::ostringstream out;
    out << "{\"id\":" << c.id
        << ",\"clienteId\":" << c.clienteId
        << ",\"clienteNome\":" << jsonString(c.clienteNome)
        << ",\"trabalhadorId\":" << c.trabalhadorId
        << ",\"trabalhadorNome\":" << jsonString(c.trabalhadorNome)
        << ",\"habilidade\":" << jsonString(c.habilidade)
        << ",\"data\":" << jsonString(c.data)
        << ",\"horas\":" << c.horas
        << ",\"valorTotal\":" << c.valorTotal
        << ",\"descricao\":" << jsonString(c.descricao)
        << ",\"status\":" << jsonString(c.status)
        << "}";
    return out.str();
}

template <typename T, typename F>
std::string vetorJson(const std::vector<T>& itens, F conversor) {
    std::string saida = "[";
    for (size_t i = 0; i < itens.size(); ++i) {
        if (i > 0) saida += ",";
        saida += conversor(itens[i]);
    }
    saida += "]";
    return saida;
}

// ============================================================
//  Banco de dados em arquivo TXT
// ============================================================

class BancoTxt {
private:
    std::string caminho_;
    BancoDados dados_;

public:
    explicit BancoTxt(std::string caminho) : caminho_(std::move(caminho)) {
        carregar();
    }

    const std::string& caminho() const {
        return caminho_;
    }

    BancoDados& dados() {
        return dados_;
    }

    const BancoDados& dados() const {
        return dados_;
    }

    void carregar() {
        dados_ = BancoDados{};

        std::ifstream arquivo(caminho_);
        if (!arquivo.is_open()) {
            carregarDadosIniciais();
            salvar();
            return;
        }

        std::string secao;
        std::string linha;
        bool temConteudo = false;

        while (std::getline(arquivo, linha)) {
            linha = trim(linha);
            if (linha.empty() || linha[0] == '#') continue;
            temConteudo = true;

            if (linha.front() == '[' && linha.back() == ']') {
                secao = linha;
                continue;
            }

            if (linha.rfind("NEXT_CLIENTE_ID=", 0) == 0) {
                dados_.proximoClienteId = paraInt(linha.substr(16), 1);
                continue;
            }

            if (linha.rfind("NEXT_TRABALHADOR_ID=", 0) == 0) {
                dados_.proximoTrabalhadorId = paraInt(linha.substr(20), 1);
                continue;
            }

            if (linha.rfind("NEXT_CONTRATO_ID=", 0) == 0) {
                dados_.proximoContratoId = paraInt(linha.substr(17), 1);
                continue;
            }

            auto partes = split(linha, '|');

            if (secao == "[CLIENTES]" && partes.size() >= 5) {
                dados_.clientes.push_back({paraInt(partes[0]), partes[1], partes[2], partes[3], partes[4]});
            } else if (secao == "[TRABALHADORES]" && partes.size() >= 6) {
                dados_.trabalhadores.push_back({paraInt(partes[0]), partes[1], partes[2], partes[3], paraDouble(partes[4]), paraInt(partes[5]), {}});
            } else if (secao == "[HABILIDADES]" && partes.size() >= 4) {
                int trabalhadorId = paraInt(partes[0]);
                Trabalhador* trabalhador = buscarTrabalhador(trabalhadorId);
                if (trabalhador != nullptr) {
                    trabalhador->habilidades.push_back({partes[1], partes[2], paraDouble(partes[3])});
                }
            } else if (secao == "[CONTRATOS]" && partes.size() >= 11) {
                dados_.contratos.push_back({
                    paraInt(partes[0]),
                    paraInt(partes[1]),
                    partes[2],
                    paraInt(partes[3]),
                    partes[4],
                    partes[5],
                    partes[6],
                    paraInt(partes[7]),
                    paraDouble(partes[8]),
                    partes[9],
                    partes[10]
                });
            }
        }

        if (!temConteudo) {
            carregarDadosIniciais();
            salvar();
        }
    }

    void salvar() const {
        std::ofstream arquivo(caminho_, std::ios::trunc);
        if (!arquivo.is_open()) {
            throw std::runtime_error("Nao foi possivel salvar o arquivo de banco de dados TXT.");
        }

        arquivo << "# Banco de dados TXT - Marido de Aluguel\n";
        arquivo << "# Arquivo de armazenamento local do projeto.\n";
        arquivo << "NEXT_CLIENTE_ID=" << dados_.proximoClienteId << "\n";
        arquivo << "NEXT_TRABALHADOR_ID=" << dados_.proximoTrabalhadorId << "\n";
        arquivo << "NEXT_CONTRATO_ID=" << dados_.proximoContratoId << "\n\n";

        arquivo << "[CLIENTES]\n";
        for (const auto& c : dados_.clientes) {
            arquivo << c.id << '|' << limparCampoTxt(c.nome) << '|' << limparCampoTxt(c.telefone) << '|'
                    << limparCampoTxt(c.cpf) << '|' << limparCampoTxt(c.endereco) << "\n";
        }

        arquivo << "\n[TRABALHADORES]\n";
        for (const auto& t : dados_.trabalhadores) {
            arquivo << t.id << '|' << limparCampoTxt(t.nome) << '|' << limparCampoTxt(t.telefone) << '|'
                    << limparCampoTxt(t.cpf) << '|' << t.avaliacaoMedia << '|' << t.totalAvaliacoes << "\n";
        }

        arquivo << "\n[HABILIDADES]\n";
        for (const auto& t : dados_.trabalhadores) {
            for (const auto& h : t.habilidades) {
                arquivo << t.id << '|' << limparCampoTxt(h.nome) << '|' << limparCampoTxt(h.descricao)
                        << '|' << h.valorHora << "\n";
            }
        }

        arquivo << "\n[CONTRATOS]\n";
        for (const auto& c : dados_.contratos) {
            arquivo << c.id << '|' << c.clienteId << '|' << limparCampoTxt(c.clienteNome) << '|'
                    << c.trabalhadorId << '|' << limparCampoTxt(c.trabalhadorNome) << '|'
                    << limparCampoTxt(c.habilidade) << '|' << limparCampoTxt(c.data) << '|'
                    << c.horas << '|' << c.valorTotal << '|' << limparCampoTxt(c.descricao) << '|'
                    << limparCampoTxt(c.status) << "\n";
        }
    }

    Cliente* buscarCliente(int id) {
        for (auto& cliente : dados_.clientes) {
            if (cliente.id == id) return &cliente;
        }
        return nullptr;
    }

    Trabalhador* buscarTrabalhador(int id) {
        for (auto& trabalhador : dados_.trabalhadores) {
            if (trabalhador.id == id) return &trabalhador;
        }
        return nullptr;
    }

    Contrato* buscarContrato(int id) {
        for (auto& contrato : dados_.contratos) {
            if (contrato.id == id) return &contrato;
        }
        return nullptr;
    }

private:
    void carregarDadosIniciais() {
        dados_.proximoClienteId = 3;
        dados_.proximoTrabalhadorId = 3;
        dados_.proximoContratoId = 1;

        dados_.clientes = {
            {1, "Ana Lima", "47-99001-1111", "111.222.333-44", "Rua das Flores, 10"},
            {2, "Bruno Reis", "47-99002-2222", "555.666.777-88", "Av. Brasil, 200"}
        };

        Trabalhador carlos{1, "Carlos Souza", "47-98001-0001", "100.200.300-01", 4.8, 4, {}};
        carlos.habilidades.push_back({"Eletricista", "Instalacoes eletricas", 80.0});
        carlos.habilidades.push_back({"Pedreiro", "Alvenaria e pequenos reparos", 60.0});

        Trabalhador davi{2, "Davi Melo", "47-98002-0002", "200.300.400-02", 4.6, 3, {}};
        davi.habilidades.push_back({"Encanador", "Reparos hidraulicos", 70.0});
        davi.habilidades.push_back({"Jardineiro", "Paisagismo e limpeza externa", 50.0});

        dados_.trabalhadores = {carlos, davi};
        dados_.contratos.clear();
    }
};

// ============================================================
//  Regras do sistema
// ============================================================

class SistemaContratacao {
private:
    BancoTxt banco_;

public:
    explicit SistemaContratacao(const std::string& caminhoBanco) : banco_(caminhoBanco) {}

    const std::string& caminhoBanco() const {
        return banco_.caminho();
    }

    std::vector<Cliente> listarClientes() const {
        return banco_.dados().clientes;
    }

    Cliente cadastrarCliente(const std::string& nome, const std::string& telefone, const std::string& cpf, const std::string& endereco) {
        if (trim(nome).empty()) throw std::invalid_argument("Nome do cliente e obrigatorio.");

        Cliente cliente;
        cliente.id = banco_.dados().proximoClienteId++;
        cliente.nome = nome;
        cliente.telefone = telefone;
        cliente.cpf = cpf;
        cliente.endereco = endereco;

        banco_.dados().clientes.push_back(cliente);
        banco_.salvar();

        return cliente;
    }

    std::vector<Trabalhador> listarTrabalhadores() const {
        return banco_.dados().trabalhadores;
    }

    std::vector<Trabalhador> listarTrabalhadoresDisponiveis(const std::string& data) const {
        std::vector<Trabalhador> disponiveis;

        for (const auto& trabalhador : banco_.dados().trabalhadores) {
            bool ocupado = false;

            for (const auto& contrato : banco_.dados().contratos) {
                if (contrato.trabalhadorId == trabalhador.id && contrato.data == data && contrato.status == "CONFIRMADO") {
                    ocupado = true;
                    break;
                }
            }

            if (!ocupado) disponiveis.push_back(trabalhador);
        }

        return disponiveis;
    }

    Trabalhador cadastrarTrabalhador(const std::string& nome, const std::string& telefone, const std::string& cpf) {
        if (trim(nome).empty()) throw std::invalid_argument("Nome do trabalhador e obrigatorio.");

        Trabalhador trabalhador;
        trabalhador.id = banco_.dados().proximoTrabalhadorId++;
        trabalhador.nome = nome;
        trabalhador.telefone = telefone;
        trabalhador.cpf = cpf;
        trabalhador.avaliacaoMedia = 0.0;
        trabalhador.totalAvaliacoes = 0;

        banco_.dados().trabalhadores.push_back(trabalhador);
        banco_.salvar();

        return trabalhador;
    }

    Trabalhador adicionarHabilidade(int trabalhadorId, const Habilidade& habilidade) {
        if (trim(habilidade.nome).empty()) throw std::invalid_argument("Nome da habilidade e obrigatorio.");
        if (habilidade.valorHora <= 0) throw std::invalid_argument("Valor por hora deve ser maior que zero.");

        Trabalhador* trabalhador = banco_.buscarTrabalhador(trabalhadorId);
        if (trabalhador == nullptr) throw std::runtime_error("Trabalhador nao encontrado.");

        trabalhador->habilidades.push_back(habilidade);
        banco_.salvar();

        return *trabalhador;
    }

    std::vector<Contrato> listarContratos() const {
        return banco_.dados().contratos;
    }

    Contrato criarContrato(int clienteId, int trabalhadorId, const std::string& habilidade, const std::string& data, int horas, const std::string& descricao) {
        if (horas <= 0) throw std::invalid_argument("Horas devem ser maiores que zero.");
        if (trim(habilidade).empty()) throw std::invalid_argument("Habilidade e obrigatoria.");
        if (trim(data).empty()) throw std::invalid_argument("Data e obrigatoria.");

        Cliente* cliente = banco_.buscarCliente(clienteId);
        if (cliente == nullptr) throw std::runtime_error("Cliente nao encontrado.");

        Trabalhador* trabalhador = banco_.buscarTrabalhador(trabalhadorId);
        if (trabalhador == nullptr) throw std::runtime_error("Trabalhador nao encontrado.");

        double valorHora = 0.0;
        for (const auto& h : trabalhador->habilidades) {
            if (h.nome == habilidade) {
                valorHora = h.valorHora;
                break;
            }
        }

        if (valorHora <= 0.0) throw std::runtime_error("Trabalhador nao possui a habilidade selecionada.");

        Contrato contrato;
        contrato.id = banco_.dados().proximoContratoId++;
        contrato.clienteId = cliente->id;
        contrato.clienteNome = cliente->nome;
        contrato.trabalhadorId = trabalhador->id;
        contrato.trabalhadorNome = trabalhador->nome;
        contrato.habilidade = habilidade;
        contrato.data = data;
        contrato.horas = horas;
        contrato.valorTotal = valorHora * horas;
        contrato.descricao = descricao;
        contrato.status = "PENDENTE";

        banco_.dados().contratos.push_back(contrato);
        banco_.salvar();

        return contrato;
    }

    Contrato alterarStatusContrato(int id, const std::string& acao, double avaliacao = 5.0) {
        Contrato* contrato = banco_.buscarContrato(id);
        if (contrato == nullptr) throw std::runtime_error("Contrato nao encontrado.");

        if (acao == "confirmar") {
            if (contrato->status != "PENDENTE") throw std::runtime_error("Apenas contratos pendentes podem ser confirmados.");
            contrato->status = "CONFIRMADO";
        } else if (acao == "concluir") {
            if (contrato->status != "CONFIRMADO") throw std::runtime_error("Apenas contratos confirmados podem ser concluidos.");
            if (avaliacao < 0.0 || avaliacao > 5.0) throw std::invalid_argument("Avaliacao deve estar entre 0 e 5.");

            Trabalhador* trabalhador = banco_.buscarTrabalhador(contrato->trabalhadorId);
            if (trabalhador != nullptr) {
                trabalhador->avaliacaoMedia = ((trabalhador->avaliacaoMedia * trabalhador->totalAvaliacoes) + avaliacao) / (trabalhador->totalAvaliacoes + 1);
                trabalhador->totalAvaliacoes++;
            }

            contrato->status = "CONCLUIDO";
        } else if (acao == "cancelar") {
            if (contrato->status == "CONCLUIDO") throw std::runtime_error("Contrato concluido nao pode ser cancelado.");
            contrato->status = "CANCELADO";
        } else {
            throw std::invalid_argument("Acao de contrato invalida.");
        }

        banco_.salvar();
        return *contrato;
    }
};

// ============================================================
//  HTTP simples
// ============================================================

struct HttpRequest {
    std::string method;
    std::string path;
    std::string queryString;
    std::map<std::string, std::string> query;
    std::map<std::string, std::string> headers;
    std::string body;
};

struct HttpResponse {
    int status{200};
    std::string contentType{"application/json; charset=utf-8"};
    std::string body;
};

void fecharSocket(SocketHandle socket) {
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}

std::string statusHttp(int status) {
    switch (status) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 500: return "Internal Server Error";
        default: return "OK";
    }
}

std::string urlDecode(const std::string& valor) {
    std::string resultado;

    for (size_t i = 0; i < valor.size(); ++i) {
        if (valor[i] == '%' && i + 2 < valor.size()) {
            std::string hex = valor.substr(i + 1, 2);
            char* fim = nullptr;
            long decodificado = std::strtol(hex.c_str(), &fim, 16);

            if (fim != nullptr && *fim == '\0') {
                resultado += static_cast<char>(decodificado);
                i += 2;
            } else {
                resultado += valor[i];
            }
        } else if (valor[i] == '+') {
            resultado += ' ';
        } else {
            resultado += valor[i];
        }
    }

    return resultado;
}

std::map<std::string, std::string> parseQuery(const std::string& queryString) {
    std::map<std::string, std::string> query;
    std::stringstream ss(queryString);
    std::string item;

    while (std::getline(ss, item, '&')) {
        auto pos = item.find('=');
        if (pos == std::string::npos) {
            query[urlDecode(item)] = "";
        } else {
            query[urlDecode(item.substr(0, pos))] = urlDecode(item.substr(pos + 1));
        }
    }

    return query;
}

std::optional<int> idDaRota(const std::string& caminho, const std::string& padrao) {
    std::smatch match;
    if (std::regex_match(caminho, match, std::regex(padrao)) && match.size() > 1) {
        return std::stoi(match[1]);
    }
    return std::nullopt;
}

HttpResponse respostaJson(int status, const std::string& corpo) {
    HttpResponse response;
    response.status = status;
    response.body = corpo;
    return response;
}

HttpResponse respostaErro(int status, const std::string& mensagem) {
    return respostaJson(status, jsonErro(mensagem));
}

std::string montarRespostaHttp(const HttpResponse& response) {
    std::ostringstream out;
    out << "HTTP/1.1 " << response.status << ' ' << statusHttp(response.status) << "\r\n";
    out << "Content-Type: " << response.contentType << "\r\n";
    out << "Access-Control-Allow-Origin: *\r\n";
    out << "Access-Control-Allow-Methods: GET, POST, PUT, PATCH, DELETE, OPTIONS\r\n";
    out << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
    out << "Connection: close\r\n";
    out << "Content-Length: " << response.body.size() << "\r\n\r\n";
    out << response.body;
    return out.str();
}

size_t contentLengthDosHeaders(const std::string& headers) {
    std::string lower = headers;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    auto pos = lower.find("content-length:");
    if (pos == std::string::npos) return 0;

    pos += std::string("content-length:").size();
    while (pos < lower.size() && std::isspace(static_cast<unsigned char>(lower[pos]))) pos++;

    size_t fim = pos;
    while (fim < lower.size() && std::isdigit(static_cast<unsigned char>(lower[fim]))) fim++;

    return static_cast<size_t>(std::stoul(lower.substr(pos, fim - pos)));
}

std::string receberTudo(SocketHandle client) {
    std::string raw;
    char buffer[4096];

    while (true) {
        int recebido = recv(client, buffer, sizeof(buffer), 0);
        if (recebido <= 0) break;

        raw.append(buffer, buffer + recebido);
        auto fimHeader = raw.find("\r\n\r\n");

        if (fimHeader != std::string::npos) {
            size_t contentLength = contentLengthDosHeaders(raw.substr(0, fimHeader + 4));
            if (raw.size() >= fimHeader + 4 + contentLength) break;
        }
    }

    return raw;
}

void enviarTudo(SocketHandle client, const std::string& dados) {
    size_t total = 0;

    while (total < dados.size()) {
        int enviado = send(client, dados.data() + total, static_cast<int>(dados.size() - total), 0);
        if (enviado <= 0) break;
        total += static_cast<size_t>(enviado);
    }
}

HttpRequest parseRequest(const std::string& raw) {
    HttpRequest request;
    auto fimHeader = raw.find("\r\n\r\n");
    std::string headers = raw.substr(0, fimHeader);
    request.body = fimHeader == std::string::npos ? "" : raw.substr(fimHeader + 4);

    std::stringstream ss(headers);
    std::string linha;
    std::getline(ss, linha);
    if (!linha.empty() && linha.back() == '\r') linha.pop_back();

    std::stringstream primeiraLinha(linha);
    std::string alvo;
    primeiraLinha >> request.method >> alvo;

    auto posQuery = alvo.find('?');
    if (posQuery == std::string::npos) {
        request.path = alvo;
    } else {
        request.path = alvo.substr(0, posQuery);
        request.queryString = alvo.substr(posQuery + 1);
        request.query = parseQuery(request.queryString);
    }

    while (std::getline(ss, linha)) {
        if (!linha.empty() && linha.back() == '\r') linha.pop_back();
        auto pos = linha.find(':');
        if (pos == std::string::npos) continue;

        std::string chave = linha.substr(0, pos);
        std::string valor = linha.substr(pos + 1);
        valor = trim(valor);

        std::transform(chave.begin(), chave.end(), chave.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });

        request.headers[chave] = valor;
    }

    return request;
}

class ApiServer {
private:
    SistemaContratacao& sistema_;

public:
    explicit ApiServer(SistemaContratacao& sistema) : sistema_(sistema) {}

    HttpResponse handle(HttpRequest request) {
        try {
            if (request.method == "OPTIONS") return respostaJson(204, "");

            if (request.path.rfind("/api/", 0) == 0) {
                request.path = request.path.substr(4);
            }

            if (request.path == "/health") {
                return respostaJson(200, "{\"status\":\"online\",\"servico\":\"Marido de Aluguel\",\"armazenamento\":\"TXT\"}");
            }

            if (request.path == "/clientes" && request.method == "GET") {
                return respostaJson(200, vetorJson(sistema_.listarClientes(), clienteJson));
            }

            if (request.path == "/clientes" && request.method == "POST") {
                Cliente cliente = sistema_.cadastrarCliente(
                    jsonGetString(request.body, "nome").value_or(""),
                    jsonGetString(request.body, "telefone").value_or(""),
                    jsonGetString(request.body, "cpf").value_or(""),
                    jsonGetString(request.body, "endereco").value_or("")
                );
                return respostaJson(201, clienteJson(cliente));
            }

            if ((request.path == "/trabalhadores" || request.path == "/profissionais") && request.method == "GET") {
                return respostaJson(200, vetorJson(sistema_.listarTrabalhadores(), trabalhadorJson));
            }

            if (request.path == "/trabalhadores/disponiveis" && request.method == "GET") {
                std::string data = "";
                auto it = request.query.find("data");
                if (it != request.query.end()) data = it->second;
                return respostaJson(200, vetorJson(sistema_.listarTrabalhadoresDisponiveis(data), trabalhadorJson));
            }

            if ((request.path == "/trabalhadores" || request.path == "/profissionais") && request.method == "POST") {
                Trabalhador trabalhador = sistema_.cadastrarTrabalhador(
                    jsonGetString(request.body, "nome").value_or(""),
                    jsonGetString(request.body, "telefone").value_or(""),
                    jsonGetString(request.body, "cpf").value_or("")
                );
                return respostaJson(201, trabalhadorJson(trabalhador));
            }

            if (auto id = idDaRota(request.path, R"(^/trabalhadores/(\d+)/habilidades$)")) {
                if (request.method != "POST") return respostaErro(405, "Metodo nao permitido.");

                Habilidade habilidade{
                    jsonGetString(request.body, "nome").value_or(""),
                    jsonGetString(request.body, "descricao").value_or(""),
                    jsonGetDouble(request.body, "valorHora").value_or(0.0)
                };

                Trabalhador trabalhador = sistema_.adicionarHabilidade(*id, habilidade);
                return respostaJson(201, trabalhadorJson(trabalhador));
            }

            if (request.path == "/contratos" && request.method == "GET") {
                return respostaJson(200, vetorJson(sistema_.listarContratos(), contratoJson));
            }

            if ((request.path == "/contratos" || request.path == "/orcamentos") && request.method == "POST") {
                int clienteId = jsonGetInt(request.body, "idCliente").value_or(jsonGetInt(request.body, "clienteId").value_or(0));
                int trabalhadorId = jsonGetInt(request.body, "idTrabalhador").value_or(jsonGetInt(request.body, "trabalhadorId").value_or(0));

                Contrato contrato = sistema_.criarContrato(
                    clienteId,
                    trabalhadorId,
                    jsonGetString(request.body, "habilidade").value_or(""),
                    jsonGetString(request.body, "data").value_or(""),
                    jsonGetInt(request.body, "horas").value_or(1),
                    jsonGetString(request.body, "descricao").value_or("")
                );

                return respostaJson(201, contratoJson(contrato));
            }

            if (auto id = idDaRota(request.path, R"(^/contratos/(\d+)/confirmar$)")) {
                if (request.method != "PATCH" && request.method != "PUT") return respostaErro(405, "Metodo nao permitido.");
                return respostaJson(200, contratoJson(sistema_.alterarStatusContrato(*id, "confirmar")));
            }

            if (auto id = idDaRota(request.path, R"(^/contratos/(\d+)/concluir$)")) {
                if (request.method != "PATCH" && request.method != "PUT") return respostaErro(405, "Metodo nao permitido.");
                double avaliacao = jsonGetDouble(request.body, "avaliacao").value_or(5.0);
                return respostaJson(200, contratoJson(sistema_.alterarStatusContrato(*id, "concluir", avaliacao)));
            }

            if (auto id = idDaRota(request.path, R"(^/contratos/(\d+)/cancelar$)")) {
                if (request.method != "PATCH" && request.method != "PUT") return respostaErro(405, "Metodo nao permitido.");
                return respostaJson(200, contratoJson(sistema_.alterarStatusContrato(*id, "cancelar")));
            }

            return respostaErro(404, "Rota nao encontrada.");
        } catch (const std::exception& erro) {
            return respostaErro(400, erro.what());
        }
    }

    void iniciar(const std::string& host, int porta) {
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("Falha ao iniciar Winsock.");
        }
#endif

        SocketHandle servidor = socket(AF_INET, SOCK_STREAM, 0);
        if (servidor == INVALID_SOCKET_HANDLE) {
            throw std::runtime_error("Nao foi possivel criar o socket do servidor.");
        }

        int opt = 1;
#ifdef _WIN32
        setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
#else
        setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

        sockaddr_in endereco{};
        endereco.sin_family = AF_INET;
        endereco.sin_port = htons(static_cast<uint16_t>(porta));
        endereco.sin_addr.s_addr = host == "0.0.0.0" ? INADDR_ANY : inet_addr(host.c_str());

        if (bind(servidor, reinterpret_cast<sockaddr*>(&endereco), sizeof(endereco)) < 0) {
            fecharSocket(servidor);
            throw std::runtime_error("Nao foi possivel usar a porta informada.");
        }

        if (listen(servidor, 16) < 0) {
            fecharSocket(servidor);
            throw std::runtime_error("Nao foi possivel iniciar a escuta do servidor.");
        }

        std::cout << "Marido de Aluguel - Servidor iniciado\n";
        std::cout << "Servidor: http://localhost:" << porta << "\n";
        std::cout << "Health:   http://localhost:" << porta << "/health\n";
        std::cout << "Banco:    " << sistema_.caminhoBanco() << "\n";

        while (true) {
            sockaddr_in clienteEndereco{};
#ifdef _WIN32
            int tamanhoEndereco = sizeof(clienteEndereco);
#else
            socklen_t tamanhoEndereco = sizeof(clienteEndereco);
#endif
            SocketHandle cliente = accept(servidor, reinterpret_cast<sockaddr*>(&clienteEndereco), &tamanhoEndereco);
            if (cliente == INVALID_SOCKET_HANDLE) continue;

            std::thread([this, cliente]() {
                std::string raw = receberTudo(cliente);

                if (!raw.empty()) {
                    HttpRequest request = parseRequest(raw);
                    HttpResponse response = handle(request);
                    enviarTudo(cliente, montarRespostaHttp(response));
                }

                fecharSocket(cliente);
            }).detach();
        }

        fecharSocket(servidor);
#ifdef _WIN32
        WSACleanup();
#endif
    }
};

// ============================================================
//  Main
// ============================================================

int main() {
    try {
        const std::string caminhoBanco = envStringOrDefault("BANCO_TXT_CAMINHO", "data/banco_dados.txt");
        const int porta = envIntOrDefault("APP_PORT", 8080);

        SistemaContratacao sistema(caminhoBanco);
        ApiServer servidor(sistema);
        servidor.iniciar("0.0.0.0", porta);
    } catch (const std::exception& erro) {
        std::cerr << "Erro ao iniciar o servidor: " << erro.what() << "\n";
        return 1;
    }

    return 0;
}

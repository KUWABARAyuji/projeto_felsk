// ============================================================
//  SISTEMA MARIDO DE ALUGUEL - BACKEND C++ 
//  ALUNO: Enzo Yuji Kuwabara, lucas 
// ============================================================

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
#include <utility>
#include <vector>

using namespace std;

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
//  Funcoes auxiliares gerais
// ============================================================

string trim(const string& valor) {
    size_t inicio = 0;

    while (inicio < valor.size() && isspace(static_cast<unsigned char>(valor[inicio]))) {
        inicio++;
    }

    size_t fim = valor.size();

    while (fim > inicio && isspace(static_cast<unsigned char>(valor[fim - 1]))) {
        fim--;
    }

    return valor.substr(inicio, fim - inicio);
}

vector<string> split(const string& texto, char separador) {
    vector<string> partes;
    stringstream ss(texto);
    string item;

    while (getline(ss, item, separador)) {
        partes.push_back(item);
    }

    return partes;
}

string limparCampoTxt(string valor) {
    replace(valor.begin(), valor.end(), '|', ' ');
    replace(valor.begin(), valor.end(), '\n', ' ');
    replace(valor.begin(), valor.end(), '\r', ' ');
    return trim(valor);
}

int paraInt(const string& valor, int padrao = 0) {
    try {
        if (trim(valor).empty()) {
            return padrao;
        }

        return stoi(valor);
    } catch (...) {
        return padrao;
    }
}

double paraDouble(const string& valor, double padrao = 0.0) {
    try {
        if (trim(valor).empty()) {
            return padrao;
        }

        return stod(valor);
    } catch (...) {
        return padrao;
    }
}

string envStringOrDefault(const char* nome, const string& padrao) {
    const char* valor = getenv(nome);

    if (valor == nullptr || string(valor).empty()) {
        return padrao;
    }

    return valor;
}

int envIntOrDefault(const char* nome, int padrao) {
    const char* valor = getenv(nome);

    if (valor == nullptr || string(valor).empty()) {
        return padrao;
    }

    return stoi(valor);
}

// ============================================================
//  Modelos principais do sistema
// ============================================================

class Cliente {
private:
    int id_;
    string nome_;
    string telefone_;
    string cpf_;
    string endereco_;

public:
    Cliente()
        : id_(0) {}

    Cliente(int id, string nome, string telefone, string cpf, string endereco)
        : id_(id),
          nome_(move(nome)),
          telefone_(move(telefone)),
          cpf_(move(cpf)),
          endereco_(move(endereco)) {}

    int getId() const {
        return id_;
    }

    string getNome() const {
        return nome_;
    }

    string getTelefone() const {
        return telefone_;
    }

    string getCpf() const {
        return cpf_;
    }

    string getEndereco() const {
        return endereco_;
    }

    void setId(int id) {
        id_ = id;
    }

    void setNome(const string& nome) {
        nome_ = nome;
    }

    void setTelefone(const string& telefone) {
        telefone_ = telefone;
    }

    void setCpf(const string& cpf) {
        cpf_ = cpf;
    }

    void setEndereco(const string& endereco) {
        endereco_ = endereco;
    }
};

class Habilidade {
private:
    string nome_;
    string descricao_;
    double valorHora_;

public:
    Habilidade()
        : valorHora_(0.0) {}

    Habilidade(string nome, string descricao, double valorHora)
        : nome_(move(nome)),
          descricao_(move(descricao)),
          valorHora_(valorHora) {}

    string getNome() const {
        return nome_;
    }

    string getDescricao() const {
        return descricao_;
    }

    double getValorHora() const {
        return valorHora_;
    }

    void setNome(const string& nome) {
        nome_ = nome;
    }

    void setDescricao(const string& descricao) {
        descricao_ = descricao;
    }

    void setValorHora(double valorHora) {
        valorHora_ = valorHora;
    }
};

class Trabalhador {
private:
    int id_;
    string nome_;
    string telefone_;
    string cpf_;
    double avaliacaoMedia_;
    int totalAvaliacoes_;
    vector<Habilidade> habilidades_;

public:
    Trabalhador()
        : id_(0),
          avaliacaoMedia_(0.0),
          totalAvaliacoes_(0) {}

    Trabalhador(int id, string nome, string telefone, string cpf, double avaliacaoMedia, int totalAvaliacoes)
        : id_(id),
          nome_(move(nome)),
          telefone_(move(telefone)),
          cpf_(move(cpf)),
          avaliacaoMedia_(avaliacaoMedia),
          totalAvaliacoes_(totalAvaliacoes) {}

    int getId() const {
        return id_;
    }

    string getNome() const {
        return nome_;
    }

    string getTelefone() const {
        return telefone_;
    }

    string getCpf() const {
        return cpf_;
    }

    double getAvaliacaoMedia() const {
        return avaliacaoMedia_;
    }

    int getTotalAvaliacoes() const {
        return totalAvaliacoes_;
    }

    const vector<Habilidade>& getHabilidades() const {
        return habilidades_;
    }

    vector<Habilidade>& getHabilidades() {
        return habilidades_;
    }

    void setId(int id) {
        id_ = id;
    }

    void setNome(const string& nome) {
        nome_ = nome;
    }

    void setTelefone(const string& telefone) {
        telefone_ = telefone;
    }

    void setCpf(const string& cpf) {
        cpf_ = cpf;
    }

    void setAvaliacaoMedia(double avaliacaoMedia) {
        avaliacaoMedia_ = avaliacaoMedia;
    }

    void setTotalAvaliacoes(int totalAvaliacoes) {
        totalAvaliacoes_ = totalAvaliacoes;
    }

    void setHabilidades(const vector<Habilidade>& habilidades) {
        habilidades_ = habilidades;
    }

    void adicionarHabilidade(const Habilidade& habilidade) {
        habilidades_.push_back(habilidade);
    }

    bool possuiHabilidade(const string& nomeHabilidade) const {
        for (const auto& habilidade : habilidades_) {
            if (habilidade.getNome() == nomeHabilidade) {
                return true;
            }
        }

        return false;
    }

    double getValorHabilidade(const string& nomeHabilidade) const {
        for (const auto& habilidade : habilidades_) {
            if (habilidade.getNome() == nomeHabilidade) {
                return habilidade.getValorHora();
            }
        }

        return 0.0;
    }

    void receberAvaliacao(double nota) {
        avaliacaoMedia_ = ((avaliacaoMedia_ * totalAvaliacoes_) + nota) / (totalAvaliacoes_ + 1);
        totalAvaliacoes_++;
    }
};

class Contrato {
private:
    int id_;
    int clienteId_;
    string clienteNome_;
    int trabalhadorId_;
    string trabalhadorNome_;
    string habilidade_;
    string data_;
    int horas_;
    double valorTotal_;
    string descricao_;
    string status_;

public:
    Contrato()
        : id_(0),
          clienteId_(0),
          trabalhadorId_(0),
          horas_(0),
          valorTotal_(0.0),
          status_("PENDENTE") {}

    Contrato(int id,
             int clienteId,
             string clienteNome,
             int trabalhadorId,
             string trabalhadorNome,
             string habilidade,
             string data,
             int horas,
             double valorTotal,
             string descricao,
             string status)
        : id_(id),
          clienteId_(clienteId),
          clienteNome_(move(clienteNome)),
          trabalhadorId_(trabalhadorId),
          trabalhadorNome_(move(trabalhadorNome)),
          habilidade_(move(habilidade)),
          data_(move(data)),
          horas_(horas),
          valorTotal_(valorTotal),
          descricao_(move(descricao)),
          status_(move(status)) {}

    int getId() const {
        return id_;
    }

    int getClienteId() const {
        return clienteId_;
    }

    string getClienteNome() const {
        return clienteNome_;
    }

    int getTrabalhadorId() const {
        return trabalhadorId_;
    }

    string getTrabalhadorNome() const {
        return trabalhadorNome_;
    }

    string getHabilidade() const {
        return habilidade_;
    }

    string getData() const {
        return data_;
    }

    int getHoras() const {
        return horas_;
    }

    double getValorTotal() const {
        return valorTotal_;
    }

    string getDescricao() const {
        return descricao_;
    }

    string getStatus() const {
        return status_;
    }

    void setId(int id) {
        id_ = id;
    }

    void setClienteId(int clienteId) {
        clienteId_ = clienteId;
    }

    void setClienteNome(const string& clienteNome) {
        clienteNome_ = clienteNome;
    }

    void setTrabalhadorId(int trabalhadorId) {
        trabalhadorId_ = trabalhadorId;
    }

    void setTrabalhadorNome(const string& trabalhadorNome) {
        trabalhadorNome_ = trabalhadorNome;
    }

    void setHabilidade(const string& habilidade) {
        habilidade_ = habilidade;
    }

    void setData(const string& data) {
        data_ = data;
    }

    void setHoras(int horas) {
        horas_ = horas;
    }

    void setValorTotal(double valorTotal) {
        valorTotal_ = valorTotal;
    }

    void setDescricao(const string& descricao) {
        descricao_ = descricao;
    }

    void setStatus(const string& status) {
        status_ = status;
    }
};

class BancoDados {
private:
    int proximoClienteId_;
    int proximoTrabalhadorId_;
    int proximoContratoId_;
    vector<Cliente> clientes_;
    vector<Trabalhador> trabalhadores_;
    vector<Contrato> contratos_;

public:
    BancoDados()
        : proximoClienteId_(1),
          proximoTrabalhadorId_(1),
          proximoContratoId_(1) {}

    int getProximoClienteId() const {
        return proximoClienteId_;
    }

    int getProximoTrabalhadorId() const {
        return proximoTrabalhadorId_;
    }

    int getProximoContratoId() const {
        return proximoContratoId_;
    }

    vector<Cliente>& getClientes() {
        return clientes_;
    }

    const vector<Cliente>& getClientes() const {
        return clientes_;
    }

    vector<Trabalhador>& getTrabalhadores() {
        return trabalhadores_;
    }

    const vector<Trabalhador>& getTrabalhadores() const {
        return trabalhadores_;
    }

    vector<Contrato>& getContratos() {
        return contratos_;
    }

    const vector<Contrato>& getContratos() const {
        return contratos_;
    }

    void setProximoClienteId(int proximoClienteId) {
        proximoClienteId_ = proximoClienteId;
    }

    void setProximoTrabalhadorId(int proximoTrabalhadorId) {
        proximoTrabalhadorId_ = proximoTrabalhadorId;
    }

    void setProximoContratoId(int proximoContratoId) {
        proximoContratoId_ = proximoContratoId;
    }

    int gerarClienteId() {
        return proximoClienteId_++;
    }

    int gerarTrabalhadorId() {
        return proximoTrabalhadorId_++;
    }

    int gerarContratoId() {
        return proximoContratoId_++;
    }

    void adicionarCliente(const Cliente& cliente) {
        clientes_.push_back(cliente);
    }

    void adicionarTrabalhador(const Trabalhador& trabalhador) {
        trabalhadores_.push_back(trabalhador);
    }

    void adicionarContrato(const Contrato& contrato) {
        contratos_.push_back(contrato);
    }

    void limpar() {
        proximoClienteId_ = 1;
        proximoTrabalhadorId_ = 1;
        proximoContratoId_ = 1;
        clientes_.clear();
        trabalhadores_.clear();
        contratos_.clear();
    }
};

// ============================================================
//  JSON simples para entrada e saida
// ============================================================

string jsonEscape(const string& valor) {
    ostringstream out;

    for (unsigned char c : valor) {
        switch (c) {
            case '"':
                out << "\\\"";
                break;

            case '\\':
                out << "\\\\";
                break;

            case '\b':
                out << "\\b";
                break;

            case '\f':
                out << "\\f";
                break;

            case '\n':
                out << "\\n";
                break;

            case '\r':
                out << "\\r";
                break;

            case '\t':
                out << "\\t";
                break;

            default:
                if (c < 0x20) {
                    out << "\\u" << hex << setw(4) << setfill('0') << static_cast<int>(c);
                } else {
                    out << c;
                }
        }
    }

    return out.str();
}

string jsonString(const string& valor) {
    return "\"" + jsonEscape(valor) + "\"";
}

optional<size_t> encontrarChaveJson(const string& corpo, const string& chave) {
    const string busca = "\"" + chave + "\"";
    size_t pos = corpo.find(busca);

    if (pos == string::npos) {
        return nullopt;
    }

    pos = corpo.find(':', pos + busca.size());

    if (pos == string::npos) {
        return nullopt;
    }

    pos++;

    while (pos < corpo.size() && isspace(static_cast<unsigned char>(corpo[pos]))) {
        pos++;
    }

    return pos;
}

optional<string> jsonGetString(const string& corpo, const string& chave) {
    auto inicio = encontrarChaveJson(corpo, chave);

    if (!inicio.has_value() || *inicio >= corpo.size() || corpo[*inicio] != '"') {
        return nullopt;
    }

    string valor;
    bool escapado = false;

    for (size_t i = *inicio + 1; i < corpo.size(); ++i) {
        char c = corpo[i];

        if (escapado) {
            switch (c) {
                case 'n':
                    valor += '\n';
                    break;

                case 'r':
                    valor += '\r';
                    break;

                case 't':
                    valor += '\t';
                    break;

                case 'b':
                    valor += '\b';
                    break;

                case 'f':
                    valor += '\f';
                    break;

                default:
                    valor += c;
                    break;
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

    return nullopt;
}

optional<int> jsonGetInt(const string& corpo, const string& chave) {
    auto inicio = encontrarChaveJson(corpo, chave);

    if (!inicio.has_value()) {
        return nullopt;
    }

    size_t fim = *inicio;

    while (fim < corpo.size() && (isdigit(static_cast<unsigned char>(corpo[fim])) || corpo[fim] == '-')) {
        fim++;
    }

    if (fim == *inicio) {
        return nullopt;
    }

    return stoi(corpo.substr(*inicio, fim - *inicio));
}

optional<double> jsonGetDouble(const string& corpo, const string& chave) {
    auto inicio = encontrarChaveJson(corpo, chave);

    if (!inicio.has_value()) {
        return nullopt;
    }

    size_t fim = *inicio;

    while (fim < corpo.size()) {
        char c = corpo[fim];

        if (!(isdigit(static_cast<unsigned char>(c)) || c == '-' || c == '+' || c == '.' || c == 'e' || c == 'E')) {
            break;
        }

        fim++;
    }

    if (fim == *inicio) {
        return nullopt;
    }

    return stod(corpo.substr(*inicio, fim - *inicio));
}

string jsonErro(const string& mensagem) {
    return "{\"erro\":" + jsonString(mensagem) + "}";
}

string clienteJson(const Cliente& c) {
    ostringstream out;

    out << "{\"id\":" << c.getId()
        << ",\"nome\":" << jsonString(c.getNome())
        << ",\"telefone\":" << jsonString(c.getTelefone())
        << ",\"cpf\":" << jsonString(c.getCpf())
        << ",\"endereco\":" << jsonString(c.getEndereco())
        << "}";

    return out.str();
}

string habilidadeJson(const Habilidade& h) {
    ostringstream out;

    out << "{\"nome\":" << jsonString(h.getNome())
        << ",\"descricao\":" << jsonString(h.getDescricao())
        << ",\"valorHora\":" << h.getValorHora()
        << "}";

    return out.str();
}

string trabalhadorJson(const Trabalhador& t) {
    ostringstream out;

    out << "{\"id\":" << t.getId()
        << ",\"nome\":" << jsonString(t.getNome())
        << ",\"telefone\":" << jsonString(t.getTelefone())
        << ",\"cpf\":" << jsonString(t.getCpf())
        << ",\"avaliacaoMedia\":" << t.getAvaliacaoMedia()
        << ",\"totalAvaliacoes\":" << t.getTotalAvaliacoes()
        << ",\"habilidades\":[";

    const auto& habilidades = t.getHabilidades();

    for (size_t i = 0; i < habilidades.size(); ++i) {
        if (i > 0) {
            out << ',';
        }

        out << habilidadeJson(habilidades[i]);
    }

    out << "]}";
    return out.str();
}

string contratoJson(const Contrato& c) {
    ostringstream out;

    out << "{\"id\":" << c.getId()
        << ",\"clienteId\":" << c.getClienteId()
        << ",\"clienteNome\":" << jsonString(c.getClienteNome())
        << ",\"trabalhadorId\":" << c.getTrabalhadorId()
        << ",\"trabalhadorNome\":" << jsonString(c.getTrabalhadorNome())
        << ",\"habilidade\":" << jsonString(c.getHabilidade())
        << ",\"data\":" << jsonString(c.getData())
        << ",\"horas\":" << c.getHoras()
        << ",\"valorTotal\":" << c.getValorTotal()
        << ",\"descricao\":" << jsonString(c.getDescricao())
        << ",\"status\":" << jsonString(c.getStatus())
        << "}";

    return out.str();
}

template <typename T, typename F>
string vetorJson(const vector<T>& itens, F conversor) {
    string saida = "[";

    for (size_t i = 0; i < itens.size(); ++i) {
        if (i > 0) {
            saida += ",";
        }

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
    string caminho_;
    BancoDados dados_;

public:
    explicit BancoTxt(string caminho)
        : caminho_(move(caminho)) {
        carregar();
    }

    const string& caminho() const {
        return caminho_;
    }

    BancoDados& dados() {
        return dados_;
    }

    const BancoDados& dados() const {
        return dados_;
    }

    void carregar() {
        dados_.limpar();

        ifstream arquivo(caminho_);

        if (!arquivo.is_open()) {
            carregarDadosIniciais();
            salvar();
            return;
        }

        string secao;
        string linha;
        bool temConteudo = false;

        while (getline(arquivo, linha)) {
            linha = trim(linha);

            if (linha.empty() || linha[0] == '#') {
                continue;
            }

            temConteudo = true;

            if (linha.front() == '[' && linha.back() == ']') {
                secao = linha;
                continue;
            }

            if (linha.rfind("NEXT_CLIENTE_ID=", 0) == 0) {
                dados_.setProximoClienteId(paraInt(linha.substr(16), 1));
                continue;
            }

            if (linha.rfind("NEXT_TRABALHADOR_ID=", 0) == 0) {
                dados_.setProximoTrabalhadorId(paraInt(linha.substr(20), 1));
                continue;
            }

            if (linha.rfind("NEXT_CONTRATO_ID=", 0) == 0) {
                dados_.setProximoContratoId(paraInt(linha.substr(17), 1));
                continue;
            }

            auto partes = split(linha, '|');

            if (secao == "[CLIENTES]" && partes.size() >= 5) {
                Cliente cliente(
                    paraInt(partes[0]),
                    partes[1],
                    partes[2],
                    partes[3],
                    partes[4]
                );

                dados_.adicionarCliente(cliente);
            } else if (secao == "[TRABALHADORES]" && partes.size() >= 6) {
                Trabalhador trabalhador(
                    paraInt(partes[0]),
                    partes[1],
                    partes[2],
                    partes[3],
                    paraDouble(partes[4]),
                    paraInt(partes[5])
                );

                dados_.adicionarTrabalhador(trabalhador);
            } else if (secao == "[HABILIDADES]" && partes.size() >= 4) {
                int trabalhadorId = paraInt(partes[0]);
                Trabalhador* trabalhador = buscarTrabalhador(trabalhadorId);

                if (trabalhador != nullptr) {
                    Habilidade habilidade(partes[1], partes[2], paraDouble(partes[3]));
                    trabalhador->adicionarHabilidade(habilidade);
                }
            } else if (secao == "[CONTRATOS]" && partes.size() >= 11) {
                Contrato contrato(
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
                );

                dados_.adicionarContrato(contrato);
            }
        }

        if (!temConteudo) {
            carregarDadosIniciais();
            salvar();
        }
    }

    void salvar() const {
        ofstream arquivo(caminho_, ios::trunc);

        if (!arquivo.is_open()) {
            throw runtime_error("Nao foi possivel salvar o arquivo de banco de dados TXT.");
        }

        arquivo << "# Banco de dados TXT - Marido de Aluguel\n";
        arquivo << "# Arquivo de armazenamento local do projeto.\n";
        arquivo << "NEXT_CLIENTE_ID=" << dados_.getProximoClienteId() << "\n";
        arquivo << "NEXT_TRABALHADOR_ID=" << dados_.getProximoTrabalhadorId() << "\n";
        arquivo << "NEXT_CONTRATO_ID=" << dados_.getProximoContratoId() << "\n\n";

        arquivo << "[CLIENTES]\n";
        for (const auto& c : dados_.getClientes()) {
            arquivo << c.getId() << '|'
                    << limparCampoTxt(c.getNome()) << '|'
                    << limparCampoTxt(c.getTelefone()) << '|'
                    << limparCampoTxt(c.getCpf()) << '|'
                    << limparCampoTxt(c.getEndereco()) << "\n";
        }

        arquivo << "\n[TRABALHADORES]\n";
        for (const auto& t : dados_.getTrabalhadores()) {
            arquivo << t.getId() << '|'
                    << limparCampoTxt(t.getNome()) << '|'
                    << limparCampoTxt(t.getTelefone()) << '|'
                    << limparCampoTxt(t.getCpf()) << '|'
                    << t.getAvaliacaoMedia() << '|'
                    << t.getTotalAvaliacoes() << "\n";
        }

        arquivo << "\n[HABILIDADES]\n";
        for (const auto& t : dados_.getTrabalhadores()) {
            for (const auto& h : t.getHabilidades()) {
                arquivo << t.getId() << '|'
                        << limparCampoTxt(h.getNome()) << '|'
                        << limparCampoTxt(h.getDescricao()) << '|'
                        << h.getValorHora() << "\n";
            }
        }

        arquivo << "\n[CONTRATOS]\n";
        for (const auto& c : dados_.getContratos()) {
            arquivo << c.getId() << '|'
                    << c.getClienteId() << '|'
                    << limparCampoTxt(c.getClienteNome()) << '|'
                    << c.getTrabalhadorId() << '|'
                    << limparCampoTxt(c.getTrabalhadorNome()) << '|'
                    << limparCampoTxt(c.getHabilidade()) << '|'
                    << limparCampoTxt(c.getData()) << '|'
                    << c.getHoras() << '|'
                    << c.getValorTotal() << '|'
                    << limparCampoTxt(c.getDescricao()) << '|'
                    << limparCampoTxt(c.getStatus()) << "\n";
        }
    }

    Cliente* buscarCliente(int id) {
        for (auto& cliente : dados_.getClientes()) {
            if (cliente.getId() == id) {
                return &cliente;
            }
        }

        return nullptr;
    }

    Trabalhador* buscarTrabalhador(int id) {
        for (auto& trabalhador : dados_.getTrabalhadores()) {
            if (trabalhador.getId() == id) {
                return &trabalhador;
            }
        }

        return nullptr;
    }

    Contrato* buscarContrato(int id) {
        for (auto& contrato : dados_.getContratos()) {
            if (contrato.getId() == id) {
                return &contrato;
            }
        }

        return nullptr;
    }

private:
    void carregarDadosIniciais() {
        dados_.limpar();
        dados_.setProximoClienteId(3);
        dados_.setProximoTrabalhadorId(3);
        dados_.setProximoContratoId(1);

        dados_.adicionarCliente(Cliente(1, "Ana Lima", "47-99001-1111", "111.222.333-44", "Rua das Flores, 10"));
        dados_.adicionarCliente(Cliente(2, "Bruno Reis", "47-99002-2222", "555.666.777-88", "Av. Brasil, 200"));

        Trabalhador carlos(1, "Carlos Souza", "47-98001-0001", "100.200.300-01", 4.8, 4);
        carlos.adicionarHabilidade(Habilidade("Eletricista", "Instalacoes eletricas", 80.0));
        carlos.adicionarHabilidade(Habilidade("Pedreiro", "Alvenaria e pequenos reparos", 60.0));

        Trabalhador davi(2, "Davi Melo", "47-98002-0002", "200.300.400-02", 4.6, 3);
        davi.adicionarHabilidade(Habilidade("Encanador", "Reparos hidraulicos", 70.0));
        davi.adicionarHabilidade(Habilidade("Jardineiro", "Paisagismo e limpeza externa", 50.0));

        dados_.adicionarTrabalhador(carlos);
        dados_.adicionarTrabalhador(davi);
    }
};

// ============================================================
//  Regras do sistema
// ============================================================

class SistemaContratacao {
private:
    BancoTxt banco_;

public:
    explicit SistemaContratacao(const string& caminhoBanco)
        : banco_(caminhoBanco) {}

    const string& caminhoBanco() const {
        return banco_.caminho();
    }

    vector<Cliente> listarClientes() const {
        return banco_.dados().getClientes();
    }

    Cliente cadastrarCliente(const string& nome, const string& telefone, const string& cpf, const string& endereco) {
        if (trim(nome).empty()) {
            throw invalid_argument("Nome do cliente e obrigatorio.");
        }

        Cliente cliente;
        cliente.setId(banco_.dados().gerarClienteId());
        cliente.setNome(nome);
        cliente.setTelefone(telefone);
        cliente.setCpf(cpf);
        cliente.setEndereco(endereco);

        banco_.dados().adicionarCliente(cliente);
        banco_.salvar();

        return cliente;
    }

    vector<Trabalhador> listarTrabalhadores() const {
        return banco_.dados().getTrabalhadores();
    }

    vector<Trabalhador> listarTrabalhadoresDisponiveis(const string& data) const {
        vector<Trabalhador> disponiveis;

        for (const auto& trabalhador : banco_.dados().getTrabalhadores()) {
            bool ocupado = false;

            for (const auto& contrato : banco_.dados().getContratos()) {
                if (contrato.getTrabalhadorId() == trabalhador.getId()
                    && contrato.getData() == data
                    && contrato.getStatus() == "CONFIRMADO") {
                    ocupado = true;
                    break;
                }
            }

            if (!ocupado) {
                disponiveis.push_back(trabalhador);
            }
        }

        return disponiveis;
    }

    Trabalhador cadastrarTrabalhador(const string& nome, const string& telefone, const string& cpf) {
        if (trim(nome).empty()) {
            throw invalid_argument("Nome do trabalhador e obrigatorio.");
        }

        Trabalhador trabalhador;
        trabalhador.setId(banco_.dados().gerarTrabalhadorId());
        trabalhador.setNome(nome);
        trabalhador.setTelefone(telefone);
        trabalhador.setCpf(cpf);
        trabalhador.setAvaliacaoMedia(0.0);
        trabalhador.setTotalAvaliacoes(0);

        banco_.dados().adicionarTrabalhador(trabalhador);
        banco_.salvar();

        return trabalhador;
    }

    Trabalhador adicionarHabilidade(int trabalhadorId, const Habilidade& habilidade) {
        if (trim(habilidade.getNome()).empty()) {
            throw invalid_argument("Nome da habilidade e obrigatorio.");
        }

        if (habilidade.getValorHora() <= 0) {
            throw invalid_argument("Valor por hora deve ser maior que zero.");
        }

        Trabalhador* trabalhador = banco_.buscarTrabalhador(trabalhadorId);

        if (trabalhador == nullptr) {
            throw runtime_error("Trabalhador nao encontrado.");
        }

        trabalhador->adicionarHabilidade(habilidade);
        banco_.salvar();

        return *trabalhador;
    }

    vector<Contrato> listarContratos() const {
        return banco_.dados().getContratos();
    }

    Contrato criarContrato(int clienteId,
                           int trabalhadorId,
                           const string& habilidade,
                           const string& data,
                           int horas,
                           const string& descricao) {
        if (horas <= 0) {
            throw invalid_argument("Horas devem ser maiores que zero.");
        }

        if (trim(habilidade).empty()) {
            throw invalid_argument("Habilidade e obrigatoria.");
        }

        if (trim(data).empty()) {
            throw invalid_argument("Data e obrigatoria.");
        }

        Cliente* cliente = banco_.buscarCliente(clienteId);

        if (cliente == nullptr) {
            throw runtime_error("Cliente nao encontrado.");
        }

        Trabalhador* trabalhador = banco_.buscarTrabalhador(trabalhadorId);

        if (trabalhador == nullptr) {
            throw runtime_error("Trabalhador nao encontrado.");
        }

        double valorHora = trabalhador->getValorHabilidade(habilidade);

        if (valorHora <= 0.0) {
            throw runtime_error("Trabalhador nao possui a habilidade selecionada.");
        }

        Contrato contrato;
        contrato.setId(banco_.dados().gerarContratoId());
        contrato.setClienteId(cliente->getId());
        contrato.setClienteNome(cliente->getNome());
        contrato.setTrabalhadorId(trabalhador->getId());
        contrato.setTrabalhadorNome(trabalhador->getNome());
        contrato.setHabilidade(habilidade);
        contrato.setData(data);
        contrato.setHoras(horas);
        contrato.setValorTotal(valorHora * horas);
        contrato.setDescricao(descricao);
        contrato.setStatus("PENDENTE");

        banco_.dados().adicionarContrato(contrato);
        banco_.salvar();

        return contrato;
    }

    Contrato alterarStatusContrato(int id, const string& acao, double avaliacao = 5.0) {
        Contrato* contrato = banco_.buscarContrato(id);

        if (contrato == nullptr) {
            throw runtime_error("Contrato nao encontrado.");
        }

        if (acao == "confirmar") {
            if (contrato->getStatus() != "PENDENTE") {
                throw runtime_error("Apenas contratos pendentes podem ser confirmados.");
            }

            contrato->setStatus("CONFIRMADO");
        } else if (acao == "concluir") {
            if (contrato->getStatus() != "CONFIRMADO") {
                throw runtime_error("Apenas contratos confirmados podem ser concluidos.");
            }

            if (avaliacao < 0.0 || avaliacao > 5.0) {
                throw invalid_argument("Avaliacao deve estar entre 0 e 5.");
            }

            Trabalhador* trabalhador = banco_.buscarTrabalhador(contrato->getTrabalhadorId());

            if (trabalhador != nullptr) {
                trabalhador->receberAvaliacao(avaliacao);
            }

            contrato->setStatus("CONCLUIDO");
        } else if (acao == "cancelar") {
            if (contrato->getStatus() == "CONCLUIDO") {
                throw runtime_error("Contrato concluido nao pode ser cancelado.");
            }

            contrato->setStatus("CANCELADO");
        } else {
            throw invalid_argument("Acao de contrato invalida.");
        }

        banco_.salvar();
        return *contrato;
    }
};

// ============================================================
//  HTTP simples
// ============================================================

class HttpRequest {
private:
    string method_;
    string path_;
    string queryString_;
    map<string, string> query_;
    map<string, string> headers_;
    string body_;

public:
    string getMethod() const {
        return method_;
    }

    string getPath() const {
        return path_;
    }

    string getQueryString() const {
        return queryString_;
    }

    const map<string, string>& getQuery() const {
        return query_;
    }

    const map<string, string>& getHeaders() const {
        return headers_;
    }

    string getBody() const {
        return body_;
    }

    void setMethod(const string& method) {
        method_ = method;
    }

    void setPath(const string& path) {
        path_ = path;
    }

    void setQueryString(const string& queryString) {
        queryString_ = queryString;
    }

    void setQuery(const map<string, string>& query) {
        query_ = query;
    }

    void setHeaders(const map<string, string>& headers) {
        headers_ = headers;
    }

    void setBody(const string& body) {
        body_ = body;
    }

    void setHeader(const string& chave, const string& valor) {
        headers_[chave] = valor;
    }

    void setQueryParam(const string& chave, const string& valor) {
        query_[chave] = valor;
    }

    void removerPrefixoApi() {
        if (path_.rfind("/api/", 0) == 0) {
            path_ = path_.substr(4);
        }
    }
};

class HttpResponse {
private:
    int status_;
    string contentType_;
    string body_;

public:
    HttpResponse()
        : status_(200),
          contentType_("application/json; charset=utf-8") {}

    int getStatus() const {
        return status_;
    }

    string getContentType() const {
        return contentType_;
    }

    string getBody() const {
        return body_;
    }

    void setStatus(int status) {
        status_ = status;
    }

    void setContentType(const string& contentType) {
        contentType_ = contentType;
    }

    void setBody(const string& body) {
        body_ = body;
    }
};

void fecharSocket(SocketHandle socket) {
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}

string statusHttp(int status) {
    switch (status) {
        case 200:
            return "OK";

        case 201:
            return "Created";

        case 204:
            return "No Content";

        case 400:
            return "Bad Request";

        case 404:
            return "Not Found";

        case 405:
            return "Method Not Allowed";

        case 500:
            return "Internal Server Error";

        default:
            return "OK";
    }
}

string urlDecode(const string& valor) {
    string resultado;

    for (size_t i = 0; i < valor.size(); ++i) {
        if (valor[i] == '%' && i + 2 < valor.size()) {
            string hexValor = valor.substr(i + 1, 2);
            char* fim = nullptr;
            long decodificado = strtol(hexValor.c_str(), &fim, 16);

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

map<string, string> parseQuery(const string& queryString) {
    map<string, string> query;
    stringstream ss(queryString);
    string item;

    while (getline(ss, item, '&')) {
        auto pos = item.find('=');

        if (pos == string::npos) {
            query[urlDecode(item)] = "";
        } else {
            query[urlDecode(item.substr(0, pos))] = urlDecode(item.substr(pos + 1));
        }
    }

    return query;
}

optional<int> idDaRota(const string& caminho, const string& padrao) {
    smatch match;

    if (regex_match(caminho, match, regex(padrao)) && match.size() > 1) {
        return stoi(match[1]);
    }

    return nullopt;
}

HttpResponse respostaJson(int status, const string& corpo) {
    HttpResponse response;
    response.setStatus(status);
    response.setBody(corpo);
    return response;
}

HttpResponse respostaErro(int status, const string& mensagem) {
    return respostaJson(status, jsonErro(mensagem));
}

string montarRespostaHttp(const HttpResponse& response) {
    ostringstream out;

    out << "HTTP/1.1 " << response.getStatus() << ' ' << statusHttp(response.getStatus()) << "\r\n";
    out << "Content-Type: " << response.getContentType() << "\r\n";
    out << "Access-Control-Allow-Origin: *\r\n";
    out << "Access-Control-Allow-Methods: GET, POST, PUT, PATCH, DELETE, OPTIONS\r\n";
    out << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
    out << "Connection: close\r\n";
    out << "Content-Length: " << response.getBody().size() << "\r\n\r\n";
    out << response.getBody();

    return out.str();
}

size_t contentLengthDosHeaders(const string& headers) {
    string lower = headers;

    transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
        return static_cast<char>(tolower(c));
    });

    auto pos = lower.find("content-length:");

    if (pos == string::npos) {
        return 0;
    }

    pos += string("content-length:").size();

    while (pos < lower.size() && isspace(static_cast<unsigned char>(lower[pos]))) {
        pos++;
    }

    size_t fim = pos;

    while (fim < lower.size() && isdigit(static_cast<unsigned char>(lower[fim]))) {
        fim++;
    }

    return static_cast<size_t>(stoul(lower.substr(pos, fim - pos)));
}

string receberTudo(SocketHandle client) {
    string raw;
    char buffer[4096];

    while (true) {
        int recebido = recv(client, buffer, sizeof(buffer), 0);

        if (recebido <= 0) {
            break;
        }

        raw.append(buffer, buffer + recebido);
        auto fimHeader = raw.find("\r\n\r\n");

        if (fimHeader != string::npos) {
            size_t contentLength = contentLengthDosHeaders(raw.substr(0, fimHeader + 4));

            if (raw.size() >= fimHeader + 4 + contentLength) {
                break;
            }
        }
    }

    return raw;
}

void enviarTudo(SocketHandle client, const string& dados) {
    size_t total = 0;

    while (total < dados.size()) {
        int enviado = send(client, dados.data() + total, static_cast<int>(dados.size() - total), 0);

        if (enviado <= 0) {
            break;
        }

        total += static_cast<size_t>(enviado);
    }
}

HttpRequest parseRequest(const string& raw) {
    HttpRequest request;
    auto fimHeader = raw.find("\r\n\r\n");
    string headers = raw.substr(0, fimHeader);
    request.setBody(fimHeader == string::npos ? "" : raw.substr(fimHeader + 4));

    stringstream ss(headers);
    string linha;
    getline(ss, linha);

    if (!linha.empty() && linha.back() == '\r') {
        linha.pop_back();
    }

    stringstream primeiraLinha(linha);
    string alvo;
    string method;
    primeiraLinha >> method >> alvo;
    request.setMethod(method);

    auto posQuery = alvo.find('?');

    if (posQuery == string::npos) {
        request.setPath(alvo);
    } else {
        request.setPath(alvo.substr(0, posQuery));
        request.setQueryString(alvo.substr(posQuery + 1));
        request.setQuery(parseQuery(request.getQueryString()));
    }

    while (getline(ss, linha)) {
        if (!linha.empty() && linha.back() == '\r') {
            linha.pop_back();
        }

        auto pos = linha.find(':');

        if (pos == string::npos) {
            continue;
        }

        string chave = linha.substr(0, pos);
        string valor = linha.substr(pos + 1);
        valor = trim(valor);

        transform(chave.begin(), chave.end(), chave.begin(), [](unsigned char c) {
            return static_cast<char>(tolower(c));
        });

        request.setHeader(chave, valor);
    }

    return request;
}

class ApiServer {
private:
    SistemaContratacao& sistema_;

public:
    explicit ApiServer(SistemaContratacao& sistema)
        : sistema_(sistema) {}

    HttpResponse handle(HttpRequest request) {
        try {
            if (request.getMethod() == "OPTIONS") {
                return respostaJson(204, "");
            }

            request.removerPrefixoApi();

            if (request.getPath() == "/health") {
                return respostaJson(200, "{\"status\":\"online\",\"servico\":\"Marido de Aluguel\",\"armazenamento\":\"TXT\"}");
            }

            if (request.getPath() == "/clientes" && request.getMethod() == "GET") {
                return respostaJson(200, vetorJson(sistema_.listarClientes(), clienteJson));
            }

            if (request.getPath() == "/clientes" && request.getMethod() == "POST") {
                Cliente cliente = sistema_.cadastrarCliente(
                    jsonGetString(request.getBody(), "nome").value_or(""),
                    jsonGetString(request.getBody(), "telefone").value_or(""),
                    jsonGetString(request.getBody(), "cpf").value_or(""),
                    jsonGetString(request.getBody(), "endereco").value_or("")
                );

                return respostaJson(201, clienteJson(cliente));
            }

            if ((request.getPath() == "/trabalhadores" || request.getPath() == "/profissionais") && request.getMethod() == "GET") {
                return respostaJson(200, vetorJson(sistema_.listarTrabalhadores(), trabalhadorJson));
            }

            if (request.getPath() == "/trabalhadores/disponiveis" && request.getMethod() == "GET") {
                string data = "";
                auto it = request.getQuery().find("data");

                if (it != request.getQuery().end()) {
                    data = it->second;
                }

                return respostaJson(200, vetorJson(sistema_.listarTrabalhadoresDisponiveis(data), trabalhadorJson));
            }

            if ((request.getPath() == "/trabalhadores" || request.getPath() == "/profissionais") && request.getMethod() == "POST") {
                Trabalhador trabalhador = sistema_.cadastrarTrabalhador(
                    jsonGetString(request.getBody(), "nome").value_or(""),
                    jsonGetString(request.getBody(), "telefone").value_or(""),
                    jsonGetString(request.getBody(), "cpf").value_or("")
                );

                return respostaJson(201, trabalhadorJson(trabalhador));
            }

            if (auto id = idDaRota(request.getPath(), R"(^/trabalhadores/(\d+)/habilidades$)")) {
                if (request.getMethod() != "POST") {
                    return respostaErro(405, "Metodo nao permitido.");
                }

                Habilidade habilidade(
                    jsonGetString(request.getBody(), "nome").value_or(""),
                    jsonGetString(request.getBody(), "descricao").value_or(""),
                    jsonGetDouble(request.getBody(), "valorHora").value_or(0.0)
                );

                Trabalhador trabalhador = sistema_.adicionarHabilidade(*id, habilidade);
                return respostaJson(201, trabalhadorJson(trabalhador));
            }

            if (request.getPath() == "/contratos" && request.getMethod() == "GET") {
                return respostaJson(200, vetorJson(sistema_.listarContratos(), contratoJson));
            }

            if ((request.getPath() == "/contratos" || request.getPath() == "/orcamentos") && request.getMethod() == "POST") {
                int clienteId = jsonGetInt(request.getBody(), "idCliente").value_or(jsonGetInt(request.getBody(), "clienteId").value_or(0));
                int trabalhadorId = jsonGetInt(request.getBody(), "idTrabalhador").value_or(jsonGetInt(request.getBody(), "trabalhadorId").value_or(0));

                Contrato contrato = sistema_.criarContrato(
                    clienteId,
                    trabalhadorId,
                    jsonGetString(request.getBody(), "habilidade").value_or(""),
                    jsonGetString(request.getBody(), "data").value_or(""),
                    jsonGetInt(request.getBody(), "horas").value_or(1),
                    jsonGetString(request.getBody(), "descricao").value_or("")
                );

                return respostaJson(201, contratoJson(contrato));
            }

            if (auto id = idDaRota(request.getPath(), R"(^/contratos/(\d+)/confirmar$)")) {
                if (request.getMethod() != "PATCH" && request.getMethod() != "PUT") {
                    return respostaErro(405, "Metodo nao permitido.");
                }

                return respostaJson(200, contratoJson(sistema_.alterarStatusContrato(*id, "confirmar")));
            }

            if (auto id = idDaRota(request.getPath(), R"(^/contratos/(\d+)/concluir$)")) {
                if (request.getMethod() != "PATCH" && request.getMethod() != "PUT") {
                    return respostaErro(405, "Metodo nao permitido.");
                }

                double avaliacao = jsonGetDouble(request.getBody(), "avaliacao").value_or(5.0);
                return respostaJson(200, contratoJson(sistema_.alterarStatusContrato(*id, "concluir", avaliacao)));
            }

            if (auto id = idDaRota(request.getPath(), R"(^/contratos/(\d+)/cancelar$)")) {
                if (request.getMethod() != "PATCH" && request.getMethod() != "PUT") {
                    return respostaErro(405, "Metodo nao permitido.");
                }

                return respostaJson(200, contratoJson(sistema_.alterarStatusContrato(*id, "cancelar")));
            }

            return respostaErro(404, "Rota nao encontrada.");
        } catch (const exception& erro) {
            return respostaErro(400, erro.what());
        }
    }

    void iniciar(const string& host, int porta) {
#ifdef _WIN32
        WSADATA wsaData;

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw runtime_error("Falha ao iniciar Winsock.");
        }
#endif

        SocketHandle servidor = socket(AF_INET, SOCK_STREAM, 0);

        if (servidor == INVALID_SOCKET_HANDLE) {
            throw runtime_error("Nao foi possivel criar o socket do servidor.");
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
            throw runtime_error("Nao foi possivel usar a porta informada.");
        }

        if (listen(servidor, 16) < 0) {
            fecharSocket(servidor);
            throw runtime_error("Nao foi possivel iniciar a escuta do servidor.");
        }

        cout << "Marido de Aluguel - Servidor iniciado\n";
        cout << "Servidor: http://localhost:" << porta << "\n";
        cout << "Health:   http://localhost:" << porta << "/health\n";
        cout << "Banco:    " << sistema_.caminhoBanco() << "\n";

        while (true) {
            sockaddr_in clienteEndereco{};

#ifdef _WIN32
            int tamanhoEndereco = sizeof(clienteEndereco);
#else
            socklen_t tamanhoEndereco = sizeof(clienteEndereco);
#endif

            SocketHandle cliente = accept(servidor, reinterpret_cast<sockaddr*>(&clienteEndereco), &tamanhoEndereco);

            if (cliente == INVALID_SOCKET_HANDLE) {
                continue;
            }

            thread([this, cliente]() {
                string raw = receberTudo(cliente);

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
        const string caminhoBanco = envStringOrDefault("BANCO_TXT_CAMINHO", "data/banco_dados.txt");
        const int porta = envIntOrDefault("APP_PORT", 8080);

        SistemaContratacao sistema(caminhoBanco);
        ApiServer servidor(sistema);
        servidor.iniciar("0.0.0.0", porta);
    } catch (const exception& erro) {
        cerr << "Erro ao iniciar o servidor: " << erro.what() << "\n";
        return 1;
    }

    return 0;
}

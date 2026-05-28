/*
 * ============================================================
 *  SISTEMA MARIDO DE ALUGUEL - API REST COM CROW
 *  Disciplina: Programacao Orientada a Objetos
 *  Converso: Interface REST em C++ com Crow
 *
 *  Compile:
 *    g++ -std=c++17 -Wall -Wextra -pedantic -o marido_aluguel MaridoAluguelCorrigido.cpp
 *
 *  Dependencias:
 *    - Crow (header-only)
 *    - nlohmann/json
 * ============================================================
 */

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "crow.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ============================================================
//  CLASSE: Data
//  Usada por Contrato e Trabalhador.
// ============================================================
class Data
{
private:
    int dia;
    int mes;
    int ano;

    static bool ehBissexto(int ano)
    {
        return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
    }

    static bool ehValida(int dia, int mes, int ano)
    {
        if (ano < 1900 || mes < 1 || mes > 12 || dia < 1)
        {
            return false;
        }

        int diasMes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (ehBissexto(ano))
        {
            diasMes[2] = 29;
        }

        return dia <= diasMes[mes];
    }

public:
    Data(int dia = 1, int mes = 1, int ano = 2000)
    {
        setData(dia, mes, ano);
    }

    int getDia() const { return dia; }
    int getMes() const { return mes; }
    int getAno() const { return ano; }

    void setData(int novoDia, int novoMes, int novoAno)
    {
        if (!ehValida(novoDia, novoMes, novoAno))
        {
            throw std::invalid_argument("Data invalida.");
        }

        dia = novoDia;
        mes = novoMes;
        ano = novoAno;
    }

    bool valida() const
    {
        return ehValida(dia, mes, ano);
    }

    bool operator==(const Data &outra) const
    {
        return dia == outra.dia && mes == outra.mes && ano == outra.ano;
    }

    std::string toString() const
    {
        char buffer[12];
        std::snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", dia, mes, ano);
        return buffer;
    }

    json toJson() const
    {
        return json{
            {"dia", dia},
            {"mes", mes},
            {"ano", ano},
            {"data", toString()}
        };
    }
};

// ============================================================
//  CLASSE: Habilidade
//  Composicao: pertence exclusivamente a um Trabalhador.
// ============================================================
class Habilidade
{
private:
    std::string nome;
    std::string descricao;
    double valorHora;

public:
    Habilidade(const std::string &nome, const std::string &descricao, double valorHora)
        : nome(nome), descricao(descricao), valorHora(valorHora)
    {
        if (nome.empty())
        {
            throw std::invalid_argument("Nome da habilidade nao pode ser vazio.");
        }
        if (valorHora <= 0)
        {
            throw std::invalid_argument("Valor por hora deve ser maior que zero.");
        }
    }

    std::string getNome() const { return nome; }
    std::string getDescricao() const { return descricao; }
    double getValorHora() const { return valorHora; }

    void setNome(const std::string &novoNome)
    {
        if (novoNome.empty())
        {
            throw std::invalid_argument("Nome da habilidade nao pode ser vazio.");
        }
        nome = novoNome;
    }

    void setDescricao(const std::string &novaDescricao)
    {
        descricao = novaDescricao;
    }

    void setValorHora(double novoValorHora)
    {
        if (novoValorHora <= 0)
        {
            throw std::invalid_argument("Valor por hora deve ser maior que zero.");
        }
        valorHora = novoValorHora;
    }

    void exibir() const
    {
        std::cout << "    [+] " << nome << " - " << descricao
                  << " (R$ " << std::fixed << std::setprecision(2) << valorHora << "/h)\n";
    }

    json toJson() const
    {
        return json{
            {"nome", nome},
            {"descricao", descricao},
            {"valorHora", valorHora}
        };
    }
};

// ============================================================
//  CLASSE: Pessoa  (abstrata)
//  Heranca: base para Cliente e Trabalhador.
//  Observacao: tudo private; classes filhas usam getters/setters.
// ============================================================
class Pessoa
{
private:
    int id;
    std::string nome;
    std::string telefone;
    std::string cpf;

public:
    Pessoa(int id, const std::string &nome, const std::string &telefone, const std::string &cpf)
        : id(id), nome(nome), telefone(telefone), cpf(cpf)
    {
        if (id <= 0)
        {
            throw std::invalid_argument("ID deve ser maior que zero.");
        }
        if (nome.empty())
        {
            throw std::invalid_argument("Nome nao pode ser vazio.");
        }
    }

    virtual ~Pessoa() = default;

    int getId() const { return id; }
    std::string getNome() const { return nome; }
    std::string getTelefone() const { return telefone; }
    std::string getCpf() const { return cpf; }

    std::string getCpfMascarado() const
    {
        if (cpf.size() < 4)
        {
            return "***";
        }
        return "***.***.***-" + cpf.substr(cpf.size() - 2);
    }

    void setNome(const std::string &novoNome)
    {
        if (novoNome.empty())
        {
            throw std::invalid_argument("Nome nao pode ser vazio.");
        }
        nome = novoNome;
    }

    void setTelefone(const std::string &novoTelefone)
    {
        telefone = novoTelefone;
    }

    void setCpf(const std::string &novoCpf)
    {
        cpf = novoCpf;
    }

    virtual void exibir() const = 0;
};

// ============================================================
//  CLASSE: Cliente
//  Herda de Pessoa.
// ============================================================
class Cliente : public Pessoa
{
private:
    std::string endereco;

public:
    Cliente(int id, const std::string &nome, const std::string &telefone,
            const std::string &cpf, const std::string &endereco)
        : Pessoa(id, nome, telefone, cpf), endereco(endereco) {}

    std::string getEndereco() const { return endereco; }

    void setEndereco(const std::string &novoEndereco)
    {
        endereco = novoEndereco;
    }

    void exibir() const override
    {
        std::cout << "  [Cliente #" << getId() << "] " << getNome()
                  << " | Tel: " << getTelefone()
                  << " | CPF: " << getCpfMascarado()
                  << " | End: " << endereco << "\n";
    }

    json toJson() const
    {
        return json{
            {"id", getId()},
            {"nome", getNome()},
            {"telefone", getTelefone()},
            {"cpf", getCpf()},
            {"cpfMascarado", getCpfMascarado()},
            {"endereco", endereco}
        };
    }
};

// ============================================================
//  CLASSE: Trabalhador
//  Herda de Pessoa.
//  Composicao com Habilidade: trabalhador possui suas habilidades.
// ============================================================
class Trabalhador : public Pessoa
{
private:
    std::vector<Habilidade> habilidades;
    double avaliacaoMedia;
    int totalAvaliacoes;
    std::vector<Data> datasOcupadas;

public:
    Trabalhador(int id, const std::string &nome, const std::string &telefone, const std::string &cpf)
        : Pessoa(id, nome, telefone, cpf), avaliacaoMedia(0.0), totalAvaliacoes(0) {}

    const std::vector<Habilidade> &getHabilidades() const { return habilidades; }
    double getAvaliacaoMedia() const { return avaliacaoMedia; }
    int getTotalAvaliacoes() const { return totalAvaliacoes; }
    const std::vector<Data> &getDatasOcupadas() const { return datasOcupadas; }

    void adicionarHabilidade(const Habilidade &habilidade)
    {
        if (possuiHabilidade(habilidade.getNome()))
        {
            throw std::runtime_error("Trabalhador ja possui essa habilidade.");
        }
        habilidades.push_back(habilidade);
    }

    bool possuiHabilidade(const std::string &nomeHabilidade) const
    {
        for (const auto &habilidade : habilidades)
        {
            if (habilidade.getNome() == nomeHabilidade)
            {
                return true;
            }
        }
        return false;
    }

    double getValorHabilidade(const std::string &nomeHabilidade) const
    {
        for (const auto &habilidade : habilidades)
        {
            if (habilidade.getNome() == nomeHabilidade)
            {
                return habilidade.getValorHora();
            }
        }
        return 0.0;
    }

    bool estaDisponivel(const Data &data) const
    {
        for (const auto &dataOcupada : datasOcupadas)
        {
            if (dataOcupada == data)
            {
                return false;
            }
        }
        return true;
    }

    void reservarData(const Data &data)
    {
        if (!estaDisponivel(data))
        {
            throw std::runtime_error("Data ja esta ocupada para este trabalhador.");
        }
        datasOcupadas.push_back(data);
    }

    void liberarData(const Data &data)
    {
        auto it = std::remove_if(datasOcupadas.begin(), datasOcupadas.end(),
                                 [&data](const Data &dataOcupada)
                                 {
                                     return dataOcupada == data;
                                 });
        datasOcupadas.erase(it, datasOcupadas.end());
    }

    void receberAvaliacao(double nota)
    {
        if (nota < 0.0 || nota > 5.0)
        {
            throw std::invalid_argument("Avaliacao deve estar entre 0 e 5.");
        }
        avaliacaoMedia = (avaliacaoMedia * totalAvaliacoes + nota) / (totalAvaliacoes + 1);
        totalAvaliacoes++;
    }

    void exibir() const override
    {
        std::cout << "  [Trabalhador #" << getId() << "] " << getNome()
                  << " | Tel: " << getTelefone()
                  << " | CPF: " << getCpfMascarado()
                  << " | Avaliacao: " << std::fixed << std::setprecision(1)
                  << avaliacaoMedia << " (" << totalAvaliacoes << "x)\n";

        if (habilidades.empty())
        {
            std::cout << "    (sem habilidades)\n";
            return;
        }

        for (const auto &habilidade : habilidades)
        {
            habilidade.exibir();
        }
    }

    json toJson() const
    {
        json habilidadesJson = json::array();
        for (const auto &h : habilidades)
        {
            habilidadesJson.push_back(h.toJson());
        }

        return json{
            {"id", getId()},
            {"nome", getNome()},
            {"telefone", getTelefone()},
            {"cpf", getCpf()},
            {"cpfMascarado", getCpfMascarado()},
            {"avaliacaoMedia", avaliacaoMedia},
            {"totalAvaliacoes", totalAvaliacoes},
            {"habilidades", habilidadesJson}
        };
    }
};

// ============================================================
//  ENUM: Status do Contrato
// ============================================================
enum class Status
{
    PENDENTE,
    CONFIRMADO,
    CONCLUIDO,
    CANCELADO
};

// ============================================================
//  CLASSE: Contrato
//  Nao possui Cliente/Trabalhador. Apenas referencia objetos que
//  pertencem ao SistemaContratacao. A composicao forte fica no sistema.
// ============================================================
class Contrato
{
private:
    static int proximoId;

    int id;
    Cliente *cliente;
    Trabalhador *trabalhador;
    std::string habilidade;
    Data data;
    int horas;
    double valorTotal;
    std::string descricao;
    Status status;

public:
    static int getProximoId() { return proximoId; }
    static void setProximoId(int novo) { proximoId = novo; }

    Contrato(Cliente *cliente, Trabalhador *trabalhador,
             const std::string &habilidade, const Data &data,
             int horas, const std::string &descricao)
        : id(proximoId++), cliente(cliente), trabalhador(trabalhador), habilidade(habilidade),
          data(data), horas(horas), valorTotal(0.0), descricao(descricao), status(Status::PENDENTE)
    {
        if (cliente == nullptr)
        {
            throw std::invalid_argument("Cliente nao pode ser nulo.");
        }
        if (trabalhador == nullptr)
        {
            throw std::invalid_argument("Trabalhador nao pode ser nulo.");
        }
        if (habilidade.empty())
        {
            throw std::invalid_argument("Habilidade nao pode ser vazia.");
        }
        if (horas <= 0)
        {
            throw std::invalid_argument("Horas devem ser maiores que zero.");
        }
        if (!trabalhador->possuiHabilidade(habilidade))
        {
            throw std::runtime_error("Trabalhador nao possui a habilidade: " + habilidade);
        }
        if (!trabalhador->estaDisponivel(data))
        {
            throw std::runtime_error("Trabalhador nao esta disponivel nessa data.");
        }

        valorTotal = trabalhador->getValorHabilidade(habilidade) * horas;
    }

    int getId() const { return id; }
    const Cliente *getCliente() const { return cliente; }
    const Trabalhador *getTrabalhador() const { return trabalhador; }
    std::string getHabilidade() const { return habilidade; }
    Data getData() const { return data; }
    int getHoras() const { return horas; }
    double getValorTotal() const { return valorTotal; }
    std::string getDescricao() const { return descricao; }
    Status getStatus() const { return status; }

    std::string statusStr() const
    {
        switch (status)
        {
        case Status::PENDENTE:
            return "PENDENTE";
        case Status::CONFIRMADO:
            return "CONFIRMADO";
        case Status::CONCLUIDO:
            return "CONCLUIDO";
        case Status::CANCELADO:
            return "CANCELADO";
        default:
            return "DESCONHECIDO";
        }
    }

    void confirmar()
    {
        if (status != Status::PENDENTE)
        {
            throw std::runtime_error("Contrato nao esta PENDENTE.");
        }
        trabalhador->reservarData(data);
        status = Status::CONFIRMADO;
    }

    void concluir(double avaliacao)
    {
        if (status != Status::CONFIRMADO)
        {
            throw std::runtime_error("Contrato nao esta CONFIRMADO.");
        }
        trabalhador->receberAvaliacao(avaliacao);
        trabalhador->liberarData(data);
        status = Status::CONCLUIDO;
    }

    void cancelar()
    {
        if (status == Status::CONCLUIDO)
        {
            throw std::runtime_error("Contrato ja foi concluido e nao pode ser cancelado.");
        }
        if (status == Status::CONFIRMADO)
        {
            trabalhador->liberarData(data);
        }
        status = Status::CANCELADO;
    }

    void exibir() const
    {
        std::cout << "  [Contrato #" << id << "] " << statusStr()
                  << " | " << cliente->getNome()
                  << " -> " << trabalhador->getNome()
                  << " | " << habilidade
                  << " | " << data.toString()
                  << " | " << horas << "h"
                  << " | R$ " << std::fixed << std::setprecision(2) << valorTotal
                  << "\n    Desc: " << descricao << "\n";
    }

    json toJson() const
    {
        return json{
            {"id", id},
            {"status", statusStr()},
            {"clienteId", cliente->getId()},
            {"clienteNome", cliente->getNome()},
            {"trabalhadorId", trabalhador->getId()},
            {"trabalhadorNome", trabalhador->getNome()},
            {"habilidade", habilidade},
            {"data", data.toJson()},
            {"horas", horas},
            {"valorTotal", valorTotal},
            {"descricao", descricao}
        };
    }
};

int Contrato::proximoId = 1;

// ============================================================
//  CLASSE: SistemaContratacao
//  Composicao: o sistema possui clientes, trabalhadores e contratos.
// ============================================================
class SistemaContratacao
{
private:
    std::vector<std::unique_ptr<Cliente>> clientes;
    std::vector<std::unique_ptr<Trabalhador>> trabalhadores;
    std::vector<std::unique_ptr<Contrato>> contratos;
    int idCliente;
    int idTrabalhador;
    const std::string nomeArquivoBanco = "banco_dados.json";

    void salvarEmArquivo() const
    {
        json banco;
        
        // Salvar clientes
        banco["clientes"] = json::array();
        for (const auto &cliente : clientes)
        {
            banco["clientes"].push_back(cliente->toJson());
        }
        
        // Salvar trabalhadores
        banco["trabalhadores"] = json::array();
        for (const auto &trabalhador : trabalhadores)
        {
            banco["trabalhadores"].push_back(trabalhador->toJson());
        }
        
        // Salvar contratos
        banco["contratos"] = json::array();
        for (const auto &contrato : contratos)
        {
            banco["contratos"].push_back(contrato->toJson());
        }
        
        banco["idCliente"] = idCliente;
        banco["idTrabalhador"] = idTrabalhador;
        banco["proximoContratoId"] = Contrato::getProximoId();
        
        std::ofstream arquivo(nomeArquivoBanco);
        arquivo << banco.dump(2);
        arquivo.close();
    }

    void carregarDoArquivo()
    {
        std::ifstream arquivo(nomeArquivoBanco);
        
        if (!arquivo.is_open())
        {
            std::cout << "[INFO] Arquivo de banco de dados nao encontrado. Iniciando vazio.\n";
            return;
        }
        
        try
        {
            json banco;
            arquivo >> banco;
            arquivo.close();
            
            // Restaurar IDs
            if (banco.contains("idCliente"))
                idCliente = banco["idCliente"].get<int>();
            if (banco.contains("idTrabalhador"))
                idTrabalhador = banco["idTrabalhador"].get<int>();
            if (banco.contains("proximoContratoId"))
                Contrato::setProximoId(banco["proximoContratoId"].get<int>());
            
            // Carregar clientes
            if (banco.contains("clientes"))
            {
                for (const auto &clienteJson : banco["clientes"])
                {
                    auto cliente = std::make_unique<Cliente>(
                        clienteJson["id"].get<int>(),
                        clienteJson["nome"].get<std::string>(),
                        clienteJson["telefone"].get<std::string>(),
                        clienteJson["cpf"].get<std::string>(),
                        clienteJson["endereco"].get<std::string>()
                    );
                    clientes.push_back(std::move(cliente));
                }
            }
            
            // Carregar trabalhadores
            if (banco.contains("trabalhadores"))
            {
                for (const auto &trabJson : banco["trabalhadores"])
                {
                    auto trabalhador = std::make_unique<Trabalhador>(
                        trabJson["id"].get<int>(),
                        trabJson["nome"].get<std::string>(),
                        trabJson["telefone"].get<std::string>(),
                        trabJson["cpf"].get<std::string>()
                    );
                    
                    // Restaurar habilidades
                    if (trabJson.contains("habilidades"))
                    {
                        for (const auto &habJson : trabJson["habilidades"])
                        {
                            Habilidade hab(
                                habJson["nome"].get<std::string>(),
                                habJson["descricao"].get<std::string>(),
                                habJson["valorHora"].get<double>()
                            );
                            trabalhador->adicionarHabilidade(hab);
                        }
                    }
                    
                    trabalhadores.push_back(std::move(trabalhador));
                }
            }
            
            std::cout << "[OK] Dados carregados do arquivo de banco de dados.\n";
        }
        catch (const std::exception &e)
        {
            std::cout << "[ERRO] Falha ao carregar dados: " << e.what() << "\n";
        }
    }

public:
    SistemaContratacao() : idCliente(1), idTrabalhador(1) 
    {
        carregarDoArquivo();
    }

    Cliente *cadastrarCliente(const std::string &nome, const std::string &telefone,
                              const std::string &cpf, const std::string &endereco)
    {
        auto cliente = std::make_unique<Cliente>(idCliente++, nome, telefone, cpf, endereco);
        Cliente *ponteiroCliente = cliente.get();
        clientes.push_back(std::move(cliente));

        std::cout << "[OK] Cliente '" << nome << "' cadastrado (ID "
                  << ponteiroCliente->getId() << ")\n";
        
        salvarEmArquivo();
        return ponteiroCliente;
    }

    Trabalhador *cadastrarTrabalhador(const std::string &nome, const std::string &telefone,
                                      const std::string &cpf)
    {
        auto trabalhador = std::make_unique<Trabalhador>(idTrabalhador++, nome, telefone, cpf);
        Trabalhador *ponteiroTrabalhador = trabalhador.get();
        trabalhadores.push_back(std::move(trabalhador));

        std::cout << "[OK] Trabalhador '" << nome << "' cadastrado (ID "
                  << ponteiroTrabalhador->getId() << ")\n";
        
        salvarEmArquivo();
        return ponteiroTrabalhador;
    }

    Cliente *buscarCliente(int id) const
    {
        for (const auto &cliente : clientes)
        {
            if (cliente->getId() == id)
            {
                return cliente.get();
            }
        }
        return nullptr;
    }

    Trabalhador *buscarTrabalhador(int id) const
    {
        for (const auto &trabalhador : trabalhadores)
        {
            if (trabalhador->getId() == id)
            {
                return trabalhador.get();
            }
        }
        return nullptr;
    }

    Contrato *buscarContrato(int id) const
    {
        for (const auto &contrato : contratos)
        {
            if (contrato->getId() == id)
            {
                return contrato.get();
            }
        }
        return nullptr;
    }

    void listarClientes() const
    {
        std::cout << "--- Clientes ---\n";
        if (clientes.empty())
        {
            std::cout << "  (vazio)\n";
            return;
        }
        for (const auto &cliente : clientes)
        {
            cliente->exibir();
        }
    }

    void listarTrabalhadores() const
    {
        std::cout << "--- Trabalhadores ---\n";
        if (trabalhadores.empty())
        {
            std::cout << "  (vazio)\n";
            return;
        }
        for (const auto &trabalhador : trabalhadores)
        {
            trabalhador->exibir();
        }
    }

    void listarContratos() const
    {
        std::cout << "--- Contratos ---\n";
        if (contratos.empty())
        {
            std::cout << "  (vazio)\n";
            return;
        }
        for (const auto &contrato : contratos)
        {
            contrato->exibir();
        }
    }

    void listarTrabalhadoresDisponiveis(const Data &data) const
    {
        std::cout << "--- Disponiveis em " << data.toString() << " ---\n";
        bool encontrou = false;

        for (const auto &trabalhador : trabalhadores)
        {
            if (trabalhador->estaDisponivel(data))
            {
                trabalhador->exibir();
                encontrou = true;
            }
        }

        if (!encontrou)
        {
            std::cout << "  (nenhum disponivel)\n";
        }
    }

    void criarContrato(int idCliente, int idTrabalhador, const std::string &habilidade,
                       const Data &data, int horas, const std::string &descricao)
    {
        Cliente *cliente = buscarCliente(idCliente);
        Trabalhador *trabalhador = buscarTrabalhador(idTrabalhador);

        if (cliente == nullptr)
        {
            throw std::runtime_error("Cliente nao encontrado.");
        }
        if (trabalhador == nullptr)
        {
            throw std::runtime_error("Trabalhador nao encontrado.");
        }

        auto contrato = std::make_unique<Contrato>(cliente, trabalhador, habilidade, data, horas, descricao);
        int idContrato = contrato->getId();
        contratos.push_back(std::move(contrato));

        std::cout << "[OK] Contrato #" << idContrato << " criado (PENDENTE)\n";
        salvarEmArquivo();
    }

    void confirmarContrato(int id)
    {
        Contrato *contrato = buscarContrato(id);
        if (contrato == nullptr)
        {
            throw std::runtime_error("Contrato nao encontrado.");
        }
        contrato->confirmar();
        std::cout << "[OK] Contrato #" << id << " confirmado.\n";
        salvarEmArquivo();
    }

    void concluirContrato(int id, double avaliacao)
    {
        Contrato *contrato = buscarContrato(id);
        if (contrato == nullptr)
        {
            throw std::runtime_error("Contrato nao encontrado.");
        }
        contrato->concluir(avaliacao);
        std::cout << "[OK] Contrato #" << id << " concluido. Avaliacao: " << avaliacao << "\n";
        salvarEmArquivo();
    }

    void cancelarContrato(int id)
    {
        Contrato *contrato = buscarContrato(id);
        if (contrato == nullptr)
        {
            throw std::runtime_error("Contrato nao encontrado.");
        }
        contrato->cancelar();
        std::cout << "[OK] Contrato #" << id << " cancelado.\n";
        salvarEmArquivo();
    }

    // ====== METODOS PARA API ======
    json obterTodosClientesJson() const
    {
        json resultado = json::array();
        for (const auto &cliente : clientes)
        {
            resultado.push_back(cliente->toJson());
        }
        return resultado;
    }

    json obterClienteJson(int id) const
    {
        Cliente *cliente = buscarCliente(id);
        if (cliente == nullptr)
        {
            return nullptr;
        }
        return cliente->toJson();
    }

    json obterTodosTrabalhoresJson() const
    {
        json resultado = json::array();
        for (const auto &trabalhador : trabalhadores)
        {
            resultado.push_back(trabalhador->toJson());
        }
        return resultado;
    }

    json obterTrabalhadorJson(int id) const
    {
        Trabalhador *trabalhador = buscarTrabalhador(id);
        if (trabalhador == nullptr)
        {
            return nullptr;
        }
        return trabalhador->toJson();
    }

    json obterTodosContratosJson() const
    {
        json resultado = json::array();
        for (const auto &contrato : contratos)
        {
            resultado.push_back(contrato->toJson());
        }
        return resultado;
    }

    json obterContratoJson(int id) const
    {
        Contrato *contrato = buscarContrato(id);
        if (contrato == nullptr)
        {
            return nullptr;
        }
        return contrato->toJson();
    }

    json obterTrabalhadoresDisponiveisJson(int dia, int mes, int ano) const
    {
        try
        {
            Data data(dia, mes, ano);
            json resultado = json::array();
            for (const auto &trabalhador : trabalhadores)
            {
                if (trabalhador->estaDisponivel(data))
                {
                    resultado.push_back(trabalhador->toJson());
                }
            }
            return resultado;
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error(e.what());
        }
    }
};
static void limparEntrada()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static int lerInt(const std::string &mensagem)
{
    int valor;
    while (true)
    {
        std::cout << mensagem;
        if (std::cin >> valor)
        {
            limparEntrada();
            return valor;
        }
        std::cin.clear();
        limparEntrada();
        std::cout << "  Valor invalido, tente novamente.\n";
    }
}

static double lerDouble(const std::string &mensagem)
{
    double valor;
    while (true)
    {
        std::cout << mensagem;
        if (std::cin >> valor)
        {
            limparEntrada();
            return valor;
        }
        std::cin.clear();
        limparEntrada();
        std::cout << "  Valor invalido, tente novamente.\n";
    }
}

static std::string lerString(const std::string &mensagem)
{
    std::string valor;
    std::cout << mensagem;
    std::getline(std::cin, valor);
    return valor;
}

static Data lerData(const std::string &mensagem)
{
    while (true)
    {
        try
        {
            std::cout << mensagem << " (DD MM AAAA): ";
            int dia, mes, ano;
            std::cin >> dia >> mes >> ano;
            limparEntrada();
            return Data(dia, mes, ano);
        }
        catch (const std::exception &erro)
        {
            std::cin.clear();
            std::cout << "  " << erro.what() << " Tente novamente.\n";
        }
    }
}

// ============================================================
//  VARIAVEIS GLOBAIS PARA API
// ============================================================
std::unique_ptr<SistemaContratacao> sistemaGlobal;

// ============================================================
//  FUNCOES AUXILIARES DE RESPOSTA
// ============================================================
inline crow::response respostaJson(int statusCode, const json &dados)
{
    auto resp = crow::response(statusCode, dados.dump());
    resp.set_header("Content-Type", "application/json");
    resp.set_header("Access-Control-Allow-Origin", "*");
    resp.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    resp.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    return resp;
}

inline crow::response respostaSucesso(const json &dados = json::object())
{
    json resposta;
    resposta["sucesso"] = true;
    resposta["dados"] = dados;
    return respostaJson(200, resposta);
}

inline crow::response respostaErro(const std::string &mensagem, int statusCode = 400)
{
    json resposta;
    resposta["sucesso"] = false;
    resposta["erro"] = mensagem;
    return respostaJson(statusCode, resposta);
}

// ============================================================
//  FUNCOES DE DADOS EXEMPLO
// ============================================================
static void popularDadosExemplo()
{
    sistemaGlobal->cadastrarCliente("Ana Lima", "47-99001-1111", "111.222.333-44", "Rua das Flores, 10");
    sistemaGlobal->cadastrarCliente("Bruno Reis", "47-99002-2222", "555.666.777-88", "Av. Brasil, 200");

    Trabalhador *trabalhador1 = sistemaGlobal->cadastrarTrabalhador("Carlos Souza", "47-98001-0001", "100.200.300-01");
    trabalhador1->adicionarHabilidade(Habilidade("Eletricista", "Instalacoes eletricas", 80.0));
    trabalhador1->adicionarHabilidade(Habilidade("Pedreiro", "Alvenaria", 60.0));

    Trabalhador *trabalhador2 = sistemaGlobal->cadastrarTrabalhador("Davi Melo", "47-98002-0002", "200.300.400-02");
    trabalhador2->adicionarHabilidade(Habilidade("Encanador", "Reparos hidraulicos", 70.0));
    trabalhador2->adicionarHabilidade(Habilidade("Jardineiro", "Paisagismo", 50.0));
}

// ============================================================
//  MAIN - SERVIDOR CROW
// ============================================================
int main()
{
    sistemaGlobal = std::make_unique<SistemaContratacao>();
    popularDadosExemplo();

    crow::SimpleApp app;

    // ===== Handler para PREFLIGHT (OPTIONS) requests =====
    CROW_ROUTE(app, "/<path>").methods("OPTIONS"_method)
    ([](const std::string &path) {
        auto resp = crow::response(200, "");
        resp.set_header("Access-Control-Allow-Origin", "*");
        resp.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        resp.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        resp.set_header("Access-Control-Max-Age", "86400");
        return resp;
    });

    // ===== GET /api/clientes =====
    CROW_ROUTE(app, "/api/clientes")
    ([]() {
        return respostaSucesso(sistemaGlobal->obterTodosClientesJson());
    });

    // ===== GET /api/clientes/<int> =====
    CROW_ROUTE(app, "/api/clientes/<int>")
    ([](int id) {
        auto cliente = sistemaGlobal->obterClienteJson(id);
        if (cliente.is_null())
        {
            return respostaErro("Cliente nao encontrado", 404);
        }
        return respostaSucesso(cliente);
    });

    // ===== POST /api/clientes =====
    CROW_ROUTE(app, "/api/clientes").methods("POST"_method)
    ([](const crow::request &req) {
        try
        {
            auto corpo = json::parse(req.body);
            auto cliente = sistemaGlobal->cadastrarCliente(
                corpo["nome"].get<std::string>(),
                corpo["telefone"].get<std::string>(),
                corpo["cpf"].get<std::string>(),
                corpo["endereco"].get<std::string>()
            );
            return respostaSucesso(json{{"id", cliente->getId()}});
        }
        catch (const std::exception &e)
        {
            return respostaErro(e.what(), 400);
        }
    });

    // ===== GET /api/trabalhadores =====
    CROW_ROUTE(app, "/api/trabalhadores")
    ([]() {
        return respostaSucesso(sistemaGlobal->obterTodosTrabalhoresJson());
    });

    // ===== GET /api/trabalhadores/<int> =====
    CROW_ROUTE(app, "/api/trabalhadores/<int>")
    ([](int id) {
        auto trab = sistemaGlobal->obterTrabalhadorJson(id);
        if (trab.is_null())
        {
            return respostaErro("Trabalhador nao encontrado", 404);
        }
        return respostaSucesso(trab);
    });

    // ===== POST /api/trabalhadores =====
    CROW_ROUTE(app, "/api/trabalhadores").methods("POST"_method)
    ([](const crow::request &req) {
        try
        {
            auto corpo = json::parse(req.body);
            auto trab = sistemaGlobal->cadastrarTrabalhador(
                corpo["nome"].get<std::string>(),
                corpo["telefone"].get<std::string>(),
                corpo["cpf"].get<std::string>()
            );
            return respostaSucesso(json{{"id", trab->getId()}});
        }
        catch (const std::exception &e)
        {
            return respostaErro(e.what(), 400);
        }
    });

    // ===== GET /api/contratos =====
    CROW_ROUTE(app, "/api/contratos")
    ([]() {
        return respostaSucesso(sistemaGlobal->obterTodosContratosJson());
    });

    // ===== GET /api/contratos/<int> =====
    CROW_ROUTE(app, "/api/contratos/<int>")
    ([](int id) {
        auto contrato = sistemaGlobal->obterContratoJson(id);
        if (contrato.is_null())
        {
            return respostaErro("Contrato nao encontrado", 404);
        }
        return respostaSucesso(contrato);
    });

    // ===== POST /api/contratos =====
    CROW_ROUTE(app, "/api/contratos").methods("POST"_method)
    ([](const crow::request &req) {
        try
        {
            auto corpo = json::parse(req.body);
            Data data(
                corpo["data"]["dia"].get<int>(),
                corpo["data"]["mes"].get<int>(),
                corpo["data"]["ano"].get<int>()
            );
            sistemaGlobal->criarContrato(
                corpo["clienteId"].get<int>(),
                corpo["trabalhadorId"].get<int>(),
                corpo["habilidade"].get<std::string>(),
                data,
                corpo["horas"].get<int>(),
                corpo["descricao"].get<std::string>()
            );
            return respostaSucesso(json{{"mensagem", "Contrato criado com sucesso"}});
        }
        catch (const std::exception &e)
        {
            return respostaErro(e.what(), 400);
        }
    });

    // ===== GET /api/trabalhadores/disponiveis =====
    CROW_ROUTE(app, "/api/trabalhadores/disponiveis")
    ([](const crow::request &req) {
        try
        {
            int dia = std::stoi(req.url_params.get("dia"));
            int mes = std::stoi(req.url_params.get("mes"));
            int ano = std::stoi(req.url_params.get("ano"));
            return respostaSucesso(sistemaGlobal->obterTrabalhadoresDisponiveisJson(dia, mes, ano));
        }
        catch (const std::exception &e)
        {
            return respostaErro(e.what(), 400);
        }
    });

    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║  SISTEMA MARIDO DE ALUGUEL - API REST  ║\n";
    std::cout << "║         Powered by Crow Framework      ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    std::cout << "\n[✓] Servidor iniciado na porta 8080\n";
    std::cout << "[✓] URL: http://localhost:8080\n";


    app.port(8080).multithreaded().run();

    return 0;
}

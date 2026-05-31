#pragma once

#include "domain/model/Cliente.hpp"
#include "domain/model/Contrato.hpp"
#include "domain/model/Data.hpp"
#include "domain/model/Trabalhador.hpp"
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace infrastructure::storage {

struct DatabaseState {
    int nextClienteId{1};
    int nextTrabalhadorId{1};
    int nextContratoId{1};
    std::vector<domain::model::Cliente> clientes;
    std::vector<domain::model::Trabalhador> trabalhadores;
    std::vector<domain::model::Contrato> contratos;
};

class TextDatabase {
private:
    std::string caminhoArquivo_;
    mutable std::mutex mutex_;

    DatabaseState carregarUnsafe() const;
    void salvarUnsafe(const DatabaseState& state) const;
    void criarArquivoInicialSeNecessario() const;

public:
    explicit TextDatabase(std::string caminhoArquivo);

    const std::string& caminhoArquivo() const;

    std::vector<domain::model::Cliente> listarClientes() const;
    std::optional<domain::model::Cliente> buscarClientePorId(int id) const;
    domain::model::Cliente salvarCliente(const domain::model::Cliente& cliente);
    bool atualizarCliente(const domain::model::Cliente& cliente);
    bool removerCliente(int id);

    std::vector<domain::model::Trabalhador> listarTrabalhadores() const;
    std::optional<domain::model::Trabalhador> buscarTrabalhadorPorId(int id) const;
    domain::model::Trabalhador salvarTrabalhador(const domain::model::Trabalhador& trabalhador);
    bool atualizarTrabalhador(const domain::model::Trabalhador& trabalhador);
    bool removerTrabalhador(int id);

    std::vector<domain::model::Contrato> listarContratos() const;
    std::optional<domain::model::Contrato> buscarContratoPorId(int id) const;
    domain::model::Contrato salvarContrato(const domain::model::Contrato& contrato);
    bool atualizarContrato(const domain::model::Contrato& contrato);
    bool removerContrato(int id);
    bool existeContratoConfirmadoNaData(int trabalhadorId, const domain::model::Data& data, int contratoIgnoradoId = 0) const;
};

}

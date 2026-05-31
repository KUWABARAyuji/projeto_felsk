#pragma once

#include "domain/model/Contrato.hpp"
#include "domain/model/Data.hpp"
#include <optional>
#include <vector>

namespace domain::repository {

class IContratoRepository {
public:
    virtual ~IContratoRepository() = default;
    virtual std::vector<domain::model::Contrato> listar() const = 0;
    virtual std::optional<domain::model::Contrato> buscarPorId(int id) const = 0;
    virtual domain::model::Contrato salvar(const domain::model::Contrato& contrato) = 0;
    virtual bool atualizar(const domain::model::Contrato& contrato) = 0;
    virtual bool remover(int id) = 0;
    virtual bool existeConfirmadoNaData(int trabalhadorId, const domain::model::Data& data, int contratoIgnoradoId = 0) const = 0;
};

}

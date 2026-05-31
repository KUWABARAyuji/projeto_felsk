#pragma once

#include "domain/repository/IContratoRepository.hpp"
#include "infrastructure/storage/TextDatabase.hpp"
#include <memory>

namespace infrastructure::repository {

class TxtContratoRepository : public domain::repository::IContratoRepository {
private:
    std::shared_ptr<storage::TextDatabase> database_;

public:
    explicit TxtContratoRepository(std::shared_ptr<storage::TextDatabase> database);

    std::vector<domain::model::Contrato> listar() const override;
    std::optional<domain::model::Contrato> buscarPorId(int id) const override;
    domain::model::Contrato salvar(const domain::model::Contrato& contrato) override;
    bool atualizar(const domain::model::Contrato& contrato) override;
    bool remover(int id) override;
    bool existeConfirmadoNaData(int trabalhadorId, const domain::model::Data& data, int contratoIgnoradoId = 0) const override;
};

}

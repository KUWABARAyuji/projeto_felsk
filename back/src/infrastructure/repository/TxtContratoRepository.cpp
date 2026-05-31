#include "infrastructure/repository/TxtContratoRepository.hpp"

namespace infrastructure::repository {

TxtContratoRepository::TxtContratoRepository(std::shared_ptr<storage::TextDatabase> database) : database_(std::move(database)) {}
std::vector<domain::model::Contrato> TxtContratoRepository::listar() const { return database_->listarContratos(); }
std::optional<domain::model::Contrato> TxtContratoRepository::buscarPorId(int id) const { return database_->buscarContratoPorId(id); }
domain::model::Contrato TxtContratoRepository::salvar(const domain::model::Contrato& contrato) { return database_->salvarContrato(contrato); }
bool TxtContratoRepository::atualizar(const domain::model::Contrato& contrato) { return database_->atualizarContrato(contrato); }
bool TxtContratoRepository::remover(int id) { return database_->removerContrato(id); }
bool TxtContratoRepository::existeConfirmadoNaData(int trabalhadorId, const domain::model::Data& data, int contratoIgnoradoId) const {
    return database_->existeContratoConfirmadoNaData(trabalhadorId, data, contratoIgnoradoId);
}

}

#include "infrastructure/repository/TxtTrabalhadorRepository.hpp"

namespace infrastructure::repository {

TxtTrabalhadorRepository::TxtTrabalhadorRepository(std::shared_ptr<storage::TextDatabase> database) : database_(std::move(database)) {}
std::vector<domain::model::Trabalhador> TxtTrabalhadorRepository::listar() const { return database_->listarTrabalhadores(); }
std::optional<domain::model::Trabalhador> TxtTrabalhadorRepository::buscarPorId(int id) const { return database_->buscarTrabalhadorPorId(id); }
domain::model::Trabalhador TxtTrabalhadorRepository::salvar(const domain::model::Trabalhador& trabalhador) { return database_->salvarTrabalhador(trabalhador); }
bool TxtTrabalhadorRepository::atualizar(const domain::model::Trabalhador& trabalhador) { return database_->atualizarTrabalhador(trabalhador); }
bool TxtTrabalhadorRepository::remover(int id) { return database_->removerTrabalhador(id); }

}

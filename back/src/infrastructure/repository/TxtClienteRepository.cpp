#include "infrastructure/repository/TxtClienteRepository.hpp"

namespace infrastructure::repository {

TxtClienteRepository::TxtClienteRepository(std::shared_ptr<storage::TextDatabase> database) : database_(std::move(database)) {}
std::vector<domain::model::Cliente> TxtClienteRepository::listar() const { return database_->listarClientes(); }
std::optional<domain::model::Cliente> TxtClienteRepository::buscarPorId(int id) const { return database_->buscarClientePorId(id); }
domain::model::Cliente TxtClienteRepository::salvar(const domain::model::Cliente& cliente) { return database_->salvarCliente(cliente); }
bool TxtClienteRepository::atualizar(const domain::model::Cliente& cliente) { return database_->atualizarCliente(cliente); }
bool TxtClienteRepository::remover(int id) { return database_->removerCliente(id); }

}

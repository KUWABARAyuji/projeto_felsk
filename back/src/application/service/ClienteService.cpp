#include "application/service/ClienteService.hpp"
#include "application/dto/DtoMapper.hpp"
#include <stdexcept>

namespace application::service {

ClienteService::ClienteService(std::shared_ptr<domain::repository::IClienteRepository> clienteRepository)
    : clienteRepository_(std::move(clienteRepository)) {}

std::vector<dto::ClienteResponse> ClienteService::listar() const {
    std::vector<dto::ClienteResponse> response;
    for (const auto& cliente : clienteRepository_->listar()) response.push_back(dto::toClienteResponse(cliente));
    return response;
}

std::optional<dto::ClienteResponse> ClienteService::buscarPorId(int id) const {
    auto cliente = clienteRepository_->buscarPorId(id);
    if (!cliente.has_value()) return std::nullopt;
    return dto::toClienteResponse(*cliente);
}

dto::ClienteResponse ClienteService::criar(const dto::CriarClienteRequest& request) {
    request.validar();
    domain::model::Cliente cliente(0, request.nome, request.telefone, request.cpf, request.endereco);
    return dto::toClienteResponse(clienteRepository_->salvar(cliente));
}

std::optional<dto::ClienteResponse> ClienteService::atualizar(int id, const dto::AtualizarClienteRequest& request) {
    request.validar();
    auto atual = clienteRepository_->buscarPorId(id);
    if (!atual.has_value()) return std::nullopt;
    domain::model::Cliente cliente(id, request.nome, request.telefone, request.cpf, request.endereco);
    clienteRepository_->atualizar(cliente);
    return dto::toClienteResponse(cliente);
}

bool ClienteService::remover(int id) {
    return clienteRepository_->remover(id);
}

}

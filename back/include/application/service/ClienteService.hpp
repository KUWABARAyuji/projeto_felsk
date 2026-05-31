#pragma once

#include "application/dto/ClienteDtos.hpp"
#include "domain/repository/IClienteRepository.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace application::service {

class ClienteService {
private:
    std::shared_ptr<domain::repository::IClienteRepository> clienteRepository_;

public:
    explicit ClienteService(std::shared_ptr<domain::repository::IClienteRepository> clienteRepository);

    std::vector<dto::ClienteResponse> listar() const;
    std::optional<dto::ClienteResponse> buscarPorId(int id) const;
    dto::ClienteResponse criar(const dto::CriarClienteRequest& request);
    std::optional<dto::ClienteResponse> atualizar(int id, const dto::AtualizarClienteRequest& request);
    bool remover(int id);
};

}

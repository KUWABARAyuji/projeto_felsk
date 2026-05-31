#pragma once

#include "application/dto/ContratoDtos.hpp"
#include "domain/repository/IClienteRepository.hpp"
#include "domain/repository/IContratoRepository.hpp"
#include "domain/repository/ITrabalhadorRepository.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace application::service {

class ContratoService {
private:
    std::shared_ptr<domain::repository::IContratoRepository> contratoRepository_;
    std::shared_ptr<domain::repository::IClienteRepository> clienteRepository_;
    std::shared_ptr<domain::repository::ITrabalhadorRepository> trabalhadorRepository_;

public:
    ContratoService(std::shared_ptr<domain::repository::IContratoRepository> contratoRepository,
                    std::shared_ptr<domain::repository::IClienteRepository> clienteRepository,
                    std::shared_ptr<domain::repository::ITrabalhadorRepository> trabalhadorRepository);

    std::vector<dto::ContratoResponse> listar() const;
    std::optional<dto::ContratoResponse> buscarPorId(int id) const;
    dto::ContratoResponse criar(const dto::CriarContratoRequest& request);
    std::optional<dto::ContratoResponse> confirmar(int id);
    std::optional<dto::ContratoResponse> concluir(int id, const dto::ConcluirContratoRequest& request);
    std::optional<dto::ContratoResponse> cancelar(int id);
};

}

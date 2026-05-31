#pragma once

#include "application/dto/HabilidadeDtos.hpp"
#include "application/dto/TrabalhadorDtos.hpp"
#include "domain/model/Data.hpp"
#include "domain/repository/IContratoRepository.hpp"
#include "domain/repository/ITrabalhadorRepository.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace application::service {

class TrabalhadorService {
private:
    std::shared_ptr<domain::repository::ITrabalhadorRepository> trabalhadorRepository_;
    std::shared_ptr<domain::repository::IContratoRepository> contratoRepository_;

public:
    TrabalhadorService(std::shared_ptr<domain::repository::ITrabalhadorRepository> trabalhadorRepository,
                       std::shared_ptr<domain::repository::IContratoRepository> contratoRepository);

    std::vector<dto::TrabalhadorResponse> listar() const;
    std::optional<dto::TrabalhadorResponse> buscarPorId(int id) const;
    dto::TrabalhadorResponse criar(const dto::CriarTrabalhadorRequest& request);
    std::optional<dto::TrabalhadorResponse> atualizar(int id, const dto::AtualizarTrabalhadorRequest& request);
    bool remover(int id);
    std::optional<dto::TrabalhadorResponse> adicionarHabilidade(int trabalhadorId, const dto::CriarHabilidadeRequest& request);
    std::vector<dto::TrabalhadorResponse> listarDisponiveis(const domain::model::Data& data) const;
};

}

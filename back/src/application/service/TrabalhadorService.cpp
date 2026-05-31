#include "application/service/TrabalhadorService.hpp"
#include "application/dto/DtoMapper.hpp"
#include <stdexcept>

namespace application::service {

TrabalhadorService::TrabalhadorService(std::shared_ptr<domain::repository::ITrabalhadorRepository> trabalhadorRepository,
                                       std::shared_ptr<domain::repository::IContratoRepository> contratoRepository)
    : trabalhadorRepository_(std::move(trabalhadorRepository)), contratoRepository_(std::move(contratoRepository)) {}

std::vector<dto::TrabalhadorResponse> TrabalhadorService::listar() const {
    std::vector<dto::TrabalhadorResponse> response;
    for (const auto& trabalhador : trabalhadorRepository_->listar()) response.push_back(dto::toTrabalhadorResponse(trabalhador));
    return response;
}

std::optional<dto::TrabalhadorResponse> TrabalhadorService::buscarPorId(int id) const {
    auto trabalhador = trabalhadorRepository_->buscarPorId(id);
    if (!trabalhador.has_value()) return std::nullopt;
    return dto::toTrabalhadorResponse(*trabalhador);
}

dto::TrabalhadorResponse TrabalhadorService::criar(const dto::CriarTrabalhadorRequest& request) {
    request.validar();
    domain::model::Trabalhador trabalhador(0, request.nome, request.telefone, request.cpf);
    return dto::toTrabalhadorResponse(trabalhadorRepository_->salvar(trabalhador));
}

std::optional<dto::TrabalhadorResponse> TrabalhadorService::atualizar(int id, const dto::AtualizarTrabalhadorRequest& request) {
    request.validar();
    auto atual = trabalhadorRepository_->buscarPorId(id);
    if (!atual.has_value()) return std::nullopt;
    domain::model::Trabalhador trabalhador(id, request.nome, request.telefone, request.cpf,
                                           atual->avaliacaoMedia(), atual->totalAvaliacoes());
    for (const auto& habilidade : atual->habilidades()) trabalhador.adicionarHabilidade(habilidade);
    trabalhadorRepository_->atualizar(trabalhador);
    return dto::toTrabalhadorResponse(trabalhador);
}

bool TrabalhadorService::remover(int id) {
    return trabalhadorRepository_->remover(id);
}

std::optional<dto::TrabalhadorResponse> TrabalhadorService::adicionarHabilidade(int trabalhadorId, const dto::CriarHabilidadeRequest& request) {
    request.validar();
    auto trabalhador = trabalhadorRepository_->buscarPorId(trabalhadorId);
    if (!trabalhador.has_value()) return std::nullopt;
    trabalhador->adicionarHabilidade(domain::model::Habilidade(request.nome, request.descricao, request.valorHora));
    trabalhadorRepository_->atualizar(*trabalhador);
    return dto::toTrabalhadorResponse(*trabalhador);
}

std::vector<dto::TrabalhadorResponse> TrabalhadorService::listarDisponiveis(const domain::model::Data& data) const {
    std::vector<dto::TrabalhadorResponse> response;
    for (const auto& trabalhador : trabalhadorRepository_->listar()) {
        if (!contratoRepository_->existeConfirmadoNaData(trabalhador.id(), data)) {
            response.push_back(dto::toTrabalhadorResponse(trabalhador));
        }
    }
    return response;
}

}

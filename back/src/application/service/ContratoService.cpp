#include "application/service/ContratoService.hpp"
#include "application/dto/DtoMapper.hpp"
#include "domain/model/Data.hpp"
#include <stdexcept>

namespace application::service {

ContratoService::ContratoService(std::shared_ptr<domain::repository::IContratoRepository> contratoRepository,
                                 std::shared_ptr<domain::repository::IClienteRepository> clienteRepository,
                                 std::shared_ptr<domain::repository::ITrabalhadorRepository> trabalhadorRepository)
    : contratoRepository_(std::move(contratoRepository)), clienteRepository_(std::move(clienteRepository)),
      trabalhadorRepository_(std::move(trabalhadorRepository)) {}

std::vector<dto::ContratoResponse> ContratoService::listar() const {
    std::vector<dto::ContratoResponse> response;
    for (const auto& contrato : contratoRepository_->listar()) response.push_back(dto::toContratoResponse(contrato));
    return response;
}

std::optional<dto::ContratoResponse> ContratoService::buscarPorId(int id) const {
    auto contrato = contratoRepository_->buscarPorId(id);
    if (!contrato.has_value()) return std::nullopt;
    return dto::toContratoResponse(*contrato);
}

dto::ContratoResponse ContratoService::criar(const dto::CriarContratoRequest& request) {
    request.validar();
    auto cliente = clienteRepository_->buscarPorId(request.idCliente);
    if (!cliente.has_value()) throw std::runtime_error("Cliente nao encontrado.");

    auto trabalhador = trabalhadorRepository_->buscarPorId(request.idTrabalhador);
    if (!trabalhador.has_value()) throw std::runtime_error("Trabalhador nao encontrado.");

    auto habilidade = trabalhador->buscarHabilidade(request.habilidade);
    if (!habilidade.has_value()) throw std::runtime_error("Trabalhador nao possui a habilidade informada.");

    auto data = domain::model::Data::fromIsoString(request.data);
    if (contratoRepository_->existeConfirmadoNaData(trabalhador->id(), data)) {
        throw std::runtime_error("Trabalhador ja possui contrato confirmado nessa data.");
    }

    const double valorTotal = habilidade->valorHora() * request.horas;
    domain::model::Contrato contrato(0, cliente->id(), cliente->nome(), trabalhador->id(), trabalhador->nome(),
                                     habilidade->nome(), data, request.horas, valorTotal, request.descricao);
    return dto::toContratoResponse(contratoRepository_->salvar(contrato));
}

std::optional<dto::ContratoResponse> ContratoService::confirmar(int id) {
    auto contrato = contratoRepository_->buscarPorId(id);
    if (!contrato.has_value()) return std::nullopt;
    if (contratoRepository_->existeConfirmadoNaData(contrato->trabalhadorId(), contrato->data(), contrato->id())) {
        throw std::runtime_error("Trabalhador ja possui contrato confirmado nessa data.");
    }
    contrato->confirmar();
    contratoRepository_->atualizar(*contrato);
    return dto::toContratoResponse(*contrato);
}

std::optional<dto::ContratoResponse> ContratoService::concluir(int id, const dto::ConcluirContratoRequest& request) {
    request.validar();
    auto contrato = contratoRepository_->buscarPorId(id);
    if (!contrato.has_value()) return std::nullopt;

    auto trabalhador = trabalhadorRepository_->buscarPorId(contrato->trabalhadorId());
    if (!trabalhador.has_value()) throw std::runtime_error("Trabalhador do contrato nao encontrado.");

    contrato->concluir();
    trabalhador->receberAvaliacao(request.avaliacao);

    contratoRepository_->atualizar(*contrato);
    trabalhadorRepository_->atualizar(*trabalhador);
    return dto::toContratoResponse(*contrato);
}

std::optional<dto::ContratoResponse> ContratoService::cancelar(int id) {
    auto contrato = contratoRepository_->buscarPorId(id);
    if (!contrato.has_value()) return std::nullopt;
    contrato->cancelar();
    contratoRepository_->atualizar(*contrato);
    return dto::toContratoResponse(*contrato);
}

}

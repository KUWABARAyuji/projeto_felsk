#include "application/dto/DtoMapper.hpp"
#include "domain/model/StatusContrato.hpp"

namespace application::dto {

ClienteResponse toClienteResponse(const domain::model::Cliente& cliente) {
    return {cliente.id(), cliente.nome(), cliente.telefone(), cliente.cpf(), cliente.endereco()};
}

HabilidadeResponse toHabilidadeResponse(const domain::model::Habilidade& habilidade) {
    return {habilidade.nome(), habilidade.descricao(), habilidade.valorHora()};
}

TrabalhadorResponse toTrabalhadorResponse(const domain::model::Trabalhador& trabalhador) {
    TrabalhadorResponse response;
    response.id = trabalhador.id();
    response.nome = trabalhador.nome();
    response.telefone = trabalhador.telefone();
    response.cpf = trabalhador.cpf();
    response.avaliacaoMedia = trabalhador.avaliacaoMedia();
    response.totalAvaliacoes = trabalhador.totalAvaliacoes();
    for (const auto& habilidade : trabalhador.habilidades()) {
        response.habilidades.push_back(toHabilidadeResponse(habilidade));
    }
    return response;
}

ContratoResponse toContratoResponse(const domain::model::Contrato& contrato) {
    return {
        contrato.id(), contrato.clienteId(), contrato.clienteNome(),
        contrato.trabalhadorId(), contrato.trabalhadorNome(), contrato.habilidade(),
        contrato.data().toIsoString(), contrato.horas(), contrato.valorTotal(),
        contrato.descricao(), domain::model::statusContratoToString(contrato.status())
    };
}

}

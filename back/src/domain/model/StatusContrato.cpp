#include "domain/model/StatusContrato.hpp"
#include <stdexcept>

namespace domain::model {

std::string statusContratoToString(StatusContrato status) {
    switch (status) {
        case StatusContrato::PENDENTE: return "PENDENTE";
        case StatusContrato::CONFIRMADO: return "CONFIRMADO";
        case StatusContrato::CONCLUIDO: return "CONCLUIDO";
        case StatusContrato::CANCELADO: return "CANCELADO";
    }
    return "PENDENTE";
}

StatusContrato statusContratoFromString(const std::string& valor) {
    if (valor == "PENDENTE") return StatusContrato::PENDENTE;
    if (valor == "CONFIRMADO") return StatusContrato::CONFIRMADO;
    if (valor == "CONCLUIDO") return StatusContrato::CONCLUIDO;
    if (valor == "CANCELADO") return StatusContrato::CANCELADO;
    throw std::invalid_argument("Status de contrato invalido: " + valor);
}

}

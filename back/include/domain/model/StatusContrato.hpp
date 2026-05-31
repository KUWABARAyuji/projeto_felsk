#pragma once

#include <string>

namespace domain::model {

enum class StatusContrato {
    PENDENTE,
    CONFIRMADO,
    CONCLUIDO,
    CANCELADO
};

std::string statusContratoToString(StatusContrato status);
StatusContrato statusContratoFromString(const std::string& valor);

}

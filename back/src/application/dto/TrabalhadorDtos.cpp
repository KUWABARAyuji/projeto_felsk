#include "application/dto/TrabalhadorDtos.hpp"
#include <stdexcept>

namespace application::dto {

void CriarTrabalhadorRequest::validar() const {
    if (nome.empty()) throw std::invalid_argument("Nome do trabalhador e obrigatorio.");
}

void AtualizarTrabalhadorRequest::validar() const {
    if (nome.empty()) throw std::invalid_argument("Nome do trabalhador e obrigatorio.");
}

}

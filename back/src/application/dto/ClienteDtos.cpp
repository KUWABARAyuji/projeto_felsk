#include "application/dto/ClienteDtos.hpp"
#include <stdexcept>

namespace application::dto {

void CriarClienteRequest::validar() const {
    if (nome.empty()) throw std::invalid_argument("Nome do cliente e obrigatorio.");
}

void AtualizarClienteRequest::validar() const {
    if (nome.empty()) throw std::invalid_argument("Nome do cliente e obrigatorio.");
}

}

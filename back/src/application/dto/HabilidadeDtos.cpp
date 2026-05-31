#include "application/dto/HabilidadeDtos.hpp"
#include <stdexcept>

namespace application::dto {

void CriarHabilidadeRequest::validar() const {
    if (nome.empty()) throw std::invalid_argument("Nome da habilidade e obrigatorio.");
    if (valorHora <= 0) throw std::invalid_argument("Valor por hora deve ser maior que zero.");
}

}

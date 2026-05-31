#include "application/dto/ContratoDtos.hpp"
#include <stdexcept>

namespace application::dto {

void CriarContratoRequest::validar() const {
    if (idCliente <= 0) throw std::invalid_argument("Informe um cliente valido.");
    if (idTrabalhador <= 0) throw std::invalid_argument("Informe um trabalhador valido.");
    if (habilidade.empty()) throw std::invalid_argument("Informe a habilidade.");
    if (data.empty()) throw std::invalid_argument("Informe a data do servico.");
    if (horas <= 0) throw std::invalid_argument("Horas devem ser maiores que zero.");
}

void ConcluirContratoRequest::validar() const {
    if (avaliacao < 0.0 || avaliacao > 5.0) throw std::invalid_argument("Avaliacao deve ficar entre 0 e 5.");
}

}

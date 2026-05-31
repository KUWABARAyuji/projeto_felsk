#pragma once

#include "application/dto/HabilidadeDtos.hpp"
#include <string>
#include <vector>

namespace application::dto {

struct CriarTrabalhadorRequest {
    std::string nome;
    std::string telefone;
    std::string cpf;
    void validar() const;
};

struct AtualizarTrabalhadorRequest {
    std::string nome;
    std::string telefone;
    std::string cpf;
    void validar() const;
};

struct TrabalhadorResponse {
    int id{};
    std::string nome;
    std::string telefone;
    std::string cpf;
    double avaliacaoMedia{};
    int totalAvaliacoes{};
    std::vector<HabilidadeResponse> habilidades;
};

}

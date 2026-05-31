#pragma once

#include <string>

namespace application::dto {

struct CriarHabilidadeRequest {
    std::string nome;
    std::string descricao;
    double valorHora{};
    void validar() const;
};

struct HabilidadeResponse {
    std::string nome;
    std::string descricao;
    double valorHora{};
};

}

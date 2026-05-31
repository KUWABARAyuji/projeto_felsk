#pragma once

#include <string>

namespace application::dto {

struct CriarClienteRequest {
    std::string nome;
    std::string telefone;
    std::string cpf;
    std::string endereco;
    void validar() const;
};

struct AtualizarClienteRequest {
    std::string nome;
    std::string telefone;
    std::string cpf;
    std::string endereco;
    void validar() const;
};

struct ClienteResponse {
    int id{};
    std::string nome;
    std::string telefone;
    std::string cpf;
    std::string endereco;
};

}

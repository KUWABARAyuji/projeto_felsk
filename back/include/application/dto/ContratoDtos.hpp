#pragma once

#include <string>

namespace application::dto {

struct CriarContratoRequest {
    int idCliente{};
    int idTrabalhador{};
    std::string habilidade;
    std::string data; // AAAA-MM-DD recebido do input type="date" do Angular
    int horas{};
    std::string descricao;
    void validar() const;
};

struct ConcluirContratoRequest {
    double avaliacao{};
    void validar() const;
};

struct ContratoResponse {
    int id{};
    int clienteId{};
    std::string clienteNome;
    int trabalhadorId{};
    std::string trabalhadorNome;
    std::string habilidade;
    std::string data;
    int horas{};
    double valorTotal{};
    std::string descricao;
    std::string status;
};

}

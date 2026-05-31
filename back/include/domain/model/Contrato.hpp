#pragma once

#include "domain/model/Data.hpp"
#include "domain/model/StatusContrato.hpp"
#include <string>

namespace domain::model {

class Contrato {
private:
    int id_;
    int clienteId_;
    std::string clienteNome_;
    int trabalhadorId_;
    std::string trabalhadorNome_;
    std::string habilidade_;
    Data data_;
    int horas_;
    double valorTotal_;
    std::string descricao_;
    StatusContrato status_;

public:
    Contrato();
    Contrato(int id, int clienteId, std::string clienteNome, int trabalhadorId, std::string trabalhadorNome,
             std::string habilidade, Data data, int horas, double valorTotal, std::string descricao,
             StatusContrato status = StatusContrato::PENDENTE);

    int id() const;
    int clienteId() const;
    const std::string& clienteNome() const;
    int trabalhadorId() const;
    const std::string& trabalhadorNome() const;
    const std::string& habilidade() const;
    const Data& data() const;
    int horas() const;
    double valorTotal() const;
    const std::string& descricao() const;
    StatusContrato status() const;

    void confirmar();
    void concluir();
    void cancelar();
};

}

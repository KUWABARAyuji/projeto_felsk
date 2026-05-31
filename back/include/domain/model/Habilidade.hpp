#pragma once

#include <string>

namespace domain::model {

class Habilidade {
private:
    std::string nome_;
    std::string descricao_;
    double valorHora_;

public:
    Habilidade();
    Habilidade(std::string nome, std::string descricao, double valorHora);

    const std::string& nome() const;
    const std::string& descricao() const;
    double valorHora() const;

    void alterarDescricao(const std::string& descricao);
    void alterarValorHora(double valorHora);
};

}

#pragma once

#include "domain/model/Pessoa.hpp"
#include "domain/model/Habilidade.hpp"
#include <optional>
#include <string>
#include <vector>

namespace domain::model {

class Trabalhador : public Pessoa {
private:
    std::vector<Habilidade> habilidades_;
    double avaliacaoMedia_;
    int totalAvaliacoes_;

public:
    Trabalhador();
    Trabalhador(int id, std::string nome, std::string telefone, std::string cpf,
                double avaliacaoMedia = 0.0, int totalAvaliacoes = 0);

    const std::vector<Habilidade>& habilidades() const;
    double avaliacaoMedia() const;
    int totalAvaliacoes() const;

    void adicionarHabilidade(const Habilidade& habilidade);
    bool possuiHabilidade(const std::string& nomeHabilidade) const;
    std::optional<Habilidade> buscarHabilidade(const std::string& nomeHabilidade) const;
    void receberAvaliacao(double nota);
};

}

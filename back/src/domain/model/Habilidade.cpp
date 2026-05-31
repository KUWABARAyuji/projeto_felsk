#include "domain/model/Habilidade.hpp"
#include <stdexcept>
#include <utility>

namespace domain::model {

Habilidade::Habilidade() : valorHora_(0.0) {}

Habilidade::Habilidade(std::string nome, std::string descricao, double valorHora)
    : nome_(std::move(nome)), descricao_(std::move(descricao)), valorHora_(valorHora) {
    if (nome_.empty()) throw std::invalid_argument("Nome da habilidade e obrigatorio.");
    if (valorHora_ <= 0) throw std::invalid_argument("Valor por hora deve ser maior que zero.");
}

const std::string& Habilidade::nome() const { return nome_; }
const std::string& Habilidade::descricao() const { return descricao_; }
double Habilidade::valorHora() const { return valorHora_; }

void Habilidade::alterarDescricao(const std::string& descricao) { descricao_ = descricao; }

void Habilidade::alterarValorHora(double valorHora) {
    if (valorHora <= 0) throw std::invalid_argument("Valor por hora deve ser maior que zero.");
    valorHora_ = valorHora;
}

}

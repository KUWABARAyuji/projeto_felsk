#include "domain/model/Trabalhador.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>

namespace domain::model {

Trabalhador::Trabalhador() : avaliacaoMedia_(0.0), totalAvaliacoes_(0) {}

Trabalhador::Trabalhador(int id, std::string nome, std::string telefone, std::string cpf,
                         double avaliacaoMedia, int totalAvaliacoes)
    : Pessoa(id, std::move(nome), std::move(telefone), std::move(cpf)),
      avaliacaoMedia_(avaliacaoMedia), totalAvaliacoes_(totalAvaliacoes) {
    if (avaliacaoMedia_ < 0.0 || avaliacaoMedia_ > 5.0) throw std::invalid_argument("Avaliacao media invalida.");
    if (totalAvaliacoes_ < 0) throw std::invalid_argument("Total de avaliacoes invalido.");
}

const std::vector<Habilidade>& Trabalhador::habilidades() const { return habilidades_; }
double Trabalhador::avaliacaoMedia() const { return avaliacaoMedia_; }
int Trabalhador::totalAvaliacoes() const { return totalAvaliacoes_; }

void Trabalhador::adicionarHabilidade(const Habilidade& habilidade) {
    if (possuiHabilidade(habilidade.nome())) {
        throw std::invalid_argument("Trabalhador ja possui essa habilidade.");
    }
    habilidades_.push_back(habilidade);
}

bool Trabalhador::possuiHabilidade(const std::string& nomeHabilidade) const {
    return std::any_of(habilidades_.begin(), habilidades_.end(), [&](const Habilidade& h) {
        return h.nome() == nomeHabilidade;
    });
}

std::optional<Habilidade> Trabalhador::buscarHabilidade(const std::string& nomeHabilidade) const {
    for (const auto& habilidade : habilidades_) {
        if (habilidade.nome() == nomeHabilidade) return habilidade;
    }
    return std::nullopt;
}

void Trabalhador::receberAvaliacao(double nota) {
    if (nota < 0.0 || nota > 5.0) throw std::invalid_argument("Avaliacao deve ficar entre 0 e 5.");
    avaliacaoMedia_ = ((avaliacaoMedia_ * totalAvaliacoes_) + nota) / (totalAvaliacoes_ + 1);
    totalAvaliacoes_++;
}

}

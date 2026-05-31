#include "domain/model/Pessoa.hpp"
#include <stdexcept>
#include <utility>

namespace domain::model {

Pessoa::Pessoa() : id_(0) {}

Pessoa::Pessoa(int id, std::string nome, std::string telefone, std::string cpf)
    : id_(id), nome_(std::move(nome)), telefone_(std::move(telefone)), cpf_(std::move(cpf)) {
    if (id_ < 0) throw std::invalid_argument("ID nao pode ser negativo.");
    if (nome_.empty()) throw std::invalid_argument("Nome e obrigatorio.");
}

int Pessoa::id() const { return id_; }
const std::string& Pessoa::nome() const { return nome_; }
const std::string& Pessoa::telefone() const { return telefone_; }
const std::string& Pessoa::cpf() const { return cpf_; }

void Pessoa::alterarNome(const std::string& nome) {
    if (nome.empty()) throw std::invalid_argument("Nome e obrigatorio.");
    nome_ = nome;
}

void Pessoa::alterarTelefone(const std::string& telefone) { telefone_ = telefone; }
void Pessoa::alterarCpf(const std::string& cpf) { cpf_ = cpf; }

}

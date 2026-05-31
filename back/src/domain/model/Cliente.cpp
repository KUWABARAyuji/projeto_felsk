#include "domain/model/Cliente.hpp"
#include <utility>

namespace domain::model {

Cliente::Cliente() = default;

Cliente::Cliente(int id, std::string nome, std::string telefone, std::string cpf, std::string endereco)
    : Pessoa(id, std::move(nome), std::move(telefone), std::move(cpf)), endereco_(std::move(endereco)) {}

const std::string& Cliente::endereco() const { return endereco_; }
void Cliente::alterarEndereco(const std::string& endereco) { endereco_ = endereco; }

}

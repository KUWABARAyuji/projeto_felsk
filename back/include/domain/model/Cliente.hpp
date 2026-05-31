#pragma once

#include "domain/model/Pessoa.hpp"
#include <string>

namespace domain::model {

class Cliente : public Pessoa {
private:
    std::string endereco_;

public:
    Cliente();
    Cliente(int id, std::string nome, std::string telefone, std::string cpf, std::string endereco);

    const std::string& endereco() const;
    void alterarEndereco(const std::string& endereco);
};

}

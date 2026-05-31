#pragma once

#include <string>

namespace domain::model {

class Pessoa {
private:
    int id_;
    std::string nome_;
    std::string telefone_;
    std::string cpf_;

public:
    Pessoa();
    Pessoa(int id, std::string nome, std::string telefone, std::string cpf);
    virtual ~Pessoa() = default;

    int id() const;
    const std::string& nome() const;
    const std::string& telefone() const;
    const std::string& cpf() const;

    void alterarNome(const std::string& nome);
    void alterarTelefone(const std::string& telefone);
    void alterarCpf(const std::string& cpf);
};

}

#pragma once

#include "domain/model/Trabalhador.hpp"
#include <optional>
#include <vector>

namespace domain::repository {

class ITrabalhadorRepository {
public:
    virtual ~ITrabalhadorRepository() = default;
    virtual std::vector<domain::model::Trabalhador> listar() const = 0;
    virtual std::optional<domain::model::Trabalhador> buscarPorId(int id) const = 0;
    virtual domain::model::Trabalhador salvar(const domain::model::Trabalhador& trabalhador) = 0;
    virtual bool atualizar(const domain::model::Trabalhador& trabalhador) = 0;
    virtual bool remover(int id) = 0;
};

}

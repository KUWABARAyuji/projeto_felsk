#pragma once

#include "domain/model/Cliente.hpp"
#include <optional>
#include <vector>

namespace domain::repository {

class IClienteRepository {
public:
    virtual ~IClienteRepository() = default;
    virtual std::vector<domain::model::Cliente> listar() const = 0;
    virtual std::optional<domain::model::Cliente> buscarPorId(int id) const = 0;
    virtual domain::model::Cliente salvar(const domain::model::Cliente& cliente) = 0;
    virtual bool atualizar(const domain::model::Cliente& cliente) = 0;
    virtual bool remover(int id) = 0;
};

}

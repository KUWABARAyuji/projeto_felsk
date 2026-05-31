#pragma once

#include "domain/repository/IClienteRepository.hpp"
#include "infrastructure/storage/TextDatabase.hpp"
#include <memory>

namespace infrastructure::repository {

class TxtClienteRepository : public domain::repository::IClienteRepository {
private:
    std::shared_ptr<storage::TextDatabase> database_;

public:
    explicit TxtClienteRepository(std::shared_ptr<storage::TextDatabase> database);

    std::vector<domain::model::Cliente> listar() const override;
    std::optional<domain::model::Cliente> buscarPorId(int id) const override;
    domain::model::Cliente salvar(const domain::model::Cliente& cliente) override;
    bool atualizar(const domain::model::Cliente& cliente) override;
    bool remover(int id) override;
};

}

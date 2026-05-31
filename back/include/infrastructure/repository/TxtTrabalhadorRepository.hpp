#pragma once

#include "domain/repository/ITrabalhadorRepository.hpp"
#include "infrastructure/storage/TextDatabase.hpp"
#include <memory>

namespace infrastructure::repository {

class TxtTrabalhadorRepository : public domain::repository::ITrabalhadorRepository {
private:
    std::shared_ptr<storage::TextDatabase> database_;

public:
    explicit TxtTrabalhadorRepository(std::shared_ptr<storage::TextDatabase> database);

    std::vector<domain::model::Trabalhador> listar() const override;
    std::optional<domain::model::Trabalhador> buscarPorId(int id) const override;
    domain::model::Trabalhador salvar(const domain::model::Trabalhador& trabalhador) override;
    bool atualizar(const domain::model::Trabalhador& trabalhador) override;
    bool remover(int id) override;
};

}

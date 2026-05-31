#pragma once

#include "application/dto/ClienteDtos.hpp"
#include "application/dto/ContratoDtos.hpp"
#include "application/dto/TrabalhadorDtos.hpp"
#include "domain/model/Cliente.hpp"
#include "domain/model/Contrato.hpp"
#include "domain/model/Habilidade.hpp"
#include "domain/model/Trabalhador.hpp"

namespace application::dto {

ClienteResponse toClienteResponse(const domain::model::Cliente& cliente);
HabilidadeResponse toHabilidadeResponse(const domain::model::Habilidade& habilidade);
TrabalhadorResponse toTrabalhadorResponse(const domain::model::Trabalhador& trabalhador);
ContratoResponse toContratoResponse(const domain::model::Contrato& contrato);

}

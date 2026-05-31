#pragma once

#include "application/dto/ClienteDtos.hpp"
#include "application/dto/ContratoDtos.hpp"
#include "application/dto/HabilidadeDtos.hpp"
#include "application/dto/TrabalhadorDtos.hpp"
#include <optional>
#include <string>
#include <vector>

namespace presentation::http::json {

std::string escape(const std::string& value);
std::string objectError(const std::string& message);
std::string objectHealth();

std::optional<std::string> getString(const std::string& body, const std::string& key);
std::optional<int> getInt(const std::string& body, const std::string& key);
std::optional<double> getDouble(const std::string& body, const std::string& key);

application::dto::CriarClienteRequest parseCriarCliente(const std::string& body);
application::dto::AtualizarClienteRequest parseAtualizarCliente(const std::string& body);
application::dto::CriarTrabalhadorRequest parseCriarTrabalhador(const std::string& body);
application::dto::AtualizarTrabalhadorRequest parseAtualizarTrabalhador(const std::string& body);
application::dto::CriarHabilidadeRequest parseCriarHabilidade(const std::string& body);
application::dto::CriarContratoRequest parseCriarContrato(const std::string& body);
application::dto::ConcluirContratoRequest parseConcluirContrato(const std::string& body);

std::string toJson(const application::dto::ClienteResponse& response);
std::string toJson(const std::vector<application::dto::ClienteResponse>& response);
std::string toJson(const application::dto::HabilidadeResponse& response);
std::string toJson(const application::dto::TrabalhadorResponse& response);
std::string toJson(const std::vector<application::dto::TrabalhadorResponse>& response);
std::string toJson(const application::dto::ContratoResponse& response);
std::string toJson(const std::vector<application::dto::ContratoResponse>& response);

}

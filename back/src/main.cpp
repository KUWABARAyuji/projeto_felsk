#include "application/service/ClienteService.hpp"
#include "application/service/ContratoService.hpp"
#include "application/service/TrabalhadorService.hpp"
#include "infrastructure/repository/TxtClienteRepository.hpp"
#include "infrastructure/repository/TxtContratoRepository.hpp"
#include "infrastructure/repository/TxtTrabalhadorRepository.hpp"
#include "infrastructure/storage/TextDatabase.hpp"
#include "presentation/http/HttpServer.hpp"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

namespace {

int envIntOrDefault(const char* name, int defaultValue) {
    const char* value = std::getenv(name);
    if (value == nullptr || std::string(value).empty()) return defaultValue;
    return std::stoi(value);
}

std::string envStringOrDefault(const char* name, const std::string& defaultValue) {
    const char* value = std::getenv(name);
    if (value == nullptr || std::string(value).empty()) return defaultValue;
    return value;
}

}

int main() {
    try {
        const std::string caminhoBanco = envStringOrDefault("BANCO_TXT_CAMINHO", "data/banco_dados.txt");
        const int porta = envIntOrDefault("APP_PORT", 8080);

        auto database = std::make_shared<infrastructure::storage::TextDatabase>(caminhoBanco);

        auto clienteRepository = std::make_shared<infrastructure::repository::TxtClienteRepository>(database);
        auto trabalhadorRepository = std::make_shared<infrastructure::repository::TxtTrabalhadorRepository>(database);
        auto contratoRepository = std::make_shared<infrastructure::repository::TxtContratoRepository>(database);

        auto clienteService = std::make_shared<application::service::ClienteService>(clienteRepository);
        auto trabalhadorService = std::make_shared<application::service::TrabalhadorService>(trabalhadorRepository, contratoRepository);
        auto contratoService = std::make_shared<application::service::ContratoService>(contratoRepository, clienteRepository, trabalhadorRepository);

        std::cout << "Banco TXT: " << database->caminhoArquivo() << "\n";

        presentation::http::HttpServer server(clienteService, trabalhadorService, contratoService);
        server.iniciar("0.0.0.0", porta);
    } catch (const std::exception& e) {
        std::cerr << "Erro ao iniciar API: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

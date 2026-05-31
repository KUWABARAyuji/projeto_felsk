#pragma once

#include "application/service/ClienteService.hpp"
#include "application/service/ContratoService.hpp"
#include "application/service/TrabalhadorService.hpp"
#include "presentation/http/HttpTypes.hpp"
#include <memory>
#include <string>

namespace presentation::http {

class HttpServer {
private:
    std::shared_ptr<application::service::ClienteService> clienteService_;
    std::shared_ptr<application::service::TrabalhadorService> trabalhadorService_;
    std::shared_ptr<application::service::ContratoService> contratoService_;

    HttpResponse handle(const HttpRequest& request);
    HttpResponse handleClientes(const HttpRequest& request);
    HttpResponse handleTrabalhadores(const HttpRequest& request);
    HttpResponse handleContratos(const HttpRequest& request);

public:
    HttpServer(std::shared_ptr<application::service::ClienteService> clienteService,
               std::shared_ptr<application::service::TrabalhadorService> trabalhadorService,
               std::shared_ptr<application::service::ContratoService> contratoService);

    void iniciar(const std::string& host, int port);
};

}

#include "presentation/http/HttpServer.hpp"
#include "domain/model/Data.hpp"
#include "presentation/http/JsonUtil.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <thread>

#ifdef _WIN32
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    using SocketHandle = SOCKET;
    static constexpr SocketHandle INVALID_SOCKET_HANDLE = INVALID_SOCKET;
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
    using SocketHandle = int;
    static constexpr SocketHandle INVALID_SOCKET_HANDLE = -1;
#endif

namespace presentation::http {
namespace {

void closeSocket(SocketHandle socket) {
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}

std::string statusText(int status) {
    switch (status) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 500: return "Internal Server Error";
        default: return "OK";
    }
}

std::string urlDecode(const std::string& value) {
    std::string result;
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '%' && i + 2 < value.size()) {
            const std::string hex = value.substr(i + 1, 2);
            char* end = nullptr;
            long decoded = std::strtol(hex.c_str(), &end, 16);
            if (end != nullptr && *end == '\0') {
                result += static_cast<char>(decoded);
                i += 2;
            } else {
                result += value[i];
            }
        } else if (value[i] == '+') {
            result += ' ';
        } else {
            result += value[i];
        }
    }
    return result;
}

std::map<std::string, std::string> parseQuery(const std::string& queryString) {
    std::map<std::string, std::string> query;
    std::stringstream ss(queryString);
    std::string pair;
    while (std::getline(ss, pair, '&')) {
        auto pos = pair.find('=');
        if (pos == std::string::npos) query[urlDecode(pair)] = "";
        else query[urlDecode(pair.substr(0, pos))] = urlDecode(pair.substr(pos + 1));
    }
    return query;
}

HttpResponse jsonResponse(int status, std::string body) {
    HttpResponse response;
    response.status = status;
    response.body = std::move(body);
    return response;
}

HttpResponse errorResponse(int status, const std::string& message) {
    return jsonResponse(status, json::objectError(message));
}

std::string getQueryValue(const HttpRequest& request, const std::string& key) {
    auto it = request.query.find(key);
    if (it == request.query.end()) return "";
    return it->second;
}

std::optional<int> idFromPath(const std::string& path, const std::string& pattern) {
    std::smatch match;
    if (std::regex_match(path, match, std::regex(pattern)) && match.size() > 1) {
        return std::stoi(match[1]);
    }
    return std::nullopt;
}

std::string buildRawResponse(const HttpResponse& response) {
    std::ostringstream out;
    out << "HTTP/1.1 " << response.status << ' ' << statusText(response.status) << "\r\n";
    out << "Content-Type: " << response.contentType << "\r\n";
    out << "Access-Control-Allow-Origin: *\r\n";
    out << "Access-Control-Allow-Methods: GET, POST, PUT, PATCH, DELETE, OPTIONS\r\n";
    out << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
    out << "Connection: close\r\n";
    for (const auto& [key, value] : response.headers) out << key << ": " << value << "\r\n";
    out << "Content-Length: " << response.body.size() << "\r\n\r\n";
    out << response.body;
    return out.str();
}

HttpRequest parseRawRequest(const std::string& raw) {
    HttpRequest request;
    auto headerEnd = raw.find("\r\n\r\n");
    const std::string headersPart = raw.substr(0, headerEnd);
    request.body = headerEnd == std::string::npos ? "" : raw.substr(headerEnd + 4);

    std::stringstream ss(headersPart);
    std::string requestLine;
    std::getline(ss, requestLine);
    if (!requestLine.empty() && requestLine.back() == '\r') requestLine.pop_back();

    std::stringstream lineStream(requestLine);
    std::string target;
    lineStream >> request.method >> target;

    auto queryPos = target.find('?');
    if (queryPos == std::string::npos) {
        request.path = target;
    } else {
        request.path = target.substr(0, queryPos);
        request.queryString = target.substr(queryPos + 1);
        request.query = parseQuery(request.queryString);
    }

    std::string line;
    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        auto pos = line.find(':');
        if (pos == std::string::npos) continue;
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        while (!value.empty() && value.front() == ' ') value.erase(value.begin());
        std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        request.headers[key] = value;
    }

    return request;
}

size_t contentLengthFromHeaders(const std::string& rawHeaders) {
    std::string lower = rawHeaders;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    auto pos = lower.find("content-length:");
    if (pos == std::string::npos) return 0;
    pos += std::string("content-length:").size();
    while (pos < lower.size() && std::isspace(static_cast<unsigned char>(lower[pos]))) pos++;
    size_t end = pos;
    while (end < lower.size() && std::isdigit(static_cast<unsigned char>(lower[end]))) end++;
    return static_cast<size_t>(std::stoul(lower.substr(pos, end - pos)));
}

std::string receiveAll(SocketHandle client) {
    std::string raw;
    char buffer[4096];
    while (true) {
        int received = recv(client, buffer, sizeof(buffer), 0);
        if (received <= 0) break;
        raw.append(buffer, buffer + received);
        auto headerEnd = raw.find("\r\n\r\n");
        if (headerEnd != std::string::npos) {
            size_t contentLength = contentLengthFromHeaders(raw.substr(0, headerEnd + 4));
            if (raw.size() >= headerEnd + 4 + contentLength) break;
        }
    }
    return raw;
}

void sendAll(SocketHandle client, const std::string& data) {
    size_t sentTotal = 0;
    while (sentTotal < data.size()) {
        int sent = send(client, data.data() + sentTotal, static_cast<int>(data.size() - sentTotal), 0);
        if (sent <= 0) break;
        sentTotal += static_cast<size_t>(sent);
    }
}

} // namespace

HttpServer::HttpServer(std::shared_ptr<application::service::ClienteService> clienteService,
                       std::shared_ptr<application::service::TrabalhadorService> trabalhadorService,
                       std::shared_ptr<application::service::ContratoService> contratoService)
    : clienteService_(std::move(clienteService)), trabalhadorService_(std::move(trabalhadorService)),
      contratoService_(std::move(contratoService)) {}

HttpResponse HttpServer::handle(const HttpRequest& request) {
    try {
        if (request.method == "OPTIONS") {
            HttpResponse response;
            response.status = 204;
            response.body = "";
            return response;
        }

        if (request.path == "/health" || request.path == "/api/health") {
            return jsonResponse(200, json::objectHealth());
        }

        if (request.path.rfind("/api/", 0) == 0) {
            HttpRequest copy = request;
            copy.path = request.path.substr(4);
            return handle(copy);
        }

        if (request.path.rfind("/clientes", 0) == 0) return handleClientes(request);
        if (request.path.rfind("/trabalhadores", 0) == 0 || request.path.rfind("/profissionais", 0) == 0) return handleTrabalhadores(request);
        if (request.path.rfind("/contratos", 0) == 0 || request.path.rfind("/orcamentos", 0) == 0) return handleContratos(request);

        return errorResponse(404, "Rota nao encontrada.");
    } catch (const std::exception& e) {
        return errorResponse(400, e.what());
    }
}

HttpResponse HttpServer::handleClientes(const HttpRequest& request) {
    if (request.path == "/clientes" && request.method == "GET") {
        return jsonResponse(200, json::toJson(clienteService_->listar()));
    }

    if (request.path == "/clientes" && request.method == "POST") {
        auto dto = json::parseCriarCliente(request.body);
        return jsonResponse(201, json::toJson(clienteService_->criar(dto)));
    }

    if (auto id = idFromPath(request.path, R"(^/clientes/(\d+)$)")) {
        if (request.method == "GET") {
            auto cliente = clienteService_->buscarPorId(*id);
            if (!cliente.has_value()) return errorResponse(404, "Cliente nao encontrado.");
            return jsonResponse(200, json::toJson(*cliente));
        }
        if (request.method == "PUT" || request.method == "PATCH") {
            auto cliente = clienteService_->atualizar(*id, json::parseAtualizarCliente(request.body));
            if (!cliente.has_value()) return errorResponse(404, "Cliente nao encontrado.");
            return jsonResponse(200, json::toJson(*cliente));
        }
        if (request.method == "DELETE") {
            if (!clienteService_->remover(*id)) return errorResponse(404, "Cliente nao encontrado.");
            return jsonResponse(200, "{\"mensagem\":\"Cliente removido.\"}");
        }
    }

    return errorResponse(405, "Metodo ou rota de cliente nao permitida.");
}

HttpResponse HttpServer::handleTrabalhadores(const HttpRequest& request) {
    if ((request.path == "/trabalhadores" || request.path == "/profissionais") && request.method == "GET") {
        return jsonResponse(200, json::toJson(trabalhadorService_->listar()));
    }

    if (request.path == "/trabalhadores/disponiveis" && request.method == "GET") {
        domain::model::Data data;
        const std::string dataIso = getQueryValue(request, "data");
        if (!dataIso.empty()) {
            data = domain::model::Data::fromIsoString(dataIso);
        } else {
            const std::string dia = getQueryValue(request, "dia");
            const std::string mes = getQueryValue(request, "mes");
            const std::string ano = getQueryValue(request, "ano");
            if (dia.empty() || mes.empty() || ano.empty()) throw std::invalid_argument("Informe data=AAAA-MM-DD ou dia, mes e ano.");
            data = domain::model::Data(std::stoi(dia), std::stoi(mes), std::stoi(ano));
        }
        return jsonResponse(200, json::toJson(trabalhadorService_->listarDisponiveis(data)));
    }

    if (request.path == "/trabalhadores" && request.method == "POST") {
        auto dto = json::parseCriarTrabalhador(request.body);
        return jsonResponse(201, json::toJson(trabalhadorService_->criar(dto)));
    }

    if (auto id = idFromPath(request.path, R"(^/trabalhadores/(\d+)$)")) {
        if (request.method == "GET") {
            auto trabalhador = trabalhadorService_->buscarPorId(*id);
            if (!trabalhador.has_value()) return errorResponse(404, "Trabalhador nao encontrado.");
            return jsonResponse(200, json::toJson(*trabalhador));
        }
        if (request.method == "PUT" || request.method == "PATCH") {
            auto trabalhador = trabalhadorService_->atualizar(*id, json::parseAtualizarTrabalhador(request.body));
            if (!trabalhador.has_value()) return errorResponse(404, "Trabalhador nao encontrado.");
            return jsonResponse(200, json::toJson(*trabalhador));
        }
        if (request.method == "DELETE") {
            if (!trabalhadorService_->remover(*id)) return errorResponse(404, "Trabalhador nao encontrado.");
            return jsonResponse(200, "{\"mensagem\":\"Trabalhador removido.\"}");
        }
    }

    std::smatch match;
    if (std::regex_match(request.path, match, std::regex(R"(^/trabalhadores/(\d+)/habilidades$)")) && request.method == "POST") {
        int trabalhadorId = std::stoi(match[1]);
        auto trabalhador = trabalhadorService_->adicionarHabilidade(trabalhadorId, json::parseCriarHabilidade(request.body));
        if (!trabalhador.has_value()) return errorResponse(404, "Trabalhador nao encontrado.");
        return jsonResponse(201, json::toJson(*trabalhador));
    }

    return errorResponse(405, "Metodo ou rota de trabalhador nao permitida.");
}

HttpResponse HttpServer::handleContratos(const HttpRequest& request) {
    const bool isOrcamento = request.path == "/orcamentos";

    if (request.path == "/contratos" && request.method == "GET") {
        return jsonResponse(200, json::toJson(contratoService_->listar()));
    }

    if ((request.path == "/contratos" || isOrcamento) && request.method == "POST") {
        auto dto = json::parseCriarContrato(request.body);
        return jsonResponse(201, json::toJson(contratoService_->criar(dto)));
    }

    if (auto id = idFromPath(request.path, R"(^/contratos/(\d+)$)")) {
        if (request.method == "GET") {
            auto contrato = contratoService_->buscarPorId(*id);
            if (!contrato.has_value()) return errorResponse(404, "Contrato nao encontrado.");
            return jsonResponse(200, json::toJson(*contrato));
        }
    }

    std::smatch match;
    if (std::regex_match(request.path, match, std::regex(R"(^/contratos/(\d+)/(confirmar|concluir|cancelar)$)")) &&
        (request.method == "PATCH" || request.method == "PUT")) {
        int id = std::stoi(match[1]);
        std::string acao = match[2];
        std::optional<application::dto::ContratoResponse> contrato;
        if (acao == "confirmar") contrato = contratoService_->confirmar(id);
        else if (acao == "cancelar") contrato = contratoService_->cancelar(id);
        else if (acao == "concluir") contrato = contratoService_->concluir(id, json::parseConcluirContrato(request.body));

        if (!contrato.has_value()) return errorResponse(404, "Contrato nao encontrado.");
        return jsonResponse(200, json::toJson(*contrato));
    }

    return errorResponse(405, "Metodo ou rota de contrato nao permitida.");
}

void HttpServer::iniciar(const std::string& host, int port) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) throw std::runtime_error("Falha ao iniciar Winsock.");
#endif

    SocketHandle serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET_HANDLE) throw std::runtime_error("Nao foi possivel criar o socket do servidor.");

    int opt = 1;
#ifdef _WIN32
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
#else
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<uint16_t>(port));
    address.sin_addr.s_addr = (host == "127.0.0.1" || host == "localhost") ? inet_addr("127.0.0.1") : INADDR_ANY;

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        closeSocket(serverSocket);
        throw std::runtime_error("Nao foi possivel vincular a porta. Verifique se ela ja esta em uso.");
    }

    if (listen(serverSocket, 20) < 0) {
        closeSocket(serverSocket);
        throw std::runtime_error("Nao foi possivel iniciar a escuta do servidor.");
    }

    std::cout << "\n==========================================\n";
    std::cout << " Marido de Aluguel - API C++ sem Crow\n";
    std::cout << " DDD + DTOs + banco TXT + HTTP nativo\n";
    std::cout << "==========================================\n";
    std::cout << "Servidor: http://localhost:" << port << "\n";
    std::cout << "Health:   http://localhost:" << port << "/health\n";
    std::cout << "Banco TXT configurado e pronto para o front Angular.\n\n";

    while (true) {
        sockaddr_in clientAddr{};
#ifdef _WIN32
        int clientSize = sizeof(clientAddr);
#else
        socklen_t clientSize = sizeof(clientAddr);
#endif
        SocketHandle client = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientSize);
        if (client == INVALID_SOCKET_HANDLE) continue;

        std::thread([this, client]() {
            try {
                std::string raw = receiveAll(client);
                HttpResponse response;
                if (raw.empty()) {
                    response = errorResponse(400, "Requisicao vazia.");
                } else {
                    HttpRequest request = parseRawRequest(raw);
                    response = handle(request);
                }
                sendAll(client, buildRawResponse(response));
            } catch (const std::exception& e) {
                sendAll(client, buildRawResponse(errorResponse(500, e.what())));
            }
            closeSocket(client);
        }).detach();
    }
}

}

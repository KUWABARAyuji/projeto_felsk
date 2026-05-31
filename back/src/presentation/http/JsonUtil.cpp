#include "presentation/http/JsonUtil.hpp"
#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace presentation::http::json {
namespace {

std::string quote(const std::string& value) {
    return "\"" + escape(value) + "\"";
}

std::optional<size_t> findKey(const std::string& body, const std::string& key) {
    const std::string needle = "\"" + key + "\"";
    size_t pos = body.find(needle);
    if (pos == std::string::npos) return std::nullopt;
    pos = body.find(':', pos + needle.size());
    if (pos == std::string::npos) return std::nullopt;
    pos++;
    while (pos < body.size() && std::isspace(static_cast<unsigned char>(body[pos]))) pos++;
    return pos;
}

std::string requiredString(const std::string& body, const std::string& key) {
    auto value = getString(body, key);
    if (!value.has_value()) throw std::invalid_argument("Campo obrigatorio ausente: " + key);
    return *value;
}

int requiredInt(const std::string& body, const std::string& key) {
    auto value = getInt(body, key);
    if (!value.has_value()) throw std::invalid_argument("Campo obrigatorio ausente: " + key);
    return *value;
}

double requiredDouble(const std::string& body, const std::string& key) {
    auto value = getDouble(body, key);
    if (!value.has_value()) throw std::invalid_argument("Campo obrigatorio ausente: " + key);
    return *value;
}

std::string join(const std::vector<std::string>& items) {
    std::string out = "[";
    for (size_t i = 0; i < items.size(); ++i) {
        if (i > 0) out += ",";
        out += items[i];
    }
    out += "]";
    return out;
}

} // namespace

std::string escape(const std::string& value) {
    std::ostringstream out;
    for (unsigned char c : value) {
        switch (c) {
            case '"': out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\b': out << "\\b"; break;
            case '\f': out << "\\f"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default:
                if (c < 0x20) {
                    out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                } else {
                    out << c;
                }
        }
    }
    return out.str();
}

std::string objectError(const std::string& message) {
    return "{\"erro\":" + quote(message) + "}";
}

std::string objectHealth() {
    return "{\"status\":\"online\",\"servico\":\"Marido de Aluguel API C++\",\"banco\":\"TXT\"}";
}

std::optional<std::string> getString(const std::string& body, const std::string& key) {
    auto start = findKey(body, key);
    if (!start.has_value() || *start >= body.size() || body[*start] != '"') return std::nullopt;
    size_t pos = *start + 1;
    std::string value;
    bool escaped = false;
    for (; pos < body.size(); ++pos) {
        char c = body[pos];
        if (escaped) {
            switch (c) {
                case 'n': value += '\n'; break;
                case 'r': value += '\r'; break;
                case 't': value += '\t'; break;
                case 'b': value += '\b'; break;
                case 'f': value += '\f'; break;
                default: value += c; break;
            }
            escaped = false;
        } else if (c == '\\') {
            escaped = true;
        } else if (c == '"') {
            return value;
        } else {
            value += c;
        }
    }
    return std::nullopt;
}

std::optional<int> getInt(const std::string& body, const std::string& key) {
    auto start = findKey(body, key);
    if (!start.has_value()) return std::nullopt;
    size_t end = *start;
    while (end < body.size() && (std::isdigit(static_cast<unsigned char>(body[end])) || body[end] == '-')) end++;
    if (end == *start) return std::nullopt;
    return std::stoi(body.substr(*start, end - *start));
}

std::optional<double> getDouble(const std::string& body, const std::string& key) {
    auto start = findKey(body, key);
    if (!start.has_value()) return std::nullopt;
    size_t end = *start;
    while (end < body.size() && (std::isdigit(static_cast<unsigned char>(body[end])) || body[end] == '-' || body[end] == '+' || body[end] == '.' || body[end] == 'e' || body[end] == 'E')) end++;
    if (end == *start) return std::nullopt;
    return std::stod(body.substr(*start, end - *start));
}

application::dto::CriarClienteRequest parseCriarCliente(const std::string& body) {
    return {requiredString(body, "nome"), getString(body, "telefone").value_or(""), getString(body, "cpf").value_or(""), getString(body, "endereco").value_or("")};
}

application::dto::AtualizarClienteRequest parseAtualizarCliente(const std::string& body) {
    return {requiredString(body, "nome"), getString(body, "telefone").value_or(""), getString(body, "cpf").value_or(""), getString(body, "endereco").value_or("")};
}

application::dto::CriarTrabalhadorRequest parseCriarTrabalhador(const std::string& body) {
    return {requiredString(body, "nome"), getString(body, "telefone").value_or(""), getString(body, "cpf").value_or("")};
}

application::dto::AtualizarTrabalhadorRequest parseAtualizarTrabalhador(const std::string& body) {
    return {requiredString(body, "nome"), getString(body, "telefone").value_or(""), getString(body, "cpf").value_or("")};
}

application::dto::CriarHabilidadeRequest parseCriarHabilidade(const std::string& body) {
    return {requiredString(body, "nome"), getString(body, "descricao").value_or(""), requiredDouble(body, "valorHora")};
}

application::dto::CriarContratoRequest parseCriarContrato(const std::string& body) {
    const int idCliente = getInt(body, "idCliente").value_or(getInt(body, "clienteId").value_or(0));
    const int idTrabalhador = getInt(body, "idTrabalhador").value_or(getInt(body, "trabalhadorId").value_or(0));
    return {idCliente, idTrabalhador, requiredString(body, "habilidade"), requiredString(body, "data"), getInt(body, "horas").value_or(1), getString(body, "descricao").value_or("")};
}

application::dto::ConcluirContratoRequest parseConcluirContrato(const std::string& body) {
    return {getDouble(body, "avaliacao").value_or(5.0)};
}

std::string toJson(const application::dto::ClienteResponse& r) {
    return "{\"id\":" + std::to_string(r.id) + ",\"nome\":" + quote(r.nome) + ",\"telefone\":" + quote(r.telefone) + ",\"cpf\":" + quote(r.cpf) + ",\"endereco\":" + quote(r.endereco) + "}";
}

std::string toJson(const std::vector<application::dto::ClienteResponse>& response) {
    std::vector<std::string> items;
    for (const auto& r : response) items.push_back(toJson(r));
    return join(items);
}

std::string toJson(const application::dto::HabilidadeResponse& r) {
    std::ostringstream out;
    out << "{\"nome\":" << quote(r.nome) << ",\"descricao\":" << quote(r.descricao)
        << ",\"valorHora\":" << r.valorHora << "}";
    return out.str();
}

std::string toJson(const application::dto::TrabalhadorResponse& r) {
    std::vector<std::string> habilidades;
    for (const auto& h : r.habilidades) habilidades.push_back(toJson(h));
    std::ostringstream out;
    out << "{\"id\":" << r.id << ",\"nome\":" << quote(r.nome) << ",\"telefone\":" << quote(r.telefone)
        << ",\"cpf\":" << quote(r.cpf) << ",\"avaliacaoMedia\":" << r.avaliacaoMedia
        << ",\"totalAvaliacoes\":" << r.totalAvaliacoes << ",\"habilidades\":" << join(habilidades) << "}";
    return out.str();
}

std::string toJson(const std::vector<application::dto::TrabalhadorResponse>& response) {
    std::vector<std::string> items;
    for (const auto& r : response) items.push_back(toJson(r));
    return join(items);
}

std::string toJson(const application::dto::ContratoResponse& r) {
    std::ostringstream out;
    out << "{\"id\":" << r.id << ",\"clienteId\":" << r.clienteId << ",\"clienteNome\":" << quote(r.clienteNome)
        << ",\"trabalhadorId\":" << r.trabalhadorId << ",\"trabalhadorNome\":" << quote(r.trabalhadorNome)
        << ",\"habilidade\":" << quote(r.habilidade) << ",\"data\":" << quote(r.data)
        << ",\"horas\":" << r.horas << ",\"valorTotal\":" << r.valorTotal
        << ",\"descricao\":" << quote(r.descricao) << ",\"status\":" << quote(r.status) << "}";
    return out.str();
}

std::string toJson(const std::vector<application::dto::ContratoResponse>& response) {
    std::vector<std::string> items;
    for (const auto& r : response) items.push_back(toJson(r));
    return join(items);
}

}

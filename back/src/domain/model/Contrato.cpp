#include "domain/model/Contrato.hpp"
#include <stdexcept>
#include <utility>

namespace domain::model {

Contrato::Contrato()
    : id_(0), clienteId_(0), trabalhadorId_(0), horas_(0), valorTotal_(0.0), status_(StatusContrato::PENDENTE) {}

Contrato::Contrato(int id, int clienteId, std::string clienteNome, int trabalhadorId, std::string trabalhadorNome,
                   std::string habilidade, Data data, int horas, double valorTotal, std::string descricao,
                   StatusContrato status)
    : id_(id), clienteId_(clienteId), clienteNome_(std::move(clienteNome)), trabalhadorId_(trabalhadorId),
      trabalhadorNome_(std::move(trabalhadorNome)), habilidade_(std::move(habilidade)), data_(data),
      horas_(horas), valorTotal_(valorTotal), descricao_(std::move(descricao)), status_(status) {
    if (id_ < 0) throw std::invalid_argument("ID do contrato nao pode ser negativo.");
    if (clienteId_ <= 0) throw std::invalid_argument("Cliente do contrato e obrigatorio.");
    if (trabalhadorId_ <= 0) throw std::invalid_argument("Trabalhador do contrato e obrigatorio.");
    if (habilidade_.empty()) throw std::invalid_argument("Habilidade do contrato e obrigatoria.");
    if (horas_ <= 0) throw std::invalid_argument("Horas devem ser maiores que zero.");
    if (valorTotal_ < 0) throw std::invalid_argument("Valor total nao pode ser negativo.");
}

int Contrato::id() const { return id_; }
int Contrato::clienteId() const { return clienteId_; }
const std::string& Contrato::clienteNome() const { return clienteNome_; }
int Contrato::trabalhadorId() const { return trabalhadorId_; }
const std::string& Contrato::trabalhadorNome() const { return trabalhadorNome_; }
const std::string& Contrato::habilidade() const { return habilidade_; }
const Data& Contrato::data() const { return data_; }
int Contrato::horas() const { return horas_; }
double Contrato::valorTotal() const { return valorTotal_; }
const std::string& Contrato::descricao() const { return descricao_; }
StatusContrato Contrato::status() const { return status_; }

void Contrato::confirmar() {
    if (status_ != StatusContrato::PENDENTE) throw std::runtime_error("Somente contratos pendentes podem ser confirmados.");
    status_ = StatusContrato::CONFIRMADO;
}

void Contrato::concluir() {
    if (status_ != StatusContrato::CONFIRMADO) throw std::runtime_error("Somente contratos confirmados podem ser concluidos.");
    status_ = StatusContrato::CONCLUIDO;
}

void Contrato::cancelar() {
    if (status_ == StatusContrato::CONCLUIDO) throw std::runtime_error("Contrato concluido nao pode ser cancelado.");
    status_ = StatusContrato::CANCELADO;
}

}

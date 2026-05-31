#include "domain/model/Data.hpp"
#include <cstdio>
#include <stdexcept>

namespace domain::model {

bool Data::ehBissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

bool Data::ehValida(int dia, int mes, int ano) {
    if (ano < 1900 || ano > 9999 || mes < 1 || mes > 12 || dia < 1) return false;
    int diasMes[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if (ehBissexto(ano)) diasMes[2] = 29;
    return dia <= diasMes[mes];
}

Data::Data() : dia_(1), mes_(1), ano_(2000) {}

Data::Data(int dia, int mes, int ano) : dia_(dia), mes_(mes), ano_(ano) {
    if (!ehValida(dia, mes, ano)) throw std::invalid_argument("Data invalida.");
}

int Data::dia() const { return dia_; }
int Data::mes() const { return mes_; }
int Data::ano() const { return ano_; }

std::string Data::toIsoString() const {
    char buffer[11];
    std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", ano_, mes_, dia_);
    return buffer;
}

std::string Data::toBrString() const {
    char buffer[11];
    std::snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", dia_, mes_, ano_);
    return buffer;
}

Data Data::fromIsoString(const std::string& dataIso) {
    int ano = 0, mes = 0, dia = 0;
    if (std::sscanf(dataIso.c_str(), "%d-%d-%d", &ano, &mes, &dia) != 3) {
        throw std::invalid_argument("Data deve estar no formato AAAA-MM-DD.");
    }
    return Data(dia, mes, ano);
}

Data Data::fromBrString(const std::string& dataBr) {
    int dia = 0, mes = 0, ano = 0;
    if (std::sscanf(dataBr.c_str(), "%d/%d/%d", &dia, &mes, &ano) != 3) {
        throw std::invalid_argument("Data deve estar no formato DD/MM/AAAA.");
    }
    return Data(dia, mes, ano);
}

bool Data::operator==(const Data& outra) const {
    return dia_ == outra.dia_ && mes_ == outra.mes_ && ano_ == outra.ano_;
}

bool Data::operator<(const Data& outra) const {
    if (ano_ != outra.ano_) return ano_ < outra.ano_;
    if (mes_ != outra.mes_) return mes_ < outra.mes_;
    return dia_ < outra.dia_;
}

}

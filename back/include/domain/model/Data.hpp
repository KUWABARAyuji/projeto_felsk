#pragma once

#include <string>

namespace domain::model {

class Data {
private:
    int dia_;
    int mes_;
    int ano_;

    static bool ehBissexto(int ano);
    static bool ehValida(int dia, int mes, int ano);

public:
    Data();
    Data(int dia, int mes, int ano);

    int dia() const;
    int mes() const;
    int ano() const;

    std::string toIsoString() const;
    std::string toBrString() const;

    static Data fromIsoString(const std::string& dataIso);
    static Data fromBrString(const std::string& dataBr);

    bool operator==(const Data& outra) const;
    bool operator<(const Data& outra) const;
};

}

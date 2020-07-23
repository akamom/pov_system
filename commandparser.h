#pragma once
#include <iostream>
#include <Map>
#include "stringop.h"

class ParameterParser_c{
public:
    std::string baseCommand;
    std::string originalCommand;

    ParameterParser_c(std::string _cmd) : originalCommand(_cmd), baseCommand(""){
        m_parameters = std::map<std::string, std::string>();
        getParametersFromCommand();
    }

    ~ParameterParser_c(){}

    bool getParameter(std::string param_name, std::string &param_value, std::string alt_param_name = "") const;
private:
    std::map<std::string, std::string> m_parameters;

    const std::string splitSymbol = "=";
    void getParametersFromCommand();
};
#include "commandparser.h"

bool ParameterParser_c::getParameter(std::string param_name, std::string &param_value, std::string alt_param_name) const{
    if (m_parameters.empty())
        return false;
    std::map<std::string, std::string>::const_iterator it = m_parameters.find(param_name); //m_parameters.at(param_name);
    // meaning key non existent
    if (it == m_parameters.end()){
        it = m_parameters.find(alt_param_name);
        auto alt_param_empty = alt_param_name.empty();
        if (!alt_param_empty && it != m_parameters.end()){
            param_value = it->second;
            return true;
        }
        //std::cout << "parameter " << param_name << (alt_param_empty ? "" : " or alternatively ") << (alt_param_empty ? "" : alt_param_name) <<  " does not exist!\n";
        return false;
    }
    param_value = it->second;
    return true;
}

void ParameterParser_c::getParametersFromCommand(){
    auto rawSplit = stringop::split(this->originalCommand, " ");
    // not even baseCommand input
    #ifdef DEBUG
    for (auto e : rawSplit){
        std::cout << e << std::endl;
    }
    #endif
    if (rawSplit.size() < 2)
        return;
    // first 2 are mandatory commands
    // baseCommand is mandatory 2 commands
    this->baseCommand = (stringop::trimString(rawSplit[0]) + " " + stringop::trimString(rawSplit[1]));
    // if only basecommand exists
    if (rawSplit.size() <= 2)
        return;

    for(std::vector<std::string>::iterator it = rawSplit.begin()+2; it != rawSplit.end(); ++it){
        auto paramSplit = stringop::split(*it, this->splitSymbol, true);
        #ifdef DEBUG
        std::cout << "param: " << *it << std::endl;
        for (auto e : paramSplit){
            std::cout << "paramsplit " << e << std::endl;
        }
        #endif
        if (paramSplit.size() > 2)
            continue;
        auto name = stringop::trimString(paramSplit.at(0));
        std::string value;
        if (paramSplit.size() == 1)
            value = "NONE";
        else
            value = stringop::trimString(paramSplit.at(1));
        #ifdef DEBUG
        std::cout << "found Parameter: " << name << "='" << value << "'"<< std::endl;
        #endif
        m_parameters.insert({name, value});
    }
}
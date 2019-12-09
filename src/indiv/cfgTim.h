//
// Created by Tim on 28/09/2019.
//

#ifndef MACHINESENBEREKENBAARHEID_CFG_H
#define MACHINESENBEREKENBAARHEID_CFG_H


#include <vector>
#include <string>
#include <map>

class cfgTim {
    std::vector<std::string> variables;
    std::vector<std::string> terminals;
    std::map<std::string, std::vector<std::vector<std::string>>> products;
    std::string start;
public:
    cfgTim(std::string input);

    cfgTim();

    void to_string();

    bool isTerminal(std::string input);

    bool isVariable(std::string input);

    std::vector<std::vector<std::string>> getProducts(std::string input);

    bool containsElement(std::string input, std::string head);

    std::map<std::string, std::vector<std::vector<std::string>>> &getProducts();

    std::string &getStart();

    const std::vector<std::string> &getVariables() const;

    std::vector<std::string> &getTerminals() ;

};


#endif //MACHINESENBEREKENBAARHEID_CFG_H

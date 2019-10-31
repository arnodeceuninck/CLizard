//
// Created by arno on 28.10.19.
//

#ifndef MB_CFG_H
#define MB_CFG_H


#include <vector>
#include <string>

class Production {
public:
    Production(const std::string &fromP, const std::string &toP);

private:
    std::string fromP;
    std::string toP;
};

class CFG {
    std::vector<std::string> nonTerminalsV;
    std::vector<std::string> terminalsT;
    std::vector<Production *> productionsP;
    char startS;
};


#endif //MB_CFG_H

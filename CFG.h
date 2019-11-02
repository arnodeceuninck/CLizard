//
// Created by arno on 28.10.19.
//

#ifndef MB_CFG_H
#define MB_CFG_H


#include <vector>
#include <string>

class Production {
public:
    Production(const std::string &fromP, const std::vector<std::string> &toP);

private:
    std::string fromP;
    std::vector<std::string> toP;
};

class CFG {
public:
    CFG(const std::vector<std::string> &nonTerminalsV, const std::vector<char> &terminalsT,
        const std::vector<Production *> &productionsP, const std::string &startS);

private:
    std::vector<std::string> nonTerminalsV;
    std::vector<char> terminalsT;
    std::vector<Production *> productionsP;
    std::string startS;
};


#endif //MB_CFG_H

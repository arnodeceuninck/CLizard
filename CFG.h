//
// Created by arno on 28.10.19.
//

#ifndef MB_CFG_H
#define MB_CFG_H


#include <vector>
#include <string>
#include "rapidjson/pointer.h"

rapidjson::Value strJSON(std::string str, rapidjson::Document::AllocatorType& allocator);

class Production {
public:
    Production(const std::string &fromP, const std::vector<std::string> &toP);

    const std::string &getFromP() const;

    const std::vector<std::string> &getToP() const;

private:
    std::string fromP;
    std::vector<std::string> toP;
};

class CFG {
public:
    CFG(const std::vector<std::string> &nonTerminalsV, const std::vector<std::string> &terminalsT,
        const std::vector<Production *> &productionsP, const std::string &startS);

    void toJSON(std::string filename);

    CFG* toCNF();
private:
    std::vector<std::string> nonTerminalsV;
    std::vector<std::string> terminalsT;
    std::vector<Production *> productionsP;
    std::string startS;
    void eliminateUselessSymbols();
    void eliminateNonGeneratingSymbols();
    void eliminateNonReachableSymbols();
    bool expandGeneratingSymbols(std::vector<std::string>& generatingSymbols); // Returns whether something has been added
    bool expandReachableSymbols(std::vector<std::string>& reachableSymbols); // Returns whether something has been added
    bool reachableProduction(Production *&production, std::vector<std::string> reachableSymbols);
};


#endif //MB_CFG_H

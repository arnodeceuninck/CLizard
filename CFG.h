//
// Created by arno on 28.10.19.
//

#ifndef MB_CFG_H
#define MB_CFG_H


#include <vector>
#include <string>
#include "rapidjson/pointer.h"
#include "const.h"

rapidjson::Value strJSON(std::string str, rapidjson::Document::AllocatorType &allocator);

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

    void toCNF();
    bool toCYK(std::string strToEvaluate, std::string& htmlOutput);

private:
    std::vector<std::string> nonTerminalsV;
    std::vector<std::string> terminalsT;
    std::vector<Production *> productionsP;
    std::string startS;

    void eliminateUselessSymbols();

    void eliminateNonGeneratingSymbols();

    void eliminateNonReachableSymbols();

    bool
    expandGeneratingSymbols(std::vector<std::string> &generatingSymbols); // Returns whether something has been added
    bool expandReachableSymbols(std::vector<std::string> &reachableSymbols); // Returns whether something has been added
    bool reachableProduction(Production *&production, std::vector<std::string> reachableSymbols);

    void eliminateEpsilonProductions();

    bool expandNullableSymbols(std::vector<std::string> &nullableSymbols);

    bool
    consistOnlyOfNullableSymbols(const std::vector<std::string> &symbolList, std::vector<std::string> &nullableSymbols);

    void removeNullableSymbol(std::string symbol, Production *production);

    void eliminateUnitProductions();

    bool expandUnitPairs(std::vector<std::pair<std::string, std::string>>& unitPairs);

    std::vector<std::vector<std::string>> findProductionsForVariable(std::string productionFrom);

    bool productionExists(std::string productionFrom, std::vector<std::string> productionTo);

    void cleanUp();

    void fixOnlyVariablesProductions();

    void splitUpLongerBodies();

    std::string findNewUnusedVariableLetter();

    bool findVariable(std::string& var, std::vector<std::string> productionTo);

    std::vector<std::string> findVariables(std::vector<std::string> productionTo);


    std::vector<std::string>
    findProductionsInCYK(int startSubStr, int endSubStr, std::vector<std::vector<std::vector<std::string>>> &cykTable);

    std::vector<std::string> findAtoBCproductions(std::vector<std::string>& B, std::vector<std::string>& C);

    std::string rowHtmlString(std::vector<std::vector<std::string>> vector);
};


void readJson(std::string filename,
              std::vector<std::string> &nonTerminalsV,
              std::vector<std::string> &terminalsT,
              std::vector<Production *> &productionsP,
              std::string &startS);


#endif //MB_CFG_H

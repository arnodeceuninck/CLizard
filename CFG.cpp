//
// Created by arno on 28.10.19.
//

#include "CFG.h"
#include "functions.h"

#include <fstream>
#include <iostream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/istreamwrapper.h"

//#include "const.h"

Production::Production(const std::string &fromP, const std::vector<std::string> &toP) : fromP(fromP), toP(toP) {}

const std::string &Production::getFromP() const {
    return fromP;
}

const std::vector<std::string> &Production::getToP() const {
    return toP;
}

CFG::CFG(const std::vector<std::string> &nonTerminalsV, const std::vector<std::string> &terminalsT,
         const std::vector<Production *> &productionsP, const std::string &startS) {
    this->nonTerminalsV = nonTerminalsV;
    this->terminalsT = terminalsT;
    this->productionsP = productionsP;
    this->startS = startS;
}

void CFG::toJSON(std::string filename) {
    // document is the root of a json message
    rapidjson::Document document;

    // define the document as an object rather than an array
    document.SetObject();

    // must pass an allocator when the object may need to allocate memory
    rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

//    document.AddMember("type", "CFG", allocator);

    // create a rapidjson array type with similar syntax to std::vector
    // non-Terminals
    rapidjson::Value alphabetRJ(rapidjson::kArrayType);
    for (std::string character: nonTerminalsV) {
        alphabetRJ.PushBack(strJSON(character, allocator), allocator);
    }
    document.AddMember("Variables", alphabetRJ, allocator);

    // Terminals
    rapidjson::Value terminalsRJ(rapidjson::kArrayType);
    for (std::string character: terminalsT) {
        terminalsRJ.PushBack(strJSON(character, allocator), allocator);
    }
    document.AddMember("Terminals", terminalsRJ, allocator);


//    document.AddMember("eps", strJSON(string(1, epsilon), allocator), allocator);

    rapidjson::Value productionsRJ(rapidjson::kArrayType);
    for (Production *production: productionsP) {

        rapidjson::Value productionRJ(rapidjson::kObjectType);

        productionRJ.AddMember("head", strJSON(production->getFromP(), allocator), allocator);

        rapidjson::Value productionsToRJ(rapidjson::kArrayType);
        for (std::string productionElement: production->getToP()) {
            productionsToRJ.PushBack(strJSON(productionElement, allocator), allocator);
        }
        productionRJ.AddMember("body", productionsToRJ, allocator);
        productionsRJ.PushBack(productionRJ, allocator);
    }
    document.AddMember("Productions", productionsRJ, allocator);

    document.AddMember("Start", strJSON(startS, allocator), allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    document.Accept(writer);

    // Source: s.n.(2019). Input/output with files. Visited on March 30, 2019 via http://www.cplusplus.com/doc/tutorial/files/
    // Yup, alweer diezelfde bron als altijd, ooit zal ik die 4 lijntjes wel vanbuiten kennen

    std::ofstream myfile;
    myfile.open(filename);

    myfile << strbuf.GetString() << std::endl;

    myfile.close();

}

void CFG::toCNF() {
    cleanUp();
    fixOnlyVariablesProductions();
    splitUpLongerBodies();
}

void CFG::eliminateUselessSymbols() {
    eliminateNonGeneratingSymbols();
    eliminateNonReachableSymbols();
}

void CFG::eliminateNonGeneratingSymbols() {
    // Find all generating symbols (and thus also the nongenerating)
    std::vector<std::string> generatingSymbols = terminalsT;
    while (expandGeneratingSymbols(generatingSymbols)) {}

    // Remove all non-generating symbols from variables, (terminals) and productions
    for (int i = 0; i < nonTerminalsV.size(); ++i) {
        if (!inVector(nonTerminalsV[i], generatingSymbols)) {
            nonTerminalsV.erase(nonTerminalsV.begin() + i);
            --i;
        }
    }

    for (int j = 0; j < productionsP.size(); ++j) {
        if (!reachableProduction(productionsP[j], generatingSymbols)) {
            productionsP.erase(productionsP.begin() + j);
            --j;
        }
    }

}

void CFG::eliminateNonReachableSymbols() {
    // Find all reachable symbols (and thus also the nonreachable)
    std::vector<std::string> reachableSymbols = {startS};
    while (expandReachableSymbols(reachableSymbols)) {}

    // Remove all non-reachable symbols from variables, terminals and productions
    for (int i = 0; i < nonTerminalsV.size(); ++i) {
        if (!inVector(nonTerminalsV[i], reachableSymbols)) {
            nonTerminalsV.erase(nonTerminalsV.begin() + i);
            --i;
        }
    }

    for (int i = 0; i < terminalsT.size(); ++i) {
        if (!inVector(terminalsT[i], reachableSymbols)) {
            terminalsT.erase(terminalsT.begin() + i);
            --i;
        }
    }


    for (int j = 0; j < productionsP.size(); ++j) {
        if (!reachableProduction(productionsP[j], reachableSymbols)) {
            productionsP.erase(productionsP.begin() + j);
            --j;
        }
    }
}

bool CFG::expandGeneratingSymbols(std::vector<std::string> &generatingSymbols) {
    bool changed = false;
    for (Production *production: productionsP) {
        for (std::string productionTo: production->getToP()) {
            if (inVector(productionTo, generatingSymbols) &&
                !inVector(production->getFromP(), generatingSymbols)) {
                generatingSymbols.push_back(production->getFromP());
                changed = true;
            }
        }
    }
    return changed;
}

bool CFG::expandReachableSymbols(std::vector<std::string> &reachableSymbols) {
    bool changed = false;
    for (Production *production: productionsP) {
        if (inVector(production->getFromP(), reachableSymbols)) {
            for (std::string productionTo: production->getToP()) {
                if (!inVector(productionTo, reachableSymbols)) {
                    reachableSymbols.push_back(productionTo);
                    changed = true;
                }
            }
        }
    }
    return changed;
}

bool CFG::reachableProduction(Production *&production, std::vector<std::string> reachableSymbols) {
    for (std::string stateTo: production->getToP()) {
        if (!inVector(stateTo, reachableSymbols)) {
            return false;
        }
    }
    return true;
}

void CFG::eliminateEpsilonProductions() {
    std::vector<std::string> nullableSymbols = {};
    while (expandNullableSymbols(nullableSymbols)) {}

    for (std::string symbol: nullableSymbols) {
        int pSize = productionsP.size();
        for (int i = 0; i < pSize; ++i) {
            removeNullableSymbol(symbol, productionsP[i]);
        }

        for (int i = 0; i < productionsP.size(); ++i) {
            if (productionsP[i]->getToP().empty()) {
                // Transition A -> e
                productionsP.erase(productionsP.begin() + i);
                i--;
            }
        }
    }
}

bool CFG::expandNullableSymbols(std::vector<std::string> &nullableSymbols) {
    bool changed = false;
    for (Production *production: productionsP) {
        if (consistOnlyOfNullableSymbols(production->getToP(), nullableSymbols)) {
            nullableSymbols.push_back(production->getFromP());
        }
    }
    return changed;
}

bool CFG::consistOnlyOfNullableSymbols(const std::vector<std::string> &symbolList,
                                       std::vector<std::string> &nullableSymbols) {
    for (std::string symbol: symbolList) {
        if (!inVector(symbol, nullableSymbols) && symbol != toString(getEPSILON())) {
            return false;
        }
    }
    return true;
}

void CFG::removeNullableSymbol(std::string symbol, Production *production) {
    bool changed = false;
    std::vector<std::string> productionsTo = production->getToP();
    std::vector<std::string> withoutSymbol = {};
    for (int i = 0; i < productionsTo.size(); ++i) {
        if (productionsTo[i] != symbol) {
            withoutSymbol.push_back(productionsTo[i]);
            changed = true;
        }
    }
    if (changed) {
        Production *newProduction = new Production(production->getFromP(), withoutSymbol);
        productionsP.push_back(newProduction);
    }
}

void CFG::eliminateUnitProductions() {
    std::vector<std::pair<std::string, std::string>> unitProductionPairs = {};
    for (std::string variable: nonTerminalsV) {
        std::pair<std::string, std::string> unitPair(variable, variable);
        unitProductionPairs.emplace_back(unitPair);
    }

    while (expandUnitPairs(unitProductionPairs)) {}

    for (auto pair: unitProductionPairs) {
        std::vector<std::vector<std::string>> productions = findProductionsForVariable(pair.second);
        for (auto production: productions) {
            if (!productionExists(pair.first, production)) {
                Production *newProduction = new Production(pair.first, production);
                productionsP.emplace_back(newProduction);
            }
        }
    }

    // Finally, remove all unit productions
    for (int i = 0; i < productionsP.size(); ++i) {
        Production *production = productionsP[i];
        auto productionTo = production->getToP();
        if (productionTo.size() == 1 && inVector(productionTo[0], nonTerminalsV)) {
            productionsP.erase(productionsP.begin() + i);
            i--;
        }
    }
}

bool CFG::expandUnitPairs(std::vector<std::pair<std::string, std::string>> &unitPairs) {
    bool changed = false;
    for (Production *production: productionsP) {
        std::vector<std::string> productionsTo = production->getToP();
        if (productionsTo.size() != 1) {
            continue;
        }
        std::string productionTo = productionsTo[0];

        for (std::pair<std::string, std::string> unitPair: unitPairs) {
            if (unitPair.second == productionTo) {
                std::pair<std::string, std::string> newPair(unitPair.first, productionTo);
                if (!inVector(newPair, unitPairs)) {
                    unitPairs.push_back(newPair);
                }
            }
        }
    }
    return changed;
}

std::vector<std::vector<std::string>> CFG::findProductionsForVariable(std::string productionFrom) {
    std::vector<std::vector<std::string>> possibleProductions = {};
    for (Production *production: productionsP) {
        if (production->getFromP() == productionFrom) {
            possibleProductions.emplace_back(production->getToP());
        }
    }
    return possibleProductions;
}

bool CFG::productionExists(std::string productionFrom, std::vector<std::string> productionTo) {
    for (Production *production: productionsP) {
        if (productionFrom == production->getFromP() &&
            productionTo == production->getToP()) {
            return true;
        }
    }
    return false;
}

void CFG::cleanUp() {
    eliminateEpsilonProductions();
    eliminateUnitProductions();
    eliminateUselessSymbols();
}

void CFG::fixOnlyVariablesProductions() {
    std::vector<std::string> terminalsOccuringInBodies = {};
    for (Production *production: productionsP) {
        auto productionTo = production->getToP();
        if (productionTo.size() >= 2) {
            // Add all terminals in list
            for (std::string replacement: productionTo) {
                if (inVector(replacement, terminalsT)) {
                    terminalsOccuringInBodies.push_back(replacement);
                }
            }
        }
    }

    // We have now collected all terminals in productions that have to be replaced,
    //  time to replace them.
    std::vector<std::pair<std::string, std::string>> replacementRules; // A set of rules to determine which terminal gets replaced by which variable
    for (std::string terminal: terminalsOccuringInBodies) {
        char t = terminal[0];
        t -= 32; // Make t an uppercase letter (or another symbol if not a letter) // TODO: check whether this makes it a uppercase letter
        while (inVector(toString(t), nonTerminalsV)) {
            t += 1; // Change te letter of t if it's already used as variable
        }

        // Add the new pair to the replacementRules
        std::pair<std::string, std::string> replacement(toString(t), terminal);
        replacementRules.push_back(replacement);

        // Add the new production to the list:
        Production *newProduction = new Production(replacement.first, {replacement.second});
        nonTerminalsV.emplace_back(replacement.first);
    }

    // Replace the terminal in the productions
    for (Production *production: productionsP) {
        auto productionTo = production->getToP();
        if (productionTo.size() < 2) continue;
        for (std::string &replacement: productionTo) {
            for (auto replacementRule: replacementRules) {
                if (replacement == replacementRule.second) {
                    replacement = replacementRule.first;
                }
            }
        }
    }
}

void CFG::splitUpLongerBodies() {

    for (int i = 0; i < productionsP.size(); ++i) {

        auto productionTo = productionsP[i]->getToP();

        // Only modify variables longer than 2
        if (productionTo.size() > 2) {

            std::string previousVar = productionsP[i]->getFromP();

            for (int j = 0; j < productionTo.size() - 2; ++j) {
                std::string newVariable = findNewUnusedVariableLetter();
                nonTerminalsV.push_back(newVariable);
                Production *production = new Production(previousVar, {productionTo[j], newVariable});
                productionsP.emplace_back(production);
                previousVar = newVariable;
            }

            int lastElement = productionTo.size() - 1;
            Production *production = new Production(previousVar,
                                                    {productionTo[lastElement - 1], productionTo[lastElement]});
            productionsP.emplace_back(production);

            productionsP.erase(productionsP.begin() + i);
            i--;
        }
    }
}

std::string CFG::findNewUnusedVariableLetter() {
    char newVariable = 'A';
    while (inVector(toString(newVariable), terminalsT)) { newVariable++; }
    return toString(newVariable);
}


rapidjson::Value strJSON(std::string str, rapidjson::Document::AllocatorType &allocator) {
    rapidjson::Value strVal;
    strVal.SetString(str.c_str(), str.length(), allocator);
    return strVal;
}

void readJson(std::string filename,
               std::vector<std::string> &nonTerminalsV,
               std::vector<std::string> &terminalsT,
               std::vector<Production *> &productionsP,
               std::string &startS) {

    // Read the file
    std::ifstream pda_stream(filename);
    rapidjson::IStreamWrapper pda_stream_wrapper(pda_stream);
    rapidjson::Document pda_document;
    pda_document.ParseStream(pda_stream_wrapper);

    startS = pda_document["Start"].GetString();

    // Terminals
    rapidjson::Value &terminals_value = pda_document["Terminals"];

    terminalsT = {};

    for (rapidjson::SizeType i = 0; i < terminals_value.Size(); i++) {
        std::string s = terminals_value[i].GetString();
        terminalsT.emplace_back(s);
    }

    // non-Terminals
    rapidjson::Value &nonterminals_value = pda_document["Variables"];

    nonTerminalsV = {};

    for (rapidjson::SizeType i = 0; i < nonterminals_value.Size(); i++) {
        std::string s = nonterminals_value[i].GetString();
        nonTerminalsV.emplace_back(s);
    }


    // Transitions
    rapidjson::Value &transitions_value = pda_document["Productions"];

    productionsP = {};

    for (rapidjson::SizeType i = 0; i < transitions_value.Size(); i++) {

        rapidjson::Value &transition = transitions_value[i];

        std::vector<std::string> replacement = {};
        rapidjson::Value &replacement_value = transition["body"];

        for (rapidjson::SizeType i = 0; i < replacement_value.Size(); i++) {
            replacement.emplace_back(replacement_value[i].GetString());
        }

        std::string inputStr = transition["head"].GetString();

        Production* production = new Production(inputStr, replacement);
        productionsP.push_back(production);
    }

}
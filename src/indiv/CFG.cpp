//
// Created by arno on 28.10.19.
//

#include "CFG.h"
#include "functions.h"

#include <fstream>
#include <iostream>
#include <algorithm>

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

void Production::setToP(const std::vector<std::string> &toP) {
    Production::toP = toP;
}

bool Production::operator==(const Production &p) {
    return p.getFromP() == fromP && p.getToP() == toP;
}

std::string Production::toStr() {
    std::string uniqueID = "";
    std::string returnStr = "";

    // from
    returnStr += "#PROD.FROM#\n" + fromP + "\n";
    returnStr += "#PROD.STATES#\n";

    // to
    int i = 0;
    for (auto state: toP) {
        i++;
        returnStr += "#PROD.STATE#\n" + state;
        if (i != toP.size()) {
            returnStr += "\n";
        }
    }

    return returnStr;
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

    std::cout << "Start Grammar: " << std::endl;
    printReadable();

    cleanUp();
    fixOnlyVariablesProductions();

    std::cout << "Fixed terminals in longer productions: " << std::endl;
    printReadable();

    splitUpLongerBodies();

    std::cout << "Splitted up longer bodies, final grammar: " << std::endl;
    printReadable();
}

void CFG::eliminateUselessSymbols() {
    eliminateNonGeneratingSymbols();

    std::cout << "Eliminated non-generating symbols: " << std::endl;
    printReadable();

    eliminateNonReachableSymbols();

    std::cout << "Eliminated non-reachable symbols: " << std::endl;
    printReadable();
}

void CFG::eliminateNonGeneratingSymbols() {
    // Find all generating symbols (and thus also the nongenerating)
    std::vector<std::string> generatingSymbols = terminalsT;

    while (expandGeneratingSymbols(generatingSymbols)) {}

    std::cout << "Generating symbols: " << vecToStr(generatingSymbols) << std::endl;

    // Remove all non-generating symbols from variables, (terminals) and productions
    for (int i = 0; i < nonTerminalsV.size(); ++i) {
        if (!inVector(nonTerminalsV[i], generatingSymbols)) {
            nonTerminalsV.erase(nonTerminalsV.begin() + i);
            --i;
        }
    }

    for (int j = 0; j < productionsP.size(); ++j) {
        if (!reachableProduction(productionsP[j], vectorUnion(terminalsT, generatingSymbols))) {
            productionsP.erase(productionsP.begin() + j);
            --j;
        }
    }

}

void CFG::eliminateNonReachableSymbols() {
    // Find all reachable symbols (and thus also the nonreachable)
    std::vector<std::string> reachableSymbols = {startS};
    while (expandReachableSymbols(reachableSymbols)) {}

    std::cout << "Reachable symbols: " << vecToStr(reachableSymbols) << std::endl;

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

    std::cout << "Nullable symbols: " << vecToStr(nullableSymbols) << std::endl;

    for (std::string symbol: nullableSymbols) {
        int pSize = productionsP.size();
        for (int i = 0; i < pSize; ++i) {
            removeNullableSymbol(symbol, productionsP[i]);
        }

        for (int i = 0; i < productionsP.size(); ++i) {
            if (productionsP[i]->getToP().empty() or
                (productionsP[i]->getToP().size() == 1 and
                 productionsP[i]->getToP()[0] == "e")) {
                // Transition A -> e
                productionsP.erase(productionsP.begin() + i);
                i--;
            }
        }
    }
}

std::string CFG::vecToStr(const std::vector<std::string> &strVec) const {
    std::string output = "";
    for (auto el: strVec) {
        output += el + ", ";
    }
    if (output.size() != 0) {
        output = output.substr(0, output.size() - 2);
    }
    return output;
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
    int symbolCount = 0;
    for (int i = 0; i < productionsTo.size(); ++i) {
        if (productionsTo[i] != symbol) {
            withoutSymbol.push_back(productionsTo[i]);
        } else {
            symbolCount++;
            changed = true;
        }
    }
    if (changed) {
        Production *newProduction = new Production(production->getFromP(), withoutSymbol);
        productionsP.push_back(newProduction);
    }

    if (symbolCount > 1) {
        std::vector<std::vector<bool>> permutations = findPermutations(symbolCount);
        permutations = filterAll0orAll1(permutations); // Since already included

        for (auto permu: permutations) {
            std::vector<std::string> withoutSymbol = {};
            int symbolCount = 0;
            for (int i = 0; i < productionsTo.size(); ++i) {
                if (productionsTo[i] != symbol) {
                    withoutSymbol.push_back(productionsTo[i]);
                } else {
                    if (permu[symbolCount]) {
                        withoutSymbol.push_back(productionsTo[i]);
                    }
                    symbolCount++;
                }
            }
            Production *newProduction = new Production(production->getFromP(), withoutSymbol);
            productionsP.push_back(newProduction);
        }
    }

    productionsP = removeDoubleProductions(productionsP);
}

void CFG::eliminateUnitProductions() {
    std::vector<std::pair<std::string, std::string>> unitProductionPairs = {};
    for (std::string variable: nonTerminalsV) {
        std::pair<std::string, std::string> unitPair(variable, variable);
        unitProductionPairs.emplace_back(unitPair);
    }

    while (expandUnitPairs(unitProductionPairs)) {}

    std::cout << "Unit pairs: ";
    std::string output = "";
    for (auto el: unitProductionPairs) {
        output += "(" + el.first + ", " + el.second + "), ";
    }
    if (output.size() != 0) {
        output = output.substr(0, output.size() - 2);
    }
    std::cout << output << std::endl;

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
                std::pair<std::string, std::string> newPair(production->getFromP(), productionTo);
                if (!inVector(newPair, unitPairs)) {
                    unitPairs.push_back(newPair);
                    changed = true;
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

    std::cout << "Eliminated epsilon productions: " << std::endl;
    printReadable();

    eliminateUnitProductions();

    std::cout << "Eliminated unit productions: " << std::endl;
    printReadable();

    eliminateUselessSymbols();
}

void CFG::fixOnlyVariablesProductions() {
    std::vector<std::string> terminalsOccuringInBodies = {};
    for (Production *production: productionsP) {
        auto productionTo = production->getToP();
        if (productionTo.size() >= 2) {
            // Add all terminals in list
            for (std::string replacement: productionTo) {
                if (inVector(replacement, terminalsT) && !inVector(replacement, terminalsOccuringInBodies)) {
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
        if (t < 97 or t > 122) { t = 'a'; }
        t -= 32; // Make t an uppercase letter (or another symbol if not a letter) // TODO: check whether this makes it a uppercase letter
        while (inVector(toString(t), nonTerminalsV)) {
            t += 1; // Change te letter of t if it's already used as variable
        }

        std::pair<std::string, std::string> replacement(toString(t), terminal);

        // Add the new production to the list:
        Production *newProduction = new Production(replacement.first, {replacement.second});
        nonTerminalsV.emplace_back(replacement.first);

        productionsP.push_back(newProduction);


        // Add the new pair to the replacementRules
        replacementRules.push_back(replacement);


    }

    // Replace the terminal in the productions
    for (int i = 0; i < productionsP.size(); ++i) {
        Production *production = productionsP[i];
        auto productionTo = production->getToP();
        if (productionTo.size() < 2) continue;
        bool changed = false;
        for (std::string &replacement: productionTo) {
            for (auto replacementRule: replacementRules) {
                if (replacement == replacementRule.second) {
                    changed = true;
                    replacement = replacementRule.first;
                }
            }
        }
        if (changed) {
            productionsP[i]->setToP(productionTo);
            i--;
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
    while (inVector(toString(newVariable), nonTerminalsV)) { newVariable++; }
    return toString(newVariable);
}

bool CFG::findVariable(std::string &var, std::vector<std::string> productionTo) {
    for (Production *production: productionsP) {
        if (production->getToP() == productionTo) {
            var = production->getFromP();
            return true;
        }
    }
    return false;
}

bool CFG::toCYK(std::string strToEvaluate, std::string &htmlString) {

    htmlString = "</table>";

    // I assume we're working with a CNF
    std::vector<std::vector<std::vector<std::string>>> cykTable = {};
    for (int l = 0; l < strToEvaluate.size(); ++l) {
        std::vector<std::vector<std::string>> row = {};
        for (int i = 0; i < strToEvaluate.size(); ++i) {
            row.push_back({});
        }
        cykTable.push_back(row);
    }

    std::vector<std::vector<std::string>> firstRow = {};
    for (char c: strToEvaluate) {
        firstRow.emplace_back(findVariables({toString(c)}));
    }
    cykTable[0] = firstRow;

    htmlString = rowHtmlString(firstRow) + htmlString;

    for (int row = 1; row < cykTable.size(); ++row) { // Row we're going to fill
        for (int column = 0; column < cykTable.size(); ++column) { // Column we're going to fill
            int i = column;
            int j = i + row; // row = j-i+1 <=> j = row + i - 1;

            if (i >= j or j > cykTable.size() - 1 or i > cykTable.size() - 1) continue;

            std::string subString = strToEvaluate.substr(i, j - i + 1);
            std::vector<std::string> productionsToStr = {};
            for (int k = i; k < j; ++k) {
                std::string subStr1 = strToEvaluate.substr(i, k - i + 1);
                std::string subStr2 = strToEvaluate.substr(k + 1, j - k);
                std::vector<std::string> productionsSubstr1 = findProductionsInCYK(i, k, cykTable);
                std::vector<std::string> productionsSubstr2 = findProductionsInCYK(k + 1, j, cykTable);
                std::vector<std::string> productionsToSubStr = findAtoBCproductions(productionsSubstr1,
                                                                                    productionsSubstr2);
                for (auto production: productionsToSubStr) {
                    if (!inVector(production, productionsToStr)) {
                        productionsToStr.push_back(production);
                    }
                }
            }
            cykTable[row][column] = productionsToStr;
        }
        htmlString = rowHtmlString(cykTable[row]) + htmlString;
    }

    htmlString = "<table border=\"1\">" + htmlString;

    // Cyk table should now be finished, check whether the startstate is in the upper left corner
    return inVector(startS, cykTable[cykTable.size() - 1][0]);
}

std::vector<std::string> CFG::findVariables(std::vector<std::string> productionTo) {
    std::vector<std::string> variables = {};
    for (Production *production: productionsP) {
        if (production->getToP() == productionTo) {
            variables.emplace_back(production->getFromP());
        }
    }
    return variables;
}

std::vector<std::string>
CFG::findProductionsInCYK(int i, int j,
                          std::vector<std::vector<std::vector<std::string>>> &cykTable) {
    int row = j - i;
    int column = i;
    return cykTable[row][column];
}

std::vector<std::string> CFG::findAtoBCproductions(std::vector<std::string> &B, std::vector<std::string> &C) {
    std::vector<std::string> A = {};
    for (Production *production: productionsP) {
        if (production->getToP().size() == 2) { // Check whether it's not a terminal
            if (inVector(production->getToP()[0], B) &&
                inVector(production->getToP()[1], C) &&
                !inVector(production->getFromP(), A)) {
                A.push_back(production->getFromP());
            }
        }
    }

    return A;
}

std::string CFG::rowHtmlString(std::vector<std::vector<std::string>> vector) {
    std::string row = "<tr>";
    for (auto subvec: vector) {
        row += "<td>{";
        for (auto el: subvec) {
            row += el + ", ";
        }
        if (!subvec.empty()) {
            row = row.substr(0, row.size() - 2);
        }
        row += "}</td>";
    }
    row += "</tr>";
    return row;
}

void CFG::printReadable() {
    std::cout << std::endl;

    std::cout << "Nonterminals: " << vecToStr(nonTerminalsV) << std::endl;

    std::cout << "Terminals: " << vecToStr(terminalsT) << std::endl;

    std::cout << "Productions: " << std::endl;
    for (Production *production: productionsP) {
        std::cout << "\t" << production->getFromP() << " -> ";
        for (auto el: production->getToP()) {
            std::cout << el;
        }
        std::cout << std::endl;
    }

    std::cout << "Startsymbol: " << startS << std::endl;
    std::cout << std::endl;
}

std::vector<std::vector<bool>> CFG::findPermutations(int count) {
    if (count == 1) {
        return {{0},
                {1}};
    } else if (count == 0) {
        return {};
    } else {
        std::vector<std::vector<bool>> allPermutations;
        for (auto permutation: findPermutations(count - 1)) {
            std::vector<bool> permutationNew = permutation;
            permutation.push_back(false);
            allPermutations.push_back(permutation);
            permutation = permutationNew;
            permutation.push_back(true);
            allPermutations.push_back(permutation);
        }
        return allPermutations;
    }

    return std::vector<std::vector<bool>>();
}

std::vector<std::vector<bool>> CFG::filterAll0orAll1(std::vector<std::vector<bool>> vector) {
    std::vector<std::vector<bool>> filtered;

    if (vector.size() == 0 or vector[0].size() == 0) return vector;

    for (auto permu: vector) {
        bool symbol = permu[0];
        bool done = false;
        for (auto el: permu) {
            if (!done and symbol != el) {
                filtered.push_back(permu);
                done = true;
            }
        }
    }
    return filtered;
}

std::vector<Production *> CFG::removeDoubleProductions(std::vector<Production *> productions) {
    std::vector<Production *> newProductions = {};
    for (auto prod: productions) {
        if (!inVector(prod, newProductions)) {
            newProductions.emplace_back(prod);
        }
    }
    return newProductions;
}

void CFG::toLRParser() {


}

const std::vector<std::string> &CFG::getNonTerminalsV() const {
    return nonTerminalsV;
}

const std::vector<std::string> &CFG::getTerminalsT() const {
    return terminalsT;
}

const std::vector<Production *> &CFG::getProductionsP() const {
    return productionsP;
}

const std::string &CFG::getStartS() const {
    return startS;
}

void CFG::splitUpLongerTerminals() {
    for (int i = 0; i < terminalsT.size(); ++i) {

        auto terminal = terminalsT[i];

        // Only modify variables longer than 2
        if (terminal.size() > 1) {

            nonTerminalsV.push_back(terminal);
            std::vector<std::string> terminalProductions;
            for (int k = 0; k < terminal.size(); ++k) {
                std::string charStr{terminal[k]};
                terminalProductions.push_back(charStr);
                terminalsT.push_back(charStr);
            }
            productionsP.emplace_back(new Production(terminal, terminalProductions));
            terminalsT.erase(terminalsT.begin() + i);
            i--;
        }
    }
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

        Production *production = new Production(inputStr, replacement);
        productionsP.push_back(production);
    }

}
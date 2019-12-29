//
// Created by arno on 20.11.19.
//

#ifndef MB_GLRPARSER_H
#define MB_GLRPARSER_H


#include <string>
#include <set>
#include <map>
#include <stack>
#include "CFG.h"
#include "ASTree.h"
#include <queue>

class CFG;

class GLRState;

class ParseOperation {
public:
    enum operation {
        undefined, shift, reduce, accept
    };

    explicit ParseOperation(GLRState *newState);

    explicit ParseOperation(Production *reduceProduction);

private:
    operation operationType;
public:
    [[nodiscard]] operation getOperationType() const;

    [[nodiscard]] Production *getReduceProduction() const;

    [[nodiscard]] GLRState *getNewState() const;

private:
    Production *reduceProduction{};
    GLRState *newState{};
};

class GLRTransition {
public:
    GLRTransition(std::string label, GLRState *stateTo);

private:
    std::string label;
    GLRState *stateTo;
public:
    [[nodiscard]] const std::string &getLabel() const;

//    void setLabel(const std::string &label);

    [[nodiscard]] GLRState *getStateTo() const;

//    void setStateTo(GLRState *stateTo);
};

class GLRState {
    std::string name;
    bool accepting;
    std::set<Production *> prodEstablished;
public:
    bool operator==(const GLRState &b) const;

    bool operator!=(const GLRState &b) const;

    bool operator<(const GLRState &b) const;

    bool operator>(const GLRState &b) const;

    bool operator<=(const GLRState &b) const;

    bool operator>=(const GLRState &b) const;

    [[nodiscard]] const std::set<Production *> &getProdEstablished() const;

//    [[nodiscard]] std::string toStr() const;

public:
    std::set<GLRState *> statesOnInput(const std::string &input);

    void increaseEstablished(Production *prod);

public:
    [[nodiscard]] bool isAccepting() const;

    void setAccepting(bool accepting);

public:
    GLRState(std::string name, std::set<Production *> productions);

    void addStateTo(GLRState *glrState, const std::string &label);

    [[nodiscard]] const std::string &getName() const;

    [[nodiscard]] const std::set<Production *> &getProductions() const;

    [[nodiscard]] const std::set<GLRTransition *> &getStatesTo() const;

private:
    std::set<Production *> productions;
    std::set<GLRTransition *> statesTo;
};

class GLRParser {
public:
//    void writeToFile(const std::string& filename);

//    explicit GLRParser(const std::string& filename, bool verbose = false); // Read from file

    explicit GLRParser(CFG *cfg, bool verbose = false);

    void toDot(std::string filename);

//    void printTable();

    std::set<std::stack<Production *>> parseString(const std::string &toParse);

private:
    bool verbose;
    std::set<std::string> nonTerminalsV;
public:
    [[nodiscard]] set<string> getNonTerminalsV() const;

private:
    GLRState *acceptState{};
    std::set<std::string> terminalsT;

//    void buildTable(); // Converts the states to the parseTable variable
//    std::map<std::pair<std::string, GLRState *>, std::set<ParseOperation *>> parseTable; // input in combination with statenumber gives you the parseoption
//
//    void addState(GLRState *s);

    GLRState *startState;
    std::set<GLRState *> states;

    static std::string symbolAfterMarker(const Production *production);

    static bool isMarkerMovedRight(Production *originalProduction, Production *productionMovedRight);

    bool closure(std::set<Production *> &markedProductions, std::set<Production *> &allMarkedProductions);

    static std::set<Production *>
    initialProductionsFromVar(const std::string &var, std::set<Production *> &allMarkedProductions);

    GLRState *findState(const std::string &stateName);

    set<ParseOperation *>
    findParseOptions(const std::string &inputChar, const GLRState *currentState) const;

    void
    printStackSet(
            const std::vector<std::pair<std::stack<std::string>, std::stack<Production *>>> &possibleParseStacks) const;

    bool
    checkProductionRules(std::vector<std::pair<std::stack<std::string>, std::stack<Production *>>> &possibleParseStacks,
                         const std::stack<std::string> &stack,
                         const std::stack<Production *> &productions,
                         const GLRState *currentState,
                         std::set<std::pair<std::stack<std::string>, Production *>> &alreadyReduced,
                         bool final = false);

    void
    removeDuplicates(vector<std::pair<std::stack<std::string>, std::stack<Production *>>> &possibleParseStacks) const;
};

template<class T>
void printStack(std::stack<T> stack);

#endif //MB_GLRPARSER_H

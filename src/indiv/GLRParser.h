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

class CFG;

class GLRState;

class ParseOperation {
public:
    enum operation {
        undefined, shift, reduce, accept
    };

    ParseOperation(GLRState *newState);

    ParseOperation(Production *reduceProduction);

private:
    operation operationType;
public:
    operation getOperationType() const;

    Production *getReduceProduction() const;

    GLRState *getNewState() const;

private:
    Production *reduceProduction;
    GLRState *newState;
};

class GLRTransition {
public:
    GLRTransition(const std::string &label, GLRState *stateTo);

private:
    std::string label;
    GLRState *stateTo;
public:
    const std::string &getLabel() const;

    void setLabel(const std::string &label);

    GLRState *getStateTo() const;

    void setStateTo(GLRState *stateTo);
};

class GLRState {
    std::string name;
    bool accepting;
    std::set<Production*> prodEstablished;
public:
    const std::set<Production *> &getProdEstablished() const;

public:
    std::set<GLRState *> statesOnInput(std::string input);

    void increaseEstablished(Production* prod);

public:
    bool isAccepting() const;

    void setAccepting(bool accepting);

public:
    GLRState(const std::string &name, const std::set<Production *> &productions);

    void addStateTo(GLRState *glrState, std::string label);

    const std::string &getName() const;

    const std::set<Production *> &getProductions() const;

    const std::set<GLRTransition *> &getStatesTo() const;

private:
    std::set<Production *> productions;
    std::set<GLRTransition *> statesTo;
};

class GLRParser {
public:
    GLRParser(CFG *cfg);

    void toDot(std::string filename);

    void printTable();

    bool parseString(std::string toParse);

private:
    std::vector<std::string> nonTerminalsV;
    std::vector<std::string> terminalsT;

    void buildTable(); // Converts the states to the parseTable variable
    std::map<std::pair<std::string, GLRState *>, std::set<ParseOperation *>> parseTable; // input in combination with statenumber gives you the parseoption

    void addState(GLRState *s);

    GLRState *startState;
    std::set<GLRState *> states;

    static std::string symbolAfterMarker(const Production *production);

    static bool isMarkerMovedRight(Production *originalProduction, Production *productionMovedRight);

    bool closure(std::set<Production *> &markedProductions, std::set<Production *> &allMarkedProductions);

    std::set<Production *>
    initialProductionsFromVar(std::string var, std::set<Production *> &allMarkedProductions);

    GLRState *findState(std::string stateName);

    std::set<ParseOperation *> findParseOptions(std::string inputChar, const GLRState *currentState) const;

    void printStackSet(const std::vector<std::stack<std::string>> &possibleParseStacks) const;

    void
    checkProductionRules(std::vector<std::stack<std::string>> &possibleParseStacks,
                         const std::stack<std::string> &stack,
                         const GLRState *currentState, std::set<std::stack<std::string>> set);
};

template<class T>
void printStack(std::stack<T> stack);

#endif //MB_GLRPARSER_H

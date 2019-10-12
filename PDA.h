//
// Created by arno on 05.10.19.
//

#ifndef MB_PDA_H
#define MB_PDA_H

#include <string>
#include <vector>
#include <stack>

class State;

class Transition;

class PDA;

class EvaluationState {
public:
    std::stack<char> stack;
    State * currentStates;

    EvaluationState(const std::stack<char> &stack, State *currentStates);

};

class Transition {
private:
    State *stateFrom; // Q
    State *stateTo; // 2^Q
    std::string stackPush; // 2^G*
    char input; // S U e
    char stackInput; // G
public:
    Transition(State *stateFrom, State *stateTo, const std::string &stackPush, char input, char stackInput);

    State *getStateFrom() const;

    State *getStateTo() const;

    const std::string &getStackPush() const;

    char getInput() const;

    char getStackInput() const;
};

class State {
private:
    std::string name;
    std::vector<Transition *> transitionsFrom;
public:
    State(const std::string &name);

    void addTransition(Transition *transition);

    const std::string &getName() const;

    std::vector<Transition*> getTransitionsOnInput(char input, char stack);
};

class PDA {
private:
    std::vector<State *> statesQ;
    std::vector<char> inputAlphabetS;
    std::vector<char> stackAlphabetG;
    State *startState;
    char startStackZ0;
    std::vector<State *> endStatesF;
    std::vector<Transition *> transitionFunctionD;

    bool containsState(std::string stateName);

    bool inStackAlphabet(char c);

    bool inStackAlphabet(std::string s);

    bool inAlphabet(char c);

    bool addTransition(Transition *transition);

    std::vector<EvaluationState> nextEvaluations(std::vector<EvaluationState> evaluations, char input);

    bool containsEndState(std::vector<EvaluationState> evaluations);

    bool isEndState(State* state);

    std::vector<EvaluationState> eclose(std::vector<EvaluationState> evaluations);

public:
    PDA(std::vector<State *> &statesQ, std::vector<char> &inputAlphabetS,
        std::vector<char> &stackAlphabetG, State *startState, char startStackZ0,
        std::vector<State *> &endStatesF, std::vector<Transition *> &transitionFunctionD);

    void toDot(std::string filename);

    bool evaluate(std::string s);
};


#endif //MB_PDA_H

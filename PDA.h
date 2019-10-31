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

class CFG;

class Production;

void read_json(std::string filename,
               std::vector<State *> &states,
               std::vector<char> &alphabet,
               std::vector<std::string> &stackAlphabet,
               std::vector<Transition *> &transition_function,
               State *&start_state,
               std::string &start_stack,
               std::vector<State *> &final_states);

class EvaluationState {
public:
    std::stack<std::string> stack;
    State * currentStates;

    EvaluationState(const std::stack<std::string> &stack, State *currentStates);

};

class Transition {
private:
    State *stateFrom; // Q
    State *stateTo; // 2^Q
    std::vector<std::string> stackPush; // 2^G*
    char input; // S U e
    std::string stackInput; // G
public:
    Transition(State *stateFrom, State *stateTo, const std::vector<std::string> &stackPush, char input, std::string stackInput);

    State *getStateFrom() const;

    State *getStateTo() const;

    const std::vector<std::string> &getStackPush() const;

    char getInput() const;

    std::string getStackInput() const;
};

class State {
private:
    std::string name;
    std::vector<Transition *> transitionsFrom;
public:
    State(const std::string &name);

    void addTransition(Transition *transition);

    const std::string &getName() const;

    std::vector<Transition*> getTransitionsOnInput(char input, std::string stack);
};

class PDA {
private:
    std::vector<State *> statesQ;
    std::vector<char> inputAlphabetS;
    std::vector<std::string> stackAlphabetG;
    State *startState;
    std::string startStackZ0;
    std::vector<State *> endStatesF;
    std::vector<Transition *> transitionFunctionD;

    bool containsState(std::string stateName);

    bool inStackAlphabet(std::string c);

    bool inStackAlphabet(std::vector<std::string> s);

    bool inAlphabet(char c);

    bool addTransition(Transition *transition);

    std::vector<EvaluationState> nextEvaluations(std::vector<EvaluationState> evaluations, char input);

    bool containsEndState(std::vector<EvaluationState> evaluations);

    bool isEndState(State* state);

    std::vector<EvaluationState> eclose(std::vector<EvaluationState> evaluations);

    // toCFG() help functions
    std::vector<std::string> getCFGVariables();
    std::vector<Production*> getCFGProductions();

public:
    PDA(std::vector<State *> &statesQ, std::vector<char> &inputAlphabetS,
        std::vector<std::string> &stackAlphabetG, State *startState, std::string startStackZ0,
        std::vector<State *> &endStatesF, std::vector<Transition *> &transitionFunctionD);

    void toDot(std::string filename);

    bool evaluate(std::string s);

    CFG* toCFG();
};


#endif //MB_PDA_H

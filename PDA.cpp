//
// Created by arno on 05.10.19.
//

#include <iostream>
#include "PDA.h"
#include "Graph.h"
#include "functions.h"


State::State(const std::string &name) : name(name) {}

void State::addTransition(Transition *transition) {
    transitionsFrom.emplace_back(transition);
}

const std::string &State::getName() const {
    return name;
}

std::vector<Transition *> State::getTransitionsOnInput(char input, char stack) {
    std::vector<Transition *> transitions;
    for (Transition *transition: transitionsFrom) {
        if (transition->getInput() == input && transition->getStackInput() == stack) {
            transitions.emplace_back(transition);
        }
    }
    return transitions;
}

Transition::Transition(State *stateFrom, State *stateTo, const std::string &stackPush, char input, char stackInput)
        : stateFrom(stateFrom), stateTo(stateTo), stackPush(stackPush), input(input), stackInput(stackInput) {}

State *Transition::getStateFrom() const {
    return stateFrom;
}

State *Transition::getStateTo() const {
    return stateTo;
}

const std::string &Transition::getStackPush() const {
    return stackPush;
}

char Transition::getInput() const {
    return input;
}

char Transition::getStackInput() const {
    return stackInput;
}


PDA::PDA(std::vector<State *> &statesQn, std::vector<char> &inputAlphabetSn, std::vector<char> &stackAlphabetGn,
         State *startStaten, char startStackZ0n, std::vector<State *> &endStatesFn,
         std::vector<Transition *> &transitionFunctionDn) {
    inputAlphabetS = inputAlphabetSn;
    stackAlphabetG = stackAlphabetGn;
    statesQ = statesQn;

    // Todo: Check whether the startState is in states
    if (containsState(startStaten->getName())) {
        startState = startStaten;
    } else {
        std::cerr << "Startstate not found" << std::endl;
    }

    if (inStackAlphabet(startStackZ0n)) {
        startStackZ0 = startStackZ0n;
    } else {
        std::cerr << "Start stack not in alphabet" << std::endl;
    }

    for (auto state: endStatesFn) {
        if (containsState(state->getName())) {
            endStatesF.push_back(state);
        } else {
            std::cerr << "End state not in states" << std::endl;
        }
    }

    for (auto transition: transitionFunctionDn) {
        addTransition(transition);
    }

    std::stack<char> stack = {};
    std::vector<State *> currentStates = {startState};
}

bool PDA::containsState(std::string stateName) {
    for (auto state: statesQ) {
        if (state->getName() == stateName) {
            return true;
        }
    }
    return false;
}

bool PDA::inStackAlphabet(char c) {
    for (auto a: stackAlphabetG) {
        if (a == c or c == 'e') {
            return true;
        }
    }
    return false;
}

bool PDA::addTransition(Transition *transition) {
    if (containsState(transition->getStateFrom()->getName()) &&
        containsState(transition->getStateTo()->getName()) &&
        inStackAlphabet(transition->getStackPush()) &&
        inAlphabet(transition->getInput()) &&
        inStackAlphabet(transition->getStackInput())) {
        transitionFunctionD.emplace_back(transition);
        transition->getStateFrom()->addTransition(transition);
        return true;
    } else {
        std::cerr << "Error adding transition" << std::endl;
    }
    return false;
}

bool PDA::inStackAlphabet(std::string s) {
    bool in = true;
    for (char c: s) {
        in = in && inStackAlphabet(c);
    }
    return in;
}

bool PDA::inAlphabet(char c) {
    for (auto a: inputAlphabetS) {
        if (a == c or c == 'e') {
            return true;
        }
    }
    return false;
}

bool PDA::evaluate(std::string s) {

    std::vector<EvaluationState> evaluations;

    std::stack<char> stack;
    stack.push(startStackZ0);

    evaluations = eclose({EvaluationState(stack, startState)});

    for (char c: s) {
        evaluations = eclose(nextEvaluations(evaluations, c));
    }

    return containsEndState(evaluations);
}

std::vector<EvaluationState> PDA::nextEvaluations(std::vector<EvaluationState> evaluations, char input) {
    std::vector<EvaluationState> newEvaluations;
    for (auto evaluation: evaluations) {
        char stackTop = evaluation.stack.top();
        std::vector<Transition *> transitions = evaluation.currentStates->getTransitionsOnInput(input, stackTop);
        evaluation.stack.pop();
        for (Transition *transition: transitions) {
            std::stack<char> newStack = evaluation.stack;
            for (int j = transition->getStackPush().size() - 1; j >= 0; --j) {
                if (transition->getStackPush()[j] != 'e') { // e is epsilon, so nothing has to be pushed then
                    newStack.push(transition->getStackPush()[j]);
                }
            }
            newEvaluations.push_back(EvaluationState(newStack, transition->getStateTo()));
        }
    }
    return newEvaluations;
}

bool PDA::containsEndState(std::vector<EvaluationState> evaluations) {
    for (EvaluationState state: evaluations) {
        if (isEndState(state.currentStates)) {
            return true;
        }
    }
    return false;
}

bool PDA::isEndState(State *state) {
    for (State *endState: endStatesF) {
        if (endState->getName() == state->getName()) {
            return true;
        }
    }
    return false;
}

// TODO: zorgt momenteel nog maar voor 1 eclose stap, terwijl dit er wel meerdere kunnen zijn (recursief)
std::vector<EvaluationState> PDA::eclose(std::vector<EvaluationState> evaluations) {
    bool addedSomething = false;
    std::vector<EvaluationState> newEvaluations;
    for (EvaluationState evaluation: evaluations) {
        std::vector<EvaluationState> recentEvaluations = {};
        for (Transition *transition: evaluation.currentStates->getTransitionsOnInput('e', evaluation.stack.top())) {
            std::stack<char> newStack = evaluation.stack;
            for (int j = transition->getStackPush().size() - 1; j >= 0; --j) {
                if (transition->getStackPush()[j] != 'e') { // e is epsilon, so nothing has to be pushed then
                    newStack.push(transition->getStackPush()[j]);
                }
            }
            recentEvaluations.emplace_back(EvaluationState(newStack, transition->getStateTo()));
        }

        if(recentEvaluations.size() > 0){
            for(EvaluationState state: eclose(recentEvaluations)){
                newEvaluations.emplace_back(state);
            }
        }
        newEvaluations.emplace_back(evaluation);
    }
    if(!addedSomething){
        return newEvaluations;
    }

    return eclose(newEvaluations);
}

void PDA::toDot(std::string filename) {
    Graph graph = Graph();
    for (vector<State *>::iterator state = statesQ.begin(); state != statesQ.end(); ++state) {
        if (isEndState(*state)) {
            graph.addNode(Node((*state)->getName(), (*state)->getName(), "doublecircle"));
        } else {
            graph.addNode(Node((*state)->getName(), (*state)->getName(), "circle"));
        }
    }

    for (vector<Transition *>::iterator connection = transitionFunctionD.begin();
         connection != transitionFunctionD.end(); ++connection) {
        std::string connectionString =
                "\"(" + toString((*connection)->getStackInput()) + ", " + toString((*connection)->getStackInput()) + "/" +
                (*connection)->getStackPush() + ")\"";
        graph.addConnection(Connection((*connection)->getStateFrom()->getName(), (*connection)->getStateTo()->getName(),
                                       connectionString));
    }

    graph.addNode(Node("invis", "invis", "circle", "style=invis"));
    graph.addConnection(Connection("invis", startState->getName(), "Start"));
    graph.build_file(filename);
}

EvaluationState::EvaluationState(const std::stack<char> &stack, State *currentStates) : stack(stack),
                                                                                        currentStates(currentStates) {}

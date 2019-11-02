//
// Created by arno on 05.10.19.
//

#include <iostream>
#include <fstream>
#include "PDA.h"
#include "Graph.h"
#include "functions.h"

#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "CFG.h"

char EPSILON = 'e';

State::State(const std::string &name) : name(name) {}

void State::addTransition(Transition *transition) {
    transitionsFrom.emplace_back(transition);
}

const std::string &State::getName() const {
    return name;
}

std::vector<Transition *> State::getTransitionsOnInput(char input, std::string stack) {
    std::vector<Transition *> transitions;
    for (Transition *transition: transitionsFrom) {
        if (transition->getInput() == input && transition->getStackInput() == stack) {
            transitions.emplace_back(transition);
        }
    }
    return transitions;
}

Transition::Transition(State *stateFrom, State *stateTo, const std::vector<std::string> &stackPush, char input,
                       std::string stackInput)
        : stateFrom(stateFrom), stateTo(stateTo), stackPush(stackPush), input(input), stackInput(stackInput) {
    this->stackPush = removeEpsilon(stackPush);
}

State *Transition::getStateFrom() const {
    return stateFrom;
}

State *Transition::getStateTo() const {
    return stateTo;
}

const std::vector<std::string> &Transition::getStackPush() const {
    return stackPush;
}

char Transition::getInput() const {
    return input;
}

std::string Transition::getStackInput() const {
    return stackInput;
}

std::vector<std::string> Transition::removeEpsilon(std::vector<std::string> oldStackPush) {
    for (int i = 0; i < oldStackPush.size(); ++i) {
        if(stackPush[i] == "e"){
            oldStackPush.erase(oldStackPush.begin() + i);
            --i;
        }
    }
    return oldStackPush;
}


PDA::PDA(std::vector<State *> &statesQn, std::vector<char> &inputAlphabetSn, std::vector<std::string> &stackAlphabetGn,
         State *startStaten, std::string startStackZ0n, std::vector<State *> &endStatesFn,
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

//    std::stack<std::string> stack = {};
//    std::vector<State *> currentStates = {startState};
}

bool PDA::containsState(std::string stateName) {
    for (auto state: statesQ) {
        if (state->getName() == stateName) {
            return true;
        }
    }
    return false;
}

bool PDA::inStackAlphabet(std::string c) {
    for (auto a: stackAlphabetG) {
        if (a == c or c == "e") {
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

bool PDA::inStackAlphabet(std::vector<std::string> s) {
    bool in = true;
    for (std::string c: s) {
        in = in && inStackAlphabet(c);
    }
    return in;
}

bool PDA::inAlphabet(char c) {
    for (auto a: inputAlphabetS) {
        if (a == c or c == EPSILON) {
            return true;
        }
    }
    return false;
}

bool PDA::evaluate(std::string s) {

    std::vector<EvaluationState> evaluations;

    std::stack<std::string> stack;
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
        std::string stackTop = evaluation.stack.top();
        std::vector<Transition *> transitions = evaluation.currentStates->getTransitionsOnInput(input, stackTop);
        evaluation.stack.pop();
        for (Transition *transition: transitions) {
            std::stack<std::string> newStack = evaluation.stack;
            for (int j = transition->getStackPush().size() - 1; j >= 0; --j) {
//                if (transition->getStackPush()[j] != "e") { // e is epsilon, so nothing has to be pushed then
                    newStack.push(transition->getStackPush()[j]);
//                }
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
        for (Transition *transition: evaluation.currentStates->getTransitionsOnInput(EPSILON, evaluation.stack.top())) {
            std::stack<std::string> newStack = evaluation.stack;
            for (int j = transition->getStackPush().size() - 1; j >= 0; --j) {
//                if (transition->getStackPush()[j] != "e") { // e is epsilon, so nothing has to be pushed then
                    newStack.push(transition->getStackPush()[j]);
//                }
            }
            recentEvaluations.emplace_back(EvaluationState(newStack, transition->getStateTo()));
        }

        if (recentEvaluations.size() > 0) {
            for (EvaluationState state: eclose(recentEvaluations)) {
                newEvaluations.emplace_back(state);
            }
        }
        newEvaluations.emplace_back(evaluation);
    }
    if (!addedSomething) {
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
                "\"(" + toString((*connection)->getInput()) + ", " + (*connection)->getStackInput() +
                "/" +
                toString((*connection)->getStackPush()) + ")\"";
        graph.addConnection(Connection((*connection)->getStateFrom()->getName(), (*connection)->getStateTo()->getName(),
                                       connectionString));
    }

    graph.addNode(Node("invis", "invis", "circle", "style=invis"));
    graph.addConnection(Connection("invis", startState->getName(), "Start"));
    graph.build_file(filename);
}

CFG *PDA::toCFG() {
    std::vector<std::string> v = getCFGVariables(); // set of variables
    std::vector<Production*> r = getCFGProductions();

    std::vector<std::string> strVec = toVecStr(inputAlphabetS);
    CFG* cfg = new CFG(v, strVec, r, "S");
    return cfg;
}

std::vector<std::string> PDA::getCFGVariables() {

    std::vector<std::string> variables = {"S"};

    for (State* stateP: statesQ){
        for (State* stateQ: statesQ){
            for(std::string inputX: stackAlphabetG){ // TODO: Check whether stackAlphabetG contains all elements  of the alphabet
                std::string variableName = "[" + stateP->getName() + inputX + stateQ->getName() + "]";
                if(!inVector(variableName, variables)){
                    variables.emplace_back(variableName);
                }
            }
        }
    }

    return variables;
}

std::vector<Production *> PDA::getCFGProductions() {

    std::vector<Production*> productions = {};

    // Productions from start state
    for(State* state: statesQ){
        std::string toState = "[" + startState->getName() + "Z0" + state->getName() + "]";
        std::string fromState = "S";
        Production* production = new Production(fromState, {toState});
        productions.emplace_back(production);
    }

    // Productions from q pop X to rk
    for (Transition* transition: transitionFunctionD){

        char a = transition->getInput();
        std::string x = transition->getStackInput();
        std::vector<std::string> y = transition->getStackPush();
        State* r = transition->getStateTo();
        State* q = transition->getStateFrom();

        std::vector<std::vector<State*>> combinations = getStateCombinations(y.size());

        for(std::vector<State*> combination: combinations) {

            State *rk = combination[combination.size() - 1];
            std::string productionFrom = "[" + q->getName() + x + rk->getName() + "]";

            std::string rY1r1 = "[" + r->getName() + y[0] + combination[0]->getName() + "]";
            std::vector<std::string> productionTo = {toString(a), rY1r1};

            for (int k = 0; k+1 < combination.size(); ++k) {

                std::string rk1 = combination[k]->getName(); // r(k-1)
                std::string yk = y[k+1];
                std::string rk = combination[k+1]->getName();

                std::string rk1Ykrk = "[" + rk1 + yk + rk + "]";
                productionTo.emplace_back(rk1Ykrk);
            }

            Production* production = new Production(productionFrom, productionTo);
            productions.emplace_back(production);
        }

    }

    // Productions qXr -> a
    for(Transition* transition: transitionFunctionD){

        // TODO: check that epsilon always leads to empty stackPush
        if(transition->getStackPush().size() == 0 &&
            inAlphabet(transition->getInput())){ // So if it is an epsilon transition

            State* r = transition->getStateTo();
            State* q = transition->getStateFrom();

            char a = transition->getInput();

            std::string qXr = "[" + q->getName() + transition->getStackInput() + r->getName() + "]";

            Production* production = new Production(qXr, {toString(a)});

            productions.emplace_back(production);

        }
    }

    return productions;
}

std::vector<std::vector<State *>> PDA::getStateCombinations(int size) {
    std::vector<std::vector<State*>> combinations = {};
    if(size == 1) {
        for(State* state: statesQ){
            combinations.push_back({state});
        }
    } else if (size < 1) {
        return {};
    } else {
        std::vector<std::vector<State*>> smallerCombinations = getStateCombinations(size-1);
        for (State* state: statesQ) {
            for(std::vector<State*> combination: smallerCombinations){
                combinations.push_back(vectorUnion({state}, combination));
            }
        }
    }

    return combinations;
}

std::vector<std::vector<State *>> PDA::cleanupCombinations(std::vector<std::vector<State *>> oldCombinations) {
    std::vector<std::vector<State*>> newCombinations = {};
    for(auto combination: oldCombinations){
        if(!inCombinations(combination, newCombinations)){
            newCombinations.push_back(combination);
        }
    }
    return newCombinations;
}

bool PDA::inCombinations(std::vector<State *> combination, std::vector<std::vector<State *>> combinations) {
    for(auto possibleCombination: combinations){
        for (int i = 0; i < possibleCombination.size(); ++i) {
            if(possibleCombination[i]->getName() != combination[i]->getName()) {
                break;
            }
            return true;
        }
    }
    return false;
}

std::vector<std::string> PDA::toVecStr(std::vector<char> vecChar) {
    std::vector<std::string> vecStr = {};
    for(char c: vecChar){
        vecStr.emplace_back(toString(c));
    }
    return vecStr;
}

EvaluationState::EvaluationState(const std::stack<std::string> &stack, State *currentStates) : stack(stack),
                                                                                        currentStates(currentStates) {}


State *findState(std::string stateName, vector<State *> &states) {
    for (State *state: states) {
        if (stateName == state->getName()) {
            return state;
        }
    }
    return nullptr;
}

void read_json(std::string filename,
               std::vector<State *> &states,
               std::vector<char> &alphabet,
               std::vector<std::string> &stackAlphabet,
               std::vector<Transition *> &transition_function,
               State *&start_state,
               std::string &start_stack,
               std::vector<State *> &final_states) {

    // Source: https://stackoverflow.com/questions/18107079/rapidjson-working-code-for-reading-document-from-file
    // Read the file
    ifstream pda_stream(filename);
    rapidjson::IStreamWrapper pda_stream_wrapper(pda_stream);
    rapidjson::Document pda_document;
    pda_document.ParseStream(pda_stream_wrapper);

    start_stack = pda_document["StartStack"].GetString();

    // Alphabet
    rapidjson::Value &alphabet_value = pda_document["Alphabet"];

    alphabet = {};

    for (rapidjson::SizeType i = 0; i < alphabet_value.Size(); i++) {
        std::string s = alphabet_value[i].GetString();
        char c = s[0];
        alphabet.emplace_back(c);
    }

    // Stack Alphabet
    rapidjson::Value &stack_alphabet_value = pda_document["StackAlphabet"];

    stackAlphabet = {};

    for (rapidjson::SizeType i = 0; i < stack_alphabet_value.Size(); i++) {
        stackAlphabet.emplace_back(stack_alphabet_value[i].GetString());
    }

    // States, start_state, final_states
    rapidjson::Value &states_value = pda_document["States"];

    states = {};

    for (rapidjson::SizeType i = 0; i < states_value.Size(); i++) {
        states.emplace_back(new State(states_value[i].GetString()));
    }

    start_state = findState(pda_document["StartState"].GetString(), states);

    // Transitions
    rapidjson::Value &transitions_value = pda_document["Transitions"];

    transition_function = {};

    for (rapidjson::SizeType i = 0; i < transitions_value.Size(); i++) {

        rapidjson::Value &transition = transitions_value[i];

        std::vector<std::string> replacement = {};
        rapidjson::Value &replacement_value = transition["replacement"];


        for (rapidjson::SizeType i = 0; i < replacement_value.Size(); i++) {
            replacement.emplace_back(replacement_value[i].GetString());
        }

        std::string inputStr = transition["input"].GetString();
        if(inputStr == ""){
            inputStr = toString(EPSILON);
        }
        char input = inputStr[0];
        transition_function.push_back(new Transition(findState(transition["from"].GetString(), states),
                                                     findState(transition["to"].GetString(), states),
                                                     replacement,
                                                     input,
                                                     transition["stacktop"].GetString()));
    }

}
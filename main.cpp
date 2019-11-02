#include <iostream>

#include "PDA.h"
#include "CFG.h"

void evaluate(PDA &pda, const std::string &evaluateString1);

PDA voorbeeldPDA1() {
    State *q0 = new State("q0");
    State *q1 = new State("q1");
    State *qf = new State("qf");

    std::vector<State *> statesQ = {q0, q1, qf};

    Transition *q0q0a = new Transition(q0, q0, {"a", "a", "a"}, 'a', "a");
    Transition *q0q0Z = new Transition(q0, q0, {"a", "a", "Z"}, 'a', "Z");
    Transition *q0q1b = new Transition(q0, q1, {"e"}, 'b', "a");
    Transition *q1q1b = new Transition(q1, q1, {"e"}, 'b', "a");
    Transition *q1qf = new Transition(q1, qf, {"Z"}, 'e', "Z");

    std::vector<Transition *> transitionFunctionD = {q0q0a, q0q0Z, q0q1b, q1q1b, q1qf};

    std::string startStack = "Z";

    std::vector<char> inputAlphabetS = {'a', 'b'};

    std::vector<std::string> stackAlphabetG = {"a", "b", "Z"};

    State *startState = q0;

    std::vector<State *> endStatesF = {qf};


    PDA pda(statesQ, inputAlphabetS, stackAlphabetG, startState, startStack, endStatesF, transitionFunctionD);

    return pda;
}

//PDA testPDA1() {
//    State *q0 = new State("q0");
//
//    std::vector<State *> statesQ = {q0};
//
//    Transition *q0q0a = new Transition(q0, q0, "a", 'a', 'a');
//
//    std::vector<Transition *> transitionFunctionD = {q0q0a};
//
//    char startStack = 'a';
//
//    std::vector<char> inputAlphabetS = {'a'};
//
//    std::vector<char> stackAlphabetG = {'a'};
//
//    State *startState = q0;
//
//    std::vector<State *> endStatesF = {q0};
//
//
//    PDA pda(statesQ, inputAlphabetS, stackAlphabetG, startState, startStack, endStatesF, transitionFunctionD);
//
//    return pda;
//}

//PDA testPDA2() {
//    State *q0 = new State("q0");
//    State *q1 = new State("q1");
//
//    std::vector<State *> statesQ = {q0, q1};
//
//    Transition *q0q1a = new Transition(q0, q1, "b", 'a', 'a');
//
//    std::vector<Transition *> transitionFunctionD = {q0q1a};
//
//    char startStack = 'a';
//
//    std::vector<char> inputAlphabetS = {'a'};
//
//    std::vector<char> stackAlphabetG = {'a', 'b'};
//
//    State *startState = q0;
//
//    std::vector<State *> endStatesF = {q1};
//
//
//    PDA pda(statesQ, inputAlphabetS, stackAlphabetG, startState, startStack, endStatesF, transitionFunctionD);
//
//    return pda;
//}

//PDA testPDA3() {
//    State *q0 = new State("q0");
//    State *q1 = new State("q1");
//    State *q2 = new State("q2");
//
//    std::vector<State *> statesQ = {q0, q1, q2};
//
//    Transition *q0q1a = new Transition(q0, q1, "b", 'a', 'a');
//    Transition *q0q2a = new Transition(q0, q2, "a", 'a', 'b');
//
//    std::vector<Transition *> transitionFunctionD = {q0q1a, q0q2a};
//
//    char startStack = 'b'; // a is accepting, b rejecting bij string "a"
//
//    std::vector<char> inputAlphabetS = {'a'};
//
//    std::vector<char> stackAlphabetG = {'a', 'b'};
//
//    State *startState = q0;
//
//    std::vector<State *> endStatesF = {q1};
//
//
//    PDA pda(statesQ, inputAlphabetS, stackAlphabetG, startState, startStack, endStatesF, transitionFunctionD);
//
//    return pda;
//}



int main() {

    std::cout << "Hello, World!" << std::endl;

//    PDA pda = voorbeeldPDA1();
//    PDA pda = testPDA3();

    std::string filename = "pdaReeks5Oef4.json";
    std::vector<State *> states = {};
    std::vector<char> alphabet = {};
    std::vector<std::string> stackAlphabet = {};
    std::vector<Transition *> transition_function = {};
    State *start_state;
    std::string start_stack;
    std::vector<State *> final_states;

    read_json(filename,
                   states,
                   alphabet,
                   stackAlphabet,
                   transition_function,
                   start_state,
                   start_stack,
                   final_states);

    PDA pda = PDA(states, alphabet, stackAlphabet, start_state, start_stack, final_states, transition_function);

    std::string evaluateString1 = "aabbbb";
    std::string evaluateString2 = "aab";

    evaluate(pda, evaluateString1);
    evaluate(pda, evaluateString2);

    pda.toDot("pda");

    CFG* cfg = pda.toCFG(); // TODO: check on too many productions
    cfg->toJSON("pdaToCFG.json");

    std::vector<std::string> nonTerminalsV;
    std::vector<std::string> terminalsT;
    std::vector<Production *> productionsP;
    std::string startS;
    readJson("Reeks7Oef2.json", nonTerminalsV, terminalsT, productionsP, startS);

    cfg = new CFG(nonTerminalsV, terminalsT, productionsP, startS);

    cfg->toCNF();
    cfg->toJSON("cfgToCNF.json");

    pda.toCFG();

    return 0;
}

void evaluate(PDA &pda, const std::string &evaluateString1) {
    switch (pda.evaluate(evaluateString1)) {
        case true:
            std::cout << "String " << evaluateString1 << " accepted" << std::endl;
            break;
        case false:
            std::cout << "String " << evaluateString1 << " rejected" << std::endl;
            break;
    }
}

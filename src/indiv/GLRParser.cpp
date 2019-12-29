//
// Created by arno on 20.11.19.
//

#include "GLRParser.h"

#include <algorithm>
#include <stack>
#include <string>
#include <utility>
#include <fstream>

#include "CFG.h"
#include "functions.h"
#include "Graph.h"

#define VERBOSE false

GLRParser::GLRParser(CFG *cfg, bool verbose) : verbose(verbose) {

    cfg->splitUpLongerTerminals();

    // Convert vector to set
    for (const auto &c: cfg->getNonTerminalsV()) {
        nonTerminalsV.insert(c);
    }
    for (const auto &c: cfg->getTerminalsT()) {
        terminalsT.insert(c);
    }

    // TODO: fix all terminals only 1 char
    std::vector<Production *> productionsP = cfg->getProductionsP();
    std::string startS = cfg->getStartS();

    int stateNr = 1;

    // Start by creating a new start state
    std::string oldStartState = startS;
    startS = startStateName();
    stateNr++;
    nonTerminalsV.insert(startS);
    // and letting this start state point to the original one
    auto *start = new Production(startS, {oldStartState});
    // (This ensures the start state appears in only one rewrite rule)
    productionsP.push_back(start);

    acceptState = new GLRState("accept", {new Production("accept", {startS})});

    // Introduce the marker ^ in all productions
    std::set<Production *> markedProductions = {};
    for (auto production: productionsP) {
        const std::vector<std::string> &prodTo = production->getToP();
        for (int i = 0; i <= prodTo.size(); ++i) { // TODO: Check whether <= doesn't give an overflow error
            std::vector<std::string> prodToMarked = prodTo;
            prodToMarked.insert(prodToMarked.begin() + i, getMarker());
            auto *markedProduction = new Production(production->getFromP(), prodToMarked);
            markedProductions.insert(markedProduction);
            if (production->getFromP() == startS && i == 0) {
                start = markedProduction;
            }
        }
    }


    // 1. Form the closure of the set containing the single marked rule S' -> ^S. Establish this set as the initial ste
    //     in the transition diastagram.
    // Start state is the new start
    std::set<Production *> startClosure = {start};
    while (closure(startClosure, markedProductions)) {}
    auto *newState = new GLRState(startS, startClosure);
    states.insert(newState);
    startState = newState;
//    std::set<Production *> establishedRulesInState = startClosure;
    // States is the vector containing all established states
//    std::set<Production *> processedProductions = {};


    // 2. While possible without redundancy do the following:
    while ("Possible without redundancy") { // Redundant once all productions are processed
        // a. Select a symbol s (terminal or nonterminal) appearing emmediately to the right of the marker in a rule in
        //     some established state A.
        std::string s;
        GLRState *stateA{};
        for (int i = 0; i < states.size(); ++i) {
            // Get the i-th element
            auto glrStateItr = states.begin();
            std::advance(glrStateItr, i);
            GLRState *glrState = *glrStateItr;

            if (glrState->getProductions().size() <= glrState->getProdEstablished().size()) {
                continue;
            }

//            std::cout << glrState->getName() << " has " << glrState->getProdEstablished().size() << " of " << glrState->getProductions().size()  << " productions established."<< std::endl;

            auto productionItr = glrState->getProductions().begin();
            Production *production;
            while (true) {
                // Check whether the current production is already established
                if (std::find(glrState->getProdEstablished().begin(), glrState->getProdEstablished().end(),
                              *productionItr) == glrState->getProdEstablished().end()) {
                    production = *productionItr;
                    glrState->increaseEstablished(production);
                    break;
                } else {
                    // TODO: fix stuck in Moved loop
//                    std::cerr << "Moved" << std::endl;
                    productionItr++;
                }
            }


            s = symbolAfterMarker(production);
            // Marker at end
            if (s.empty()) {
                i--;
                continue;
            }
            stateA = glrState;
            break;

        }
        if (s.empty()) {
//            std::cout << "No more found." << std::endl;
            break;
        }

        // b. Let X denote the collection of all marked rules in A that have s immediately to the right of their markers.
        std::set<Production *> sImmRightX;
        for (Production *production: stateA->getProductions()) {
            if (s == symbolAfterMarker(production)) {
                sImmRightX.insert(production);
            }
        }

        // c. Let Y be the set of marked rules obtained by moving the marker in each rule in X to the right of the symbol s.
        std::set<Production *> sMovedRightOfX;
        for (Production *production: sImmRightX) {
            for (Production *prod: markedProductions) {
                if (isMarkerMovedRight(production, prod)) {
                    sMovedRightOfX.insert(prod);
                }
            }
        }

        // d. If the closure of Y has not yet been established as a state, do so now.

        // Find a new name for the variable
//        char newVariable = 'A';
//        while (inVector(toString(newVariable), nonTerminalsV)) { newVariable++; }
        std::string var = "STATE#" + std::to_string(stateNr++);
        nonTerminalsV.insert(var);

        // Calculate the closure
        while (closure(sMovedRightOfX, markedProductions)) {}

        // Find another state with this closure
        GLRState *closureY{};
        bool stateExists = false;
        for (GLRState *s2: states) {
            if (s2->getProductions() == sMovedRightOfX) {
                var = s2->getName();
                stateExists = true;
                closureY = s2;
            }
        }

        if (!stateExists) {
            closureY = new GLRState(var, sMovedRightOfX);
            states.insert(closureY);
        }

        // e. Draw an arc labeled s from state A to closure of Y
        stateA->addStateTo(closureY, s);
//        std::string filename = "output/yeet" + var + ".dot";
//        toDot(filename);
    }

    // 3. Let each state represented by marked rules in terminal form be accept states in the automaton.
    for (auto state: states) {
        bool containsTerminalProduction = false; // TODO: check containsProduction or allProductions
        for (auto rule: state->getProductions()) {
            auto prodTo = rule->getToP();
            if (!prodTo.empty() && prodTo[prodTo.size() - 1] == getMarker()) {
                containsTerminalProduction = true;
            }
        }
        if (containsTerminalProduction) {
            state->setAccepting(true);
        }
    }

//    toDot("output/GLRParserTransitionScheme.dot");
//    buildTable();
//    printTable();
}

std::string GLRParser::symbolAfterMarker(const Production *production) {
    std::string symbol;
    bool previousmarker = false;
    for (const std::string &var: production->getToP()) {
        if (previousmarker) {
            return var;
        }
        previousmarker = (var == getMarker());
    }
    return symbol;
}

bool GLRParser::isMarkerMovedRight(Production *originalProduction, Production *productionMovedRight) {
    const auto &originalProductionTo = originalProduction->getToP();
    const auto &productionMovedRightTo = productionMovedRight->getToP();

    if (originalProductionTo.size() != productionMovedRightTo.size() ||
        originalProduction->getFromP() != productionMovedRight->getFromP() ||
        originalProductionTo == productionMovedRightTo) {
        return false;
    }

    bool previousMarker = false;
    std::string previoussymbol;
    for (int i = 0; i < originalProductionTo.size(); ++i) {
        if (originalProductionTo.at(i) != productionMovedRightTo.at(i)) {

            if (originalProductionTo.at(i) == getMarker()) {
                // Case: This is the original marker
                previousMarker = true;
                previoussymbol = productionMovedRightTo.at(i);

            } else if (previousMarker &&
                       previoussymbol == originalProductionTo.at(i) &&
                       productionMovedRightTo.at(i) == getMarker()) {
                // Case: This is the moved marker
                previoussymbol = "";
                previousMarker = false;

            } else {
                return false;
            }
        }
    }
    return true;
}

bool GLRParser::closure(std::set<Production *> &markedProductions, std::set<Production *> &allMarkedProductions) {
    std::set<std::string> terminalsImmRightOfMarker;
    std::set<Production *> followingTerminalInit;
    int originalMarkedProductionsSize = markedProductions.size();
    for (auto production: markedProductions) {
        std::vector<std::string> prodTo = production->getToP();
        auto it = std::find(prodTo.begin(), prodTo.end(), getMarker());
        // find the index
        int index = std::distance(prodTo.begin(), it);

        // Find the character to the immidiate right
        if (index + 1 < prodTo.size()) {
            // Check whether it's a terminal and whether it isn't included yet in the list
            std::string nextChar = prodTo.at(index + 1);
            if (std::find(terminalsT.begin(), terminalsT.end(), nextChar) != terminalsT.end() &&
                std::find(terminalsImmRightOfMarker.begin(), terminalsImmRightOfMarker.end(), prodTo.at(index + 1)) ==
                terminalsImmRightOfMarker.end()) {
                terminalsImmRightOfMarker.insert(nextChar);
//                changed = true;
            } else if (std::find(nonTerminalsV.begin(), nonTerminalsV.end(), nextChar) != nonTerminalsV.end()) {
                std::set<Production *> initProductionsFromVar = initialProductionsFromVar(nextChar,
                                                                                          allMarkedProductions);
//                for(auto prod: initProductionsFromVar){
//                    followingTerminalInit.insert(prod);
//                }
                set_union(followingTerminalInit.begin(), followingTerminalInit.end(), initProductionsFromVar.begin(),
                          initProductionsFromVar.end(),
                          std::inserter(followingTerminalInit, followingTerminalInit.begin()));
            }
        }
    }

    set_union(markedProductions.begin(), markedProductions.end(), followingTerminalInit.begin(),
              followingTerminalInit.end(), std::inserter(markedProductions, markedProductions.begin()));

    // "Then, we add to the set the initial forms of all rewrite rules in the grammar whose left-hand sides consist of
    // those nonterminals as well." -> What do they mean with consists of? Contains all of them? Only contains those? ...
    for (auto production: allMarkedProductions) {
        for (const auto &c: production->getToP()) {
            std::string str;
            if (std::find(terminalsImmRightOfMarker.begin(), terminalsImmRightOfMarker.end(), c) ==
                terminalsImmRightOfMarker.end()) {
                // The char is not in the list with specified terminals
                break;
            }
            if (c == getMarker()) {
                // this must only be the production of the form of the current production, not all first productions
                auto prodTo1 = production->getToP();
                prodTo1.erase(std::find(prodTo1.begin(), prodTo1.end(), getMarker()));
                std::set<Production *> init = initialProductionsFromVar(production->getFromP(), allMarkedProductions);
                for (auto prod: init) {
                    auto prodTo2 = prod->getToP();
                    prodTo2.erase(std::find(prodTo2.begin(), prodTo2.end(), getMarker()));
                    if (prodTo1 == prodTo2) { // TODO: check if still working (changed from if true)
                        markedProductions.insert(production);
//                        changed = true;
                    }
                }
                break;
            }
            str += c;
        }
    }

    // return whether anything has been changed

    return originalMarkedProductionsSize != markedProductions.size();
}

std::set<Production *>
GLRParser::initialProductionsFromVar(const std::string &var, std::set<Production *> &allMarkedProductions) {
    std::set<Production *> initProdsFromVar = {};
    for (auto production: allMarkedProductions) {
        if (production->getFromP() == var &&
            !production->getToP().empty() && production->getToP()[0] == getMarker()) {
            initProdsFromVar.insert(production);
        }
    }
    return initProdsFromVar;
}

const std::string &GLRState::getName() const {
    return name;
}

const std::set<Production *> &GLRState::getProductions() const {
    return productions;
}

const std::set<GLRTransition *> &GLRState::getStatesTo() const {
    return statesTo;
}

GLRState::GLRState(std::string name, std::set<Production *> productions) : name(std::move(name)),
                                                                           productions(std::move(productions)),
                                                                           accepting(false) {
//    prodEstablished = productions.size();
}

void GLRState::addStateTo(GLRState *glrState, const std::string &label) {
    auto *transition = new GLRTransition(label, glrState);
    statesTo.insert(transition);
}

bool GLRState::isAccepting() const {
    return accepting;
}

void GLRState::setAccepting(bool accepting) {
    GLRState::accepting = accepting;
}


void GLRState::increaseEstablished(Production *prod) {
    prodEstablished.insert(prod);
}

std::set<GLRState *> GLRState::statesOnInput(const std::string &input) {
    std::set<GLRState *> statesOnInp;
    for (GLRTransition *state: statesTo) {
        if (state->getLabel() == input) {
            statesOnInp.insert(state->getStateTo());
        }
    }
    return statesOnInp;
}

const set<Production *> &GLRState::getProdEstablished() const {
    return prodEstablished;
}

//std::string GLRState::toStr() const {
//    std::string uniqueID = uniqueID;
//    std::string returnStr = returnStr;
//
//    // name
//    returnStr += "#STATE.NAME#\n";
//    returnStr += name + "\n";
//    returnStr += "#STATE.ACCEPTING#\n";
//    // accepting
//    if (accepting) {
//        returnStr += "1\n";
//    } else {
//        returnStr += "0\n";
//    }
//    returnStr += "#STATE.PRODUCTIONS#\n";
//    // productions
//    for (auto production: productions) {
//        returnStr += "#STATE.PRODUCTION#\n" + production->toStr() + "\n";
//    }
//    returnStr += "#STATE.STATESTO#";
//    if (!statesTo.empty()) {
//        returnStr += "\n";
//    }
//
//    // Statesto
//    int i = 0;
//    for (auto state: statesTo) {
//        i++;
//        returnStr += "#STATE.STATETO#\n" + state->getStateTo()->getName() + "\n#LABEL#\n" + state->getLabel();
//        if (i != statesTo.size()) {
//            returnStr += "\n";
//        }
//    }
//    return returnStr;
//}

bool GLRState::operator==(const GLRState &b) const {
    return name == b.getName();
}

bool GLRState::operator!=(const GLRState &b) const {
    return name != b.getName();
}

bool GLRState::operator<(const GLRState &b) const {
    return name < b.getName();
}

bool GLRState::operator>(const GLRState &b) const {
    return name > b.getName();
}

bool GLRState::operator<=(const GLRState &b) const {
    return name <= b.getName();
}

bool GLRState::operator>=(const GLRState &b) const {
    return false;
}

//void GLRParser::addState(GLRState *s) {
//    states.insert(s);
//}

void GLRParser::toDot(std::string filename) {
    Graph graph = Graph();
    graph.setRankdir("TB");
    for (auto state: states) {
        // TODO: getProductions()
        std::string stateLable = "{" + state->getName() + "|";
        bool firstElem = true;
        for (auto prod: state->getProductions()) {
            if (!firstElem) { stateLable += "\\n"; }
            firstElem = false;

            stateLable += prod->getFromP();
            stateLable += " -\\> ";
            for (const auto &el: prod->getToP()) {
                stateLable += el;
            }
        }

        if (state->isAccepting()) {
            stateLable += "|accepting state";
        }

        stateLable += "}";
        graph.addNode(Node(state->getName(), stateLable, "Mrecord"));

        for (auto transition: state->getStatesTo()) {
            const std::string &connectionString = transition->getLabel();
            std::string stateTo = transition->getStateTo()->getName();
            graph.addConnection(Connection(state->getName(), stateTo,
                                           connectionString));
        }
    }


    graph.addNode(Node("invis", "invis", "circle", "style=invis"));
    graph.addConnection(Connection("invis", startStateName(), "Start"));
    graph.build_file(std::move(filename));
}

//void GLRParser::buildTable() {
//    // TODO
//    // Nummers in tabel zijn verschillende state
//    // ingevulde dinges zijn dan verschillende producties
//    // Komt wel goed
//    // xoxo Arno van gisteren
//    for (auto state: states) {
//        if (!state->isAccepting()) {
//            for (auto nextChar: state->getStatesTo()) {
//
//                std::string input = nextChar->getLabel();
//                GLRState *newState = nextChar->getStateTo();
//
//                auto *parseOperation = new ParseOperation(newState);
//                std::pair<std::string, GLRState *> searchKey{input, state};
//                std::set<ParseOperation *> parseOperations = parseTable[searchKey]; // TODO: What if searchkey hasn't been introduced yet? does this return an empty set?
//
//                parseOperations.insert(parseOperation);
//
//                // TODO: What if there already exists an element with this searchkey? (I want it to be replaced then)
//                parseTable[searchKey] = parseOperations; // TODO: In what case can there be more than 1 ParseOperation?
//            }
//        } else if (state->isAccepting()) {
//            for (auto prod: state->getProductions()) {
//                if (prod->getToP()[prod->getToP().size() - 1] ==
//                    getMarker()) { // TODO: what todo with the nonFinal productions
//                    //. https://www.geeksforgeeks.org/follow-set-in-syntax-analysis/
//                    auto *parseOperationb = new ParseOperation(prod);
////                    std::pair<std::string, GLRState *> searchKeyb{input, newState};
////                    std::set<ParseOperation *> parseOperationsb = parseTable[searchKey];
////                        if(parseOperations == nullptr){
////                            parseOperations = new std::set<ParseOperation*>;
////                        }
////                    parseOperationsb.insert(parseOperationb);
////                    parseTable[searchKeyb] = parseOperationsb;
////                        parseTable.insert(
////                                std::pair<std::pair<std::string, GLRState *>, std::set<ParseOperation *>>(searchKeyb,
////                                                                                                          parseOperationsb));
//
//                }
//
//            }
//        }
//    }
//}

//void GLRParser::printTable() {
//    std::cout << "From (state, input) to newState(shift) or ReduceOp" << std::endl;
//    for (const auto &el: parseTable) {
//        auto searchKey = el.first;
//        auto opSet = el.second;
//
//        std::cout << "From (" << searchKey.second->getName() << ", " << searchKey.first << ") to ";
//
//        for (auto op: opSet) {
//            if (op->getOperationType() == op->shift) {
//                std::cout << op->getNewState()->getName() << " ";
//            } else if (op->getOperationType() == op->reduce) {
//                std::cout << op->getReduceProduction()->getFromP() << "->";
//                for (const auto &reduceTo: op->getReduceProduction()->getToP()) {
//                    std::cout << reduceTo;
//                }
//                std::cout << " ";
//            }
//        }
//
//        std::cout << endl;
//    }
//}

// ParseString using the transition scheme
set<stack<Production *>> GLRParser::parseString(const std::string &toParse) {

    std::vector<std::pair<std::stack<std::string>, std::stack<Production *>>> possibleParseStacks; // Must be a vector to be able to append at the end

    std::stack<std::string> startStack;
    startStack.push(startState->getName());
    possibleParseStacks.push_back(std::pair<std::stack<std::string>, std::stack<Production *>>(startStack, {}));

    bool finalStack = false;

    std::string succes;

    int position = 0;
    for (auto c: toParse) {
        position++;

//        if (c == ' ' or c == '\n') { continue; }

        std::vector<std::pair<std::stack<std::string>, std::stack<Production *>>> newPossibleParseStacks;
        for (auto &stack : possibleParseStacks) {
            // Can't use auto iterator, because we're inserting elements during the loop

            if (stack.first.top() == "accept" and position == toParse.size()) { break; }
            GLRState *currentState = findState(stack.first.top());

            //            std::pair<std::string, GLRState *> searchKey{toString(c), currentState};
//            std::set<ParseOperation *> parseOperations = parseTable[searchKey];

            set<ParseOperation *> parseOperations = findParseOptions(toString(c), currentState);

            for (auto parseOperation: parseOperations) {

                std::pair<std::stack<std::string>, std::stack<Production *>> newStack = stack;

                if (parseOperation->getOperationType() == ParseOperation::shift) {

                    newStack.first.push(toString(c));
                    newStack.first.push(parseOperation->getNewState()->getName());
                    newPossibleParseStacks.push_back(newStack);

                } else {
                    continue; // Don't push the stack back
                }

            }
        }
        possibleParseStacks = newPossibleParseStacks;

        if (!possibleParseStacks.empty()) {
            succes += c;
        }

        if (verbose) {
            std::cout << std::endl << "Output after processing " << toString(c) << std::endl;
            printStackSet(possibleParseStacks);
        }

        std::set<std::pair<std::stack<std::string>, Production *>> alreadyReducedStacks; // S -> a, S->ab, after input a, you only want one of the stacks to be reduced
        // Check the production rules in the new added stacks
        for (int i = 0; i < possibleParseStacks.size(); ++i) {
            const std::pair<std::stack<std::string>, std::stack<Production *>> stack = possibleParseStacks[i];
            GLRState *currentState = findState(stack.first.top());
            std::stack<Production *> productions = stack.second;
            if (checkProductionRules(possibleParseStacks, stack.first, productions, currentState,
                                     alreadyReducedStacks, finalStack)) {
                if (position == toParse.size()) {
                    finalStack = true;
                    break;
                } else {
                    continue;
                }
            }
        }

        removeDuplicates(possibleParseStacks);

        if (verbose) {
            std::cout << "Output after reducing and eleminating doubles: " << std::endl;
            printStackSet(possibleParseStacks);
        }
    }

//    bool final = false;
    std::set<std::stack<Production *>> finals;
    for (auto stack: possibleParseStacks) {
        // TODO: Check: is it always the case when top == 1, the string is accepted?
        if (stack.first.top() == "accept") {
//                final = true;
            finals.insert(
                    stack.second); // TODO: Check whether it still returns and the object doesn't get destroyed when leaving the function
            std::cout << "String accepted." << std::endl;
        }
    }

    if (finals.empty()) {
        std::cout << "Something went wrong, probably after this text: " << std::endl;
        std::cout << succes << std::endl;
    }

    return finals;
}

void
GLRParser::removeDuplicates(
        vector<std::pair<std::stack<std::string>, std::stack<Production *>>> &possibleParseStacks) const {// if there are two identical stacks, remove one of them
    for (int j = 0; j < possibleParseStacks.size(); ++j) {
        if (count(possibleParseStacks.begin(), possibleParseStacks.end(), possibleParseStacks[j]) > 1) {
            possibleParseStacks.erase(possibleParseStacks.begin() + j);
        }
    }
}

bool
GLRParser::checkProductionRules(
        vector<pair<std::stack<std::string>, std::stack<Production *>>> &possibleParseStacks,
        const std::stack<std::string> &stack,
        const std::stack<Production *> &productions,
        const GLRState *currentState,
        set<pair<std::stack<std::string>, Production *>> &alreadyReducedStacks,
        bool final) {// Check for possible reduces

    bool finalStack = false;
    if (currentState->isAccepting()) {
        // TODO: push production on stack
        set<ParseOperation *> parseOperations;

        // Find the possible reduces
        for (auto prod: currentState->getProductions()) {
            if (prod->getToP()[prod->getToP().size() - 1] == getMarker() &&
                alreadyReducedStacks.find(std::pair<std::stack<std::string>, Production *>(stack, prod)) ==
                alreadyReducedStacks.end()) {
                parseOperations.insert(new ParseOperation(prod));
                alreadyReducedStacks.insert(std::pair<std::stack<std::string>, Production *>(stack, prod));
            }
        }

        for (ParseOperation *parseOperation: parseOperations) {
            std::stack<std::string> newStack = stack;
            std::stack<Production *> newProductions = productions;

            // Pop the left side of the production from the stack
            for (int i = 0; i < parseOperation->getReduceProduction()->getToP().size() - 1; ++i) {
                // pop all elements from the stack (exclude the marker)
                newStack.pop(); // pop the state marker
                newStack.pop(); // pop the string element
            }

            // Push the right side and determine the new top state
            string newVariable = parseOperation->getReduceProduction()->getFromP();
            GLRState *currentTopState = findState(newStack.top());
            newStack.push(newVariable);
            newProductions.push(parseOperation->getReduceProduction());

            set<ParseOperation *> parseOperationsb = findParseOptions(newVariable, currentTopState);

            for (auto stackOp: parseOperationsb) {

                // For every new possible stack, create a new state
                std::stack<string> newNewStack = newStack;
                std::string stateName;
                if (stackOp->getOperationType() == ParseOperation::accept) {
//                    if (!final) {
//                        continue;
//                    }
//                    std::stack<std::string> acceptStack;
//                    acceptStack.push("accept");
//                    possibleParseStacks.emplace_back(acceptStack,
//                                                     newProductions);
                    finalStack = final;
                    stateName = "accept";
                } else {
                    stateName = stackOp->getNewState()->getName();
                }
                newNewStack.push(stateName);
                // Add this to the possibleParseStacks, so it can still be evaluated this round. (because this stack hasn't yet consumed char c)
                possibleParseStacks.emplace_back(newNewStack,
                                                 newProductions); // i (on which we're iterating) mustn't decrease, because we're checking the size & inserting at the end

//                cout << "Reduced, new stack: ";
//                printStack(newNewStack);
//                cout << endl;
            }
        }
    }


    return finalStack;
}

void
GLRParser::printStackSet(
        const vector<pair<std::stack<std::string>, std::stack<Production *>>> &possibleParseStacks) const {
    for (const auto &stack: possibleParseStacks) {
        printStack(stack.first);
        cout << endl;
    }
    cout << endl;
}

set<ParseOperation *>
GLRParser::findParseOptions(const std::string &inputChar, const GLRState *currentState) const {

    set<ParseOperation *> parseOperations;

    if (currentState->getName() == startStateName() && inputChar == startState->getName()) {
        parseOperations.insert(new ParseOperation(new Production("accept", {startStateName()})));
    }

    for (auto stateTo: currentState->getStatesTo()) {
        if (stateTo->getLabel() == inputChar) {
            parseOperations.insert(new ParseOperation(stateTo->getStateTo()));

//            if (stateTo->getStateTo()->isAccepting()) {
//                for (auto prod: stateTo->getStateTo()->getProductions()) {
//                    if (prod->getToP()[prod->getToP().size() - 1] == getMarker()) {
//                        parseOperations.insert(new ParseOperation(prod));
//                    }
//                }
//            }

        }
    }
    return parseOperations;
}


GLRState *GLRParser::findState(const std::string &stateName) {
//    return *std::find(states.begin(), states.end(), stateName);
    if (stateName == "accept") {
        return acceptState;
    }


    for (GLRState *state: states) {
        if (state->getName() == stateName) {
            return state;
        }
    }


    return nullptr;
}

set<string> GLRParser::getNonTerminalsV() const {
    return nonTerminalsV;
}
//
//void GLRParser::writeToFile(const std::string& filename) {
//    std::string uniqueTag = uniqueTag;
//    std::string nonTerminalsTag = uniqueTag + "#NONTERMINALS#";
//    std::string terminalTag = uniqueTag + "#TERMINALS#";
//    std::string statesTag = uniqueTag + "#STATES#";
//    std::string acceptStateTag = uniqueTag + "#ACCEPTSTATE#";
//    std::string startStateTag = uniqueTag + "#STARTSTATE#";
//    std::string endTag = "#END#";
//    std::string startTag = "#BEGIN#";
//
//    ofstream outputFile;
//    outputFile.open(filename);
//
//    // Nonterminals
//    outputFile << nonTerminalsTag << std::endl;
//    for (const auto& var: nonTerminalsV) {
//        outputFile << var << std::endl;
//    }
//
//    // Terminals
//    outputFile << terminalTag << std::endl;
//    for (const auto& var: terminalsT) {
//        outputFile << var << endl;
//    }
//    // States
//    int i = 0;
//    outputFile << statesTag << std::endl;
//    for (auto var: states) {
//        i++;
//        outputFile << var->toStr() << std::endl;
//    }
//
//    // Acceptstate
//    outputFile << acceptStateTag << std::endl;
//    outputFile << acceptState->toStr() << std::endl;
//    // StartState
//    outputFile << startStateTag << std::endl;
//    outputFile << startState->getName();
//
//    outputFile.close();
//}

//GLRParser::GLRParser(const std::string& filename, bool verbose) : verbose(verbose) {
//    // Source: https://stackoverflow.com/questions/7868936/read-file-line-by-line-using-ifstream-in-c
//    std::ifstream input(filename);
//
//    std::string stateName;
//    bool stateAccepting = false;
//    std::set<Production *> productions;
//    std::string productionFrom;
//    std::vector<std::string> prodTo;
//
//    std::string lastRead = lastRead;
//    for (std::string line; getline(input, line);) {
//        // TODO: Remove \n from line
//        if (line == "#NONTERMINALS#" or line == "#TERMINALS#" or line == "#STATES#" or line == "#STATE.NAME#" or
//            line == "#STATE.ACCEPTING#" or line == "#STATE.PRODUCTIONS#" or line == "#STATE.PRODUCTION#" or
//            line == "#PROD.FROM#" or line == "#PROD.STATES#" or line == "#PROD.STATE#" or line == "#STARTSTATE#" or
//            line == "#ACCEPTSTATE#") {
//
//            if ((lastRead == "#PROD.FROM#" or lastRead == "#PROD.STATES#" or lastRead == "#PROD.STATE#")
//                and line == "#STATE.PRODUCTION#") {
//                // Process the previous production
//                productions.insert(new Production(productionFrom, prodTo));
//                prodTo = {};
//                productionFrom = "";
//            }
//
//            lastRead = line;
//            continue;
//        } else if (lastRead == "#NONTERMINALS#") {
//            terminalsT.insert(line);
//        } else if (lastRead == "#TERMINALS#") {
//            terminalsT.insert(line);
//        } else if (lastRead == "#STATES#") {
//            // This shouldn't be possible, since this is immediately followed by a state.name
//        } else if (lastRead == "#STATE.NAME#") {
//            stateName = line;
//        } else if (lastRead == "#STATE.ACCEPTING#") {
//            if (line == "0") {
//                stateAccepting = false;
//            } else if (line == "1") {
//                stateAccepting = true;
//            }
//        } else if (lastRead == "#STATE.PRODUCTIONS#") {
//            // This shouldn't be possible, since this is immediately followed by a state.production
//        } else if (lastRead == "#STATE.PRODUCTION#") {
//            // This shouldn't be possible, since this is immediately followed by a prod.from
//        } else if (lastRead == "#PROD.FROM#") {
//            productionFrom = line;
//        } else if (lastRead == "#PROD.STATES#") {
//            // This shouldn't be possible, since this is immediately followed by a prod.from
//        } else if (lastRead == "#PROD.STATE#") {
//            prodTo.push_back(line);
//        } else if (lastRead == "#STARTSTATE#") {
//            startState = findState(line);
//        }
//    }
//}

GLRTransition::GLRTransition(
        std::string label, GLRState
*stateTo) : label(std::move(label)), stateTo(stateTo) {}

const string &GLRTransition::getLabel() const {
    return label;
}

//void GLRTransition::setLabel(const string &label) {
//    GLRTransition::label = label;
//}

GLRState *GLRTransition::getStateTo() const {
    return stateTo;
}

//void GLRTransition::setStateTo(GLRState *stateTo) {
//    GLRTransition::stateTo = stateTo;
//}

ParseOperation::ParseOperation(Production *reduceProduction) : reduceProduction(
        reduceProduction) {
    if (reduceProduction->getFromP() == "accept") {
        operationType = accept;
    } else {
        operationType = reduce;
    }
}

ParseOperation::ParseOperation(GLRState *newState) : newState(newState) { operationType = shift; }

ParseOperation::operation ParseOperation::getOperationType() const {
    return operationType;
}

Production *ParseOperation::getReduceProduction() const {
    return reduceProduction;
}

GLRState *ParseOperation::getNewState() const {
    return newState;
}

template<class T>
void printStack(std::stack<T> stack) {
    std::string output;
//    int initSize = stack.size();
    while (!stack.empty()) {
        std::string top = stack.top();
        output = top + output;
        stack.pop();
        if (!stack.empty()) {
            output = ", " + output;
        }

    }
    std::cout << output;
}


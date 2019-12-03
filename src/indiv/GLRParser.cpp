//
// Created by arno on 20.11.19.
//

#include "GLRParser.h"

#include <algorithm>
#include <stack>
#include <string>

#include "CFG.h"
#include "functions.h"
#include "Graph.h"

GLRParser::GLRParser(CFG *cfg) {

    nonTerminalsV = cfg->getNonTerminalsV();
    terminalsT = cfg->getTerminalsT();
    std::vector<Production *> productionsP = cfg->getProductionsP();
    std::string startS = cfg->getStartS();

    int stateNr = 1;

    // Start by creating a new start state
    std::string oldStartState = startS;
    startS = std::to_string(stateNr++); // "Sb";
    nonTerminalsV.push_back(startS);
    // and letting this start state point to the original one
    Production *start = new Production(startS, {oldStartState});
    // (This ensures the start state appears in only one rewrite rule)
    productionsP.push_back(start);


    // Introduce the marker ^ in all productions
    std::set<Production *> markedProductions = {};
    for (auto production: productionsP) {
        std::vector<std::string> prodTo = production->getToP();
        for (int i = 0; i <= prodTo.size(); ++i) { // TODO: Check whether <= doesn't give an overflow error
            std::vector<std::string> prodToMarked = prodTo;
            prodToMarked.insert(prodToMarked.begin() + i, getMarker());
            Production *markedProduction = new Production(production->getFromP(), prodToMarked);
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
    while (closure(startClosure, markedProductions)) {};
    GLRState *newState = new GLRState(startS, startClosure);
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
        GLRState *stateA;
        for (int i = 0; i < states.size(); ++i) {
            // Get the i-th element
            auto glrStateItr = states.begin();
            std::advance(glrStateItr, i);
            GLRState* glrState = *glrStateItr;

            // TODO: Debug glrState->getName == "C"
            std::cout << glrState->getName() << " has " << glrState->getProdEstablished().size() << " of " << glrState->getProductions().size()  << " productions established."<< std::endl;

            if (glrState->getProductions().size() <= glrState->getProdEstablished().size()) {
                continue;
            }

            auto productionItr = glrState->getProductions().begin();
            Production* production;
            while (true){
                // Check whether the current production is already established
                if(std::find(glrState->getProdEstablished().begin(), glrState->getProdEstablished().end(), *productionItr) == glrState->getProdEstablished().end()){
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
            if (s == "") {
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
        std::string var = std::to_string(stateNr++);
        nonTerminalsV.emplace_back(var);

        // Calculate the closure
        while (closure(sMovedRightOfX, markedProductions)) {}

        // Find another state with this closure
        GLRState *closureY;
        bool stateExists = false;
        for (GLRState *s: states) {
            if (s->getProductions() == sMovedRightOfX) {
                var = s->getName();
                stateExists = true;
                closureY = s;
            }
        }

        if (!stateExists) {
            closureY = new GLRState(var, sMovedRightOfX);
            states.insert(closureY);
        }

        // e. Draw an arc labeled s from state A to closure of Y
        stateA->addStateTo(closureY, s);
//        std::string filename = "debug/yeet" + var + ".dot";
//        toDot(filename);
    }

    // 3. Let each state represented by marked rules in terminal form be accept states in the automaton.
    for (auto state: states) {
        bool containsTerminalProduction = false; // TODO: check containsProduction or allProductions
        for (auto rule: state->getProductions()) {
            auto prodTo = rule->getToP();
            if (prodTo.size() > 0 && prodTo[prodTo.size() - 1] == getMarker()) {
                containsTerminalProduction = true;
            }
        }
        if (containsTerminalProduction) {
            state->setAccepting(true);
        }
    }

    toDot("GLRParserTransitionScheme.dot");
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
        for (auto c: production->getToP()) {
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
                    if (prodTo1 == prodTo1) {
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
GLRParser::initialProductionsFromVar(std::string var, std::set<Production *> &allMarkedProductions) {
    std::set<Production *> initProdsFromVar = {};
    for (auto production: allMarkedProductions) {
        if (production->getFromP() == var &&
            production->getToP().size() > 0 && production->getToP()[0] == getMarker()) {
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

GLRState::GLRState(const std::string &name, const std::set<Production *> &productions) : name(name),
                                                                                         productions(productions),
                                                                                         accepting(false) {
//    prodEstablished = productions.size();
}

void GLRState::addStateTo(GLRState *glrState, std::string label) {
    GLRTransition *transition = new GLRTransition(label, glrState);
    statesTo.insert(transition);
}

bool GLRState::isAccepting() const {
    return accepting;
}

void GLRState::setAccepting(bool accepting) {
    GLRState::accepting = accepting;
}


void GLRState::increaseEstablished(Production* prod) {
    prodEstablished.insert(prod);
}

std::set<GLRState *> GLRState::statesOnInput(std::string input) {
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

void GLRParser::addState(GLRState *s) {
    states.insert(s);
}

void GLRParser::toDot(std::string filename) {
    Graph graph = Graph();
    graph.setRankdir("TB");
    for (auto state: states) {
        // TODO: getProductions()
        std::string stateLable = "{" + state->getName() + "|";
        bool firstElem = true;
        for (auto prod: state->getProductions()) {
            if (!firstElem) { stateLable += "\\n"; };
            firstElem = false;

            stateLable += prod->getFromP();
            stateLable += " -\\> ";
            for (auto el: prod->getToP()) {
                stateLable += el;
            }
        }

        if (state->isAccepting()) {
            stateLable += "|accepting state";
        }

        stateLable += "}";
        graph.addNode(Node(state->getName(), stateLable, "Mrecord"));

        for (auto transition: state->getStatesTo()) {
            std::string connectionString = transition->getLabel();
            std::string stateTo = transition->getStateTo()->getName();
            graph.addConnection(Connection(state->getName(), stateTo,
                                           connectionString));
        }
    }


    graph.addNode(Node("invis", "invis", "circle", "style=invis"));
    graph.addConnection(Connection("invis", "1", "Start"));
    graph.build_file(filename);
}

void GLRParser::buildTable() {
    // TODO
    // Nummers in tabel zijn verschillende state
    // ingevulde dinges zijn dan verschillende producties
    // Komt wel goed
    // xoxo Arno van gisteren
    for (auto state: states) {
        if (!state->isAccepting()) {
            for (auto nextChar: state->getStatesTo()) {

                std::string input = nextChar->getLabel();
                GLRState *newState = nextChar->getStateTo();

                ParseOperation *parseOperation = new ParseOperation(newState);
                std::pair<std::string, GLRState *> searchKey{input, state};
                std::set<ParseOperation *> parseOperations = parseTable[searchKey]; // TODO: What if searchkey hasn't been introduced yet? does this return an empty set?

                parseOperations.insert(parseOperation);

                // TODO: What if there already exists an element with this searchkey? (I want it to be replaced then)
                parseTable[searchKey] = parseOperations; // TODO: In what case can there be more than 1 ParseOperation?
            }
        } else if (state->isAccepting()) {
            for (auto prod: state->getProductions()) {
                if (prod->getToP()[prod->getToP().size() - 1] ==
                    getMarker()) { // TODO: what todo with the nonFinal productions
                    //. https://www.geeksforgeeks.org/follow-set-in-syntax-analysis/
                    ParseOperation *parseOperationb = new ParseOperation(prod);
//                    std::pair<std::string, GLRState *> searchKeyb{input, newState};
//                    std::set<ParseOperation *> parseOperationsb = parseTable[searchKey];
//                        if(parseOperations == nullptr){
//                            parseOperations = new std::set<ParseOperation*>;
//                        }
//                    parseOperationsb.insert(parseOperationb);
//                    parseTable[searchKeyb] = parseOperationsb;
//                        parseTable.insert(
//                                std::pair<std::pair<std::string, GLRState *>, std::set<ParseOperation *>>(searchKeyb,
//                                                                                                          parseOperationsb));

                }

            }
        }
    }
}

void GLRParser::printTable() {
    std::cout << "From (state, input) to newState(shift) or ReduceOp" << std::endl;
    for (auto el: parseTable) {
        auto searchKey = el.first;
        auto opSet = el.second;

        std::cout << "From (" << searchKey.second->getName() << ", " << searchKey.first << ") to ";

        for (auto op: opSet) {
            if (op->getOperationType() == op->shift) {
                std::cout << op->getNewState()->getName() << " ";
            } else if (op->getOperationType() == op->reduce) {
                std::cout << op->getReduceProduction()->getFromP() << "->";
                for (auto reduceTo: op->getReduceProduction()->getToP()) {
                    std::cout << reduceTo;
                }
                std::cout << " ";
            }
        }

        std::cout << endl;
    }
}

// ParseString using the transition scheme
bool GLRParser::parseString(std::string toParse) {

    std::vector<std::stack<std::string>> possibleParseStacks; // Must be a vector to be able to append at the end

    std::stack<std::string> startStack;
    startStack.push(startState->getName());
    possibleParseStacks.push_back(startStack);

    for (auto c: toParse) {

        std::cout << std::endl << "Output before consuming " << toString(c) << std::endl;
        printStackSet(possibleParseStacks);

        std::vector<std::stack<std::string>> newPossibleParseStacks;
        for (int i = 0; i < possibleParseStacks.size(); i++) {
            // Can't use auto iterator, because we're inserting elements during the loop

            std::stack<std::string> &stack = possibleParseStacks[i];
            GLRState *currentState = findState(stack.top());

            //            std::pair<std::string, GLRState *> searchKey{toString(c), currentState};
//            std::set<ParseOperation *> parseOperations = parseTable[searchKey];

            set<ParseOperation *> parseOperations = findParseOptions(toString(c), currentState);

            for (auto parseOperation: parseOperations) {

                std::stack<std::string> newStack = stack;

                if (parseOperation->getOperationType() == ParseOperation::shift) {

                    newStack.push(toString(c));
                    newStack.push(parseOperation->getNewState()->getName());
                    newPossibleParseStacks.push_back(newStack);

                } else {
                    continue; // Don't push the stack back
                }

            }
        }
        possibleParseStacks = newPossibleParseStacks;
        std::set<std::pair<std::stack<std::string>, Production *>> alreadyReducedStacks; // S -> a, S->ab, after input a, you only want one of the stacks to be reduced
        // Check the production rules in the new added stacks
        for (int i = 0; i < possibleParseStacks.size(); ++i) {
            const std::stack<std::string> stack = possibleParseStacks[i];
            GLRState *currentState = findState(stack.top());
            checkProductionRules(possibleParseStacks, stack, currentState, alreadyReducedStacks);
        }

        // if there are two identical stacks, remove one of them
        for (int j = 0; j < possibleParseStacks.size(); ++j) {
            if (std::count(possibleParseStacks.begin(), possibleParseStacks.end(), possibleParseStacks[j]) > 1) {
                possibleParseStacks.erase(possibleParseStacks.begin() + j);
            }
        }
    }

    bool final = false;
    for (auto stack: possibleParseStacks) {
        if (stack.top() == "1") {
            stack.pop();
            if (stack.top() == "1" && stack.size() == 1) {
                final = true;
                std::cout << "String accepted." << std::endl;
            }
        }
    }
    return final;
}

void
GLRParser::checkProductionRules(std::vector<std::stack<std::string>> &possibleParseStacks,
                                const std::stack<std::string> &stack,
                                const GLRState *currentState,
                                set<pair<std::stack<std::string>, Production *>> alreadyReducedStacks) {// Check for possible reduces

    if (currentState->isAccepting()) {

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

            set<ParseOperation *> parseOperationsb = findParseOptions(newVariable, currentTopState);

            for (auto stackOp: parseOperationsb) {

                // For every new possible stack, create a new state
                std::stack<string> newNewStack = newStack;
                if (stackOp->getOperationType() != ParseOperation::accept) {
                    newNewStack.push(stackOp->getNewState()->getName());
                }
                // Add this to the possibleParseStacks, so it can still be evaluated this round. (because this stack hasn't yet consumed char c)
                possibleParseStacks.push_back(
                        newNewStack); // i (on which we're iterating) mustn't decrease, because we're checking the size & inserting at the end

                cout << "Reduced, new stack: ";
                printStack(newNewStack);
                cout << endl;
            }
        }
    }
}

void GLRParser::printStackSet(const std::vector<std::stack<std::string>> &possibleParseStacks) const {
    for (auto stack: possibleParseStacks) {
        printStack(stack);
        cout << endl;
    }
    cout << endl;
}

set<ParseOperation *> GLRParser::findParseOptions(std::string inputChar, const GLRState *currentState) const {

    set<ParseOperation *> parseOperations;

    if (currentState == startState && inputChar == "1") {
        parseOperations.insert(new ParseOperation(new Production("accept", {"1"})));
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


GLRState *GLRParser::findState(std::string stateName) {
//    return *std::find(states.begin(), states.end(), stateName);
    for (GLRState *state: states) {
        if (state->getName() == stateName) {
            return state;
        }
    }
    return nullptr;
}

GLRTransition::GLRTransition(const std::string &label, GLRState *stateTo) : label(label), stateTo(stateTo) {}

const string &GLRTransition::getLabel() const {
    return label;
}

void GLRTransition::setLabel(const string &label) {
    GLRTransition::label = label;
}

GLRState *GLRTransition::getStateTo() const {
    return stateTo;
}

void GLRTransition::setStateTo(GLRState *stateTo) {
    GLRTransition::stateTo = stateTo;
}

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
    int initSize = stack.size();
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


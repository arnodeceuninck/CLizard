//
// Created by arno on 04.12.19.
//

#include "ASTree.h"
#include "CFG.h"
#include <algorithm>
#include <utility>

ASTree::ASTree(std::stack<Production *> &productions, const std::set<std::string> &nonTerminals, std::string newRoot) : root(newRoot) {

    if (productions.empty()) { return; }

    Production *production = productions.top();
    productions.pop();

    bool containsNonTerminals = false;
    for (const auto &character: production->getToP()) {
        if (nonTerminals.find(character) != nonTerminals.end()) {
            containsNonTerminals = true;
            break;
        }
    }

    if (!containsNonTerminals) {
        for (const auto &prod: production->getToP()) {
            if(prod == getMarker()){ continue; }
            subtrees.push_back(new ASTree(productions, nonTerminals, prod));
        }
        return;
    }

//    root = production->getFromP();

    int totProd = production->getToP().size();
    for (int j = 0; j < totProd; ++j) {

        std::string prodToJ = production->getToP()[totProd-j-1];

        if(prodToJ == getMarker()){
            continue;
        }

        if (nonTerminals.find(prodToJ) != nonTerminals.end()) {
            subtrees.insert(subtrees.begin(), new ASTree(productions, nonTerminals, prodToJ));
        }
    }

    return;
}

std::string ASTree::yield() {
    if(subtrees.size() == 0){
        // terminal
        return root;
    }
    std::string yield;
    for(auto subtree: subtrees){
        yield += subtree->yield();
    }
    return yield;
}

void ASTree::toDot(std::string filename) {

}

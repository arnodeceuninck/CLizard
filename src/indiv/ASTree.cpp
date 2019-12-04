//
// Created by arno on 04.12.19.
//

#include "ASTree.h"
#include "CFG.h"
#include <algorithm>
#include <utility>

ASTree::ASTree(std::stack<Production *> &productions, const std::set<std::string> &nonTerminals) {

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
            subtrees.push_back(new ASTree(prod));
        }
        return;
    }

    root = production->getFromP();

    for (int j = 0; j < production->getToP().size(); ++j) {
        subtrees.insert(subtrees.begin(), new ASTree(productions, nonTerminals));
    }

    return;
}

ASTree::ASTree(std::string root) : root(std::move(root)) {
}

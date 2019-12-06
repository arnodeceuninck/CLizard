//
// Created by arno on 04.12.19.
//

#include "ASTree.h"
#include "CFG.h"
#include <algorithm>
#include <utility>
#include <sstream>

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
    Graph graph = Graph();
    expandDot(graph);
    graph.build_file(filename);
}

void ASTree::expandDot(Graph &graph) {

    // Source: https://stackoverflow.com/questions/7850125/convert-this-pointer-to-string
    const void *address = static_cast<const void *>(this);
    std::stringstream ss;
    ss << address;
    std::string name = ss.str();
    name = name.substr(2, name.length() - 2);

    graph.addNode(Node(name, root, "circle"));

    for (auto subtree: subtrees) {

        const void *address2 = static_cast<const void *>(subtree);
        std::stringstream ss2;
        ss2 << address;
        std::string nameSub = ss.str();
        nameSub = nameSub.substr(2, name.length() - 2);

        graph.addConnection(Connection(name, nameSub, ""));

        subtree->expandDot(graph);
    }
}

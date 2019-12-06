//
// Created by arno on 04.12.19.
//

#include "ASTree.h"
#include "CFG.h"
#include <algorithm>
#include <utility>
#include <sstream>

ASTree::ASTree(std::stack<Production *> &productions, const std::set<std::string> &nonTerminals, std::string newRoot,
               ASTree *parent) : root(newRoot), parent(parent) {

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
            subtrees.push_back(new ASTree(productions, nonTerminals, prod, this));
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
            subtrees.insert(subtrees.begin(), new ASTree(productions, nonTerminals, prodToJ, this));
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


    std::string name = dotName(this);

    graph.addNode(Node(name, root, "circle"));

    for (auto subtree: subtrees) {

        std::string nameSub = dotName(subtree);

        graph.addConnection(Connection(name, nameSub, "yeet"));

        subtree->expandDot(graph);
    }
}

std::string ASTree::dotName(ASTree *ast) {
    // Source: https://stackoverflow.com/questions/7850125/convert-this-pointer-to-string
    const void *address = static_cast<const void *>(ast);
    std::stringstream ss;
    ss << address;
    std::string name = ss.str();

    unsigned int x;
    std::stringstream ss2;
    ss2 << std::hex << name;
    ss2 >> x;

    std::string nameNum = std::to_string(x);
    return nameNum;
}

ASTree *ASTree::getParent() const {
    return parent;
}

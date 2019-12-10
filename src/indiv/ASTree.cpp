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

//    root = production->getFromP();

    int totProd = production->getToP().size();
    for (int j = 0; j < totProd; ++j) {

        std::string prodToJ = production->getToP()[totProd-j-1];

        if(prodToJ == getMarker()){
            continue;
        }

        if (nonTerminals.find(prodToJ) != nonTerminals.end()) {
            subtrees.insert(subtrees.begin(), new ASTree(productions, nonTerminals, prodToJ, this));
        } else {
            std::stack<Production *> emptyStack;
            subtrees.insert(subtrees.begin(), new ASTree(emptyStack, nonTerminals, prodToJ, this));
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
    graph.setRankdir("TB");
    expandDot(graph);
    graph.build_file(filename);
}

void ASTree::expandDot(Graph &graph) {


    std::string name = dotName(this);

    graph.addNode(Node(name, root, "circle"));

    for (auto subtree: subtrees) {

        std::string nameSub = dotName(subtree);

        graph.addConnection(Connection(name, nameSub, ""));

        subtree->expandDot(graph);
    }
}

std::string ASTree::dotName(ASTree *ast) {
    // This generates an unique identifier
    if (!ast->getParent()) {
        return "T";
    } else {
        auto subtreesParent = ast->getParent()->getSubtrees();
        int j;
        for (j = 0; j < subtreesParent.size(); ++j) {
            if (subtreesParent[j] == ast) {
                break;
            }
        }
        return dotName(ast->parent) + std::to_string(j) + "N";
    }
}

ASTree *ASTree::getParent() const {
    return parent;
}

const vector<ASTree *> &ASTree::getSubtrees() const {
    return subtrees;
}

const string &ASTree::getRoot() const {
    return root;
}

ASTree::ASTree() : root(""), parent(nullptr), subtrees({}) {

}

void ASTree::find(string basicString, vector<ASTree *> &vector) {
    if (root == basicString) {
        vector.emplace_back(this);
    }
    for (auto child: subtrees) {
        child->find(basicString, vector);
    }
}

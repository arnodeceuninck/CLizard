//
// Created by arno on 04.12.19.
//

#ifndef MB_ASTREE_H
#define MB_ASTREE_H

#include <stack>
#include <string>
#include <vector>
#include <set>
//#include "CFG.h"

#include "Graph.h"

class Production;

class ASTree {
    std::vector<ASTree *> subtrees;
public:
    const vector<ASTree *> &getSubtrees() const;

    const string &getRoot() const;

private:
    std::string root;

    ASTree *parent;
public:
    ASTree *getParent() const;

private:

    void expandDot(Graph &g);

    std::string dotName(ASTree *ast);

public:
    std::string yield();

    void toDot(std::string filename);
    // The yield determines the string, e.g. if production Variable is a "function", yield gives the contents of this function
    // Terminal if subtrees is empty, it's a terminal, else a variable
public:
    ASTree(std::stack<Production *> &productions, const std::set<std::string> &nonTerminals, std::string newRoot,
           ASTree *parent);
};


#endif //MB_ASTREE_H

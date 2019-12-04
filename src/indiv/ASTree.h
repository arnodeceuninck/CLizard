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

class Production;

class ASTree {
    std::vector<ASTree *> subtrees;
    std::string root;

    ASTree(std::stack<Production *> &productions, const std::set<std::string> &nonTerminals);

    ASTree(std::string root);
    // The yield determines the string, e.g. if production Variable is a "function", yield gives the contents of this function
    // Terminal if subtrees is empty, it's a terminal, else a variable
};


#endif //MB_ASTREE_H

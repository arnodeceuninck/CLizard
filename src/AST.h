//
// Created by arno on 06.12.19.
//

#ifndef CLIZARD_AST_H
#define CLIZARD_AST_H

#include "indiv/ASTree.h"

//// IMPORTANT: Be sure to have executed ./compile.sh, so the grammar file is generated

class AST {
public:
    /**
     * Get the syntax tree of a c++ file
     * @param filename The filename of the c++ file of which the contents must be parsed to an AST
     */
    explicit AST(std::string filename);

    /**
     * Visualize the AST in a dot file
     * @param filename The file that will contain the dot code
     */
    void toDot(std::string filename = "AST.dot");

    std::vector<AST *> find(std::string varName);

    std::string yield();

private:
    ASTree *ast;

    AST(ASTree *ast);
};


#endif //CLIZARD_AST_H

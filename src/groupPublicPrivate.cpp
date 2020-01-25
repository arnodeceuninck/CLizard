//
// Created by arno on 17.11.19.
//

#include <fstream>
#include "groupPublicPrivate.h"
#include "AST.h"

int groupPublicPrivate(const std::vector<std::string> &inputFiles) {

    std::fstream file;

    for (int i = 0; i < inputFiles.size(); ++i) {

        file.open(inputFiles[i]);
        AST ast(inputFiles[i]);

        std::vector<AST *> asts = ast.find("class-declaration");

        for (int j = 0; j < asts.size(); ++j) {
            std::vector<std::string> publics, privates;

            ClassOperation(*asts[i], publics, privates);


            std::string dotname = "tree" + std::to_string(j) + ".dot";
            asts[j]->toDot(dotname);
        }

        std::cout << ast.yield() << std::endl;
        ast.toDot("tree.dot");

        file.close();
    }
    return 0;
}

int ClassOperation(AST &parsetree, std::vector<std::string>& publics, std::vector<std::string>& privates) {
    std::vector<std::string> *currentState = &privates;
    //std::vector<ASTree *> subtrees = parsetree.getSubtrees();
    ASTree *currentTree = parsetree.find("declaration-seq")[0]->getAst();
    while (currentTree->getRoot() == "declaration-seq") {
        currentTree = currentTree->getSubtrees()[0];
    }
    AST current(currentTree);
    if (currentTree->getSubtrees()[0]->getRoot() == "pp-statements") {
        AST *found = current.find("block-declaration")[0];
        if (currentTree->getSubtrees()[0]->getSubtrees()[0]->getRoot() == "private") {
            currentState = &privates;
        } else {
            currentState = &publics;
        }
        currentState->push_back(found->getSubtrees()[0]->yield());
    }
    currentTree = currentTree->getParent()->getParent();
    current = AST(currentTree->getSubtrees()[1]);
    while (currentTree->getRoot() != "class-declaration") {
        std::vector<AST *> declaration = current.find("block-declaration");
        std::vector<AST*> ppStatement = current.find("pp-statements");
        if(!ppStatement.empty()){
            if(ppStatement[0]->getSubtrees()[0]->yield() == "private"){
                currentState = &privates;
            } else{
                currentState = &publics;
            }
        }
        currentState->push_back(declaration[0]->yield());
        currentTree = currentTree->getParent();
        current = AST(currentTree->getSubtrees()[1]);
    }
    return 0;
}

int writeToFile(std::string name, std::fstream file, std::vector<std::string> &publics,
                std::vector<std::string> &privates) {
    std::string currentline;
    std::string wantedline = "class " + name + " {\n";
    while(currentline != wantedline){
        std::getline(file, currentline);

    }
        return 0;
}

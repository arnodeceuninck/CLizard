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
        std::string fullyield = ast.yield();

        std::vector<AST *> asts = ast.find("class-declaration");

        for (int j = 0; j < asts.size(); ++j) {
            std::vector<std::string> publics, privates;
            std::string nameOfClass = asts[j]->find("class-type")[0]->yield();
            //std::cout << asts[j]->yield() << std::endl;

            ClassOperation(*asts[i], publics, privates);
            std::string newYield = writeToFile(publics,privates,asts[j]->yield());

            fullyield.replace(fullyield.begin()+fullyield.find(nameOfClass)-6, fullyield.begin() + endOfClassFinder(nameOfClass, fullyield), newYield);

            //std::cout << fullyield << std::endl;

            //std::string dotname = "tree" + std::to_string(j) + ".dot";
            //asts[j]->toDot(dotname);
        }

        //std::cout << fullyield << std::endl;
        //ast.toDot("tree.dot");
        file.close();
        std::ofstream file2(inputFiles[i]);
        file2 << fullyield;
        file2.close();
    }
    return 0;
}

int ClassOperation(AST &parsetree, std::vector<std::string>& publics, std::vector<std::string>& privates) {
    std::vector<std::string> *currentState = &privates;
    //std::vector<ASTree *> subtrees = parsetree.getSubtrees();
    ASTree *currentTree = parsetree.find("declaration-seq")[0]->getASTree();
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

std::string writeToFile(std::vector<std::string> &publics, std::vector<std::string> &privates, std::string classString) {
    std::size_t pos = classString.find('{');
    classString.erase(classString.begin()+pos+1, classString.end());
    classString += '\n';
    for (auto & i : privates) {
        classString += '\t';
        stringNormalizer(i);
        classString += i + "\n\n";
    }
    classString += "public:\n\n";
    for (auto & j : publics) {
        classString += '\t';
        stringNormalizer(j);
        classString += j + "\n\n";
    }
    classString += "};\n";
    return classString;
}

void stringNormalizer(std::string &string) {
    std::size_t pos = string.find(';');
    string.erase(string.begin()+pos+1, string.end());
}

std::size_t endOfClassFinder(std::string name, const std::string &classtring) {
    std::size_t start = classtring.find(name);
    std::stack<char>stack;
    for (unsigned int i = start; i < classtring.size(); ++i) {
        //std::cout << classtring[i] << std::endl;
        if(classtring[i] == '{') stack.push('{');
        if(classtring[i] == '}'){
            stack.pop();
            if(stack.empty()) return i+2;
        }
    }
    return -1;
}

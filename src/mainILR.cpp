#include <iostream>
#include <vector>
#include <fstream>
#include "includeLoopRecognition.h"
#include "rearrangeFunctions.h"
#include "namingConventions.h"
#include "groupPublicPrivate.h"
#include "simplifyMathematicalExpressions.h"
#include "AST.h"

void example() {
    std::string filename = "vbn/basic/simpleCombination.cpp";
    AST ast(filename);

    std::vector<AST *> funcDefs = ast.find("function-definition");

    std::cout << "The first function in the parsed file is: " << std::endl;
    std::cout << funcDefs[0]->yield();
}

int main(int argc, char *argv[]) {

    std::cout << "Hello, World!" << std::endl;

//    example(); // Assure this one keeps working when extending the grammar

    std::string filename = "vbn/basic/testTjenne.cpp";
    AST ast(filename);

    filename = "output/parsed.dot";
    ast.toDot(filename);
}

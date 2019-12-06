#include <iostream>
#include <vector>
#include <fstream>
#include "includeLoopRecognition.h"
#include "rearrangeFunctions.h"
#include "namingConventions.h"
#include "groupPublicPrivate.h"
#include "simplifyMathematicalExpressions.h"
#include "AST.h"

int main(int argc, char *argv[]) {

    std::cout << "Hello, World!" << std::endl;

    std::string filename = "vbn/simple.cpp";
    AST ast(filename);
}

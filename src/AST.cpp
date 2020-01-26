//
// Created by arno on 06.12.19.
//

#include <fstream>
#include <sstream>
#include <ctime>

#include "AST.h"
#include "indiv/CFG.h"
#include "indiv/GLRParser.h"
#include "GLRP.h"


AST::AST(std::string filename) {

    if (!GLRP::getInstance().isLoaded()) {
        GLRP::getInstance().loadParser();
    }
    GLRParser *glrParser = GLRP::getInstance().getGlrParser();

//      glrParser->toDot("output/glrParser.dot");

    std::cout << "Parsing string..." << std::endl;

    // Get the contents of the file (Source: https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring)
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string parseString = buffer.str();

    // Parse the contents of the file
    std::set<std::stack<Production *>> parsedVersions = glrParser->parseString(parseString);

    std::cout << "Done parsing" << std::endl;


    // Analyze the result of the parsing
    if (parsedVersions.size() == 0) {
        // No possible parse combinations found
        std::cerr << "Error parsing: Incorrect code" << std::endl;
        throw std::runtime_error("Parsing error");
    } else {

        if (parsedVersions.size() > 1) {
            // Multiple parse combinations possible
            std::cerr << "Error parsing: Code has ambiguities, taking the first one" << std::endl;
        }

        // One unique parse combination found

        // Transform the vector of nonTerminals in a set (Source: https://www.techiedelight.com/convert-vector-set-cpp/)
        std::set<std::string> glrNonTerminals = glrParser->getNonTerminalsV();
        std::set<std::string> nonTerminals(glrNonTerminals.begin(), glrNonTerminals.end());

//        // Get the only element in the set
//        std::stack<Production *> productions = *(parsedVersions.begin());

        int i = 0;
        for (auto productions: parsedVersions) {
            if (productions.empty()) {
                std::cerr << "Error: Empty productions" << std::endl;
                return;
            }
            // Create an ASTree with this list of productions
            ast = new ASTree(productions, nonTerminals, productions.top()->getFromP(), nullptr);
//            filename = "output/parsed" + std::to_string(i) + ".dot";
//            ast->toDot(filename);
            i++;
        }
    }
}

void AST::toDot(std::string filename) {
    return ast->toDot(filename);
}

std::vector<AST *> AST::find(std::string varName) {
    std::vector<ASTree *> trees1;
    std::vector<AST *> trees2;
    ast->find(varName, trees1);
    for (auto tree: trees1) {
        trees2.emplace_back(new AST(tree));
    }
    return trees2;
}

AST::AST(ASTree *ast) : ast(ast) {}

std::string AST::yield() {
    return ast->yield();
}


//
// Created by arno on 06.12.19.
//

#include <fstream>
#include <sstream>

#include "AST.h"
#include "indiv/CFG.h"
#include "indiv/GLRParser.h"


AST::AST(std::string filename) {

    // Generate a CFG for the c++ language
    std::vector<std::string> nonTerminalsV;
    std::vector<std::string> terminalsT;
    std::vector<Production *> productionsP;
    std::string startS;

    readJson("grammar/cpp.json", nonTerminalsV, terminalsT, productionsP, startS);

    CFG *cfg = new CFG(nonTerminalsV, terminalsT, productionsP, startS);

    // Generate a GLRParser
    GLRParser *glrParser = new GLRParser(cfg);

    // Get the contents of the file (Source: https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring)
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string parseString = buffer.str();

    // Parse the contents of the file
    std::set<std::stack<Production *>> parsedVersions = glrParser->parseString(parseString);

    // Analyze the result of the parsing
    if (parsedVersions.size() == 0) {
        // No possible parse combinations found
        std::cerr << "Error parsing: Incorrect code" << std::endl;
    } else {

        if (parsedVersions.size() > 1) {
            // Multiple parse combinations possible
            std::cerr << "Error parsing: Code has ambiguities, taking the first one" << std::endl;
        }

        // One unique parse combination found

        // Transform the vector of nonTerminals in a set (Source: https://www.techiedelight.com/convert-vector-set-cpp/)
        std::vector<std::string> glrNonTerminals = glrParser->getNonTerminalsV();
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
            ast = ASTree(productions, nonTerminals, productions.top()->getFromP(), nullptr);
            filename = "output/parsed" + std::to_string(i);
            ast.toDot(filename);
            i++;
        }
    }
}

void AST::toDot(std::string filename) {
    return ast.toDot(filename);
}


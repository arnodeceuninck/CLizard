//
// Created by arno on 06.12.19.
//

#include <fstream>
#include <sstream>
#include <ctime>

#include "AST.h"
#include "indiv/CFG.h"
#include "indiv/GLRParser.h"


AST::AST(std::string filename) {

    // Source: https://stackoverflow.com/questions/3220477/how-to-use-clock-in-c
    std::clock_t startTime;
    double duration;

    std::cout << "Creating CFG..." << std::endl;
    startTime = std::clock();

    // Generate a CFG for the c++ language
    std::vector<std::string> nonTerminalsV;
    std::vector<std::string> terminalsT;
    std::vector<Production *> productionsP;
    std::string startS;

    readJson("grammar/Simple Grammar/cpp.json", nonTerminalsV, terminalsT, productionsP, startS);

    CFG *cfg = new CFG(nonTerminalsV, terminalsT, productionsP, startS);

    duration = (std::clock() - startTime) / (double) CLOCKS_PER_SEC;
    std::cout << "Time Required for creating CFG: " << duration << std::endl;
    std::cout << "Generating GLRParser..." << std::endl;
    startTime = std::clock();

    // Generate a GLRParser
    GLRParser *glrParser = new GLRParser(cfg);

    duration = (std::clock() - startTime) / (double) CLOCKS_PER_SEC;
    std::cout << "Time Required for creating GLRParser: " << duration << std::endl;

//    std::cout << "Generating dot file for GLRParser..." << std::endl;
//    startTime = std::clock();
//
//    glrParser->toDot("output/glrParser.dot");
//
//    duration = (std::clock() - startTime) / (double) CLOCKS_PER_SEC;
//    std::cout << "Time Required for creating dot file: " << duration << std::endl;

    std::cout << "Parsing string..." << std::endl;
    startTime = std::clock();


    // Get the contents of the file (Source: https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring)
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string parseString = buffer.str();

    // Parse the contents of the file
    std::set<std::stack<Production *>> parsedVersions = glrParser->parseString(parseString);

    duration = (std::clock() - startTime) / (double) CLOCKS_PER_SEC;
    std::cout << "Time Required for parsing string: " << duration << std::endl;


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
            filename = "output/parsed" + std::to_string(i) + ".dot";
            ast->toDot(filename);
            i++;
        }
    }
}

void AST::toDot(std::string filename) {
    return ast->toDot(filename);
}


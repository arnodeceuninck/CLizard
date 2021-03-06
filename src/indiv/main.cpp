#include <iostream>
#include <fstream>

#include "PDA.h"
#include "CFG.h"
#include "GLRParser.h"

int main(int argc, char *argv[]) {

    std::cout << "Hello, World!" << std::endl;

    if (argc < 4) { // ./indiv type input output is always required
        std::cerr << "ERROR: Not enough arguments: At least 3 required: ./indiv [type] [input] <string> [output]"
                  << std::endl;
        return 1;
    }

    std::string type = argv[1];

    if (type == "pda2cfg") {

        std::string inputFile = argv[2];

        std::vector<State *> states = {};
        std::vector<char> alphabet = {};
        std::vector<std::string> stackAlphabet = {};
        std::vector<Transition *> transition_function = {};
        State *start_state;
        std::string start_stack;
        std::vector<State *> final_states;

        read_json(inputFile,
                  states,
                  alphabet,
                  stackAlphabet,
                  transition_function,
                  start_state,
                  start_stack,
                  final_states);

        PDA pda = PDA(states, alphabet, stackAlphabet, start_state, start_stack, final_states, transition_function);

        CFG *cfg = pda.toCFG();

        std::string outputFile = argv[3];

        // Source: Nawaz (2012). How to redirect cin and cout to files. Visited on November 11, 2019 via https://stackoverflow.com/questions/10150468/how-to-redirect-cin-and-cout-to-files.
        std::ofstream out(outputFile);
        std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
        std::cout.rdbuf(out.rdbuf()); //redirect std::cout to outputFile!

        cfg->printReadable();

        std::cout.rdbuf(coutbuf); //reset to standard output again

        if(argc > 4){
            cfg->toJSON(argv[4]);
        }

    } else if (type == "cnf") {

        std::string inputFile = argv[2];

        std::vector<std::string> nonTerminalsV;
        std::vector<std::string> terminalsT;
        std::vector<Production *> productionsP;
        std::string startS;

        readJson(inputFile, nonTerminalsV, terminalsT, productionsP, startS);

        CFG* cfg = new CFG(nonTerminalsV, terminalsT, productionsP, startS);

        std::string outputFile = argv[3];

        // Source: Nawaz (2012). How to redirect cin and cout to files. Visited on November 11, 2019 via https://stackoverflow.com/questions/10150468/how-to-redirect-cin-and-cout-to-files.
        std::ofstream out(outputFile);
        std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
        std::cout.rdbuf(out.rdbuf()); //redirect std::cout to outputFile!

        cfg->toCNF();

        std::cout.rdbuf(coutbuf); //reset to standard output again

        if(argc > 4){
            cfg->toJSON(argv[4]);
        }

    } else if (type == "cyk") {

        if (argc < 5) {
            std::cerr << "Not enough arguments provided for cyk: ./indiv cyk [input] [string] [output]" << std::endl;
            return 1;
        }

        std::string inputFile = argv[2];

        std::vector<std::string> nonTerminalsV;
        std::vector<std::string> terminalsT;
        std::vector<Production *> productionsP;
        std::string startS;

        readJson(inputFile, nonTerminalsV, terminalsT, productionsP, startS);

        CFG* cfg = new CFG(nonTerminalsV, terminalsT, productionsP, startS);

        std::string evaluation = argv[3];
        std::string outputFile = argv[4];

        std::ofstream file;
        file.open(outputFile);

        std::string html;
        if (cfg->toCYK(evaluation, html)) {
            file << "The string \"" << evaluation << "\" is accepted." << std::endl;
        } else {
            file << "The string \"" << evaluation << "\" is rejected." << std::endl;
        }

        std::string outputHtmlFile = outputFile + ".html";

        file << "You can find the CYK-table in the file \"" << outputHtmlFile << "\"." << std::endl;

        file.close();

        file.open(outputHtmlFile);
        file << html;
        file.close();

    } else if (type == "lr"){
        // Example arguments: lr AppendixA.json LRoutput.txt zazabzbz

        std::string inputFile = argv[2];

        std::vector<std::string> nonTerminalsV;
        std::vector<std::string> terminalsT;
        std::vector<Production *> productionsP;
        std::string startS;

        readJson(inputFile, nonTerminalsV, terminalsT, productionsP, startS);

        CFG *cfg = new CFG(nonTerminalsV, terminalsT, productionsP, startS);

        std::string outputFile = argv[3];

        // Source: Nawaz (2012). How to redirect cin and cout to files. Visited on November 11, 2019 via https://stackoverflow.com/questions/10150468/how-to-redirect-cin-and-cout-to-files.
        std::ofstream out(outputFile);
        std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
        std::cout.rdbuf(out.rdbuf()); //redirect std::cout to outputFile!

        GLRParser *glrParser = new GLRParser(cfg);

        glrParser->writeToFile("output/parser.glp");

        if (argc > 4) {
            std::set<std::stack<Production *>> parsedVersions = glrParser->parseString(argv[4]);
            switch (parsedVersions.size()) {
                case 0:
                    std::cerr << "Error parsing: Incorrect code" << std::endl;
                    break;
                case 1:
                    if (argc > 5) {
                        std::set<std::string> nonTerminals;
                        for (auto str: glrParser->getNonTerminalsV()) {
                            nonTerminals.insert(str);
                        }
                        std::stack<Production *> productions = *(parsedVersions.begin());
                        ASTree ast{productions, nonTerminals, productions.top()->getFromP(), nullptr};
                        ast.toDot(argv[5]);
                    }
                    break;
                default:
                    std::cerr << "Error parsing: Code has ambiguities" << std::endl;
            }
        }

        std::cout.rdbuf(coutbuf); //reset to standard output again

    } else {
        std::cerr << "Type \"" << type << "\" not supported." << std::endl;
    }

    return 0;
}
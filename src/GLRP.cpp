//
// Created by arno on 29/12/2019.
//

#include "GLRP.h"
#define VERBOSE false

GLRP &GLRP::getInstance() {
    static GLRP glrParser;
    return glrParser;
}

void GLRP::loadParser() {
    if (isLoaded())
        return;

    std::cout << "Creating CFG..." << std::endl;

    // Generate a CFG for the c++ language
    std::vector<std::string> nonTerminalsV;
    std::vector<std::string> terminalsT;
    std::vector<Production *> productionsP;
    std::string startS;

    readJson("grammar/Simple Grammar/cpp.json", nonTerminalsV, terminalsT, productionsP, startS);

    CFG *cfg = new CFG(nonTerminalsV, terminalsT, productionsP, startS);

    std::cout << "Creating GLRParser..." << std::endl;
    // Generate a GLRParser
    setGlrParser(new GLRParser(cfg, VERBOSE));

//    glrParser->toDot("output/GLRParser202500126.dot");

    std::cout << "Done" << std::endl;

    setLoaded(true);

}

bool GLRP::isLoaded() {
    return loaded;
}

GLRParser *GLRP::getGlrParser() {
    return glrParser;
}

void GLRP::setLoaded(bool loaded) {
    GLRP::loaded = loaded;
}

void GLRP::setGlrParser(GLRParser *glrParser) {
    GLRP::glrParser = glrParser;
}

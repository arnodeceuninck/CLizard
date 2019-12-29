//
// Created by arno on 29/12/2019.
//

#ifndef CLIZARD_GLRP_H
#define CLIZARD_GLRP_H


#include "indiv/GLRParser.h"

class GLRP {
public:
    static GLRP &getInstance();

    GLRP(GLRP const &) = delete;

    void operator=(GLRP const &) = delete;

    void loadParser();

private:
    bool loaded;

    void setLoaded(bool loaded);

    void setGlrParser(GLRParser *glrParser);

private:
    GLRParser *glrParser;
public:
    GLRParser *getGlrParser();

    bool isLoaded();

private:
    GLRP() = default;
};


#endif //CLIZARD_GLRP_H

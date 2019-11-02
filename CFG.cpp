//
// Created by arno on 28.10.19.
//

#include "CFG.h"

Production::Production(const std::string &fromP, const std::vector<std::string> &toP) : fromP(fromP), toP(toP) {}

CFG::CFG(const std::vector<std::string> &nonTerminalsV, const std::vector<char> &terminalsT,
         const std::vector<Production *> &productionsP, const std::string &startS) : nonTerminalsV(nonTerminalsV),
                                                                                     terminalsT(terminalsT),
                                                                                     productionsP(productionsP),
                                                                                     startS(startS) {}

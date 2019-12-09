//
// Created by Tim on 11/11/2019.
//

#ifndef LL_1_CYK_H
#define LL_1_CYK_H

bool cykcheck(cfgTim* language, std::string input, bool html=false, std::string outputname="");

std::vector<std::string> makeable(cfgTim* language, std::string);

inline void htmlprint(std::string outputname, std::vector<std::vector<std::vector<std::string>>> table, std::string originalinput, bool accepting);

#endif //LL_1_CYK_H

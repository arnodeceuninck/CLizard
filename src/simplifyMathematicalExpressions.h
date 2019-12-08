//
// Created by arno on 17.11.19.
//

#ifndef CLIZARD_SIMPLIFYMATHEMATICALEXPRESSIONS_H
#define CLIZARD_SIMPLIFYMATHEMATICALEXPRESSIONS_H

#include <vector>
#include <string>

int simplifyMathematicalExpressions(const std::vector<std::string> &inputFiles);

std::vector<std::string> toPRN(std::string input);

std::vector<std::string> divideNumbersOperators(std::string input);

#endif //CLIZARD_SIMPLIFYMATHEMATICALEXPRESSIONS_H

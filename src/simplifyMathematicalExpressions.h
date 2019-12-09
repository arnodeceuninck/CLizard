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

bool isNumber(std::string input);

std::string toInfix(std::vector<std::string> input);

std::vector<std::string> simplify(std::vector<std::string> input);

bool isOperator(std::string input);

int checkPrecedence(std::string op);

#endif //CLIZARD_SIMPLIFYMATHEMATICALEXPRESSIONS_H

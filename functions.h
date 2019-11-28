//
// Created by arno on 10.10.19.
//

#ifndef MB_FUNCTIONS_H
#define MB_FUNCTIONS_H

#include <string>
#include <vector>

class State;
class Production;

bool inVector(const Production* p, const std::vector<Production*> &productions);

std::string toString(char c);
std::string toString(std::vector<std::string> strings);
bool inVector(std::string s, std::vector<std::string> vec);
std::vector<State*> vectorUnion(const std::vector<State*>& a, const std::vector<State*>& b);
//std::vector<Production*> vectorUnion(const std::vector<Production*>& a, const std::vector<Production*>& b);
std::vector<std::string> vectorUnion(const std::vector<std::string>& a, const std::vector<std::string>& b);
bool inVector(std::pair<std::string, std::string> pair, std::vector<std::pair<std::string, std::string>> pairs);

#endif //MB_FUNCTIONS_H

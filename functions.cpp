//
// Created by arno on 10.10.19.
//

#include "functions.h"

std::string toString(char c) {
    std::string s(1, c);
    return s;
}

std::string toString(std::vector<std::string> strings) {
    std::string final = "";
    for (auto s: strings) {
        final += s;
    }
    return final;
}

bool inVector(std::string s, std::vector<std::string> vec) {
    for (std::string str: vec) {
        if (str == s) {
            return true;
        }
    }
    return false;
}

std::vector<State *> vectorUnion(const std::vector<State *> &a, const std::vector<State *> &b) {

    std::vector<State *> vecUnion = {};

    for (State *state: a) {
        vecUnion.emplace_back(state);
    }

    for (State *state: b) {
        vecUnion.emplace_back(state);
    }

    return vecUnion;
}



bool inVector(std::pair<std::string, std::string> pair, std::vector<std::pair<std::string, std::string>> pairs) {
    for(auto element: pairs){
        if(element.first == pair.first &&
           element.second == pair.second){
            return true;
        }
    }
    return false;
}

std::vector<std::string> vectorUnion(const std::vector<std::string> &a, const std::vector<std::string> &b) {
    std::vector<std::string> vecUnion = {};

    for (std::string state: a) {
        vecUnion.emplace_back(state);
    }

    for (std::string state: b) {
        vecUnion.emplace_back(state);
    }

    return vecUnion;
}

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
    for(auto s: strings){
        final += s;
    }
    return final;
}

bool inVector(std::string s, std::vector<std::string> vec) {
    for(std::string str: vec){
        if(str==s){
            return true;
        }
    }
    return false;
}

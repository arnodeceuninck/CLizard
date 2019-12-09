//
// Created by Tim on 28/09/2019.
//

#include <fstream>
#include <iostream>
#include "cfgTim.h"
#include "json.hpp"

cfgTim::cfgTim(std::string input) {
    std::ifstream i (input);
    nlohmann::json parser(input);
    i >> parser;
    variables = parser["Variables"].get<std::vector<std::string>>();
    terminals = parser["Terminals"].get<std::vector<std::string>>();
    for (int j = 0; j < parser["Productions"].size(); ++j) {
        auto head = parser["Productions"].at(j)["head"].get<std::string>();
        auto body = parser["Productions"][j]["body"].get<std::vector<std::string>>();
        if(body.empty()){
            body = {"e"};
        }
        products[head].push_back(body);
    }
    start = parser["Start"].get<std::string>();
}

void cfgTim::to_string() {
    std::cout << "Output: " << std::endl;
    auto itemslists = products[start];
    std::cout << start + " -> ";
    for (auto vectors : itemslists) {
        for (auto items : vectors) {
            std::cout << items;
            if(items != vectors.end()[-1]){
                std::cout << ' ';
            }
        }
        if(vectors != products[start].end()[-1]){
            std::cout << " | ";
        }
    }

    std::cout << '\n';

    for (auto ptr = products.begin(); ptr != products.end(); ptr++) {
        if(ptr->first == start){
            continue;
        }
        std::cout << ptr->first + " -> ";
        for (auto ors = ptr->second.begin(); ors != ptr->second.end(); ors++) {
            for (auto items = ors->begin(); items != ors->end(); items++) {
                std::cout << items->c_str();
                if(*items != ors->end()[-1]) {
                    std::cout << ' ';
                }
            }
            if(*ors != ptr->second.end()[-1]){
                std::cout << " | ";
            }
        }
        std::cout << '\n';
    }
}

bool cfgTim::isTerminal(std::string input) {
    for (int i = 0; i < terminals.size(); ++i) {
        if (terminals[i] == input){
            return true;
        }
    }
    return false;
}

bool cfgTim::isVariable(std::string input) {
    for (int i = 0; i < variables.size(); ++i) {
        if (variables[i] == input){
            return true;
        }
    }
    return false;
}

std::vector<std::vector<std::string>> cfgTim::getProducts(std::string input) {
//    std::vector<std::vector<std::string>> product;
//    for( std::map<std::string, std::vector<std::vector<std::string>>>::iterator itr = products.begin(); itr != products.end(); ++itr){
//            product.emplace_back(itr->second);
//    }
    return products[input];
}

bool cfgTim::containsElement(std::string input, std::string head) {
    std::vector<std::vector<std::string>> body = products[head];
    for (int i = 0; i < body.size(); ++i) {
        for (int j = 0; j < body[i].size(); ++j) {
            if(body[i][j] == input){
                return true;
            }
        }
    }
    return false;
}

std::map<std::string, std::vector<std::vector<std::string>>> &cfgTim::getProducts() {
    return products;
}

std::string &cfgTim::getStart() {
    return start;
}

const std::vector<std::string> &cfgTim::getVariables() const {
    return variables;
}

std::vector<std::string> &cfgTim::getTerminals() {
    return terminals;
}

cfgTim::cfgTim() {}

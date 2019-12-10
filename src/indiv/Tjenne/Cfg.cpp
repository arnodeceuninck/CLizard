//
// Created by treap on 28.09.19.
//

#include "Cfg.h"
#include "json.hpp"
#include <fstream>



void Cfg::readJson(const std::string file) {
    nlohmann::json parser;
    std::fstream f;
    std::string s;
    f >> s;
    std::cout<<s<<std::endl;
    f.open(file);
    f >> parser;

    int i = 0;

    //Variables
    while(parser["Variables"][i] != nullptr){
        this->variables.push_back( parser["Variables"][i]);
        i += 1;
    }

    i = 0;
    //Terminals
    while(parser["Terminals"][i] != nullptr){
        this->terminals.push_back( parser["Terminals"][i]);
        i += 1;
    }

    i=0;
    //Productions
    while(parser["Productions"][i] != nullptr){
        Cfg::Production p;
        p.setHead(parser["Productions"][i]["head"]);
        int j = 0;
        std::vector<std::string> limbs;
        while(parser["Productions"][i]["body"][j] != nullptr){
            limbs.push_back(parser["Productions"][i]["body"][j]);
            j+=1;
        }
        p.setBody(limbs);

        this->productions.push_back(p);
        i += 1;
    }

    this->start = parser["Start"];

}



std::vector<Cfg::Production> Cfg::findByVar(std::string& var) const{
    std::vector<Cfg::Production> results;
    for(auto& prod: this->productions){
        if(var == prod.getHead()){
            results.push_back(prod);
        }
    }
    return results;
}


void Cfg::printInfo()const {
    for (auto& var: this->variables) {
        std::string s = var;
        std::vector<Cfg::Production> prods = findByVar(s);

        s += " -> ";
        for (int j = 0; j < prods.size(); ++j) {
            auto& prod = prods[j];

                for (int i = 0; i < prod.getBody().size(); ++i) {
                    std::string compare = s + prod.getBody()[i];

                    s += prod.getBody()[i] + " ";



                }
                if(prod.getBody().empty()) {
                    s += "ε ";
                }

                if(j != prods.size()-1){
                    s += "| ";

                }
        }
        std::cout<<s<<std::endl;
    }

}


///////////////Getters and setters////////////////////

const std::string &Cfg::Production::getHead() const {
    return head;
}

void Cfg::Production::setHead(const std::string &head) {
    Production::head = head;
}

const std::vector<std::string> &Cfg::Production::getBody() const {
    return body;
}

void Cfg::Production::setBody(const std::vector<std::string> &body) {
    Production::body = body;
}

//
// Created by treap on 28.09.19.
//

#ifndef UNTITLED_CFG_H
#define UNTITLED_CFG_H
#include <vector>
#include <iostream>


class Cfg {
public:
    class Production {
        std::string head;
        std::vector<std::string>  body;
    public:
        const std::string &getHead() const;

        void setHead(const std::string &head);

        const std::vector<std::string> &getBody() const;

        void setBody(const std::vector<std::string> &body);
    };

private:
    std::vector<Cfg::Production> productions;
    std::vector<std::string> terminals;
    std::vector<std::string> variables;
    std::string start;

    std::vector<Cfg::Production> findByVar(std::string& var) const;



public:
    void readJson(std::string file);
    void printInfo()const;

};


#endif //UNTITLED_CFG_H

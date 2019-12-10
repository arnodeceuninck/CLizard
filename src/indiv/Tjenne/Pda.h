//
// Created by treap on 04.10.19.
//

#ifndef UNTITLED_PDA_H
#define UNTITLED_PDA_H

#include <iostream>
#include "Node.h"
#include <vector>
#include <map>


class Pda {

    //transition class for PDA
public:



private:
std::string alphabet;
std::string stack_alphabet;
    Tjen::Node* start;
std::vector<Tjen::Node*> allNodes;
std::vector<Tjen::Node*>endNodes;
std::vector<Tjen::Node::Transition*> alltransitions;



    //functions
public:
    void addFunct(Tjen::Node*from, Tjen::Node* to, char oldStack, std::string newStack, const char input);
    void addNode(std::string name, bool start, bool end);
    void addNode(Tjen::Node* n, bool start, bool end);

    void createDot();
    void readJson(const std::string file);



    bool traverse(std::string input);
    std::vector<Tjen::Node*> findEclosure(Tjen::Node* n);


private:
    Tjen::Node* searchNode(std::string &name);

    bool checkIfDouble(std::vector<Tjen::Node*>& v, Tjen::Node*n);
    bool checkForNode(std::vector<Tjen::Node*>& v, Tjen::Node*n);
    bool checkForTrans(std::vector<Tjen::Node::Transition*>& v, Tjen::Node::Transition* t);
    std::map<const char, unsigned int> initMap(Tjen::Node*n);







    //getters, setters, constructors
public:
    Pda();

    Tjen::Node *getStart() const;

    void setAlphabet(std::string alphabet);
    void setStackAlphabet(std::string stackAlphabet);



    const std::vector<Tjen::Node *> &getEndNodes() const;

    const std::string getAlphabet() const;

    const std::vector<Tjen::Node *> &getAllNodes() const;


};


#endif //UNTITLED_PDA_H

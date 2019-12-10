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
Node* start;
std::vector<Node*> allNodes;
std::vector<Node*>endNodes;
std::vector<Node::Transition*> alltransitions;



    //functions
public:
    void addFunct(Node*from, Node* to, char oldStack, std::string newStack, const char input);
    void addNode(std::string name, bool start, bool end);
    void addNode(Node* n, bool start, bool end);

    void createDot();
    void readJson(const std::string file);



    bool traverse(std::string input);
    std::vector<Node*> findEclosure(Node* n);


private:
    Node* searchNode(std::string &name);

    bool checkIfDouble(std::vector<Node*>& v, Node*n);
    bool checkForNode(std::vector<Node*>& v, Node*n);
    bool checkForTrans(std::vector<Node::Transition*>& v, Node::Transition* t);
    std::map<const char, unsigned int> initMap(Node*n);







    //getters, setters, constructors
public:
    Pda();

    Node *getStart() const;

    void setAlphabet(std::string alphabet);
    void setStackAlphabet(std::string stackAlphabet);



    const std::vector<Node *> &getEndNodes() const;

    const std::string getAlphabet() const;

    const std::vector<Node *> &getAllNodes() const;


};


#endif //UNTITLED_PDA_H

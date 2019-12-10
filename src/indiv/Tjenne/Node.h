//
// Created by treap on 04.10.19.
//

#ifndef UNTITLED_NODE_H
#define UNTITLED_NODE_H

#include <iostream>
#include <vector>
#include <stack>


class Node {


    ///Transition class///
public:
    class Transition{
        Node* from;
        Node* to;
        char oldStack;
        std::string newstack;
        char input;

        //getters, setters, constructors
    public:

        Transition(Node* from, Node* to, char &oldStack, std::string &newStack, const char input);


        Node *getFrom() ;

        void setFrom(Node *from);

        Node *getTo() const;

        void setTo(Node *to);

        const char &getOldStack() const;

        void setOldStack(const char &oldStack);

        const std::string &getNewstack() const;

        void setNewstack(std::string &newStack);

        const char getInput() const;

        void setInput(const char input);
    };



private:
std::string name;
std::vector<Node::Transition*> transitions;
std::stack<char> stack;


public:
    Node(std::string name);
    Node(Node* n);
    void addFunct(Node::Transition *t);
    void editStack(const std::string &newTop);






    ///getters, setters, constructors
    const std::string &getName() const;

    std::stack<char> getStack() ;

    void setStack(std::stack<char> stack);

    const std::vector<Node::Transition *> &getTransitions() const;

};


#endif //UNTITLED_NODE_H

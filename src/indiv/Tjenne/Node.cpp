//
// Created by treap on 04.10.19.
//

#include "Node.h"

Tjen::Node::Node(std::string name) {
    this->name = name;
    stack.push('z');

}

Tjen::Node::Node(Node *n) {
    stack = n->getStack();
    name = n->getName();
    transitions = n->getTransitions();
}

void Tjen::Node::addFunct(Node::Transition *t) {
    this->transitions.push_back(t);
}

const std::string &Tjen::Node::getName() const {
    return name;
}

const std::vector<Tjen::Node::Transition *> &Tjen::Node::getTransitions() const {
    return transitions;
}

std::stack<char> Tjen::Node::getStack() {
    return stack;
}

void Tjen::Node::setStack(std::stack<char> stack) {
    Node::stack = stack;
}


void Tjen::Node::editStack( const std::string &newTop) {
    if(newTop == "ε"){
        stack.pop();

    }else if(newTop.size() >= 2){
        for (int i = 0; i < newTop.size()-1; ++i) {
            char t = newTop[0];
            stack.push(t);
        }

    }
}






////////////////////Transition class////////////////////////

///getters, setters, constructors




Tjen::Node::Transition::Transition(Node* from, Node* to, char &oldStack, std::string &newStack, const char input) {
    this->from = from;
    this->newstack = newStack;
    this->oldStack = oldStack;
    this->to = to;
    this->input = input;
}

Tjen::Node *Tjen::Node::Transition::getFrom()  {
    return from;
}

void Tjen::Node::Transition::setFrom(Node *from) {
    Transition::from = from;
}

Tjen::Node *Tjen::Node::Transition::getTo() const {
    return to;
}

void Tjen::Node::Transition::setTo(Node *to) {
    Transition::to = to;
}

const char &Tjen::Node::Transition::getOldStack() const {
    return oldStack;
}

void Tjen::Node::Transition::setOldStack(const char &oldStack) {
    Transition::oldStack = oldStack;
}

const std::string &Tjen::Node::Transition::getNewstack() const {
    return newstack;
}

void Tjen::Node::Transition::setNewstack(std::string &newstack) {
    Transition::newstack = newstack;
}

const char Tjen::Node::Transition::getInput() const {
    return input;
}

void Tjen::Node::Transition::setInput(const char input) {
    Transition::input = input;
}

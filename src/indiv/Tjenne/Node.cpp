//
// Created by treap on 04.10.19.
//

#include "Node.h"

Node::Node(std::string name) {
    this->name = name;
    stack.push('z');

}

Node::Node(Node *n) {
    stack = n->getStack();
    name = n->getName();
    transitions = n->getTransitions();
}

void Node::addFunct(Node::Transition *t) {
    this->transitions.push_back(t);
}

const std::string &Node::getName() const {
    return name;
}

const std::vector<Node::Transition *> &Node::getTransitions() const {
    return transitions;
}

std::stack<char> Node::getStack() {
    return stack;
}

void Node::setStack(std::stack<char> stack) {
    Node::stack = stack;
}


void Node::editStack( const std::string &newTop) {
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




Node::Transition::Transition(Node* from, Node* to, char &oldStack, std::string &newStack, const char input) {
    this->from = from;
    this->newstack = newStack;
    this->oldStack = oldStack;
    this->to = to;
    this->input = input;
}

Node *Node::Transition::getFrom()  {
    return from;
}

void Node::Transition::setFrom(Node *from) {
    Transition::from = from;
}

Node *Node::Transition::getTo() const {
    return to;
}

void Node::Transition::setTo(Node *to) {
    Transition::to = to;
}

const char &Node::Transition::getOldStack() const {
    return oldStack;
}

void Node::Transition::setOldStack(const char &oldStack) {
    Transition::oldStack = oldStack;
}

const std::string &Node::Transition::getNewstack() const {
    return newstack;
}

void Node::Transition::setNewstack(std::string &newstack) {
    Transition::newstack = newstack;
}

const char Node::Transition::getInput() const {
    return input;
}

void Node::Transition::setInput(const char input) {
    Transition::input = input;
}

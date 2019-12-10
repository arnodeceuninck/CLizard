//
// Created by treap on 04.10.19.
//

#include "Pda.h"
#include <fstream>
#include "json.hpp"


///////////////////////PDA class/////////////////////////////
Pda::Pda() {}


std::map<const char, unsigned int> Pda::initMap(Tjen::Node *n) {
    std::map<const char, unsigned int> m;
    for (auto &f: n->getTransitions()) {
        m[f->getInput()] ++;
    }
    return m;
}


    bool Pda::checkForNode(std::vector<Tjen::Node *> &v, Tjen::Node *n) {
        for (Tjen::Node *current: v) {
            if (n == current) {
                return true;
            }
        }
        return false;
    }


    bool Pda::checkIfDouble(std::vector<Tjen::Node *> &v, Tjen::Node *n) {
    for(Tjen::Node* state : v){
        if(state->getStack() == n->getStack() && state->getName() == n->getName()){
            return true;
        }

    }
        return false;
}



void Pda::addFunct(Tjen::Node *from, Tjen::Node *to, char oldStack, std::string newStack, const char input) {
    Tjen::Node::Transition* t = new Tjen::Node::Transition(from,to,oldStack,newStack,input);
    this->alltransitions.push_back(t);
    from->addFunct(t);
}



void Pda::addNode(std::string name, bool start, bool end) {
    Tjen::Node * n = new Tjen::Node(name);
    this->allNodes.push_back(n);

    if(start){
        this->start = n;
    }
    if(end){
        this->endNodes.push_back(n);
    }
}

void Pda::addNode(Tjen::Node *n, bool start, bool end) {
    this->allNodes.push_back(n);

    if(start){
        this->start = n;
    }
    if(end){
        this->endNodes.push_back(n);
    }
}





// kijkt of string aanvaard wordt of niet
bool Pda::traverse(std::string input) {

    //init
    std::stack<char> s;
    s.push('z');
    this->getStart()->setStack(s);
    std::vector<Tjen::Node*> activeStates = {findEclosure(this->getStart())};


    //begin traverse
    for (unsigned int i = 0; i < input.size(); ++i) {
        std::vector<Tjen::Node*> newActives;


        // loop over active states
        for(Tjen::Node* n: activeStates){
            std::map<const char, unsigned int> counter = initMap(n);
            // loop over transition
            for(auto& f: n->getTransitions()){
                if((f->getInput() == input[i] && f->getOldStack() == n->getStack().top()) || (f->getInput() == ' ' && f->getOldStack() == n->getStack().top()) || (f->getInput() == input[i] && f->getOldStack() == ' ') ){

                    //eclosurre adden

                    Tjen::Node* next = new Tjen::Node(f->getTo());
                    next->setStack(n->getStack());
                    next->editStack(f->getNewstack());
                    std::vector<Tjen::Node*> eclos = findEclosure(next);

                    newActives.insert(newActives.begin(),eclos.begin() , eclos.end());


                }
            }
        }
        activeStates = newActives;

    }

    for(Tjen::Node* n : activeStates){
        for(Tjen::Node* end : this->getEndNodes()){
            if(end->getName() == n->getName()){
                return true;
            }
        }
    }
    return false;

}

Tjen::Node* Pda::searchNode(std::string &name) {
    for(Tjen::Node* node: allNodes){
        if(node->getName() == name){
            return node;
        }
    }
    return nullptr;
}

std::vector<Tjen::Node*> Pda::findEclosure(Tjen::Node *n) {

    std::vector<Tjen::Node *> eclosure; //totaal
    eclosure.push_back(n);

    bool found = true;
    bool first = true;
    while (found) {

        std::vector<Tjen::Node *> newclosure; //gevonden

        found = false;
        for (auto &node: eclosure) {
            for (auto &f: node->getTransitions()) {
                const char in = f->getInput();
                const char *e = "ε";

                Tjen::Node *next = new Tjen::Node(f->getTo());
                next->setStack(node->getStack());
                if (in == *e) {
                    next->editStack(f->getNewstack());

                    if (!checkIfDouble(eclosure, next) && node->getStack().top() == f->getOldStack()) {
                        newclosure.push_back(next);
                        found = true;
                    }

                }

            }
        }
        if (first) {
            eclosure.pop_back();
            first = false;
        }
        eclosure.insert(eclosure.begin(), newclosure.begin(), newclosure.end());
    }
    eclosure.push_back(n);
    return eclosure;
}



bool Pda::checkForTrans(std::vector<Tjen::Node::Transition*> &v, Tjen::Node::Transition *t) {
    for (Tjen::Node::Transition* trans:v) {
        if(t->getTo() == trans->getTo()  && t->getFrom() == trans->getFrom()){ return true;}

    }
    return false;
}

//maakt dot file en png aan voor pda
void Pda::createDot() {

    //file aanmaak en init stuff
    std::ofstream file ("PDA.dot");
    std::string start = this->getStart()->getName();
    std::string s = "digraph {\nrankdir=LR \n";
    std::string e = "invis -> "+ start + " \n";
    s+= "invis [style=invisible] \n";

    //endstates dubbele rand geven
    std::vector<Tjen::Node*> ends = this->getEndNodes();
    for (Tjen::Node* node: this->getAllNodes()) {
        if(checkForNode(ends, node)){
            s+= node->getName() + " [shape=doublecircle] \n";
        }


        //transities toevoegen
        std::vector<Tjen::Node::Transition*> used;
        for (auto& t: node->getTransitions()) {
            if (!checkForTrans(used, t)) {
                std::string label = "[label=\"";
                for (auto &t2: node->getTransitions()) {
                    if (t->getTo() == t2->getTo()) {
                        label += t->getInput();
                        label += ", ";
                        label += t->getOldStack();
                        label += "/" + t->getNewstack() + "\\n";
                    }
                }
                label += "\"]";

                e += node->getName() + " -> " + t->getTo()->getName() + label + "\n";
                used.push_back(t);
            }
        }
    }

    s+=e +"}";
    file << s;
    file.close();

    //convergeer commando
    std::string to_png = "dot PDA.dot -Tpng -o PDA.png";
    system("dot PDA.dot -Tpng -o PDA.png");
}

void Pda::readJson(const std::string file) {
    nlohmann::json parser;
    std::fstream f;
    std::string s;
    f >> s;
    std::cout<<s<<std::endl;
    f.open(file);
    f >> parser;

    int i = 0;

    std::string sa;
    while(parser["StackAlphabet"][i] != nullptr){
        std::string c = parser["StackAlphabet"][i];
        if(c == "Z0"){
            c="z";
        }

        sa += c;
        i+=1;
    }
    this->stack_alphabet = sa;
    i = 0;
    sa = "";
    while(parser["Alphabet"][i] != nullptr){
        std::string c = parser["Alphabet"][i];
        if(c == "Z0"){
            c="z";
        }

        sa += c;
        i+=1;
    }

    this->alphabet = sa;


    i = 0;
    //nodes toevoegen

    std::string startname= parser["StartState"];

    while(parser["States"][i] != nullptr){
        bool endstate = false;
        bool startState = false;
        std::string c = parser["States"][i];

        if(parser["States"][i] == "qf"){
            endstate = true;
        }
        if(startname == c){
            startState = true;
        }
        addNode(parser["States"][i],startState, endstate);

        i+=1;


    }

    i = 0;
    while(parser["Transitions"][i] != nullptr){

        std::string rep;
        int j =0;
        while(parser["Transitions"][i]["replacement"][j] != nullptr){
            std::string r = parser["Transitions"][i]["replacement"][j];
            rep += r;
            j+=1;

        }
        char stacktop;
        std::string top = parser["Transitions"][i]["stacktop"];
        if(top == "Z0"){
            stacktop = 'z';
        }else{
            stacktop = top[0];
        }

        std::string n = parser["Transitions"][i]["from"];
        Tjen::Node* from = searchNode(n);
        n = parser["Transitions"][i]["to"];
        Tjen::Node* to = searchNode(n);
        std::string input_s = parser["Transitions"][i]["input"];
        char c = input_s[0];
        const char* input_c = &c;

        addFunct(from,to,stacktop,rep,*input_c);
        i+=1;
    }
}








///getters, setters, constructors

void Pda::setAlphabet(const std::string alphabet) {
    Pda::alphabet = alphabet;
}

void Pda::setStackAlphabet(std::string stackAlphabet) {
    stack_alphabet = stackAlphabet;
}

Tjen::Node *Pda::getStart() const {
    return start;
}

const std::string Pda::getAlphabet() const {
    return alphabet;
}

const std::vector<Tjen::Node *> &Pda::getEndNodes() const {
    return endNodes;
}

const std::vector<Tjen::Node *> &Pda::getAllNodes() const {
    return allNodes;
}

















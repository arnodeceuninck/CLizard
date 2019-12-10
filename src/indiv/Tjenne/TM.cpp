//
// Created by treap on 22.10.19.
//

#include "TM.h"
#include "json.hpp"
#include  <fstream>
#include <sstream>
#include <typeinfo>




/////////////////////////////// Turing Machine ///////////////////////////////

TM::TM() {}


std::string TM::tapeTostring(Tape* tape) {
    std::string s;
    for(char c: tape->getTracks()[0]){
        s+=c;
    }
    return s;
}



bool TM::traverse(std::string &input, bool subroutine) {
    setTape(input);

    if (tapeCount > 1) {
        std::cerr << "Can't traverse a multi tape TM, use the 'simulateMulti' function" << std::endl;
        return false;
    }

    Node *current = start;
    bool possible = true;
    auto t = this->getTapes()[0];

    while (possible) {

        for (auto &sub: current->getSubroutines()) {
            unsigned int head = tapes[0]->getHead();
            std::string in = this->tapeTostring(tapes[0]);
            sub->traverse(in, true);
            tapes[0]->setTracks({});
            tapes = {};
            this->setTape(in);
            tapes[0]->setHead(head);

        }

        possible = false;
        for (auto &trans: current->getTransitions()) {
            if (trans->getInput()[0] == t->getTracks()[0][t->getHead()] && !possible) {
                current = trans->getNext();
                t->getTracks()[0][t->getHead()] = trans->getNewTape()[0];
                unsigned int head = t->getHead();
                changeHead(trans->getNewDir()[0], t->getTracks()[0], head);
                t->setHead(head);
                possible = true;

            }
        }


        }


        if (subroutine) {
            input = this->tapeTostring(tapes[0]);
        }

    return current->isEndstate();
}


void TM::addTransition(TM::Node *from, TM::Node *to, std::vector<char> input, std::vector<char> newTape, std::vector<char> dir) {
    auto t = new Node::Transition(input,newTape,to,dir);
    from->addTrans(t);
}

void TM::changeHead(char direction, std::vector<char> &track, unsigned int &head) {

            if(direction == 'R'){
                if(head == track.size()-1){
                    track.push_back('B');
                    return;
                }else{
                    head+=1;
                    return;
                }


            }else if (direction == 'L'){
                if(head == 0){
                    track.insert(track.begin(), 'B');
                    return;
                }else{
                    head-=1;
                    return;
                }
            }

        }






// adds node to TM via info
void TM::addNode(std::string name, bool endstate, bool start) {
    auto n = new TM::Node(name);
    n->setEndstate(endstate);
    this->allNodes.push_back(n);
    if(endstate){
        this->endStates.push_back(n);
    }
    if(start){
        this->start = n;
    }
}

// adds node to TM via existing node
void TM::addNode(TM::Node *n, bool endstate, bool start) {
    if(endstate){
        this->endStates.push_back(n);
    }
    allNodes.push_back(n);
    n->setEndstate(endstate);

    if(start){
        this->start = n;
    }
}

void TM::setTape(std::string &tape) {
    Tape* t = new Tape(tape);
    t->setHead(0);
    this->tapes.push_back(t);
}


TM::Node *TM::getStart() const {
    return start;
}

void TM::setStart(TM::Node *start) {
    TM::start = start;
}

const std::string &TM::getAlphabet() const {
    return alphabet;
}

void TM::setAlphabet(const std::string &alphabet) {
    TM::alphabet = alphabet;
}

const std::vector<TM::Node *> &TM::getAllNodes() const {
    return allNodes;
}

const std::string &TM::getTapeAlphabet() const {
    return tapeAlphabet;
}

void TM::setTapeAlphabet(const std::string &alphabet) {
    TM::tapeAlphabet = alphabet;
}


void TM::readJson(const std::string &file, bool subroutine) {
    nlohmann::json parser;
    std::fstream f;
    std::string s;
    f >> s;
    std::cout<<s<<std::endl;
    f.open("./input/"+file);
    f >> parser;


    // decide if multi tape or not
    bool multi = false;
    TM::tapeCount = 1;
    if( parser.find("Tapes") != parser.end()){
        multi = true;
        TM::tapeCount = 3;
    }



    int i = 0;
    //nodes toevoegen

    std::vector<std::string> finalStates;
    while(parser["FinalStates"][i] != nullptr){
        finalStates.push_back(parser["FinalStates"][i]);
        i +=1;
    }

    std::string startname= parser["StartState"];

    i = 0;
    while(parser["States"][i] != nullptr){
        bool endstate = false;
        bool startState = false;
        std::string c = parser["States"][i];

        for(std::string name: finalStates){
            if(name == parser["States"][i]){
                endstate = true;
                break;
            }
        }

        if(startname == c){
            startState = true;
        }
        addNode(parser["States"][i],endstate,startState);

        i+=1;
    }

    i = 0;


    std::string sa;
    while(parser["InputAlphabet"][i] != nullptr){
        std::string c = parser["InputAlphabet"][i];
        if(c == "Z0"){
            c="z";
        }

        sa += c;
        i+=1;
    }
    this->alphabet = sa;
    i = 0;
    sa = "";
    while(parser["TapeAlphabet"][i] != nullptr){
        std::string c = parser["TapeAlphabet"][i];
        if(c == "Z0"){
            c="z";
        }

        sa += c;
        i+=1;
    }

    this->setTapeAlphabet(sa);

    i =0;

    while(parser["Transitions"][i] != nullptr){
        std::string from_name = parser["Transitions"][i]["from"];
        std::string to_name = parser["Transitions"][i]["to"];


        std::vector<char> read;
        std::vector<char> write;
        std::vector<char> direction;


        if(!multi) {
            std::string os = parser["Transitions"][i]["read"];
            read.push_back(os[0]);
            os = parser["Transitions"][i]["write"];
            write.push_back(os[0]);
            os = parser["Transitions"][i]["direction"];
            if(os  == "left"){
                direction.push_back('L');
            } else if(os == "right"){
                direction.push_back('R');
            }else{
                direction.push_back(' ');
            }


            //als multi_tape
        }else{
            std::string os;
            int j = 0;
            while(parser["Transitions"][i]["read"][j] != nullptr){
                os = parser["Transitions"][i]["read"][j];
                read.push_back(os[0]);
                j+=1;
            }
            j = 0;
            while(parser["Transitions"][i]["write"][j] != nullptr){
                os = parser["Transitions"][i]["write"][j];
                write.push_back(os[0]);
                j+=1;
            }


            j =0;
            while(parser["Transitions"][i]["direction"][j] != nullptr){
                os = parser["Transitions"][i]["direction"][j];

                if(os == "right"){
                    direction.push_back('R');
                }else if(os == "left"){
                    direction.push_back('L');
                }else{
                    direction.push_back(' ');
                }

                j+=1;
            }



        }





        TM::Node* from = findNode(from_name);
        TM::Node* to = findNode(to_name);

        addTransition(from, to, read, write, direction);
        i +=1;
    }
}


bool TM::simulateMulti(std::vector<std::string> &input) {

    for (unsigned int j = 0; j < this->tapeCount; ++j) {
        setTape(input[j]);
    }

    // copy constructor adden
    TM sim = TM();
    sim.setAllNodes(this->getAllNodes());
    std::string blank = " ";
    sim.setTape(blank);
    sim.getTapes()[0]->setTracks({});



    for (unsigned int i = 0; i < this->getTapeCount(); ++i) {
        std::vector<char> emptyTrack = {'0'};
        sim.addTrack(emptyTrack);
        sim.addTrack(this->getTapes()[i]->getTracks()[0]);
    }

    bool possible = true;

    Node* current = start;

    while(possible){
        possible = false;
        for(auto & t : current->getTransitions()){
            bool found = true;
            for (unsigned int i = 1; i < tapeCount+1; ++i) {
                char tracksign = sim.getTapes()[0]->getTracks()[(2*i)-1][tapes[0]->getHead()];
                if(t->getInput()[i-1] != tracksign){
                    found = false;
                }
            }

            if(found){
                for (unsigned int i = 0; i < (2*tapeCount)-1; ++i) {
                    char c = sim.getTapes()[0]->getTracks()[i][0];
                    unsigned int head = c - '0';

                    sim.getTapes()[0]->getTracks()[i+1][head] = t->getNewTape()[i/2];
                    changeHead(t->getNewDir()[i/2], sim.getTapes()[0]->getTracks()[i+1], head);

                    sim.getTapes()[0]->getTracks()[i][0] = head + '0';
                    i+=1;
                }

                current = t->getNext();
                possible = true;
            }


        }
    }
    return current->isEndstate();

}


void TM::addTrack(std::vector<char> track) {
    this->tapes[0]->getTracks().push_back(track);
}


TM::Node* TM::findNode(std::string &name) {
    for(TM::Node* n: TM::allNodes){
        if(name == n->getName()){
            return n;
        }
    }
    return nullptr;
}


// subroutine

void TM::addSubroutine(TM * sub, TM::Node* n) {


    n->addSubroutine(sub);
}

std::vector<TM::Tape *> TM::getTapes() {
    return tapes;
}

unsigned int TM::getTapeCount() const {
    return tapeCount;
}


bool TM::checkfornode(std::vector<TM::Node*>& v, TM::Node* n) {
    for(Node* check : v){
        if(n->getName()==check->getName()){
            return true;
        }
    }
    return false;

}

bool TM::checkfortrans(std::vector<TM::Node::Transition *>& v, TM::Node::Transition *t) {
    for(TM::Node::Transition* check: v){
        if(check->getNext() == t->getNext()){
            return true;
        }
    }
    return false;
}

void TM::setAllNodes(const std::vector<TM::Node *> &allNodes) {
    TM::allNodes = allNodes;
}

void TM::toDot(std::string &outputname) {
    std::ofstream file("./output/"+outputname+ ".dot");
    std::string start = this->getStart()->getName();
    std::string s = "digraph {\nrankdir=LR \n";
    std::string e = "invis -> " + start + " \n";
    s += "invis [style=invisible] \n";
    unsigned int subroutineNr = 0;

    std::vector<Node *> ends = this->getEndStates(); // endstates

    // loop over all nodes
    for (Node *node: this->getAllNodes()) {
        if (checkfornode(ends, node)) {  // check if finalstate
            s += node->getName() + " [shape=doublecircle] \n";
        }
        std::vector<Node::Transition*> used; // vector to check if transition exists already

        // loop over nodes transitions
        for(auto& t: node->getTransitions()){
            if(!checkfortrans(used,t)) { // trans exists check
                std::string label = "[label=\"";
                // loop another time over node's transes to check for same arrow
                for (auto &t2: node->getTransitions()) {
                    if (t->getNext() == t2->getNext() ) {
                        label += "[";
                        unsigned int i =0;
                        // tape input
                        for (char c: t2->getInput()) {
                            if(t2->getInput().size() > 1 && i != 0){
                                label += ", ";
                            }
                            i += 1;
                            label += c;
                        }
                        label += ']';
                        label += " -> ";
                        label += '[';
                        i = 0;
                        //new tape
                        for (char c : t2->getNewTape()) {
                            if(t2->getNewTape().size() > 1 && i != 0){
                                label += ", ";
                            }
                            label += c;
                            i += 1;
                        }
                        label += "], [";

                        i = 0;
                        // direction
                        for (char c : t2->getNewDir()) {
                            if(t2->getNewDir().size() > 1 && i != 0){
                                label += ", ";
                            }
                            if(c == ' '){
                                c = 'S';
                            }
                            label += c;
                            i += 1;
                        }
                        label += "] \n";

                    }
                }
                label += "\"]";

                e += node->getName() + " -> " + t->getNext()->getName() + label + "\n";
                used.push_back(t);
            }

        }

        for(auto& sub : node->getSubroutines()){
            e += node->getName() + " -> sub" + std::to_string(subroutineNr)+ "_node_" + sub->getStart()->getName() + "[label =\"subroutine\"] \n";
            for(Node* end : sub->getEndStates()){
                e += "sub" + std::to_string(subroutineNr)+ "_node_" +end->getName() + " -> " + node->getName() + "\n";
            }
            for (Node *node: sub->getAllNodes()) {

                std::vector<Node::Transition*> used;
                for(auto& t: node->getTransitions()) {
                    if (!checkfortrans(used, t)) {
                        std::string label = "[label=\"";
                        for (auto &t2: node->getTransitions()) {
                            if (t->getNext() == t2->getNext()) {
                                label += "[";
                                unsigned int i = 0;
                                for (char c: t2->getInput()) {
                                    if (t2->getInput().size() > 1 && i != 0) {
                                        label += ", ";
                                    }
                                    i += 1;
                                    label += c;
                                }
                                label += ']';
                                label += " -> ";
                                label += '[';
                                i = 0;
                                for (char c : t2->getNewTape()) {
                                    if (t2->getNewTape().size() > 1 && i != 0) {
                                        label += ", ";
                                    }
                                    label += c;
                                    i += 1;
                                }
                                label += "], [";

                                i = 0;
                                for (char c : t2->getNewDir()) {
                                    if (t2->getNewDir().size() > 1 && i != 0) {
                                        label += ", ";
                                    }
                                    if (c == ' ') {
                                        c = 'S';
                                    }
                                    label += c;
                                    i += 1;
                                }
                                label += "] \n";

                            }
                        }
                        label += "\"]";

                        e += "sub"+ std::to_string(subroutineNr) + "_node_" + node->getName() + " -> " + "sub"+ std::to_string(subroutineNr)+ "_node_" +t->getNext()->getName() + label + "\n";
                        used.push_back(t);
                    }
                }
            }
            subroutineNr +=1;

        }

    }
    s+=e +"}";
    file << s;
    file.close();

    //convergeer commando
    std::string to_png = "dot  ./output/"+outputname+".dot" +" -Tpng -o ./output/"+outputname+".png";
    system(to_png.c_str());

}





/////////////////////////////// Node ///////////////////////////////

TM::Node::Node(std::string name) {
    this->name = name;
}

void TM::Node::addTrans(TM::Node::Transition *t) {
    transitions.push_back(t);
}

std::vector<TM::Node::Transition *> &TM::Node::getTransitions()  {
    return transitions;
}

bool TM::Node::isEndstate() const {
    return endstate;
}

void TM::Node::setEndstate(bool endstate) {
    Node::endstate = endstate;
}


//subroutines

const std::vector<TM*> &TM::Node::getSubroutines() const {
    return subroutines;
}

void TM::Node::addSubroutine(TM*sub) {
    subroutines.push_back(sub);
}

void TM::Node::runSubroutine(std::string & input, TM* sub, unsigned int head) {
    sub->setTape(input);
    sub->getTapes()[0]->setHead(head);
    sub->traverse(input);

}

const std::string &TM::Node::getName() const {
    return name;
}




/////////////////////////////// Transition ///////////////////////////////

TM::Node::Transition::Transition(std::vector<char> &input, std::vector<char> &newTape, Node* next, std::vector<char> &dir ) {
    this->input = input;
    this->newDir = dir;
    this->newTape = newTape;
    this->next = next;
}

const std::vector<char> &TM::Node::Transition::getInput() const {
    return input;
}


std::vector<char> TM::Node::Transition::getNewTape() const {
    return newTape;
}

TM::Node *TM::Node::Transition::getNext() const {
    return next;
}

std::vector<char> TM::Node::Transition::getNewDir() const {
    return newDir;
}






/////////////////////////////// Tape ///////////////////////////////

TM::Tape::Tape(std::string &t) {
    this->makeTape(t);
    this->head = 0;
    this->dir = 'R';
}


void TM::Tape::makeTape(std::string &tape) {
    this->getTracks().push_back({});
    for(char c:tape){
        this->getTracks()[0].push_back(c);
    }
}




//getters and setters

unsigned int TM::Tape::getHead() {
    return head;
}

void TM::Tape::setHead(unsigned int head) {
    Tape::head = head;
}

char TM::Tape::getDir() const {
    return dir;
}

void TM::Tape::setDir(char dir) {
    Tape::dir = dir;
}

 std::vector<std::vector<char>> &TM::Tape::getTracks(){
    return tracks;
}

void TM::Tape::setTracks(const std::vector<std::vector<char>> &tracks) {
    Tape::tracks = tracks;
}
    const std::vector<TM::Node *> &TM::getEndStates() const {
        return endStates;
    }



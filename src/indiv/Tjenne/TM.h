//
// Created by treap on 22.10.19.
//

#ifndef UNTITLED_TM_H
#define UNTITLED_TM_H

#include <iostream>
#include <vector>
#include "Node.h"
#include <tuple>


class TM {
public:


    class Node{
    public:

        class Transition {
            std::vector<char> input;
            std::vector<char> newTape;
            TM::Node *next;
            std::vector<char> newDir;
            std::vector<TM *> subroutines;


        public:


            Transition(std::vector<char> &input, std::vector<char> &newTape, TM::Node* next, std::vector<char> &dir );

            const std::vector<char> &getInput() const;

            std::vector<char> getNewTape() const;

            Node *getNext() const;

            std::vector<char> getNewDir() const;


        };




    private:
        std::string name;
        bool endstate;
        std::vector<Node::Transition*>  transitions;
        std::vector<TM*> subroutines;
    public:


        void addTrans(Transition* t);
        Node(std::string name);

        bool isEndstate() const;

        void setEndstate(bool endstate);

        std::vector<TM::Node::Transition *> &getTransitions() ;

        //subroutines

        void addSubroutine(TM* sub);
        void runSubroutine(std::string &input, TM* sub, unsigned int head);

        const std::vector<TM*> &getSubroutines() const;

        const std::string &getName() const;


    };


    class Tape{
        unsigned int head;
        char dir;
        std::vector<std::vector<char>> tracks;

        void makeTape(std::string &tape);   //zet string om naar tape (vector)


    public:

        Tape(std::string &t);


        unsigned int getHead();

        void setHead(unsigned int head);

        char getDir() const;

        void setDir(char dir);

        std::vector<std::vector<char>> &getTracks();

        void setTracks(const std::vector<std::vector<char>> &tracks);


    };


private:
    std::vector<Tape*> tapes;
    unsigned int tapeCount;
    std::vector<TM::Node*> allNodes;
    std::vector<TM::Node*> endStates;
    TM::Node* start;
    std::string alphabet;
    std::string tapeAlphabet;
    unsigned int subroutineCount;


    void changeHead(char direction, std::vector<char>& track, unsigned int & head);
    std::string tapeTostring(Tape* tape);
    TM::Node* findNode(std::string& name);
    bool checkfornode(std::vector<TM::Node*>& v, TM::Node* n);
    bool checkfortrans(std::vector<TM::Node::Transition*>& v, TM::Node::Transition* t);



public:
    TM();

    void addTransition(TM::Node* from, TM::Node* to, std::vector<char> input, std::vector<char> newTape, std::vector<char> dir);

    void setTape(std::string &tape);

    std::vector<Tape*> getTapes() ;

    void addNode(std::string name, bool endstate, bool start= false);
    void addNode(TM::Node* n, bool endstate, bool start=false);

    bool traverse(std::string &input, bool subroutine = false);

    Node *getStart() const;

    void setStart(TM::Node *start);

    unsigned int getTapeCount() const;


    const std::string &getAlphabet() const;

    void setAlphabet(const std::string &alphabet);

    const std::string &getTapeAlphabet() const;

    void setTapeAlphabet(const std::string &alphabet);

    const std::vector<TM::Node *> &getEndStates() const;

    const std::vector<TM::Node *> &getAllNodes() const;

    void setAllNodes(const std::vector<TM::Node *> &allNodes);

    void readJson(const std::string & file, bool subroutine);

    void addTrack(std::vector<char> track);

    bool simulateMulti(std::vector<std::string> &input);

    void toDot(std::string& outputName);






    // Subroutines

    void addSubroutine(TM* subRoutine,TM::Node* n);


};


#endif //UNTITLED_TM_H

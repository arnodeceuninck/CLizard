//
// Created by arno on 2/23/19.
//

#ifndef DFA_GRAPH_H
#define DFA_GRAPH_H

#include <iostream>
#include <vector>

using namespace std;

class Node {
public:


    Node(const string &name, const string &label, const string &shape);
    Node(const string &name, const string &label, const string &shape, const string &extra);

    const string &getName() const;

    void setName(const string &name);

    const string &getLabel() const;

    void setLabel(const string &label);

    const string &getShape() const;

    void setShape(const string &shape);

    string to_string();

private:
    string name;
    string label;
    string shape;
    string extra;
};

class Connection {
    string name_from;
public:
    string to_string();

    const string &getName_from() const;

    void setName_from(const string &name_from);

    const string &getName_to() const;

    void setName_to(const string &name_to);

    const string &getLabel() const;

    void setLabel(const string &label);

private:
    string name_to;
    string label;


public:
    Connection(const string &name_from, const string &name_to, const string &label);
};

class Graph {
    vector<Node> nodes;
    vector<Connection> connections;
    std::string rankdir;
public:
    void setRankdir(const string &rankdir);

public:
    string to_string();
    void build_file(string filename);
    void addNode(Node node);
    void addConnection(Connection connection);
};


#endif //DFA_GRAPH_H

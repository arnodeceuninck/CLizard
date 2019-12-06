//
// Created by arno on 2/23/19.
//

#include "Graph.h"
#include <fstream>
#include <stdlib.h>

Node::Node(const string &name, const string &label, const string &shape) : name(name), label(label), shape(shape), extra("") {}

const string &Node::getName() const {
    return name;
}

void Node::setName(const string &name) {
    Node::name = name;
}

const string &Node::getLabel() const {
    return label;
}

void Node::setLabel(const string &label) {
    Node::label = label;
}

const string &Node::getShape() const {
    return shape;
}

void Node::setShape(const string &shape) {
    Node::shape = shape;
}

string Node::to_string() {
    string str = name + "[label=\"" + label + "\" shape=" + shape + " " + extra + "]";
    return str;
}

Node::Node(const string &name, const string &label, const string &shape, const string &extra) : name(name), label(label), shape(shape), extra(extra) {}

Connection::Connection(const string &name_from, const string &name_to, const string &label) : name_from(name_from),
                                                                                              name_to(name_to),
                                                                                              label(label)
                                                                                              {}

const string &Connection::getName_from() const {
    return name_from;
}

void Connection::setName_from(const string &name_from) {
    Connection::name_from = name_from;
}

const string &Connection::getName_to() const {
    return name_to;
}

void Connection::setName_to(const string &name_to) {
    Connection::name_to = name_to;
}

const string &Connection::getLabel() const {
    return label;
}

void Connection::setLabel(const string &label) {
    Connection::label = label;
}

string Connection::to_string() {
    string str = name_from + "->" + name_to;
    if (!label.empty()) {
        str += "[label=\"" + label + "\"]";
    }
    return str;
}

void Graph::addNode(Node node) {
    nodes.push_back(node);
}

void Graph::addConnection(Connection connection) {
    connections.push_back(connection);
}

string Graph::to_string() {
    string str = "digraph G { \n";
    for(vector<Node>::iterator node = nodes.begin(); node != nodes.end(); ++node){
        str += node->to_string() + "\n";
    }
    for(vector<Connection>::iterator connection = connections.begin(); connection != connections.end(); ++connection){
        str += connection->to_string() + "\n";
    }
    if(rankdir.empty()){
        rankdir = "LR";
    }
    str += "rankdir=" + rankdir + "\n}";
    return str;
}

void Graph::build_file(string filename) {
    // Source: http://www.cplusplus.com/doc/tutorial/files/
    ofstream myfile;
    myfile.open (filename);
    myfile << to_string();
    myfile.close();

    // Source: http://www.cplusplus.com/reference/cstdlib/system/
    const string command = "dot -Tpng " + filename + " -o " + filename + ".png";
    system(command.c_str());
}

void Graph::setRankdir(const string &rankdir) {
    Graph::rankdir = rankdir;
}

//
// Created by arno on 17.11.19.
// Code by Sam
//

#ifndef CLIZARD_REARRANGEFUNCTIONS_H
#define CLIZARD_REARRANGEFUNCTIONS_H

#include <vector>
#include <string>
#include <memory>

int rearrangeFunctionsH(const std::vector<std::string> &inputFiles);

int rearrangeFunctionsOrderCalled(const std::vector<std::string> &inputFiles);

class GraphNode {

public:
    explicit GraphNode(int value);

    bool isLeaf();

    void addPointer(const std::weak_ptr<GraphNode>& node);

    bool hasPointer(int value);

    int getValue() const;

    void setValue(int value);

    std::string dotGraph();

private:
    int m_value;
    std::vector<std::weak_ptr<GraphNode>> m_pointers;

};

class GraphSam {

public:
    std::vector<int> getLeafs() const;

    void addNode(int value);

    void addConnection(int parent, int child);

    void deleteNode(int value);

    void swap(int a, int b);

    std::string dotGraph();

private:
    std::vector<std::shared_ptr<GraphNode>> m_nodes;

};

#endif //CLIZARD_REARRANGEFUNCTIONS_H

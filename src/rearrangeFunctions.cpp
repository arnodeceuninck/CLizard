//
// Created by arno on 17.11.19.
// Code by Sam
//

#include <iostream>
#include <fstream>
#include <vector>

#include "rearrangeFunctions.h"
#include "AST.h"

/**
 * Helper function to get the name of a function in an AST tree
 * */
std::string functionName(/*const*/ AST *function) {

    // Find function name node and return yield
    auto func_name = function->find("function-name");
    return func_name[0]->yield();

}

/**
 * Helper function to find index of element in vector
 * */
template<typename T>
size_t getIndex(const std::vector<T> &v, const T &item) {

    for (size_t i = 0; i < v.size(); i++) {
        if (v[i] == item) {
            return i;
        }
    }

    return v.size();

}

/**
 * Friend function of ASTree.h
 * Swaps the 2 nodes
 * */
void swapNodes(ASTree *p, ASTree *q) {

    // Get the parents
    auto p_parent = p->getParent();
    auto q_parent = q->getParent();

    // Get indexes of p and q in the parents
    size_t p_index = getIndex(p_parent->subtrees, p);
    size_t q_index = getIndex(q_parent->subtrees, q);

    // Set the child at index to the other node
    p_parent->subtrees[p_index] = q;
    q_parent->subtrees[q_index] = p;

    // Swap the parents
    p->parent = q_parent;
    q->parent = p_parent;

}

int rearrangeFunctionsH(const std::vector<std::string> &inputFiles) {

    std::cout << "RFH CALLED" << std::endl;

    // Generate tree from H file
    std::string h_file = inputFiles[1];
    AST ast_h(h_file);
    ast_h.toDot("ast_h.dot");

    // Generate tree from cpp file
    std::string cpp_file = inputFiles[0];
    AST ast_cpp(cpp_file);
    ast_cpp.toDot("ast_cpp.dot");

    // Find the function definitions/declarations
    auto func_decl = ast_h.find("function-declaration");
    auto func_def = ast_cpp.find("function-definition");

    if (func_def.size() < func_decl.size()) {
        std::cout << "Cpp file cannot have less functions than the H file." << std::endl;
        return 0;
    }

    // Loop over declarations
    for (size_t i = 0; i < func_decl.size(); i++) {

        // Loop over definitions
        for (size_t j = 0; j < func_def.size(); j++) {

            // Check if function names match
            std::string name_i = functionName(func_decl[i]);
            std::string name_j = functionName(func_def[j]);
            if (name_i == name_j) {

                std::cout << "Found function: " << name_i << std::endl;
                // Do nothing when already in place
                if (i != j) {
                    swapNodes(func_def[i]->getASTree(), func_def[j]->getASTree());
                    // The tree changed, search new tree
                    func_def = ast_cpp.find("function-definition");
                }

                // Goto next declaration
                break;
            }

        }

    }

    // Write yield to file
    ast_cpp.toDot("ast_cpp2.dot");
    std::string output_string = ast_cpp.yield();
    std::ofstream output_file;
    output_file.open(cpp_file);
    output_file << output_string;
    output_file.close();

    return 0;
}

int rearrangeFunctionsOrderCalled(const std::vector<std::string> &inputFiles) {

    std::cout << "ROC CALLED" << std::endl;

    // Generate tree from main file
    std::string main_file = inputFiles[0];
    AST ast_main(main_file);
    ast_main.toDot("ast_main.dot");

    // Generate tree from H file
    std::string h_file = inputFiles[1];
    AST ast_h(h_file);

    // Get the functions
    auto func_decl = ast_h.find("function-declaration");
    std::cout << "func decl" << std::endl;
    for (const auto &f : func_decl)
        std::cout << functionName(f) << std::endl;

    auto func_call_all = ast_main.find("function-call");

    // Only keep function calls that come from h file
    std::vector<AST *> func_call;
    for (auto const &call : func_call_all) {
        for (auto const &decl : func_decl) {

            // If found keep it
            if (functionName(call) == functionName(decl)) {
                func_call.push_back(call);
                break;
            }

        }
    }

    std::cout << "func calls" << std::endl;
    for (const auto &f : func_call)
        std::cout << functionName(f) << std::endl;

    // Generate tree from CPP file
    std::string cpp_file = inputFiles[2];
    AST ast_cpp(cpp_file);

    // Get the definitions
    auto func_def = ast_cpp.find("function-definition");
    std::cout << "func def" << std::endl;
    for (const auto &f : func_def)
        std::cout << functionName(f) << std::endl;

    // Create a graph
    GraphSam g;

    // Populate graph with func declarations
    for (int i = 0; i < func_decl.size(); i++)
        g.addNode(i);

    // Add all connections
    // O(n^4) LMAOOOOOOOOOOOOOOOOOO
    for (int i = 0; i < func_decl.size(); i++) {

        std::string decl_name = functionName(func_decl[i]);

        // Find all function calls in definition
        for (const auto &f : func_def) {

            std::string def_name = functionName(f);
            if (decl_name == def_name) {

                // Add all function calls in func_decl as connections
                auto calls = f->find("function-call");
                for (const auto &c : calls) {

                    std::string call_name = functionName(c);

                    // Find index
                    for (int j = 0; j < func_decl.size(); j++) {
                        std::string name = functionName(func_decl[j]);
                        if (name == call_name) {
                            g.addConnection(i, j);
                            break;
                        }
                    }

                }

            }

        }

    }

    std::ofstream graph_file;
    graph_file.open("graph.dot");
    graph_file << g.dotGraph();
    graph_file.close();

    auto leafs = g.getLeafs();
    int index = 0;
    while (!leafs.empty()) {

        //std::cout << "start prio lookup" << std::endl;
        int prio = 0;
        int prio_level = 9999999;

        // Find highest priority
        for (int i : leafs) {

            // Find prio level
            int p_lvl = -1;

            std::string decl_name = functionName(func_decl[i]);
            //std::cout << decl_name << std::endl;


            // Search function calls
            for (int j = 0; j < func_call.size(); j++) {
                std::string call_name = functionName(func_call[j]);
                //std::cout << call_name << std::endl;
                if (decl_name == call_name) {
                    p_lvl = j;
                    break;
                }
            }

            // If prio is -1 or p_lvl is lower
            // This node has prio
            if (p_lvl < prio_level) {
                prio_level = p_lvl;
                prio = i;
            }

        }

        //std::cout << "found prio" << std::endl;

        // Swap the prio node with node at index
        std::cout << "Sorted function with name: " << functionName(func_decl[prio]) << std::endl;
        if (index != prio) {

            swapNodes(func_decl[prio]->getASTree(), func_decl[index]->getASTree());

            g.swap(prio, index);

            func_decl = ast_h.find("function-declaration");
        }

        // Remove prio from graph
        g.deleteNode(index);

        //std::cout << "deleted from graph" << std::endl;

        // Get new leafs
        leafs = g.getLeafs();

        //std::cout << "got new leafs" << std::endl;

        // Increase index
        index++;

    }

    /*
    // Sort same as RFH
    // Loop over calls
    for (size_t i = 0; i < func_call.size(); i++) {

        // Loop over declarations
        for (size_t j = 0; j < func_decl.size(); j++) {

            // Check if function names match
            std::string name_i = functionName(func_call[i]);
            std::string name_j = functionName(func_decl[j]);
            if (name_i == name_j) {

                std::cout << "Found function: " << name_i << std::endl;
                // Do nothing when already in place
                if (i != j) {
                    swapNodes(func_decl[i]->getASTree(), func_decl[j]->getASTree());
                    // The tree changed, search new tree
                    func_decl = ast_h.find("function-declaration");
                }

                // Goto next call
                break;
            }

        }

    }*/

    // Write yield to file
    ast_h.toDot("ast_h2.dot");
    std::string output_string = ast_h.yield();
    std::ofstream output_file;
    output_file.open(h_file);
    output_file << output_string;
    output_file.close();

    return 0;
}

GraphNode::GraphNode(int value) : m_value(value) {}

bool GraphNode::isLeaf() {

    // Remove inactive pointers
    for (auto iter = m_pointers.begin(); iter != m_pointers.end(); iter++)
        if (iter->expired()) {
            iter = m_pointers.erase(iter);
            iter--;
            //std::cout << "deleted a node" << std::endl;
        }

    //std::cout << "ok" << std::endl;
    return m_pointers.empty();
}

void GraphNode::addPointer(const std::weak_ptr<GraphNode> &node) {
    m_pointers.push_back(node);
}

int GraphNode::getValue() const {
    return m_value;
}

bool GraphNode::hasPointer(int value) {
    for (auto iter = m_pointers.begin(); iter != m_pointers.end(); iter++)
        if (iter->expired()) {
            iter = m_pointers.erase(iter);
            iter--;
        } else {
            auto pointer = iter->lock();
            if (pointer->getValue() == value)
                return true;
        }

    return false;
}

void GraphNode::setValue(int value) {
    m_value = value;
}

std::string GraphNode::dotGraph() {
    std::string result = "\t" + std::to_string(m_value) + "\n";
    for (auto iter = m_pointers.begin(); iter != m_pointers.end(); iter++)
        if (iter->expired()) {
            iter = m_pointers.erase(iter);
            iter--;
        } else {
            auto pointer = iter->lock();
            result += "\t" + std::to_string(m_value) + "->" + std::to_string(pointer->m_value) + "\n";
        }

    return result;
}

std::vector<int> GraphSam::getLeafs() const {
    std::vector<int> result;
    for (const auto &n : m_nodes) {
        if (n->isLeaf())
            result.push_back(n->getValue());
    }
    return result;
}

void GraphSam::addNode(int value) {

    // No duplicates
    for (const auto &n : m_nodes)
        if (n->getValue() == value)
            return;

    auto node = std::make_shared<GraphNode>(value);
    m_nodes.push_back(node);
}

void GraphSam::addConnection(int parent, int child) {

    // Search for parent node
    for (auto &n : m_nodes) {
        if (n->getValue() == parent) {

            // Avoid circular connections
            if (n->hasPointer(child))
                return;

            // Search for child node
            for (const auto &m : m_nodes) {
                if (m->getValue() == child)
                    n->addPointer(m);
            }

        }
    }

}

void GraphSam::deleteNode(int value) {

    for (auto iter = m_nodes.begin(); iter != m_nodes.end(); iter++) {
        if ((*iter)->getValue() == value) {
            iter->reset();
            m_nodes.erase(iter);
            return;
        }
    }

}

void GraphSam::swap(int a, int b) {

    if (a == b) return;

    for (auto &n : m_nodes) {
        if (n->getValue() == a) {
            for (auto &m : m_nodes) {
                if (m->getValue() == b) {
                    n->setValue(b);
                    m->setValue(a);
                    return;
                }
            }
        }
    }

}

std::string GraphSam::dotGraph() {
    std::string result = "digraph {\n";
    for (auto& n : m_nodes) {
        result += n->dotGraph();
    }
    result += "}";
    return result;
}

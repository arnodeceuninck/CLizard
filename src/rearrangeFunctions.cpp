//
// Created by arno on 17.11.19.
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
 template <typename T>
 size_t getIndex(std::vector<T> v, T item) {

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
    output_file.open(cpp_file + "rfh.cpp");
    output_file << output_string;
    output_file.close();

    return 1;
}

int rearrangeFunctionsOrderCalled(const std::vector<std::string> &inputFiles) {
    return 0;
}

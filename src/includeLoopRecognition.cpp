//
// Created by arno on 17.11.19.
//

#include <iostream>
#include "includeLoopRecognition.h"

#include "AST.h"

string removeDots(string basicString);

int includeLoopRecognition(const std::vector<std::string> &inputFiles) {

    Graph graph;

    if (inputFiles.empty()) { return 0; }

    std::vector<AST> asts;

    for (auto file: inputFiles) {

//        std::cout << realFilename(file) << std::endl;

        std::string realFileName = realFilename(file);
        graph.addNode(Node(removeDots(realFileName), realFileName, "circle"));

        auto ast = AST(file);
        asts.emplace_back(ast);

        for (auto tree: ast.find("libname")) {
            graph.addConnection(
                    Connection(removeDots(realFileName), removeDots(realFilename(tree->yield())), "includes"));
        }
        for (auto tree: ast.find("filename")) {
            graph.addConnection(
                    Connection(removeDots(realFileName), removeDots(realFilename(tree->yield())), "includes"));
        }
    }

    graph.build_file("output/ILR.dot");

    auto loops = graph.checkLoops();

    for (auto loop: loops) {
        std::cout << "Include loop found. \nThere is a circular dependency in the files ";
        for (auto file: loop) {
            if (file != loop[0]) {
                std::cout << ", ";
            }
            std::cout << file;
        }
        std::cout
                << ". \nHave you tried using predeclarations and the use of pointers, so you can break the loop in one of these files? Are all the includes really necessary in the file?"
                << std::endl;
    }

    if (loops.empty()) {
        std::cout << "No include loops found." << std::endl;
    }

    return 0;
}

string removeDots(string basicString) {
    string newString;
    for (char c: basicString) {
        if (c != '.')
            newString += c;
    }
    return newString;
}

std::string realFilename(std::string file) {
    std::string filename;
    for (int i = file.size() - 1; i >= 0; --i) {
        char c = file[i];
        if (c == '/') {
            break;
        }
        filename = c + filename;
    }
    return filename;
}

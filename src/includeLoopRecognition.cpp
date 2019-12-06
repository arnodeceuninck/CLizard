//
// Created by arno on 17.11.19.
//

#include <iostream>
#include "includeLoopRecognition.h"

#include "AST.h"

int includeLoopRecognition(const std::vector<std::string> &inputFiles) {

    if (inputFiles.empty()) { return 0; }

    std::vector<AST> asts;

    for (auto file: inputFiles) {
        auto ast = AST(file);
        asts.emplace_back(ast);
    }

    return 0;
}

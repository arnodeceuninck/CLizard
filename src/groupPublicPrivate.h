//
// Created by arno on 17.11.19.
//

#ifndef CLIZARD_GROUPPUBLICPRIVATE_H
#define CLIZARD_GROUPPUBLICPRIVATE_H

#include <vector>
#include <string>

class AST;

int groupPublicPrivate(const std::vector<std::string> &inputFiles);

int ClassOperation(AST& parsetree, std::vector<std::string>& publics, std::vector<std::string>& privates);

int writeToFile(std::string name, std::fstream file, std::vector<std::string>& publics, std::vector<std::string>& privates);

#endif //CLIZARD_GROUPPUBLICPRIVATE_H

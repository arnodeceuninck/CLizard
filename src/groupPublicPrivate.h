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

std::string writeToFile(std::vector<std::string>& publics, std::vector<std::string>& privates, std::string classString);

void stringNormalizer(std::string& string);

std::size_t endOfClassFinder(std::string name, const std::string& classtring);

#endif //CLIZARD_GROUPPUBLICPRIVATE_H

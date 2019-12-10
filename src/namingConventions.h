//
// Created by arno on 17.11.19.
//

#ifndef CLIZARD_NAMINGCONVENTIONS_H
#define CLIZARD_NAMINGCONVENTIONS_H

#include <vector>
#include <string>

int namingConventionsClasses(const std::vector<std::string> &inputFiles);

int namingConventionsVariables(const std::vector<std::string> &inputFiles);

int namingConventionsFunctions(const std::vector<std::string> &inputFiles);

std::string correctClassName(std::string& name);





#endif //CLIZARD_NAMINGCONVENTIONS_H

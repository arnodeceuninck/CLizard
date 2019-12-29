//
// Created by arno on 17.11.19.
//

#ifndef CLIZARD_NAMINGCONVENTIONS_H
#define CLIZARD_NAMINGCONVENTIONS_H

#include <vector>
#include <string>
#include "indiv/Tjenne/Pda.h"

namespace namingConventions {

    int convertToConventions(const std::vector<std::string> &inputFiles);

    int namingConventionsClasses(const std::vector<std::string> &inputFiles);

    int namingConventionsVariables(const std::vector<std::string> &inputFiles);

    int namingConventionsFunctions(const std::vector<std::string> &inputFiles);

    std::string correctClassName(std::string name);

    //dit zoekt de classname in een blokje code bv: class CooleKlass{} -> cooleKlass wordt gereturned
    std::string findClassName(std::string &word);

    // verbetert de incorrecte variable -en function names volgens de conventions
    std::string correctName(std::string name);

    // past voor gegeven files alle oude namen aan volgens de conventions
    void adjustForAllFiles(const std::vector<std::string> &inputFiles, std::vector<std::string> &oldNames, std::vector<Pda *> &pdas, bool forClass);

    std::string editToNewName(std::string &word, const std::string &oldName, const std::string &correctName);

    bool findNameInVector(const std::vector<std::string> &allNames, const std::string &name);

    std::string getFileName(const std::string &filePath);

    Pda *makeNamePda(std::string &name);

    }




#endif //CLIZARD_NAMINGCONVENTIONS_H

//
// Created by arno on 17.11.19.
//

#ifndef CLIZARD_NAMINGCONVENTIONS_H
#define CLIZARD_NAMINGCONVENTIONS_H

#include <vector>
#include <string>
#include "indiv/Tjenne/Pda.h"

namespace namingConventions {

    int convertToConventions(const std::vector<std::string> &inputFiles);   //alle drie de operaties samen

    int namingConventionsClasses(const std::vector<std::string> &inputFiles);   //verbetert gegeven files naar versie met correcte class names

    int namingConventionsVariables(const std::vector<std::string> &inputFiles); //verbetert gegeven files naar versie met correcte variable names

    int namingConventionsFunctions(const std::vector<std::string> &inputFiles); //verbetert gegeven files naar versie met correcte function names

    std::string correctClassName(std::string name); //past de naam van een class correct aan en returned deze string

    //dit zoekt de classname in een blokje code bv: class CooleKlass{} -> cooleKlass wordt gereturned
    std::string findClassName(std::string &word);

    // verbetert de incorrecte variable -en function names volgens de conventions
    std::string correctName(std::string name);

    // past voor gegeven files alle oude namen aan volgens de conventions
    void adjustForAllFiles(const std::vector<std::string> &inputFiles, std::vector<std::string> &oldNames, std::vector<Pda *> &pdas, bool forClass);    //past alle files aan naar de correcte versie

    std::string editToNewName(std::string &word, const std::string &oldName, const std::string &correctName);   //past een stuk tekst aan naar de correcte versie

    bool findNameInVector(const std::vector<std::string> &allNames, const std::string &name);   //zoekt een string in een vector met strings

    std::string getFileName(const std::string &filePath);       //haalt de filename uit een path

    Pda *makeNamePda(std::string &name);      //stelt een pda op voor een bepaald  woord

    }




#endif //CLIZARD_NAMINGCONVENTIONS_H

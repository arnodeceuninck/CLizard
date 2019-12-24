//
// Created by arno on 17.11.19.
//

#include "namingConventions.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include "indiv/Tjenne/Pda.h"
#include "AST.h"


//searches the filename in a given path and returns this
std::string getFileName(const std::string &filePath) {
    unsigned int pos = filePath.size() - 1;
    while (filePath[pos - 1] != '/') {
        pos -= 1;
    }
    std::string fileName;
    while (pos != filePath.size()) {
        fileName += filePath[pos];
        pos += 1;
    }
    return fileName;
}


// dit maakt een pda aan die een bepaalde string kan herkennen in een tekst
Pda *makeNamePda(std::string &name) {
    Pda *p = new Pda();
    std::string alphabeth;
    unsigned int nodeCount = 1;
    for (unsigned int c = 0; c < name.size() + 1; ++c) {
        std::string nodename = "q" + std::to_string(c);
        p->addNode(nodename, c == 0, c == name.size());
        bool present = false;
        for (char &ch: alphabeth) {
            if (ch == name[c]) {
                present = true;
            }
        }
        if (!present) {
            alphabeth += name[c];
        }
    }
    for (unsigned int i = 0; i < p->getAllNodes().size() - 1; ++i) {
        p->addFunct(p->getAllNodes()[i], p->getAllNodes()[i + 1], ' ', " ", name[i]);
    }
    p->addFunct(p->getAllNodes()[0], p->getAllNodes()[0], ' ', " ", ' ');
    p->addFunct(p->getAllNodes()[p->getAllNodes().size() - 1], p->getAllNodes()[p->getAllNodes().size() - 1], ' ',
                " ", ' ');


    p->setAlphabet(alphabeth);
    p->setStackAlphabet(alphabeth);
    return p;
}

std::string editToNewName(std::string &word, const std::string &oldName, const std::string &correctName) {
    unsigned int correctCount = 0;
    unsigned int start = 0;
    unsigned int end = 0;
    std::string newName;

    bool found = false;
    unsigned int oldname_i = 0;
    for (unsigned int c = 0; c < word.size(); ++c) {

         if (word[c] == oldName[oldname_i] && !found) {
             if (correctCount == oldName.size() - 1) {
                 end = c;
                 found = true;
             }else{
                 correctCount += 1;
                 oldname_i += 1;
                 if (correctCount == 1) {
                     start = c;
                 }
             }

        } else {
            correctCount = 0;
            oldname_i = 0;
        }
    }
    unsigned int pos = 0;
    while (pos != start) {
        newName += word[pos];
        pos += 1;
    }
    newName += correctName;
    pos = end + 1;
    while (pos < word.size()) {
        newName += word[pos];
        pos += 1;
    }
    return newName;


}



namespace classConventions {

//dit zoekt de classname in een blokje code bv: class CooleKlass{} -> cooleKlass wordt gereturned
    std::string findClassName(std::string &word) {
        std::string name;
        for (unsigned int c = 0; c < word.size(); ++c) {
            if (word[c] != '{') {
                name.push_back(word[c]);
            } else {
                return name;
            }
        }
        return name;
    }

// verbetert de incorrecte class names volgens de conventions
    std::string correctClassName(std::string &name) {
        std::string corrected;
        unsigned int c = 0;
        while (!isalpha(name[c])) {
            c++;
        }
        if (islower(name[c])) {
            name[c] = toupper(name[c]);
        }
        for (; c < name.size(); ++c) {
            if (isupper(name[c]) && isupper(name[c + 1])) {
                name[c + 1] = _tolower(name[c + 1]);
                corrected += name[c];
            } else if (name[c] == '_' && name[c + 1] == '_') {
                name.erase(c, 1);
            } else if (!isalpha(name[c]) && !isdigit(name[c])) {
                name.erase(c, 1);
            } else {
                corrected += name[c];
            }
        }
        return corrected;

    }



// past een reeks c++ files aan zodat alle classnames hier voldoen aan de conventions
    int namingConventionsClasses(const std::vector<std::string> &inputFiles) {

        Pda classRecognizer;
        classRecognizer.readJson("vbn/NamingConvTests/classRec.json");
        Pda *classChecker = new Pda();
        classChecker->readJson("vbn/NamingConvTests/classChecker.json");
        std::fstream buffer;
        std::vector<std::string> oldclassNames;
        std::vector<std::string> newclassNames;
        std::vector<Pda *> classPdas;


// loop over files om alle classes te bepalen en een verbeterde versie hiervan op te slaan
        for (auto &file: inputFiles) {
            buffer.open(file.c_str());
            std::string line;
            while (std::getline(buffer, line)) {
                bool found = false;
                std::stringstream stream(line);
                std::string word;
                while (stream >> word) {
                    if (found) {
                        std::string name = findClassName(word);
                        if (!classChecker->traverse(name)) {
                            oldclassNames.push_back(name);
                            classPdas.push_back(makeNamePda(name));
                            newclassNames.push_back(correctClassName(name));
                            found = false;

                        } else {
                            found = false;
                        }
                    }
                    if (classRecognizer.traverse(word)) {
                        found = true;
                    }

                }

            }
            buffer.close();
        }
        // loopt een tweede maal over alle files om alle instanties van de foutieve class names aan te passen naar de verbeterde versie
//            system("exec rm vbn/NamingConvTests/outputs/*"); // nog een goede if statement vinden


        for (auto &file: inputFiles) {
            buffer.open(file.c_str());
            std::string line = "vbn/NamingConvTests/outputs/" + getFileName(file);
            std::ofstream editedfile(line);
            std::string editedstring;

            while (std::getline(buffer, line)) {
                std::stringstream stream(line);
                std::string word;

                while (stream >> word) {

                    for (unsigned int i = 0; i < classPdas.size(); ++i) {
                        Pda *tester = classPdas[i];
                        if (tester->traverse(word)) {
                            word = editToNewName(word, oldclassNames[i], newclassNames[i]);
                            std::cout << "ye";


                        }
                    }
                    editedstring += word + ' ';
                }
                editedstring += "\n";

            }
            editedfile << editedstring;
            editedfile.close();
            buffer.close();

        }

        return 0;
    }
}

bool findNameInVector(const std::vector<std::string>& allNames, const std::string& name){
    for(auto& testName: allNames){
        if(name == testName){
            return true;
        }
    }
    return false;
}


// verbetert de incorrecte var names volgens de conventions
std::string correctVarName(std::string name) {
    std::string corrected;
    unsigned int c = 0;
    while (!isalpha(name[c])) {
        c++;
    }
    if (isupper(name[c])) {
        name[c] = tolower(name[c]);
    }
    for (; c < name.size(); ++c) {
        if (isupper(name[c]) && isupper(name[c + 1])) {
            name[c + 1] = _tolower(name[c + 1]);
            corrected += name[c];
        } else if (name[c] == '_' && name[c + 1] == '_') {
            name.erase(c, 1);
        } else if (!isalpha(name[c]) && !isdigit(name[c])) {
            name.erase(c, 1);
        } else {
            corrected += name[c];
        }
    }
    return corrected;

}

int namingConventionsVariables(const std::vector<std::string> &inputFiles) {
    fstream buffer("vbn/NamingConvTests/allVarTypes");
    std::stringstream stream;
    stream << buffer.rdbuf();
    buffer.close();
    std::string varfile = stream.str();
    std::vector<std::string> allVarNames;
    std::vector<std::string> correctVarNames;
    std::vector<Pda *> varPdas;


    // bepaalt via ast alle nieuwe varNames

    for(const std::string& input: inputFiles) {

        std::vector<std::string> newVarNames;

        AST *ast = new AST(input);
        ast->toDot("vbn/NamingConvTests/outputs/yeeters.dot");
        std::vector<AST *> vars = ast->find("variable"); // all found var types

        //update de allVarTypes met de nieuwe var types
        for (unsigned int i = 0; i < vars.size(); ++i) {
            std::string name = vars[i]->yield();
            if (!(name == "true" || name == "false") && !findNameInVector(allVarNames, name)) {
                varfile += name + '\n';
                allVarNames.push_back(name);
                newVarNames.push_back(name);
            }
        }
        ofstream outPutter;
        outPutter.open("vbn/NamingConvTests/allVarTypes");
        outPutter << varfile;
        outPutter.close();


        for (std::string &oldName: newVarNames) {
            varPdas.emplace_back(makeNamePda(oldName));
            correctVarNames.push_back(correctVarName(oldName));
        }
    }


    for(auto& file: inputFiles){
        std::string fileName = "vbn/NamingConvTests/outputs/" + getFileName(file);
        buffer.open(file.c_str());
        std::string line;
        std::ofstream editedfile(fileName);
        std::string editedstring;

        while (std::getline(buffer, line)) {
            std::stringstream stream(line);
            std::string word;

            while (stream >> word) {

                std::string longestFit;
                for (unsigned int i = 0; i < varPdas.size(); ++i) {
                    Pda *tester = varPdas[i];
                    if (tester->traverse(word)) {
                        word = editToNewName(word, allVarNames[i], correctVarNames[i]);
                        if(word.size() > longestFit.size()){
                            longestFit = word;
                        }

                    }
                }
                if(!longestFit.empty()){
                    editedstring += longestFit + ' ';
                }else{
                    editedstring += word + ' ';
                }
            }
            editedstring += "\n";

        }
        editedfile << editedstring;
        editedfile.close();
        buffer.close();

    }





    return 0;
}

int namingConventionsFunctions(const std::vector<std::string> &inputFiles) {
    return 0;
}

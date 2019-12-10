//
// Created by arno on 17.11.19.
//

#include "namingConventions.h"
#include <fstream>
#include <sstream>
#include <ctype.h>
#include "indiv/Tjenne/Pda.h"
#include "indiv/Tjenne/json.hpp"




std::string findClassName(std::string &word){
    std::string name;
    for (unsigned int c = 0; c < word.size(); ++c) {
        if(word[c] != '{'){
            name.push_back(word[c]);
        }else{
            return name;
        }
    }
    return name;
}

std::string correctClassName(std::string& name) {
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
        }else {
            corrected += name[c];
        }
    }
    return corrected;

}

int namingConventionsClasses(const std::vector<std::string> &inputFiles) {

    Pda classRecognizer;
    classRecognizer.readJson("vbn/NamingConvTests/classRec.json");
    Pda classChecker;
    classChecker.readJson("vbn/NamingConvTests/classChecker.json");
    std::fstream buffer;
    std::vector<std::string> oldclassNames;
    std::vector<std::string> newclassNames;



    for(auto& file: inputFiles){
        buffer.open(file.c_str());
        std::string line;
        while(std::getline(buffer, line)){
            bool found = false;
            std::stringstream stream(line);
            std::string word;
            while(stream >> word){
                if(found){
                    std::string name = findClassName(word);
                    if(classChecker.traverse(name)){
                        oldclassNames.push_back(name);
                        found = false;

                    }
                }
                if(classRecognizer.traverse(word)){
                    found = true;
                }

            }

        }
    }


    return 0;
}

int namingConventionsVariables(const std::vector<std::string> &inputFiles) {
    return 0;
}

int namingConventionsFunctions(const std::vector<std::string> &inputFiles) {
    return 0;
}

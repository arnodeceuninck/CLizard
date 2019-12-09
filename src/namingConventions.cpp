//
// Created by arno on 17.11.19.
//

#include "namingConventions.h"
#include <fstream>
#include <sstream>
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

int namingConventionsClasses(const std::vector<std::string> &inputFiles) {

    Pda classrecognizer;
    classrecognizer.readJson("vbn/NamingConvTests/classRec.json");
    std::fstream buffer;
    std::vector<std::string> classNames;


    for(auto& file: inputFiles){
        buffer.open(file.c_str());
        std::string line;
        while(std::getline(buffer, line)){
            bool found = false;
            std::stringstream stream(line);
            std::string word;
            while(stream >> word){
                if(found){
                    classNames.push_back(findClassName(word));
                    found = false;
                }
                if(classrecognizer.traverse(word)){
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

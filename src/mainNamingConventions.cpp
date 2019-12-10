//
// Created by treap on 09.12.19.
//

//// FOR TESTING OF NAMINGCONVENTION FIXES

#include "namingConventions.h"

int main(int argc, char** argv){
std::vector<std::string> filenames;

    for (unsigned int file = 0; file < argc; ++file) {
        filenames.emplace_back(argv[file]);
    }
    std::string classTest = "-CL+__=AS69__S";
    correctClassName(classTest);

    namingConventionsClasses(filenames);

}
//
// Created by treap on 09.12.19.
//

//// FOR TESTING OF NAMINGCONVENTION FIXES

#include "namingConventions.h"

int main(int argc, char** argv){
std::vector<std::string> filenames;

    for (unsigned int file = 1; file < argc; ++file) {
        filenames.emplace_back(argv[file]);
    }

    namingConventionsClasses(filenames);

}
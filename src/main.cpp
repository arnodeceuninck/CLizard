#include <iostream>
#include <vector>
#include <fstream>
#include "includeLoopRecognition.h"
#include "rearrangeFunctions.h"
#include "namingConventions.h"
#include "groupPublicPrivate.h"
#include "simplifyMathematicalExpressions.h"

enum ReadOp {
    Unspecified, BackupSuffix, BackupPrefix, InputFiles
};

void setReadOp(ReadOp &readOp, std::string &argument, bool &overrideFile, std::string &feature);

int main(int argc, char *argv1[]) {

    std::cout << "Hello, World!" << std::endl;

    // Setting up the backup prefix and suffix
    // e.g. if the filename is a.cpp, it will be changed to [backupPrefix]a.cpp[backupSuffix]
    // One of them is most of the times empty
    std::string backupSuffix = "";
    std::string backupPrefix = "backup-";
    std::string outputPrefix = "output-"; // Overrides backup

    bool cli = argc == 1;
    std::vector<std::string> argv(argv1, argv1 + argc);


    while (cli or (argc >= 1 and argv[0] != "exit")) {

        // Bool to check whether you should override your original file.
        // If this is true, the backup files will contain the original files.
        // If this is false, the backup files will contain the edited version of the files.
        // There is always one of them which contains the content of your original file and one containing the edited version
        bool overrideFile = true;

        std::string operation; // Contains the feature that will be applied
        std::vector<std::string> inputFiles; // Contains a list of the filenames of all input files

        // Iterate through all arguments and store their information
        ReadOp readOp = Unspecified;
        for (int arg = 1; arg < argc; ++arg) {

            std::string argument = argv[arg];

            switch (readOp) {
                case Unspecified:
                    if (argument[0] == '-') {
                        setReadOp(readOp, argument, overrideFile, operation);
                    } else {
                        std::cerr << "No idea what you want to do with " << argument << ". Ignoring." << std::endl;
                    }
                    break;
                case BackupSuffix:
                    backupSuffix = argument;
                    readOp = Unspecified;
                    break;
                case BackupPrefix:
                    backupPrefix = argument;
                    readOp = Unspecified;
                    break;
                case InputFiles:
                    if (argument[0] == '-') {
                        setReadOp(readOp, argument, overrideFile, operation);
                    } else {
                        inputFiles.emplace_back(argument);
                    }
                    break;
            }

        }

        // All data has been read from argv

        std::vector<std::string> backupInputFiles;

        if (!overrideFile) {
            backupPrefix = outputPrefix;
        }

        // Backup all files
        for (const std::string &filename: inputFiles) {

            // The name of the backup file
            std::string backupName = backupSuffix;

            // Add the prefix right after the last backslash
            bool prefixAdded = false;
            for (int i = filename.size() - 1; i >= 0; --i) {
                char c = filename[i];
                if (!prefixAdded && c == '/') {
                    backupName = backupPrefix + backupName;
                    prefixAdded = true;
                }
                backupName = c + backupName;
            }
            if (!prefixAdded) {
                backupName = backupPrefix + backupName;
            }

            // Copy the file
            // Source: https://stackoverflow.com/questions/10195343/copy-a-file-in-a-sane-safe-and-efficient-way
            std::ifstream file(filename, std::ios::binary);
            std::ofstream backup(backupName, std::ios::binary);

            backup << file.rdbuf();

            if (!overrideFile) {
                backupInputFiles.push_back(backupName);
            }

        }

        if (!overrideFile) {
            inputFiles = backupInputFiles;
        }

        // Everything is backed up, time to process it
        int returnCode;
        if (operation == "ilr") {
            if (!overrideFile) {
                std::cout << "It's illegal to run ilr with -b" << std::endl;
                return 420;
            }
            returnCode = includeLoopRecognition(inputFiles);
        } else if (operation == "rfh") {
            returnCode = rearrangeFunctionsH(inputFiles);
        } else if (operation == "roc") {
            returnCode = rearrangeFunctionsOrderCalled(inputFiles);
        } else if (operation == "ncc") {
            returnCode = namingConventions::namingConventionsClasses(inputFiles);
        } else if (operation == "ncv") {
            returnCode = namingConventions::namingConventionsVariables(inputFiles);
        } else if (operation == "ncf") {
            returnCode = namingConventions::namingConventionsFunctions(inputFiles);
        } else if (operation == "gpp") {
            returnCode = groupPublicPrivate(inputFiles);
        } else if (operation == "sme") {
            returnCode = simplifyMathematicalExpressions(inputFiles);
        } else {
            returnCode = 404;
            std::cout << "Operation " << operation << " not found." << std::endl;
        }

        if (returnCode != 0) {
            std::cout << "Something went wrong, please restore the return files." << std::endl;
        }

        if (cli) {

            std::cout << std::endl;

            if (argc == 1)
                std::cout << "You probably forgot some arguments, but you can still type them here" << std::endl;
            std::cout
                    << "Please enter your arguments or choose one of the numbers for an example. Type \"exit\" to exit"
                    << std::endl;
            std::map<int, std::tuple<std::string, std::string>> examples; // argc and string with the program argument itself
            examples[0] = {"Include loop recognition",
                           "CLizard -ilr -i vbn/ILR/A.h vbn/ILR/A.cpp vbn/ILR/B.h vbn/ILR/B.cpp vbn/ILR/C.h vbn/ILR/C.cpp vbn/ILR/D.h vbn/ILR/D.cpp"};

            examples[1] = {"Rearrange functions (.h)", "clizard -rfh -b -i a.cpp a.h"};
            examples[2] = {"Rearrange functions (order called)", "clizard -roc -b -i a.cpp a.h"};
            examples[3] = {"Naming conventions (class)", "clizard -ncc -i main.cpp a.cpp a.h"};
            examples[4] = {"Naming conventions (function)", "clizard -ncf -i main.cpp a.cpp a.h"};
            examples[5] = {"Naming conventions (variable)", "clizard -ncv -i main.cpp a.cpp a.h"};
            examples[6] = {"Group public and private", "clizard -gpp -i vbn/GroupingPPTests/TestClass.h"};
            examples[7] = {"Simplify mathematical expressions", "clizard -sme -i vbn/SimplifyTests/MATHZ.cpp    "};

            for (int j = 0; j < examples.size(); ++j) {
                auto example = examples[j];
                std::cout << j << ") " << std::get<0>(example) << ": " << std::get<1>(example) << std::endl;
            }

            std::string input;
            std::cout << "Please enter the arguments: ";
            std::getline(std::cin, input);

            try {
                auto choice = examples[stoi(input)];
                input = std::get<1>(choice);
            } catch (...) {
                // Yeet, using input as arguments
            }

            std::cout << "Using arguments: " << input << std::endl;

            argv = {};
            std::string word;
            for (auto str: input) {
                if (str == ' ') {
                    if (word == "exit") {
                        cli = false;
                        argv = {};
                        break;
                    }
                    argv.push_back(word);
                    word = "";
                } else {
                    word += str;
                }
            }

            if (word == "exit") {
                cli = false;
                argv = {};
                break;
            }
            argv.push_back(word);
            word = "";

            argc = argv.size();

        } else {
            argc = 0;
        }

        std::cout << std::endl;
    }

    return 0;
}

void setReadOp(ReadOp &readOp, std::string &argument, bool &overrideFile, std::string &feature) {

    std::string operation = argument.substr(1, argument.size() - 1); // Remove the '-'

    if (operation == "suf") {
        readOp = BackupSuffix;
    } else if (operation == "pre") {
        readOp = BackupPrefix;
    } else if (operation == "i") {
        readOp = InputFiles;
    } else if (operation == "b") {
        overrideFile = false;
        readOp = Unspecified;
    } else {
        feature = operation;
        readOp = Unspecified;
    }
}

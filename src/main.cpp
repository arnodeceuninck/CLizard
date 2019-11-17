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

int main(int argc, char *argv[]) {

    std::cout << "Hello, World!" << std::endl;

    // Setting up the backup prefix and suffix
    // e.g. if the filename is a.cpp, it will be changed to [backupPrefix]a.cpp[backupSuffix]
    // One of them is most of the times empty
    std::string backupSuffix = "";
    std::string backupPrefix = "backup/";

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

    // Backup all files
    for (const std::string &filename: inputFiles) {

        // The name of the backup file
        std::string backupName = backupPrefix;
        backupName += filename;
        backupName += backupSuffix;

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
        returnCode = includeLoopRecognition(inputFiles);
    } else if (operation == "rfh") {
        returnCode = rearrangeFunctionsH(inputFiles);
    } else if (operation == "roc") {
        returnCode = rearrangeFunctionsOrderCalled(inputFiles);
    } else if (operation == "ncc") {
        returnCode = namingConventionsClasses(inputFiles);
    } else if (operation == "ncv") {
        returnCode = namingConventionsVariables(inputFiles);
    } else if (operation == "ncf") {
        returnCode = namingConventionsFunctions(inputFiles);
    } else if (operation == "gpp") {
        returnCode = groupPublicPrivate(inputFiles);
    } else if (operation == "sme") {
        returnCode = simplifyMathematicalExpressions(inputFiles);
    } else {
        returnCode = 404;
        std::cerr << "Operation " << operation << " not found." << std::endl;
    }

    if (returnCode != 0) {
        // TODO: Restore original files
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

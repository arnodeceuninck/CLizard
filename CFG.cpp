//
// Created by arno on 28.10.19.
//

#include "CFG.h"
#include "functions.h"

#include <fstream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

Production::Production(const std::string &fromP, const std::vector<std::string> &toP) : fromP(fromP), toP(toP) {}

const std::string &Production::getFromP() const {
    return fromP;
}

const std::vector<std::string> &Production::getToP() const {
    return toP;
}

CFG::CFG(const std::vector<std::string> &nonTerminalsV, const std::vector<char> &terminalsT,
         const std::vector<Production *> &productionsP, const std::string &startS) : nonTerminalsV(nonTerminalsV),
                                                                                     terminalsT(terminalsT),
                                                                                     productionsP(productionsP),
                                                                                     startS(startS) {}

void CFG::toJSON(std::string filename) {
    // document is the root of a json message
    rapidjson::Document document;

    // define the document as an object rather than an array
    document.SetObject();

    // must pass an allocator when the object may need to allocate memory
    rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

//    document.AddMember("type", "CFG", allocator);

    // create a rapidjson array type with similar syntax to std::vector
    // non-Terminals
    rapidjson::Value alphabetRJ(rapidjson::kArrayType);
    for (std::string character: nonTerminalsV) {
        alphabetRJ.PushBack(strJSON(character, allocator), allocator);
    }
    document.AddMember("Variables", alphabetRJ, allocator);

    // Terminals
    rapidjson::Value terminalsRJ(rapidjson::kArrayType);
    for (char character: terminalsT) {
        terminalsRJ.PushBack(strJSON(toString(character), allocator), allocator);
    }
    document.AddMember("Terminals", terminalsRJ, allocator);


//    document.AddMember("eps", strJSON(string(1, epsilon), allocator), allocator);

    rapidjson::Value productionsRJ(rapidjson::kArrayType);
    for (Production* production: productionsP) {

        rapidjson::Value productionRJ(rapidjson::kObjectType);

        productionRJ.AddMember("head", strJSON(production->getFromP(), allocator), allocator);

        rapidjson::Value productionsToRJ(rapidjson::kArrayType);
        for (std::string productionElement: production->getToP()) {
            productionsToRJ.PushBack(strJSON(productionElement, allocator), allocator);
        }
        productionRJ.AddMember("body", productionsToRJ, allocator);
        productionsRJ.PushBack(productionRJ, allocator);
    }
    document.AddMember("Productions", productionsRJ, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    document.Accept(writer);

    // Source: s.n.(2019). Input/output with files. Visited on March 30, 2019 via http://www.cplusplus.com/doc/tutorial/files/
    // Yup, alweer diezelfde bron als altijd, ooit zal ik die 4 lijntjes wel vanbuiten kennen

    std::ofstream myfile;
    myfile.open(filename);

    myfile << strbuf.GetString() << std::endl;

    myfile.close();

}


rapidjson::Value strJSON(std::string str, rapidjson::Document::AllocatorType &allocator) {
    rapidjson::Value strVal;
    strVal.SetString(str.c_str(), str.length(), allocator);
    return strVal;
}

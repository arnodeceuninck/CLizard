//
// Created by Tim on 11/11/2019.
//


#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include "cfgTim.h"
#include "CYK.h"

bool cykcheck(cfgTim* language, std::string input, bool html, std::string outputname){
    std::vector<std::vector<std::vector<std::string>>> table(input.size());
    for (int k = 0; k < table.size(); ++k) {
        for (int i = 0; i < input.size(); ++i) {
            table[k].push_back(std::vector<std::string>());
        }
    }
    for (int i = 0; i < input.size(); ++i) {
        std::string currentletter = std::string(1, input[i]);
        for (int j = 0; j < language->getVariables().size(); ++j) {
            std::string currentvariable = language->getVariables()[j];
            if(language->containsElement(currentletter, currentvariable)){
                table[i][i].push_back(currentvariable);
            }
        }
    }
    int row = 1;
    while (table[0][input.size()-1].empty()) {
        for (int colum = 0; colum < table.size()-row; ++colum) {
            std::vector<std::string> newset;
            int currentchanger = 1;
            int rowcounter = row;
            if (colum < table[colum].size()){
                while(currentchanger <= row){
                    std::vector<std::string> result1 = table[colum][colum+currentchanger-1];       //combine the sets, ex (Xii, Xi+1,j) = (Xcolum,colum+changer, Xcolum+changer+1,row)
                    std::vector<std::string> result2 = table[colum+currentchanger][colum+row];
                    for (int i = 0; i < result1.size(); ++i) {
                        for (int j = 0; j < result2.size(); ++j) {
                            std::string r1r2 = result1[i] + result2[j];
                            newset.push_back(r1r2);
                        }
                    }
                    rowcounter++;
                    currentchanger++;
                }
                std::vector<std::string> tobeassinged = {};
                for (int k = 0; k < newset.size(); ++k) {
                    std::vector<std::string> created = makeable(language, newset[k]);
                    tobeassinged.insert(tobeassinged.begin(), created.begin(), created.end());
                }
                if(tobeassinged.empty()){
                    tobeassinged = {"∅"};
                }
                table[colum][colum+row] = tobeassinged;
            }
        }
        row++;
    }

    bool accept;

    if(std::find(table[0][input.size()-1].begin(), table[0][input.size()-1].end(), language->getStart()) != table[0][input.size()-1].end()){
        accept = true;
    } else{
        accept = false;
    }

    if(html){
        htmlprint(outputname, table, input, accept);
    }

    return accept;

}

std::vector<std::string> makeable(cfgTim* language, std::string input){
    std::vector<std::string> variables;
    for (std::map<std::string, std::vector<std::vector<std::string>>>::iterator itr = language->getProducts().begin();
    itr != language->getProducts().end(); ++itr){
        for (int i = 0; i < itr->second.size(); ++i) {
            if(itr->second[i].size() != 1){
                if(itr->second[i][0] == std::string(1,input[0]) && itr->second[i][1] == std::string(1,input[1])){
                    variables.push_back(itr->first);
                }
            }
        }
    }
    return variables;
}

void htmlprint(std::string outputname, std::vector<std::vector<std::vector<std::string>>> table, std::string originalinput, bool accept) {
    for (int m = 0; m < table.size(); ++m) {
        for (int i = table[m].size()-1; i > -1; --i) {
            if(table[m][i].empty()){
                table[m].erase(table[m].begin()+i);
            }
        }
    }
    std::ofstream outputfile;
    outputfile.open(outputname);
    outputfile << "<html>\n"
                  "<head>\n"
                  "\t<style>\n"
                  "table, th, td {\n"
                  "  border: 1px solid black;\n"
                  "  font-family:\"Courier New\", Courier, monospace; \n"
                  "  font-size:100%\n"
                  "}\n"
                  "table {\n"
                  "  border-collapse: collapse;\n"
                  "}\n"
                  "td,th {\n"
                  "  height: 50px;\n"
                  "  width: 100;\n"
                  "  text-align: center;\n"
                  "}\n"
                  "tr:nth-child(even) {background-color: #f2f2f2;}\n"
                  "th {\n"
                  "  background-color: #4CAF50;\n"
                  "  color: white;\n"
                  "}\n"
                  "</style>\n"
                  "</head>\n"
                  "<body>\n"
                  "<table>\n";

    for (int i = table.size()-1; i > -1; --i) {
        outputfile << "<tr>";
        for (int j = 0; j < table[i].size(); ++j) {
            if(table[j][i].empty()){
                //outputfile << "<td></td>"; //delete this to remove cells
                continue;
            }
            if(table[j][i][0] == "∅") {
                outputfile << "<td>&oslash;</td>";
                continue;
            }
            outputfile << "<td>{";
            for (int k = 0; k < table[j][i].size(); ++k) {
                outputfile << table[j][i][k];
                if(k < table[j][i].size()-1){
                    outputfile << ",";
                }
            }
            outputfile << "}</td>";
        }
        outputfile << "</tr>\n";
    }

    outputfile << "<tr>";

    for (int l = 0; l < originalinput.size(); ++l) {
        outputfile << "<th>" << originalinput[l] << "</th>";
    }

    outputfile << "</tr>";

    outputfile << "De string word";

    if(!accept){
        outputfile << " niet";
    }

    outputfile << " geaccepteerd";

    outputfile <<  "</table>\n"
                   "</body>\n"
                   "</html>";
    outputfile.close();
}

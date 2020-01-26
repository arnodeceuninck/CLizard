//
// Created by arno on 17.11.19.
//

#include "simplifyMathematicalExpressions.h"
#include <stack>
#include <iostream>
//#include <stdio.h>
#include <iterator>
#include <algorithm>
#include <fstream>
#include "AST.h"

int simplifyMathematicalExpressions(const std::vector<std::string> &inputFiles) {
    //std::vector<std::string> test = divideNumbersOperators("((15 / (7 − (1 + 1))) × 3) − (2 + (1 + 1))");
    //std::vector<std::string> test = toPRN("((15/(7-(1+1)))*3)-(2+(1+1))");
    //std::vector<std::string> test2 = simplify(test);

    std::ifstream file;

    for (int i = 0; i < inputFiles.size(); ++i) {

        file.open(inputFiles[i]);
        AST ast(inputFiles[i]);

        std::string fullYield = ast.yield();

        std::vector<AST *> asts = ast.findNonRecursive("multiplicative-expression");

        for (int j = 0; j < asts.size(); ++j) {
            std::string yield = asts[j]->yield();
            std::string newYield = toInfix(simplify(toPRN(yield)));
            fullYield.replace(fullYield.begin() + fullYield.find(yield), fullYield.begin() + fullYield.find(yield) + yield.size(), newYield);
        }



        file.close();
        std::ofstream file2(inputFiles[i]);
        file2 << fullYield;
        file2.close();
    }


    //std::cout << toInfix(simplify(toPRN("((15/(7-(var+1)))*3)-(2+(1+1))"))) << std::endl;
    //std::cout << toInfix(simplify(toPRN("2+9/3*2"))) << std::endl;
    return 0;
}

std::vector<std::string> toPRN(std::string input) {
    std::vector<std::string> output;
    std::vector<std::string> operators{"+", "-", "*", "/"};
    std::stack<std::string> stack;

    std::vector<std::string> dividedInput = divideNumbersOperators(input);

    for (int i = 0; i < dividedInput.size(); ++i) {
        if(std::find(operators.begin(), operators.end(), dividedInput[i]) != operators.end()){
            while (!stack.empty() and (checkPrecedence(dividedInput[i]) < checkPrecedence(stack.top()) ||
                    (checkPrecedence(dividedInput[i]) == checkPrecedence(stack.top()))) and stack.top() != "(") {
                output.push_back(stack.top());
                stack.pop();
            }
            stack.push(dividedInput[i]);
/*            if(dividedInput[i] == ")"){
                while (stack.top() != "(") {
                    output.push_back(stack.top());
                    stack.pop();
                }
                stack.pop();
            }else{
                while(checkPrecedence(dividedInput[i]) < checkPrecedence(stack.top())){
                    output.push_back(stack.top());
                    stack.pop();
                }
            }*/
        } else if (dividedInput[i] == "("){
            stack.push("(");
        }else if (dividedInput[i] == ")"){
            while(stack.top() != "("){
                output.push_back(stack.top());
                stack.pop();
            }
            if(stack.top() == "("){
                stack.pop();
            }
        }else{
            output.push_back(dividedInput[i]);
        }
    }
    while(!stack.empty()){
        output.push_back(stack.top());
        stack.pop();
    }
    stringNormalizer(output);
    return output;
}

std::vector<std::string> divideNumbersOperators(std::string input) {
    std::vector<std::string> output;
    std::vector<std::string> operators{"+", "-", "*", "/", "(", ")"};
    for (int i = 0; i < input.size(); ++i) {
        if(input[i] == ' '){
            continue;
        }
        if(std::find(operators.begin(), operators.end(), std::string(1,input[i])) != operators.end()){
            std::string temp(1, input[i]);
            output.push_back(temp);
        } else{
            int length = 1;
            while(std::find(operators.begin(), operators.end(), std::string(1,input[i+length])) == operators.end() and i+length < input.size()){
                length++;
            }
            output.push_back(input.substr(i, length));
            i += length-1;
        }
    }
    return output;
}

bool isNumber(std::string input) {
    for (int i = 0; i < input.size(); ++i) {
        if(!std::isdigit(input[i])){
            if(input[i] == '.'){
                continue;
            }
            return false;
        }
    }
    return true;
}

std::string toInfix(std::vector<std::string> input) {
    std::stack<std::string> stack;
    std::vector<std::string> operators{"+", "-", "*", "/"};
    std::string output;
    for (int i = 0; i < input.size(); ++i) {
        if(std::find(operators.begin(), operators.end(), input[i]) != operators.end()){
            std::string x2 = stack.top();
            stack.pop();
            std::string x1 = stack.top();
            stack.pop();
            stack.push(std::string("(" + x1 + input[i] + x2 + ")"));
        } else{
            stack.push(input[i]);
        }
    }
    output = stack.top();
    if(*output.begin() == '(' and *output.end() == ')') {
        output.pop_back();
        output.erase(output.begin());
    }
    return output;
}

std::vector<std::string> simplify(std::vector<std::string> input) {
    std::vector<std::string>::iterator pos = std::next(input.begin());
    std::vector<std::string> operators{"+", "-", "*", "/"};
    while(pos != input.end() && std::next(pos) != input.end()){
        while(std::next(pos) != input.end() && !isOperator(*std::next(pos))){
            pos++;
        }

        float temp;
        if(isNumber(*pos) && isNumber(*std::prev(pos))){
            std::string operatorToken = *std::next(pos);
            if (operatorToken == "+") {
                temp = std::stof(*std::prev(pos)) + std::stof(*pos);
            } else if (operatorToken == "-") {
                temp = std::stof(*std::prev(pos)) - std::stof(*pos);
            } else if (operatorToken == "/") {
                temp = std::stof(*std::prev(pos)) / std::stof(*pos);
            } else if (operatorToken == "*") {
                temp = std::stof(*std::prev(pos)) * std::stof(*pos);
            }
            *pos = std::to_string(temp);
            input.erase(std::next(pos));
            input.erase(std::prev(pos));
            pos = std::prev(pos);
        } else{
            pos++;
        }
    }

    return input;
}

bool isOperator(std::string input){
    if(input.size() > 1){
        return false;
    }
    return (input <= "a" || input >= "z") && (input <= "A" || input >= "Z") && (input <= "0" || input >= "9");
}

int checkPrecedence(std::string op) {
    std::vector<std::string> precedence1 = {"+", "-"};
    std::vector<std::string> precedence2 = {"*", "/"};
    if(std::find(precedence1.begin(), precedence1.end(), op) != precedence1.end()){
        return 1;
    } else if(std::find(precedence2.begin(), precedence2.end(), op) != precedence2.end()){
        return 2;
    }
    return 0;
}

void stringNormalizer(std::vector<std::string> &vector) {
    for (int i = 0; i < vector.size(); ++i) {
        for (int j = 0; j < vector[i].size(); ++j) {
            if(vector[i][j] == ' '){
                vector[i].erase(vector[i].begin()+j);
            }
        }
    }
}

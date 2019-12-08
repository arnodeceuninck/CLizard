//
// Created by arno on 17.11.19.
//

#include "simplifyMathematicalExpressions.h"
#include <stack>
//#include <stdio.h>
#include <algorithm>

int simplifyMathematicalExpressions(const std::vector<std::string> &inputFiles) {
    //std::vector<std::string> test = divideNumbersOperators("((15 / (7 − (1 + 1))) × 3) − (2 + (1 + 1))");
    std::vector<std::string> test = toPRN("((15/(7-(1+1)))*3)-(2+(1+1))");
    return 0;
}

std::vector<std::string> toPRN(std::string input) {
    std::vector<std::string> output;
    std::vector<std::string> operators{"+", "-", "*", "/", "(", ")"};
    std::stack<std::string> stack;

    std::vector<std::string> dividedInput = divideNumbersOperators(input);

    for (int i = 0; i < dividedInput.size(); ++i) {
        if(std::find(operators.begin(), operators.end(), dividedInput[i]) != operators.end()){
            if(dividedInput[i] == ")"){
                while (stack.top() != "(") {
                    output.push_back(stack.top());
                    stack.pop();
                }
                stack.pop();
            }else{
                stack.push(dividedInput[i]);
            }
        } else {
            output.push_back(dividedInput[i]);
        }
    }
    while(!stack.empty()){
        output.push_back(stack.top());
        stack.pop();
    }

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

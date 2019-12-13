# Created by Arno on 02/12/2019

# Source: https://stackoverflow.com/questions/3368969/find-string-between-two-substrings
import re

# Source: https://stackoverflow.com/questions/54059917/generate-all-length-n-permutations-of-true-false
import itertools

import sys

# charactersToEscape = ["\"", "\\"]  # A list of all characters that need an \ in front of them in json
charactersToEscape = ["\""]
uniqueTag = "#715585#"  # Random number to ensure this is unique and not accidentally in the code
optionalTag = "#OPTIONAL" + uniqueTag


class Production:
    fromV = ""
    toV = []


filename = sys.argv[1]
html = open(filename)  # The command line argument should point to this file: "grammar.txt"

terminals = set()
nonTerminals = set()
allChars = set()

productions = []

sectionName = ""
for line in html:

    if line[0] == "%":
        continue

    if line[len(line) - 1] == "\n":
        line = line[:-1]

    currentProductionTo = []
    lastReadRuleFrom = ""
    arrowForming = False
    arrowFormed = False
    readingFrom = True
    spaceRead = False
    currentProduction = ""
    previousChar = ""
    previousPreviousChar = ""
    for char in line:

        if readingFrom:
            # First check on spaces, skip them if not escaped
            if char == " " and previousChar != "\\":
                spaceRead = True
                previousPreviousChar = previousChar
                previousChar = char
                continue

            # Begin of reading an arrow
            if char == "-":
                arrowForming = True

            # Continue reading arrow
            elif arrowForming:
                if char == ">":
                    arrowFormed = True
                    readingFrom = False
                    # arrowForming = False
                    nonTerminals.add(lastReadRuleFrom)
                    allChars.add(lastReadRuleFrom)
                else:
                    lastReadRuleFrom += "-" + char
                    arrowForming = False

            else:
                lastReadRuleFrom += char

        # Not reading form, meaning reading to part
        elif not readingFrom:
            if char == "|" and previousChar != "\\":
                production = Production()
                production.fromV = lastReadRuleFrom
                production.toV = currentProductionTo
                productions.append(production)
                currentProductionTo = list()
                currentProduction = str()
            elif char == "\\" and previousChar != "\\":
                previousPreviousChar = previousChar
                previousChar = char
                continue
            else:
                if char == " ":
                    if previousChar == "|" or (previousChar == ">" and arrowForming):
                        arrowForming = False
                        if previousPreviousChar != "\\":
                            previousPreviousChar = previousChar
                            previousChar = char
                            continue
                        else:
                            currentProduction += char
                    elif previousChar == "\\":
                        currentProduction += char
                    else:
                        currentProductionTo.append(currentProduction)
                        allChars.add(currentProduction)
                        currentProduction = str()
                else:
                    currentProduction += char
                if currentProduction == "OPT:":
                    currentProductionTo.append(optionalTag)
                    currentProduction = ""

        previousPreviousChar = previousChar
        previousChar = char

html.close()

terminals = allChars - nonTerminals

# if filter:
#     nonTerminals = supportedVariables

# Remove all OPTIONALTAGS (by generating all combinations with/without them
for production in productions:
    if optionalTag in production.toV:
        occurances = production.toV.count(optionalTag)
        combinations = list(itertools.product([False, True], repeat=occurances))
        for combination in combinations:

            newProduction = Production()
            newProduction.fromV = production.fromV
            newProduction.toV = []

            optionalTagNr = 0
            optionalel = False
            for prodTo in production.toV:
                if prodTo == optionalTag:
                    optionalel = True
                elif optionalel:
                    if combination[optionalTagNr]:
                        newProduction.toV.append(prodTo)
                    optionalel = False
                    optionalTagNr += 1
                else:
                    newProduction.toV.append(prodTo)

            productions.append(newProduction)
            # print(newProduction.fromV, newProduction.toV)

i = 0
while i < len(productions):
    prod = productions[i]
    if optionalTag in prod.toV:
        productions.pop(i)
    else:
        i += 1


# TODO: Do this in the parser
# The c++ parser only supports terminals of one char -> split up any terminal that is more than one char
# i = 0
# newTerminals = set()
# for terminal in terminals:
#     if len(terminal) > 1:
#         # TODO: Should I remove the original production here? -> only if it gives problems
#         newVar = composedTerminal + str(i) + "#"
#         i += 1
#         production = Production()
#         production.fromV = newVar
#         for char in terminal:
#             production.toV.append(char)
#             newTerminals.add(char)
#         productions.add(production)
# for terminal in newTerminals:
#     terminals.add(terminal)

def addJsonStr(variable):
    string = "\""
    for char in variable:
        if char in charactersToEscape:
            string += "\\"
        string += char
    string += "\"" + ", "
    return string


jsonstr = "{"

jsonstr += "\"Variables\": ["
for variable in nonTerminals:
    jsonstr += addJsonStr(variable)
if nonTerminals: jsonstr = jsonstr[:-2]  # Remove the last ,
jsonstr += "],"

jsonstr += "\"Terminals\": ["
for terminal in terminals:
    jsonstr += addJsonStr(terminal)
if terminals: jsonstr = jsonstr[:-2]  # Remove the last ,
jsonstr += "],"

jsonstr += "\"Productions\": ["
for production in productions:
    jsonstr += "{"

    jsonstr += "\"head\": "
    jsonstr += addJsonStr(production.fromV)

    jsonstr += "\"body\": ["
    for productionTo in production.toV:
        jsonstr += addJsonStr(productionTo)
    if production.toV: jsonstr = jsonstr[:-2]  # Remove the last ,
    jsonstr += "]"

    jsonstr += "}, "
if productions: jsonstr = jsonstr[:-2]  # Remove the last ,
jsonstr += "],"

jsonstr += "\"Start\": \"translation-unit\""  # TODO: Maybe translation-unit and pre-processing file both

jsonstr += "}"

f = open(sys.argv[2], "w+")  # argv[2] should be cpp.json
f.write(jsonstr)
f.close()

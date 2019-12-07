# Created by Arno on 02/12/2019

# Source: https://stackoverflow.com/questions/3368969/find-string-between-two-substrings
import re

# Source: https://stackoverflow.com/questions/54059917/generate-all-length-n-permutations-of-true-false
import itertools

import sys

supportedSections = ["lex.name",
                     "basic.link",
                     "expr.prim.general",
                     "dcl.dcl", "dcl.spec",
                     "dct.type.simple",
                     "dcl.attr.grammar",
                     "dcl.decl",  # Till here required for parsing int a;
                     "dcl.fct.spec",
                     "dcl.spec",
                     "dcl.fct",
                     "dcl.init",
                     "expr.ass",
                     "expr.cond",
                     "expr.log.or",
                     "expr.log.and",
                     "expr.or",
                     "expr.xor",
                     "expr.bit.and",
                     "expr.eq",
                     "expr.rel",
                     "expr.shift",
                     "expr.add",
                     "expr.mul",
                     "expr.mptr.oper",
                     "expr.cast",
                     "expr.unary",
                     "expr.post",
                     "expr.prim.general",
                     "lex.literal.kinds",
                     "lex.icon" # till here required for int a(); (function declaration)
                     ]

intaVariables = ["translation-unit",  # List of variables for supporting int.cpp
                 "declaration-seq",
                 "declaration",
                 "block-declaration",
                 "simple-declaration",  # int a;
                 "attribute-specifier-seq", "decl-specifier-seq", "init-declarator-list",
                 "decl-specifier", "attribute-specifier-seq",
                 "type-specifier",
                 "trailing-type-specifier",
                 "simple-type-specifier",  # int
                 "init-declarator-list",
                 "init-declarator",
                 "declarator",
                 "ptr-declarator",
                 "noptr-declarator",
                 "declarator-id",
                 "id-expression",
                 "unqualified-id",
                 "identifier",
                 "identifier-nondigit",
                 "nondigit"]  # a

supportedVariables = intaVariables
filter = True

# "simple-type-specifier",
# "keyword",

charactersToEscape = ["\"", "\\"]  # A list of all characters that need an \ in front of them in json
uniqueTag = "#715585#"  # Random number to ensure this is unique and not accidentally in the code
optionalTag = "#OPTIONAL" + uniqueTag


class Production:
    fromV = ""
    toV = []


html = open(sys.argv[1])  # The command line argument should point to this file: "Hyperlinked C++ BNF Grammar.html"
# html = open("test.html")
terminals = set()
nonTerminals = set()

productions = []

lastReadRuleFrom = ""
currentProductionTo = []

sectionName = ""
for line in html:

    # Check production from
    if 'class="RuleColumn"' in line:
        ruleColumn = re.search("<b>(.*)</b>", line)  # <b>ProductionFrom:</b>
        betweenTags = ruleColumn.group(1)
        rule = betweenTags[:-1]  # Remove the ":" at the end
        lastReadRuleFrom = rule
        nonTerminals.add(rule)

    # Check production to element non-terminal
    elif 'class="NonTerminalSymbol"' in line:
        nonTerminalSymbolWithTags = re.search(">(.*)</a>", line)  # <b>ProductionFrom:</b>
        nonTerminalSymbol = nonTerminalSymbolWithTags.group(1)
        # nonTerminals.add(nonTerminalSymbol) # TOCHECK: Is this required, since we already add all rulecolumns
        if '<sub>opt</sub>' in line:
            currentProductionTo.append(optionalTag)  # TODO: filter out all optional
        currentProductionTo.append(nonTerminalSymbol)


    # Check production to element terminal
    elif 'class="TerminalSymbol"' in line:
        terminalSymbolWithTags = re.search("<b>(.*)</b>", line)  # <b>ProductionFrom:</b>
        terminalSymbol = terminalSymbolWithTags.group(1)
        terminals.add(terminalSymbol)
        if '<sub>opt</sub>' in line:
            currentProductionTo.append(optionalTag)
        currentProductionTo.append(terminalSymbol)

    elif '<td><h2><a name=' in line:
        sectionNameWithTags = re.search('">(.*)</a>', line)
        sectionName = sectionNameWithTags.group(1)

    # Check end of production to
    elif '</td>' in line:
        # Own additions
        # if lastReadRuleFrom == "id-expression":
        #     currentProductionTo.append(optionalTag)
        #     terminals.add(" ")
        #     currentProductionTo.append(" ")

        # Default handling
        if currentProductionTo:
            # if lastReadRuleFrom not in supportedVariables and filter:
            #     currentProductionTo = []
            #     continue
            if sectionName not in supportedSections and filter:
                currentProductionTo = []
                continue
            # Only add it when currentProductionTo is not empty
            print(sectionName)
            production = Production()
            production.fromV = lastReadRuleFrom
            production.toV = currentProductionTo
            productions.append(production)
            currentProductionTo = []

    # Check for the exceptions
    elif 'class="Description"' in line:
        if lastReadRuleFrom == "lparen":
            # a ( character not immediately preceded by white-space
            production = Production()
            production.fromV = lastReadRuleFrom
            production.toV = ["("]  # Unable to check for no-whitespace in front of (
        elif lastReadRuleFrom == "new-line":
            # the new-line character
            production = Production()
            production.fromV = lastReadRuleFrom
            production.toV = ["\n"]  # In the assumption \n is the only new line char
        elif lastReadRuleFrom == "d-char":
            # any member of the basic source character set, except: space, the left parenthesis (, the right
            # parenthesis ), the backslash \, and the control characters representing horizontal tab, vertical tab,
            # form feed, and newline.
            continue
        elif lastReadRuleFrom == "r-char":
            # any member of the source character set, except a right parenthesis ) followed by the initial
            # d-char-sequence (which may be empty) followed by a double quote ".
            continue
        elif lastReadRuleFrom == "s-char":
            # any member of the source character set except the double-quote ", backslash \, or new-line character
            continue
        elif lastReadRuleFrom == "c-char":
            # any member of the source character set except the single quote ', backslash \, or new-line character
            continue
        elif lastReadRuleFrom == "punctuator" or lastReadRuleFrom == "operator-token":
            # Look at preprocessing-op-or-punc below
            production = Production()
            production.fromV = lastReadRuleFrom
            production.toV = ["preprocessing-op-or-punc"]
        elif lastReadRuleFrom == "identifier-nondigit":
            # other implementation-defined characters
            continue
        elif lastReadRuleFrom == "q-char":
            # any member of the source character set except new-line and "
            continue
        elif lastReadRuleFrom == "h-char":
            # any member of the source character set except new-line and &gt;
            continue
        elif lastReadRuleFrom == "preprocessing-token":
            # each non-white-space character that cannot be one of the above
            continue
html.close()

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

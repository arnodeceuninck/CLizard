# Created by Arno on 02/12/2019

import re  # Source: https://stackoverflow.com/questions/3368969/find-string-between-two-substrings

charactersToEscape = ["\"", "\\"]  # A list of all characters that need an \ in front of them in json


class Production:
    fromV = ""
    toV = []


html = open("Hyperlinked C++ BNF Grammar.html")

optionalTag = "#OPTIONAL#715585#"  # Random number to ensure this is unique and not accidentally in the code

terminals = set()
nonTerminals = set()

productions = set()

lastReadRuleFrom = ""
currentProductionTo = []

for line in html:

    # Check production from
    if 'class="RuleColumn"' in line:
        ruleColumn = re.search("<b>(.*)</b>", line)  # <b>ProductionFrom:</b>
        betweenTags = ruleColumn.group(1)
        rule = betweenTags[:-1]  # Remove the ":" at the end
        lastReadRuleFrom = rule

    # Check production to element non-terminal
    elif 'class="NonTerminalSymbol"' in line:
        nonTerminalSymbolWithTags = re.search(">(.*)</a>", line)  # <b>ProductionFrom:</b>
        nonTerminalSymbol = nonTerminalSymbolWithTags.group(1)
        nonTerminals.add(nonTerminalSymbol)
        if '<sub>opt</sub>' in line:
            nonTerminalSymbol = optionalTag + nonTerminalSymbol  # TODO: filter out all optional
        currentProductionTo.append(nonTerminalSymbol)

    # Check production to element terminal
    elif 'class="TerminalSymbol"' in line:
        terminalSymbolWithTags = re.search("<b>(.*)</b>", line)  # <b>ProductionFrom:</b>
        terminalSymbol = terminalSymbolWithTags.group(1)
        terminals.add(terminalSymbol)
        if '<sub>opt</sub>' in line:
            nonTerminalSymbol = optionalTag + terminalSymbol
        currentProductionTo.append(terminalSymbol)

    # Check end of production to
    elif '</td>' in line:
        if currentProductionTo:
            # Only add it when currentProductionTo is not empty
            production = Production()
            production.fromV = lastReadRuleFrom
            production.toV = currentProductionTo
            productions.add(production)
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
jsonstr = jsonstr[:-2]  # Remove the last ,
jsonstr += "],"

jsonstr += "\"Terminals\": ["
for terminal in terminals:
    jsonstr += addJsonStr(terminal)
jsonstr = jsonstr[:-2]  # Remove the last ,
jsonstr += "],"

jsonstr += "\"Productions\": ["
for production in productions:
    jsonstr += "{"

    jsonstr += "\"head\": "
    jsonstr += addJsonStr(production.fromV)

    jsonstr += "\"body\": ["
    for productionTo in production.toV:
        jsonstr += addJsonStr(productionTo)
    jsonstr = jsonstr[:-2]  # Remove the last ,
    jsonstr += "]"

    jsonstr += "}, "
jsonstr = jsonstr[:-2]  # Remove the last ,
jsonstr += "],"

jsonstr += "\"Start\": \"translation-unit\""  # TODO: Maybe translation-unit and pre-processing file both

jsonstr += "}"

f = open("cpp.json", "w+")
f.write(jsonstr)
f.close()

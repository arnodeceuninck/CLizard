# CLizard
by JetBigBrains

## Getting started
Run the "compile.sh" file to compile the program, folowed by the "run.sh" to run the program. 

```
./compile.sh && ./run.sh
```

## Features
Note: Currently it's only possible to apply one feature per time.
### Include loop recognition
This feature will help you find include loops.

```
./clizard -ilr -i a.cpp b.cpp a.h b.h
```

### Rearrange functions (.h)
All functions in your .cpp will be given the same order as the functions in your .h file.

```
./clizard -rfh -i a.cpp a.h
```

### Rearrange functions (order called)
All functions in your .h functions will be given the same order in which they are called. 

```
./clizard -roc -i main.cpp a.cpp a.h
```

### Naming conventions (class)
Convert all class names in your files to names that follow the correct naming conventions.

```
./clizard -ncc -i main.cpp a.cpp a.h
```

### Naming conventions (function)
Convert all class names in your files to names that follow the correct naming conventions.

```
./clizard -ncf -i main.cpp a.cpp a.h
```

### Naming conventions (variable)
Convert all class names in your files to names that follow the correct naming conventions.

```
./clizard -ncv -i main.cpp a.cpp a.h
```

### Group public and private
When this feature is applied, your public and private functions will all be ordered together, so you don't have more than once the word public or private in your class.

```
./clizard -gpp -i main.cpp a.cpp a.h
```

### Simplify mathematical expressions
Simplify all mathematical expressions in your code.
```
./clizard -sme -i main.cpp a.cpp a.h
```

## Arguments
### Features
`-ilr`  Include loop recognition <br>
`-rfh`  Rearrange functions (.h) <br>
`-roc`  Rearrange functions (order called) <br>
`-ncc`  Class naming conventions <br>
`-ncv`  Variable naming conventions <br>
`-gpp`  Group public private <br>
`-sme`  Simplify mathematical expressions <br>
### Extra
`-i`    Input files <br>
`-b`    Write output to backup instead of to the original file <br>
`-suf`  Override the default backup suffix with the string given after this argument <br>
`-pre`  Override the default backup prefix with the string given after this argument <br>

## Running the tests
(Insert instruction of how to run GTests)

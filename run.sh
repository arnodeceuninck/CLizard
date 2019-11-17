# Check input loop recognition on the file main.cpp
# The output won't be written to main.cpp (-b)
# The output won't get the default suffix, but it will be outputed to "src/main.cpp-processed.cpp"
./clizard ilr -b -suf "-processed.cpp" "vbn/main.cpp"
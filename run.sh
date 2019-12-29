# Check input loop recognition on the file main.cpp
# The output won't be written to main.cpp (-b)
# The output won't get the default suffix, but it will be outputed to "src/main.cpp-processed.cpp"
./CLizard -ilr -i vbn/ILR/A.h vbn/ILR/A.cpp vbn/ILR/B.h vbn/ILR/B.cpp vbn/ILR/C.h vbn/ILR/C.cpp vbn/ILR/D.h vbn/ILR/D.cpp

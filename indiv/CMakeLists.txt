cmake_minimum_required(VERSION 3.15)
project(MB)

set(CMAKE_CXX_STANDARD 11)

add_executable(MB main.cpp PDA.cpp Graph.cpp functions.h functions.cpp CFG.cpp CFG.h const.h const.cpp GLRParser.cpp GLRParser.h)
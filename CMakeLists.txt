cmake_minimum_required(VERSION 3.15)
project(CLizard)

set(CMAKE_CXX_STANDARD 17)

add_executable(CLizard src/main.cpp src/includeLoopRecognition.cpp src/includeLoopRecognition.h src/rearrangeFunctions.cpp src/rearrangeFunctions.h src/namingConventions.cpp src/namingConventions.h src/groupPublicPrivate.cpp src/groupPublicPrivate.h src/simplifyMathematicalExpressions.cpp src/simplifyMathematicalExpressions.h)
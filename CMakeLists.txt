cmake_minimum_required(VERSION 3.15)
project(CLizard)

set(CMAKE_CXX_STANDARD 17)

set(INDIV src/indiv/PDA.cpp src/indiv/Graph.cpp src/indiv/functions.h src/indiv/functions.cpp src/indiv/CFG.cpp src/indiv/CFG.h src/indiv/const.h src/indiv/const.cpp src/indiv/GLRParser.cpp src/indiv/GLRParser.h src/indiv/ASTree.cpp src/indiv/ASTree.h)
set(ALWAYS_REQUIRED src/AST.cpp src/AST.h ${INDIV})

set(ARNO src/includeLoopRecognition.cpp src/includeLoopRecognition.h)
set(SAM src/rearrangeFunctions.cpp src/rearrangeFunctions.h)
set(TJENNE src/namingConventions.cpp src/namingConventions.h)
set(TIM src/groupPublicPrivate.cpp src/groupPublicPrivate.h src/simplifyMathematicalExpressions.cpp src/simplifyMathematicalExpressions.h)

add_executable(CLizard src/main.cpp ${ALWAYS_REQUIRED} ${ARNO} ${SAM} ${TJENNE} ${TIM})
add_executable(Arno src/mainILR.cpp ${ALWAYS_REQUIRED} ${ARNO})
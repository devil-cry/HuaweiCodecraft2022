cmake_minimum_required(VERSION 3.20)
project(CodeCraft-2022)

include_directories(${PROJECT_SOURCE_DIR}/)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)

SET(CMAKE_BUILD_TYPE "release")
if (CMAKE_BUILD_TYPE STREQUAL debug)
    add_definitions(-D_DEBUG)
endif ()

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++11")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -std=c++11")

aux_source_directory(. DIR_SRCS)

add_executable(CodeCraft-2022 ${DIR_SRCS} CodeCraft-2022.cpp Solver.cpp Solver.h SiteNode.cpp SiteNode.h UserNode.cpp UserNode.h ISAP.cpp ISAP.h)

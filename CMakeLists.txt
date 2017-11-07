cmake_minimum_required(VERSION 3.9)
project(Othello_Server)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp board.cpp uct.cpp fullsearch.cpp)
add_executable(Othello_Server ${SOURCE_FILES})

cmake_minimum_required(VERSION 3.9)
project(Othello_Server)

set(CMAKE_CXX_STANDARD 17)


set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -g -Og -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -Wall")

set(SOURCE_FILES main.cpp board.cpp uct.cpp fullsearch.cpp Book.cpp eval.cpp)
add_executable(Othello_Server ${SOURCE_FILES})

cmake_minimum_required(VERSION 3.10)

project(simple_example CXX)

include_directories(../../lib)

set(CMAKE_CXX_FLAGS 
    "-std=c++11 -Wall -Wcast-align -Wconversion -Wold-style-cast -Wwrite-strings ")

add_executable(
  simple_example
  main.cpp
)

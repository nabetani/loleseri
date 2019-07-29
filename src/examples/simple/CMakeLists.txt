cmake_minimum_required(VERSION 3.10)

project(simple_example CXX)

include_directories(../../lib)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "^Windows$")
else()
set(CMAKE_CXX_FLAGS 
    "-std=c++11 -Wall -Wcast-align -Wconversion -Wold-style-cast -Wwrite-strings ")
endif()

add_executable(
  simple_example
  main.cpp
)

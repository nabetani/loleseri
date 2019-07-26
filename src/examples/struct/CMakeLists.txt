cmake_minimum_required(VERSION 3.10)

project(struct_example CXX)

include_directories(../../lib)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wcast-align -Wcast-qual -Wconversion -Wendif-labels  -Winit-self -Winline -Wlogical-op -Wmissing-include-dirs -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-promo -Wswitch-default -Wswitch-enum -Wwrite-strings ")

add_executable(struct_example
  main.cpp
)

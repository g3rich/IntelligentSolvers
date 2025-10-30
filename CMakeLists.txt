cmake_minimum_required(VERSION 4.0)
project(BAI)

set(CMAKE_CXX_STANDARD 20)

add_executable(BAI main.cpp
        Situation.cpp
        Situation.h
        Solver.cpp
        Solver.h)

cmake_minimum_required(VERSION 3.17)
project(TA_groep)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        json.hpp
        main.cpp
        Vraag.cpp Vraag.h
        Quiz.cpp Quiz.h
        automaten/FA.cpp
        automaten/DFA.cpp
        automaten/NFA.cpp
        automaten/ENFA.cpp
        automaten/RE.cpp)

add_executable(TA_groep ${SOURCE_FILES})
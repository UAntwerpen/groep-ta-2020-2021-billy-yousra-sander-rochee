cmake_minimum_required(VERSION 3.17)
project(TA_groep)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        json.hpp
        main.cpp)

add_executable(TA_groep ${SOURCE_FILES})
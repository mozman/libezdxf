cmake_minimum_required(VERSION 3.17)
project(libezdxf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_library(libezdxf SHARED
        src/tag_loader.cpp
        src/tag_loader.hpp)

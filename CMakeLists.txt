cmake_minimum_required(VERSION 3.17)
project(ezdxf_cpp)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_executable(ezdxf_cpp
        src/tagloader.cpp
        src/tagloader.hpp)

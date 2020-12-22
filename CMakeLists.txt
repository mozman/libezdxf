cmake_minimum_required(VERSION 3.17)
project(libezdxf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)
include_directories(extern/catch)
include_directories(tests)

add_library(ezdxf SHARED
        src/ezdxf.cpp src/ezdxf.hpp
        src/tag_loader.cpp src/tag_loader.hpp
        src/tag_types.cpp src/tag_types.hpp)

add_executable(TestMain
        tests/TestMain.cpp
        tests/TestTagTypes.cpp)
target_link_libraries(TestMain PRIVATE ezdxf)

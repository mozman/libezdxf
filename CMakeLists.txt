cmake_minimum_required(VERSION 3.17)
project(ezdxf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_library(ezdxf STATIC
        src/ezdxf.cpp src/ezdxf.hpp src/types.hpp
        src/tag_loader.cpp src/tag_loader.hpp
        src/tag_types.cpp src/tag_types.hpp
        src/tag_allocator.cpp src/tag_allocator.hpp)

add_executable(run_tests
        tests/run_tests.cpp
        tests/test_tag_types.cpp)
target_link_libraries(run_tests PRIVATE ezdxf)

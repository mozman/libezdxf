cmake_minimum_required(VERSION 3.17)
project(ezdxf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_library(ezdxf STATIC
        src/ezdxf.cpp include/ezdxf/ezdxf.hpp include/ezdxf/types.hpp
        src/tag_loader.cpp include/ezdxf/tag_loader.hpp
        src/tag_types.cpp include/ezdxf/tag_types.hpp
        src/tag_allocator.cpp include/ezdxf/tag_allocator.hpp)

add_executable(run_tests
        tests/run_tests.cpp
        tests/test_tag_types.cpp)
target_link_libraries(run_tests PRIVATE ezdxf)

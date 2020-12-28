cmake_minimum_required(VERSION 3.17)
project(ezdxf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_library(ezdxf STATIC
        include/ezdxf/ezdxf.hpp
        include/ezdxf/math.hpp
        include/ezdxf/type.hpp
        include/ezdxf/math/base.hpp
        include/ezdxf/math/vec3.hpp
        include/ezdxf/tag/allocator.hpp
        include/ezdxf/tag/loader.hpp
        include/ezdxf/tag/tag.hpp
        src/ezdxf.cpp
        src/tag/allocator.cpp
        src/tag/loader.cpp
        src/tag/tag.cpp
        )

add_executable(run_tests
        tests/run_tests.cpp
        tests/test_math_base.cpp
        tests/test_math_vec3.cpp
        tests/test_tag_tag.cpp
        )

target_link_libraries(run_tests PRIVATE ezdxf)

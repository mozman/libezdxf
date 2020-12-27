cmake_minimum_required(VERSION 3.17)
project(ezdxf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_library(ezdxf STATIC
        src/ezdxf.cpp include/ezdxf/ezdxf.hpp include/ezdxf/types.hpp
        src/tag_loader.cpp include/ezdxf/tag_loader.hpp
        src/tag_types.cpp include/ezdxf/tag_types.hpp
        src/tag_allocator.cpp include/ezdxf/tag_allocator.hpp
        include/ezdxf/math/vec3.hpp include/ezdxf/math/base.hpp include/ezdxf/math.hpp)

add_executable(run_tests
        tests/run_tests.cpp
        tests/test_tag_types.cpp tests/test_math_vec3.cpp tests/test_math_base.cpp)
target_link_libraries(run_tests PRIVATE ezdxf)

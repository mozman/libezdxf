cmake_minimum_required(VERSION 3.17)
project(ezdxf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(extern)

add_library(ezdxf STATIC
        include/ezdxf/ezdxf.hpp
        include/ezdxf/math.hpp
        include/ezdxf/resource_reference.hpp
        include/ezdxf/type.hpp
        include/ezdxf/math/base.hpp
        include/ezdxf/math/vec3.hpp
        include/ezdxf/tag/loader.hpp
        include/ezdxf/tag/tag.hpp
        include/ezdxf/utils.hpp
        src/ezdxf.cpp
        src/tag/loader.cpp
        src/tag/tag.cpp
        src/type.cpp
        src/utils.cpp
        )

add_executable(run_tests
        tests/run_tests.cpp
        tests/0_tag/001_tag.cpp
        tests/0_tag/002_loader.cpp
        tests/1_math/101_base.cpp
        tests/1_math/102_vec3.cpp
        tests/2_utils/201_trim_strings.cpp
        tests/2_utils/202_safe_converters.cpp
        tests/2_utils/203_hexlify.cpp tests/2_utils/204_dxf_version.cpp)

target_link_libraries(run_tests PRIVATE ezdxf)

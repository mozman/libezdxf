// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include "catch.hpp"
#include "ezdxf.hpp"

TEST_CASE("Vertex group codes for the x-axis should be VERTEX.",
          "[tag_types]") {
    // A x-axis vertex tag starts the collecting process of 2 or 3 vertex axis.
    int code = GENERATE(10, 18, 210, 1010, 1013);
    REQUIRE(ezdxf::group_code_type(code) == ezdxf::TagType::VERTEX);
}

TEST_CASE("Vertex group codes for y- and z-axis should be DECIMAL.",
          "[tag_types]") {
    // The vertex collecting process should not be started by a y- or z-axis
    // tag, ezdxf consider vertex ordering deviation from xyz as invalid,
    // but unordered axis are valid by the DXF reference -- most (all usable)
    // DXF related tools write vertices in xyz order!
    // The Python version of ezdxf has a recover mode which can read and
    // restructure files with unordered vertex axis.
    int code = GENERATE(20, 30, 220, 230, 1020, 1030);
    REQUIRE(ezdxf::group_code_type(code) == ezdxf::TagType::DECIMAL);
}

TEST_CASE("Check group codes of type INTEGER.",
          "[tag_types]") {
    int code = GENERATE(60, 79, 1060, 1071);
    REQUIRE(ezdxf::group_code_type(code) == ezdxf::TagType::INTEGER);
}

TEST_CASE("Check group codes of type TEXT.",
          "[tag_types]") {
    int code = GENERATE(0, 9, 100, 1000);
    REQUIRE(ezdxf::group_code_type(code) == ezdxf::TagType::TEXT);
}

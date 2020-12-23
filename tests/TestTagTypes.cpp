// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include "catch.hpp"
#include "ezdxf.cpp"

TEST_CASE("Vertex group codes for the x-axis should be VERTEX.",
          "[tag_types]") {
    // A x-axis vertex tag starts the collecting process of 2 or 3 vertex axis.
    REQUIRE(ezdxf::group_code_type(10) == ezdxf::TagType::VERTEX);
    REQUIRE(ezdxf::group_code_type(18) == ezdxf::TagType::VERTEX);
    REQUIRE(ezdxf::group_code_type(210) == ezdxf::TagType::VERTEX);
    REQUIRE(ezdxf::group_code_type(1010) == ezdxf::TagType::VERTEX);
    REQUIRE(ezdxf::group_code_type(1013) == ezdxf::TagType::VERTEX);
}

TEST_CASE("Vertex group codes for y- and z-axis should be DECIMAL.",
          "[tag_types]") {
    // The vertex collecting process should not be started by a y- or z-axis
    // tag, ezdxf consider vertex ordering deviation from xyz as invalid,
    // but this is valid by the DXF reference -- most (all usable) DXF related
    // tools write vertices in xyz order! The Python version of ezdxf has a
    // recover mode which can read files with unordered vertex axis.
    REQUIRE(ezdxf::group_code_type(20) == ezdxf::TagType::DECIMAL);
    REQUIRE(ezdxf::group_code_type(30) == ezdxf::TagType::DECIMAL);
    REQUIRE(ezdxf::group_code_type(220) == ezdxf::TagType::DECIMAL);
    REQUIRE(ezdxf::group_code_type(1020) == ezdxf::TagType::DECIMAL);
}

TEST_CASE("Check group codes of type INTEGER.",
          "[tag_types]") {
    REQUIRE(ezdxf::group_code_type(60) == ezdxf::TagType::INTEGER);
    REQUIRE(ezdxf::group_code_type(79) == ezdxf::TagType::INTEGER);
    REQUIRE(ezdxf::group_code_type(1060) == ezdxf::TagType::INTEGER);
    REQUIRE(ezdxf::group_code_type(1071) == ezdxf::TagType::INTEGER);
}

TEST_CASE("Check group codes of type TEXT.",
          "[tag_types]") {
    REQUIRE(ezdxf::group_code_type(0) == ezdxf::TagType::TEXT);
    REQUIRE(ezdxf::group_code_type(1000) == ezdxf::TagType::TEXT);
}

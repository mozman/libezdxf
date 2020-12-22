// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include "catch.hpp"
#include "ezdxf.cpp"

TEST_CASE("Group code type of x-axis tags should be VERTEX.", "[tag][types]") {
    REQUIRE(ezdxf::group_code_type(10) == ezdxf::TagType::VERTEX);
    REQUIRE(ezdxf::group_code_type(18) == ezdxf::TagType::VERTEX);
    REQUIRE(ezdxf::group_code_type(210) == ezdxf::TagType::VERTEX);
    REQUIRE(ezdxf::group_code_type(1010) == ezdxf::TagType::VERTEX);
    REQUIRE(ezdxf::group_code_type(1013) == ezdxf::TagType::VERTEX);
}

TEST_CASE("Group code type of y- and z- axis tags should be DECIMAL.",
          "[tag][types]") {
    REQUIRE(ezdxf::group_code_type(20) == ezdxf::TagType::DECIMAL);
    REQUIRE(ezdxf::group_code_type(30) == ezdxf::TagType::DECIMAL);
    REQUIRE(ezdxf::group_code_type(220) == ezdxf::TagType::DECIMAL);
    REQUIRE(ezdxf::group_code_type(1020) == ezdxf::TagType::DECIMAL);
}

TEST_CASE("Group code of type INTEGER.", "[tag][types]") {
    REQUIRE(ezdxf::group_code_type(60) == ezdxf::TagType::INTEGER);
    REQUIRE(ezdxf::group_code_type(79) == ezdxf::TagType::INTEGER);
    REQUIRE(ezdxf::group_code_type(1060) == ezdxf::TagType::INTEGER);
    REQUIRE(ezdxf::group_code_type(1071) == ezdxf::TagType::INTEGER);
}

TEST_CASE("Group code of type STRING.", "[tag][types]") {
    REQUIRE(ezdxf::group_code_type(0) == ezdxf::TagType::STRING);
    REQUIRE(ezdxf::group_code_type(1000) == ezdxf::TagType::STRING);
}

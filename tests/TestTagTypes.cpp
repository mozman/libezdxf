// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include "catch.hpp"
#include "ezdxf.cpp"

TEST_CASE( "Life, the universe and everything", "[42][theAnswer]" ) {
    REQUIRE(ezdxf::group_code_type(0) == ezdxf::TagType::STRING );
}
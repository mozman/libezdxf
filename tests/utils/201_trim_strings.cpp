// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include <catch2/catch.hpp>
#include <ezdxf/utils.hpp>

using namespace ezdxf::utils;

TEST_CASE("Test trimming strings.", "[utils][trim]") {
    SECTION("Test rtrim()") {
        auto s = std::string("\t123    \t\r\n");
        rtrim(s);
        REQUIRE(s == "\t123");

        rtrim(s);  // do nothing
        REQUIRE(s == "\t123");
    }

    SECTION("Test ltrim())") {
        auto s = std::string("\t   123\n");
        ltrim(s);
        REQUIRE(s == "123\n");

        ltrim(s);  // do nothing
        REQUIRE(s == "123\n");
    }

    SECTION("Test trim())") {
        auto s = std::string("\t   123  \n");
        trim(s);
        REQUIRE(s == "123");

        trim(s);  // do nothing
        REQUIRE(s == "123");
    }

    SECTION("Test rtrim_endl())") {
        auto s = std::string("\t   123  \r\n");
        rtrim_endl(s);
        REQUIRE(s == "\t   123  ");

        rtrim_endl(s); // do nothing
        REQUIRE(s == "\t   123  ");

        s = "\t   123  \n\r";
        rtrim_endl(s);
        REQUIRE(s == "\t   123  ");

        // Remove endl just from end of string:
        s = "\n   123  \n\r";
        rtrim_endl(s);
        REQUIRE(s == "\n   123  ");
    }
}
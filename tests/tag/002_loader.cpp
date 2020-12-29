// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include <catch2/catch.hpp>
#include <ezdxf/tag/loader.hpp>


TEST_CASE("Test BasicLoader from strings.", "[tag][BasicLoader]") {
    SECTION("Test valid structure tags.") {
        auto bl = ezdxf::tag::BasicLoader("0\nSECTION\n0\nEOF");
        REQUIRE(bl.is_empty() == false);

        // Look at first tag:
        REQUIRE(bl.peek().is_struct_tag(0, "SECTION"));
        REQUIRE(bl.is_empty() == false);

        // Get first tag:
        auto tag0 = bl.get();
        REQUIRE(tag0.is_struct_tag(0, "SECTION"));
        REQUIRE(bl.is_empty() == false);

        // Look at the next tag:
        REQUIRE(bl.peek().is_struct_tag(0, "EOF"));

        // Get last tag:
        auto tag1= bl.get();
        REQUIRE(tag1.is_struct_tag(0, "EOF"));

        // Look at the next tag or in this case EOF is reached:
        REQUIRE(bl.peek().is_error_tag());
        REQUIRE(bl.is_empty() == true);
    }

    SECTION("Test structure tags with whitespace and mixed line ending.") {
        auto bl = ezdxf::tag::BasicLoader(" 0 \n  SECTION \r\n  0\n  EOF \r\n");
        auto tag = bl.get();
        REQUIRE(tag.is_struct_tag(0, "SECTION"));
        tag= bl.get();
        REQUIRE(tag.is_struct_tag(0, "EOF"));
        REQUIRE(bl.is_empty() == true);
    }

    SECTION("Test skip comments.") {
        auto bl = ezdxf::tag::BasicLoader("999\nxxx\n0\nSECTION\n0\nEOF\n999\nxxx\n");
        auto tag = bl.get();
        REQUIRE(tag.is_struct_tag(0, "SECTION"));
        tag= bl.get();
        REQUIRE(tag.is_struct_tag(0, "EOF"));
        REQUIRE(bl.is_empty() == true);
    }

    SECTION("Test if empty string tags are preserved.") {
        auto bl = ezdxf::tag::BasicLoader("1\n\n2\n \n0\nEOF\n");
        auto tag = bl.get();
        REQUIRE(tag.group_code() == 1);
        REQUIRE(tag.string().empty());

        tag= bl.get();
        REQUIRE(tag.group_code() == 2);
        REQUIRE(tag.string() == " ");
    }
}
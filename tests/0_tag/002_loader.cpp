// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include <catch2/catch.hpp>
#include <ezdxf/tag/loader.hpp>


TEST_CASE("Test BasicLoader() loading from strings.", "[tag][BasicLoader]") {
    SECTION("Test valid structure tags.") {
        auto reader = ezdxf::tag::BasicLoader("0\nSECTION\n0\nEOF");
        REQUIRE(reader.is_empty() == false);

        // Look at first tag:
        REQUIRE(reader.peek().equals(0, "SECTION"));
        REQUIRE(reader.is_empty() == false);

        // Get first tag:
        auto tag = reader.get();
        REQUIRE(tag.equals(0, "SECTION"));
        REQUIRE(reader.is_empty() == false);

        // Look at the next tag:
        REQUIRE(reader.peek().equals(0, "EOF"));

        // Get last tag:
        tag = reader.get();
        REQUIRE(tag.equals(0, "EOF"));

        // Look at the next tag or in this case EOF is reached:
        REQUIRE(reader.peek().is_error_tag());
        REQUIRE(reader.is_empty() == true);
    }

    SECTION("Test loading structure tags surrounded by whitespaces and "
            "mixed line endings.") {
        auto reader = ezdxf::tag::BasicLoader(
                " 0 \n  SECTION \r\n  0\n  EOF \r\n");
        auto tag = reader.get();
        REQUIRE(tag.equals(0, "SECTION"));
        tag = reader.get();
        REQUIRE(tag.equals(0, "EOF"));
        REQUIRE(reader.is_empty());
    }

    SECTION("Test if comments will be skipped.") {
        auto reader = ezdxf::tag::BasicLoader(
                "999\nxxx\n0\nSECTION\n0\nEOF\n999\nxxx\n");
        auto tag = reader.get();
        REQUIRE(tag.equals(0, "SECTION"));
        tag = reader.get();
        REQUIRE(tag.equals(0, "EOF"));
        // Comment tags after EOF should be removed:
        REQUIRE(reader.is_empty());
    }

    SECTION("Test if empty string tags are preserved.") {
        auto reader = ezdxf::tag::BasicLoader("1\n\n2\n \n0\nEOF\n");
        auto tag = reader.get();
        REQUIRE(tag.group_code() == 1);
        REQUIRE(tag.string().empty());

        tag = reader.get();
        REQUIRE(tag.group_code() == 2);
        REQUIRE(tag.string() == " ");
    }
}
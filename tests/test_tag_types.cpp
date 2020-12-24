// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include "catch.hpp"
#include "tag_types.cpp" // test local defined objects

TEST_CASE("Test group codes of type kText.", "[tag_types]") {
    short code = GENERATE(0, 8, 100, 1000);
    REQUIRE(ezdxf::group_code_type(code) == ezdxf::TagType::kText);
}

TEST_CASE("Test group codes of type kInteger.", "[tag_types]") {
    short code = GENERATE(60, 70, 79, 1060, 1071);
    REQUIRE(ezdxf::group_code_type(code) == ezdxf::TagType::kInteger);
}

TEST_CASE("Test group codes of type kDecimal.", "[tag_types]") {
    short code = GENERATE(40, 48, 50, 51, 1014, 1059);
    REQUIRE(ezdxf::group_code_type(code) == ezdxf::TagType::kDecimal);
}

TEST_CASE("Test vertex group codes.", "[tag_types]") {
    SECTION("Vertex x-axis group code is kVertex") {
        // A x-axis vertex tag starts the collecting process of 2 or 3 vertex axis.
        short code = GENERATE(10, 18, 210, 1010, 1013);
        REQUIRE(ezdxf::group_code_type(code) == ezdxf::TagType::kVertex);
    }

    SECTION("Vertex y- and z-axis group codes is kDecimal") {
        // The vertex collecting process should not be started by a y- or z-axis
        // tag, ezdxf consider vertex ordering deviation from xyz as invalid,
        // but unordered axis are valid by the DXF reference -- most (all usable)
        // DXF related tools write vertices in xyz order!
        // The Python version of ezdxf has a recover mode which can read and
        // restructure files with unordered vertex axis.
        short code = GENERATE(20, 30, 220, 230, 1020, 1030);
        REQUIRE(ezdxf::group_code_type(code) == ezdxf::TagType::kDecimal);
    }
}

TEST_CASE("Test group codes out of defined range are kUndefined.",
          "[tag_types]") {
    short code = GENERATE(-1, 1072);
    REQUIRE(ezdxf::group_code_type(code) == ezdxf::TagType::kUndefined);

}

TEST_CASE("Test if TagType::kUndefined is 0", "[tag_types]") {
    REQUIRE(ezdxf::TagType::kUndefined == 0);
}

TEST_CASE("Test TagTypeCache", "[tag_types]") {
    auto cache = ezdxf::TagTypeCache();

    SECTION("Test if new cache is empty.") {
        for (short code = 0; code < ezdxf::kGroupCodeCount; code++) {
            REQUIRE(cache.get(code) == ezdxf::TagType::kUndefined);
        }
    }
    // Fill cache with some data:
    cache.set(0, ezdxf::TagType::kText);
    cache.set(8, ezdxf::TagType::kText);
    cache.set(10, ezdxf::TagType::kVertex);

    SECTION("Test cache hit.") {
        REQUIRE(cache.get(0) == ezdxf::TagType::kText);
        REQUIRE(cache.get(8) == ezdxf::TagType::kText);
        REQUIRE(cache.get(10) == ezdxf::TagType::kVertex);
    }

    SECTION("Test cache miss.") {
        short code = GENERATE(1, 11, 1000);
        REQUIRE(cache.get(code) == ezdxf::TagType::kUndefined);
    }

    SECTION("Test if group codes out of defined range are kUndefined.") {
        short code = GENERATE(-1, 1072);
        REQUIRE(cache.get(code) == ezdxf::TagType::kUndefined);
    }
}
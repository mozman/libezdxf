// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include "catch.hpp"
#include "../src/tag_types.cpp" // test local defined objects

TEST_CASE("Test group codes of type kString.", "[tag_types]") {
    short code = GENERATE(0, 8, 100, 1000);
    REQUIRE(ezdxf::group_code_type(code) == ezdxf::TagType::kString);
}

TEST_CASE("Test group codes of type kInteger.", "[tag_types]") {
    short code = GENERATE(60, 70, 79, 1060, 1071);
    REQUIRE(ezdxf::group_code_type(code) == ezdxf::TagType::kInteger);
}

TEST_CASE("Test group codes of type kReal.", "[tag_types]") {
    short code = GENERATE(40, 48, 50, 51, 1014, 1059);
    REQUIRE(ezdxf::group_code_type(code) == ezdxf::TagType::kReal);
}

TEST_CASE("Test vertex group codes.", "[tag_types]") {
    SECTION("Vertex x-axis group code is kVec3") {
        // A x-axis vertex tag starts the collecting process of 2 or 3 vertex axis.
        short code = GENERATE(10, 18, 210, 1010, 1013);
        REQUIRE(ezdxf::group_code_type(code) == ezdxf::TagType::kVec3);
    }

    SECTION("Vertex y- and z-axis group codes is kReal") {
        // The vertex collecting process should not be started by a y- or z-axis
        // tag, ezdxf consider vertex ordering deviation from xyz as invalid,
        // but unordered axis are valid by the DXF reference -- most (all usable)
        // DXF related tools write vertices in xyz order!
        // The Python version of ezdxf has a recover mode which can read and
        // restructure files with unordered vertex axis.
        short code = GENERATE(20, 30, 220, 230, 1020, 1030);
        REQUIRE(ezdxf::group_code_type(code) == ezdxf::TagType::kReal);
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
    cache.set(0, ezdxf::TagType::kString);
    cache.set(8, ezdxf::TagType::kString);
    cache.set(10, ezdxf::TagType::kVec3);

    SECTION("Test cache hit.") {
        REQUIRE(cache.get(0) == ezdxf::TagType::kString);
        REQUIRE(cache.get(8) == ezdxf::TagType::kString);
        REQUIRE(cache.get(10) == ezdxf::TagType::kVec3);
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

TEST_CASE("Test StringTag", "[tag_types]") {
    auto tag = ezdxf::StringTag{0, "LINE"};
    SECTION("Test get dedicated value type.") {
        REQUIRE(tag.type() == ezdxf::TagType::kString);
        REQUIRE(tag.group_code() == 0);
        REQUIRE(tag.string() == "LINE");
    }

    SECTION("Test for specific structure tags as string tags.") {
        REQUIRE(ezdxf::StringTag(0, "SECTION").equals(0, "SECTION") == true);
        REQUIRE(ezdxf::StringTag(0, "SECTION").equals(0, "ENDSEC") == false);
        REQUIRE(ezdxf::StringTag(100, "AcDbEntity").equals(100, "AcDbEntity") == true);
    }

    SECTION("Test for specific structure tags at invalid tag types.") {
        // This equal test should not throw an exception!
        REQUIRE(ezdxf::IntegerTag(0, 0).equals(0, "SECTION") == false);
        REQUIRE(ezdxf::RealTag(0, 0).equals(0, "SECTION") == false);
    }

    SECTION("Test other type values throw bad_cast exceptions.") {
        REQUIRE_THROWS_AS(tag.real(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.integer(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.vec3(), std::bad_cast);
    }
}

TEST_CASE("Test IntegerTag", "[tag_types]") {
    auto tag = ezdxf::IntegerTag{70, 16};
    SECTION("Test get dedicated value type.") {
        REQUIRE(tag.type() == ezdxf::TagType::kInteger);
        REQUIRE(tag.group_code() == 70);
        REQUIRE(tag.integer() == 16);
    }

    SECTION("Test other type values throw bad_cast exceptions.") {
        REQUIRE_THROWS_AS(tag.real(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.string(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.vec3(), std::bad_cast);
    }
}

TEST_CASE("Test RealTag", "[tag_types]") {
    auto tag = ezdxf::RealTag{40, 1.0};
    SECTION("Test get dedicated value type.") {
        REQUIRE(tag.type() == ezdxf::TagType::kReal);
        REQUIRE(tag.group_code() == 40);
        REQUIRE(tag.real() == 1.0);
    }

    SECTION("Test other type values throw bad_cast exceptions.") {
        REQUIRE_THROWS_AS(tag.integer(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.string(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.vec3(), std::bad_cast);
    }
}

TEST_CASE("Test Vec3Tag", "[tag_types]") {
    auto tag = ezdxf::Vec3Tag{10, 1.0, 2.0, 3.0};
    SECTION("Test get dedicated value type.") {
        REQUIRE(tag.type() == ezdxf::TagType::kVec3);
        REQUIRE(tag.group_code() == 10);
        REQUIRE(tag.vec3() == ezdxf::math::Vec3(1.0, 2.0, 3.0));
        REQUIRE(tag.export_z());
    }

    SECTION("Test other type values throw bad_cast exceptions.") {
        REQUIRE_THROWS_AS(tag.integer(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.string(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.real(), std::bad_cast);
    }
}

TEST_CASE("Store different tag types in a container.", "[tag_types]") {
    auto container = std::vector<ezdxf::DXFTag *>{};
    container.push_back(new ezdxf::StringTag(1, "NAME"));
    container.push_back(new ezdxf::IntegerTag(70, 7));
    container.push_back(new ezdxf::RealTag(40, 13.0));

    REQUIRE(container.size() == 3);
    REQUIRE(container[0]->type() == ezdxf::TagType::kString);
    REQUIRE(container[0]->string() == "NAME");
    REQUIRE(container[1]->type() == ezdxf::TagType::kInteger);
    REQUIRE(container[1]->integer() == 7);
    REQUIRE(container[2]->type() == ezdxf::TagType::kReal);
    REQUIRE(container[2]->real() == 13.0);
}
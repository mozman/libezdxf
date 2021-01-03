// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include <catch2/catch.hpp>
#include "../../src/tag/tag.cpp" // test local defined objects

using namespace ezdxf::tag;

TEST_CASE("Test group codes of type kString.", "[tag]") {
    int code = GENERATE(0, 8, 100, 1000);
    REQUIRE(group_code_type(code) == TagType::kString);
}

TEST_CASE("Test group codes of type kInteger.", "[tag]") {
    int code = GENERATE(60, 70, 79, 1060, 1071);
    REQUIRE(group_code_type(code) == TagType::kInteger);
}

TEST_CASE("Test group codes of type kReal.", "[tag]") {
    int code = GENERATE(40, 48, 50, 51, 1014, 1059);
    REQUIRE(group_code_type(code) == TagType::kReal);
}

TEST_CASE("Test vertex group codes.", "[tag]") {
    SECTION("Vertex x-axis group code is kVec3") {
        // A x-axis vertex tag starts the collecting process of 2 or 3 vertex axis.
        int code = GENERATE(10, 18, 210, 1010, 1013);
        REQUIRE(group_code_type(code) == TagType::kVec3);
    }

    SECTION("Vertex y- and z-axis group codes is kReal") {
        // The vertex collecting process should not be started by a y- or z-axis
        // tag, ezdxf consider vertex ordering deviation from xyz as invalid,
        // but unordered axis are valid by the DXF reference -- most (all usable)
        // DXF related tools write vertices in xyz order!
        // The Python version of ezdxf has a recover mode which can read and
        // restructure files with unordered vertex axis.
        int code = GENERATE(20, 30, 220, 230, 1020, 1030);
        REQUIRE(group_code_type(code) == TagType::kReal);
    }
}

TEST_CASE("Test group codes out of defined range are kUndefined.",
          "[tag_types]") {
    int code = GENERATE(-1, 1072);
    REQUIRE(group_code_type(code) == TagType::kUndefined);

}

TEST_CASE("Test if TagType::kUndefined is 0", "[tag]") {
    REQUIRE(static_cast<int>(TagType::kUndefined) == 0);
}

TEST_CASE("Test TagTypeCache", "[tag]") {
    auto cache = TypeCache();

    SECTION("Test if new cache is empty.") {
        for (int code = 0; code < kGroupCodeCount; code++) {
            REQUIRE(cache.get(code) == TagType::kUndefined);
        }
    }
    // Fill cache with some data:
    cache.set(0, TagType::kString);
    cache.set(8, TagType::kString);
    cache.set(10, TagType::kVec3);

    SECTION("Test cache hit.") {
        REQUIRE(cache.get(0) == TagType::kString);
        REQUIRE(cache.get(8) == TagType::kString);
        REQUIRE(cache.get(10) == TagType::kVec3);
    }

    SECTION("Test cache miss.") {
        int code = GENERATE(1, 11, 1000);
        REQUIRE(cache.get(code) == TagType::kUndefined);
    }

    SECTION("Test if group codes out of defined range are kUndefined.") {
        int code = GENERATE(-1, 1072);
        REQUIRE(cache.get(code) == TagType::kUndefined);
    }
}

TEST_CASE("Test undefined DXFTag", "[tag]") {
    auto tag = DXFTag{70};
    SECTION("Test get dedicated value type.") {
        REQUIRE(tag.type() == TagType::kUndefined);
        REQUIRE(tag.is_undefined() == true);
        REQUIRE(tag.group_code() == 70);
        REQUIRE(tag.is_error_tag() == false);
        REQUIRE(tag.has_string_value() == false);
        REQUIRE(tag.has_integer_value() == false);
        REQUIRE(tag.has_real_value() == false);
        REQUIRE(tag.has_vec3_value() == false);
    }

    SECTION("Test all type values casts throw bad_cast exceptions.") {
        REQUIRE_THROWS_AS(tag.integer(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.real(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.string(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.vec3(), std::bad_cast);
    }
}

TEST_CASE("Test StringTag", "[tag]") {
    auto tag = StringTag{0, "LINE"};
    SECTION("Test get dedicated value type.") {
        REQUIRE(tag.type() == TagType::kString);
        REQUIRE(tag.group_code() == 0);
        REQUIRE(tag.string() == "LINE");
        REQUIRE(tag.is_error_tag() == false);
        REQUIRE(tag.is_undefined() == false);
        REQUIRE(tag.has_string_value() == true);
    }

    SECTION("Test for specific structure tags as string tags.") {
        REQUIRE(StringTag(0, "SECTION").equals(0, "SECTION") == true);
        REQUIRE(StringTag(0, "SECTION").equals(0, "ENDSEC") == false);
        REQUIRE(StringTag(100, "AcDbEntity").equals(
                100, "AcDbEntity") == true);
    }

    SECTION("Test for specific structure tags at invalid tag types.") {
        // This equal test should not throw an exception!
        REQUIRE(IntegerTag(0, 0).equals(0, "SECTION") == false);
        REQUIRE(RealTag(0, 0).equals(0, "SECTION") == false);
    }

    SECTION("Test other type values throw bad_cast exceptions.") {
        REQUIRE_THROWS_AS(tag.real(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.integer(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.vec3(), std::bad_cast);
    }
}

TEST_CASE("Test IntegerTag", "[tag]") {
    auto tag = IntegerTag{70, 16};
    SECTION("Test get dedicated value type.") {
        REQUIRE(tag.type() == TagType::kInteger);
        REQUIRE(tag.group_code() == 70);
        REQUIRE(tag.integer() == 16);
        REQUIRE(tag.is_error_tag() == false);
        REQUIRE(tag.is_undefined() == false);
        REQUIRE(tag.has_integer_value() == true);
    }

    SECTION("Test other type values throw bad_cast exceptions.") {
        REQUIRE_THROWS_AS(tag.real(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.string(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.vec3(), std::bad_cast);
    }
}

TEST_CASE("Test RealTag", "[tag]") {
    auto tag = RealTag{40, 1.0};
    SECTION("Test get dedicated value type.") {
        REQUIRE(tag.type() == TagType::kReal);
        REQUIRE(tag.group_code() == 40);
        REQUIRE(tag.real() == 1.0);
        REQUIRE(tag.is_error_tag() == false);
        REQUIRE(tag.is_undefined() == false);
        REQUIRE(tag.has_real_value() == true);
    }

    SECTION("Test other type values throw bad_cast exceptions.") {
        REQUIRE_THROWS_AS(tag.integer(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.string(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.vec3(), std::bad_cast);
    }
}

TEST_CASE("Test Vec3Tag", "[tag]") {
    auto tag = Vec3Tag{10, 1.0, 2.0, 3.0};
    SECTION("Test get dedicated value type.") {
        REQUIRE(tag.type() == TagType::kVec3);
        REQUIRE(tag.group_code() == 10);
        REQUIRE(tag.vec3() == Vec3(1.0, 2.0, 3.0));
        REQUIRE(tag.has_vec3_value() == true);
        REQUIRE(tag.is_error_tag() == false);
        REQUIRE(tag.is_undefined() == false);
        REQUIRE(tag.export_vec2() == false);
    }

    SECTION("Test other type values throw bad_cast exceptions.") {
        REQUIRE_THROWS_AS(tag.integer(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.string(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.real(), std::bad_cast);
    }
}

TEST_CASE("Test Vec2Tag, Vec3 type loaded as 2d type.", "[tag]") {
    auto tag = Vec2Tag{10, 1.0, 2.0};
    SECTION("Test get dedicated value type.") {
        // The following two tests have different results to Vec3Tag.
        REQUIRE(tag.type() == TagType::kVec2);
        REQUIRE(tag.export_vec2() == true);

        // Everything else should be equal to Vec3tag:
        REQUIRE(tag.group_code() == 10);
        REQUIRE(tag.vec3() == Vec3(1.0, 2.0, 0.0));
        REQUIRE(tag.has_vec3_value() == true);
        REQUIRE(tag.is_error_tag() == false);
        REQUIRE(tag.is_undefined() == false);
    }

    SECTION("Test other type values throw bad_cast exceptions.") {
        REQUIRE_THROWS_AS(tag.integer(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.string(), std::bad_cast);
        REQUIRE_THROWS_AS(tag.real(), std::bad_cast);
    }
}

TEST_CASE("Store polymorphic tag types in a vector.", "[tag]") {
    auto container = std::vector<std::unique_ptr<DXFTag>>{};
    container.push_back(std::make_unique<StringTag>(1, "NAME"));
    container.push_back(std::make_unique<IntegerTag>(70, 7));
    container.push_back(std::make_unique<RealTag>(40, 13.0));

    REQUIRE(container.size() == 3);
    REQUIRE(container[0]->type() == TagType::kString);
    REQUIRE(container[0]->string() == "NAME");
    REQUIRE(container[1]->type() == TagType::kInteger);
    REQUIRE(container[1]->integer() == 7);
    REQUIRE(container[2]->type() == TagType::kReal);
    REQUIRE(container[2]->real() == 13.0);
}

TEST_CASE("Test error tag.", "[tag]") {
    auto error = ezdxf::tag::make_error_tag();
    REQUIRE(error->is_error_tag() == true);
    REQUIRE(error->type() == TagType::kUndefined);
    // Indicates only an error and has no error message attached:
    REQUIRE_THROWS_AS(error->string(), std::bad_cast);
}

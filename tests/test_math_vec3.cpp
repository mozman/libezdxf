// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include "catch.hpp"
#include <ezdxf/math/vec3.hpp>

using ezdxf::math::Vec3;

TEST_CASE("Test vector comparisons.", "[math][vec3]") {
    // See also testing of ezdxf::math::is_close in test_math_base.cpp

    SECTION ("Test comparison operators near 0.0") {
        // Default absolute tolerance is 1e-12.
        auto v = Vec3{};
        REQUIRE(v == Vec3(0, 0, 0));
        REQUIRE(v == Vec3(1e-12, 0, 0));
        REQUIRE(v == Vec3(0, 1e-12, 0));
        REQUIRE(v == Vec3(0, 0, 1e-12));
        REQUIRE(v.is_close(Vec3(1e-12, 1e-12, 1e-12)) == true);

        // Leaving the tolerance zone:
        REQUIRE(v != Vec3(1e-11, 0, 0));
        REQUIRE(v != Vec3(0, 1e-11, 0));
        REQUIRE(v != Vec3(0, 0, 1e-11));
        REQUIRE(v.is_close(Vec3(1e-11, 1e-11, 1e-11)) == false);
    }

    SECTION ("Test comparison operators near 1.0") {
        // Default relative tolerance is 1e-9:
        auto v = Vec3{1, 0, 0};
        REQUIRE(v == Vec3(1 + 1e-10, 0, 0));
        REQUIRE(v.is_close(Vec3(1 + 1e-10, 0, 0)) == true);
        // This should be true, but floating point imprecision shows up:
        REQUIRE(v != Vec3(1 + 1e-9, 0, 0));
        // Leaving the tolerance zone:
        REQUIRE(v != Vec3(1 + 1e-8, 0, 0));
        REQUIRE(v.is_close(Vec3(1 + 1e-8, 0, 0)) == false);
    }

    SECTION ("Test is_close() with custom absolute tolerance.") {
        // Default relative tolerance is 1e-9:
        auto v = Vec3{1, 1, 1};
        const double abs_tol = 1e-8;
        const double in = 1.0 + 1e-9; // 'in' the tolerance zone
        const double out = 1.0 + 1e-7; // 'out' the tolerance zone

        REQUIRE(v.is_close(Vec3(in, 1, 1), abs_tol) == true);
        REQUIRE(v.is_close(Vec3(1, in, 1), abs_tol) == true);
        REQUIRE(v.is_close(Vec3(1, 1, in), abs_tol) == true);
        REQUIRE(v.is_close(Vec3(in, in, in), abs_tol) == true);

        REQUIRE(v.is_close(Vec3(1 + abs_tol, 1, 1), abs_tol) == true);
        // Leaving the tolerance zone:
        REQUIRE(v.is_close(Vec3(out, 1, 1), abs_tol) == false);
        REQUIRE(v.is_close(Vec3(1, out, 1), abs_tol) == false);
        REQUIRE(v.is_close(Vec3(1, 1, out), abs_tol) == false);
        REQUIRE(v.is_close(Vec3(out, out, out), abs_tol) == false);
    }
}


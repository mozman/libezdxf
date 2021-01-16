// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include <catch2/catch.hpp>
#include "ezdxf/math/base.hpp"

using ezdxf::math::is_close;

TEST_CASE("Testing ezdxf::math::is_close()", "[math]") {
    // Default absolute tolerance for comparisons is 1e-12
    // Relative tolerance for comparisons is a fixed value: 1e-9

    SECTION("Test equality near 0.0 using the default tolerance.") {
        // Relative tolerance has no impact near 0.0:
        REQUIRE(is_close(0, 0) == true);
        REQUIRE(is_close(1e-12, 0) == true);
        // Leaving the tolerance zone:
        REQUIRE(is_close(1e-11, 0) == false);
    }

    SECTION("Test equality near 0.0 using a custom absolute tolerance.") {
        // Relative tolerance has no impact near 0.0:
        const double abs_tol = 1e-9;
        REQUIRE(is_close(0, 0, abs_tol) == true);
        REQUIRE(is_close(1e-9, 0, abs_tol) == true);
        // Leaving the tolerance zone:
        REQUIRE(is_close(1e-8, 0, abs_tol) == false);
    }

    SECTION("Test equality near 1.0 using the default tolerance.") {
        // Relative tolerance of 1e-9 is now in charge:
        REQUIRE(is_close(1, 1) == true);
        REQUIRE(is_close(1.0 + 1e-12, 1.0) == true);
        REQUIRE(is_close(1.0 + 1e-10, 1.0) == true);
        // This should be true, but floating point imprecision shows up:
        REQUIRE(is_close(1.0 + ezdxf::math::kRelTol, 1.0) == false);
        // Leaving the tolerance zone:
        REQUIRE(is_close(1.0 + 1e-8, 1.0) == false);
    }

    SECTION("Test equality near 1.0 using a custom absolute tolerance.") {
        const double abs_tol = 1e-8;
        REQUIRE(is_close(1, 1, abs_tol) == true);
        REQUIRE(is_close(1.0 + 1e-10, 1.0, abs_tol) == true);
        // Absolute tolerance overrides relative tolerance of 1e-9:
        REQUIRE(is_close(1.0 + ezdxf::math::kRelTol, 1.0, abs_tol) == true);
        // This is true as it should be, but also shows the unreliability of
        // floating point comparisons (see previous test):
        REQUIRE(is_close(1.0 + abs_tol, 1.0, abs_tol) == true);
        // Leaving the tolerance zone:
        REQUIRE(is_close(1.0 + 1e-7, 1.0, abs_tol) == false);
    }

}
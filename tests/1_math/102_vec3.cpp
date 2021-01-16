// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include <catch2/catch.hpp>
#include <vector>
#include "ezdxf/math/vec3.hpp"


using ezdxf::math::Vec3;

TEST_CASE("Test Vec3 basic usage.", "[math][vec3]") {
    SECTION("Test access to data members.") {
        auto v = Vec3{1, 2, 3};
        REQUIRE(v.x() == 1.0);
        REQUIRE(v.y() == 2.0);
        REQUIRE(v.z() == 3.0);
    }

    SECTION("Test tuple deconstruction of data members.") {
        auto v = Vec3{1, 2, 3};
        auto[x, y, z] = v.tuple();
        REQUIRE(x == 1.0);
        REQUIRE(y == 2.0);
        REQUIRE(z == 3.0);
    }

    SECTION("Test Vec3 as variable.") {
        auto sum = Vec3{};
        for (auto d: std::vector<double>{1, 2, 3}) {
            // Operator "+=" is not implemented, because Vec3() is designed as
            // immutable data type:
            sum = sum + Vec3(d, d, d);
        }
        REQUIRE(sum == Vec3(6, 6, 6));
    }

    SECTION("Test Vec3 usage in a vector container.") {
        auto v = Vec3{1, 2, 3};
        auto vertices = std::vector<Vec3>{};
        vertices.push_back(v);
        vertices.push_back(v);
        vertices.push_back(v);

        REQUIRE(vertices.size() == 3);
        REQUIRE(vertices[0] == v);

        // Remove last vertex from container:
        auto v2 = vertices[2];
        vertices.pop_back();
        REQUIRE(v2 == v);
    }
}


TEST_CASE("Test vector comparisons.", "[math][vec3]") {
    // See also testing of ezdxf::math::is_close in test_math_base.cpp

    SECTION ("Test comparison operators near 0.0") {
        // Default absolute tolerance is 1e-12.
        auto v = Vec3{};
        const double x = 1e-12;
        REQUIRE(v == Vec3(0, 0, 0));
        REQUIRE(v == Vec3(x, 0, 0));
        REQUIRE(v == Vec3(0, x, 0));
        REQUIRE(v == Vec3(0, 0, x));
        REQUIRE(v.is_close(Vec3(x, x, x)) == true);

        // Leaving the tolerance zone:
        const double y = 1e-11;
        REQUIRE(v != Vec3(y, 0, 0));
        REQUIRE(v != Vec3(0, y, 0));
        REQUIRE(v != Vec3(0, 0, y));
        REQUIRE(v.is_close(Vec3(y, y, y)) == false);
    }

    SECTION ("Test is_close_zero()") {
        // Default absolute tolerance is 1e-12.
        const double x = 1e-12;
        REQUIRE(Vec3().is_close_zero() == true);
        REQUIRE(Vec3(x, 0, 0).is_close_zero() == true);
        REQUIRE(Vec3(0, x, 0).is_close_zero() == true);
        REQUIRE(Vec3(0, 0, x).is_close_zero() == true);
        REQUIRE(Vec3(x, x, x).is_close_zero() == true);
    }

    SECTION ("Test not is_close_zero()") {
        // Default absolute tolerance is 1e-12.
        const double x = 1e-10;
        REQUIRE(Vec3(x, 0, 0).is_close_zero() == false);
        REQUIRE(Vec3(0, x, 0).is_close_zero() == false);
        REQUIRE(Vec3(0, 0, x).is_close_zero() == false);
        REQUIRE(Vec3(x, x, x).is_close_zero() == false);
    }

    SECTION ("Test is_close_zero() with user defined tolerance") {
        const double usr_tol = 1e-9;
        const double x = 1e-10;
        const double y = 1e-8;
        REQUIRE(Vec3(x, x, x).is_close_zero(usr_tol) == true);
        REQUIRE(Vec3(usr_tol, 0, 0).is_close_zero(usr_tol) == true);
        REQUIRE(Vec3(y, y, y).is_close_zero(usr_tol) == false);
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

    SECTION ("Test is_close() with user defined tolerance.") {
        // Default relative tolerance is 1e-9:
        auto v = Vec3{1, 1, 1};
        const double usr_tol = 1e-8;
        const double in = 1.0 + 1e-9; // 'in' the tolerance zone
        const double out = 1.0 + 1e-7; // 'out' the tolerance zone

        REQUIRE(v.is_close(Vec3(in, 1, 1), usr_tol) == true);
        REQUIRE(v.is_close(Vec3(1, in, 1), usr_tol) == true);
        REQUIRE(v.is_close(Vec3(1, 1, in), usr_tol) == true);
        REQUIRE(v.is_close(Vec3(in, in, in), usr_tol) == true);

        REQUIRE(v.is_close(Vec3(1 + usr_tol, 1, 1), usr_tol) == true);
        // Leaving the tolerance zone:
        REQUIRE(v.is_close(Vec3(out, 1, 1), usr_tol) == false);
        REQUIRE(v.is_close(Vec3(1, out, 1), usr_tol) == false);
        REQUIRE(v.is_close(Vec3(1, 1, out), usr_tol) == false);
        REQUIRE(v.is_close(Vec3(out, out, out), usr_tol) == false);
    }
}

TEST_CASE("Test math operators", "[math][vec3]") {
    auto v1 = Vec3{1, 2, 3};
    auto v2 = Vec3{4, 5, 6};
    SECTION("Test add operator '+' ") {
        REQUIRE(v1 + v2 == Vec3{5, 7, 9});
        REQUIRE(v1 + Vec3{} == v1);
    }

    SECTION("Test subtract operator '-' ") {
        REQUIRE(v2 - v1 == Vec3{3, 3, 3});
        REQUIRE((v1 - v1) == Vec3{});
        REQUIRE((v1 - v1).is_close_zero());
    }

    SECTION("Test scalar multiply operator '*' ") {
        REQUIRE(v1 * 2 == Vec3{2, 4, 6});
        REQUIRE(2 * v1 == Vec3{2, 4, 6});
        REQUIRE(v1 * 0 == Vec3{});
    }

}

TEST_CASE("Test member functions", "[math][vec3]") {
    SECTION("Test from_radians()") {
        REQUIRE(Vec3::from_radians(0) == Vec3{1, 0, 0});
        REQUIRE(Vec3::from_radians(ezdxf::math::kDeg2Rad * 45.0) ==
                Vec3{0.7071067811865476, 0.7071067811865476, 0.0});
    }

    SECTION("Test from_radians() with scaling") {
        REQUIRE(Vec3::from_radians(0, 5) == Vec3{5, 0, 0});
        REQUIRE(Vec3::from_radians(ezdxf::math::kPi2, 3) == Vec3{0, 3, 0});
    }

    SECTION("Test magnitude2()") {
        REQUIRE(Vec3{2, 0, 0}.magnitude2() == 4.0);
        REQUIRE(Vec3{2, 2, 0}.magnitude2() == 8.0);
        REQUIRE(Vec3{2, 2, 2}.magnitude2() == 12.0);
    }

    SECTION("Test magnitude()") {
        REQUIRE(Vec3{2, 0, 0}.magnitude() == 2.0);
        REQUIRE(Vec3{0, 2, 0}.magnitude() == 2.0);
        REQUIRE(Vec3{0, 0, 2}.magnitude() == 2.0);
        REQUIRE(Vec3{3, 4, 5}.magnitude() == 7.0710678118654755);
        REQUIRE(Vec3{3, 4, 5}.magnitude() == Vec3{-3, -4, -5}.magnitude());
    }

    SECTION("Test normalize()") {
        REQUIRE(Vec3{2, 0, 0}.normalize() == Vec3{1, 0, 0});
        REQUIRE(Vec3{0, 3, 0}.normalize() == Vec3{0, 1, 0});
        REQUIRE(Vec3{0, 0, -5}.normalize() == Vec3{0, 0, -1});
    }

    SECTION("Test normalize() with scaling") {
        REQUIRE(Vec3{2, 0, 0}.normalize(3) == Vec3{3, 0, 0});
        REQUIRE(Vec3{0, 3, 0}.normalize(3) == Vec3{0, 3, 0});
        REQUIRE(Vec3{0, 0, -5}.normalize(3) == Vec3{0, 0, -3});
    }

    SECTION("Test dot() product") {
        auto v1 = Vec3{2, 7, 1};
        auto v2 = Vec3{3, 9, 8};
        REQUIRE(v1.dot(v2) == 77);
    }

    SECTION("Test cross() product") {
        auto v1 = Vec3{2, 7, 9};
        auto v2 = Vec3{3, 9, 1};
        REQUIRE(v1.cross(v2) == Vec3{-74, 25, -3});
    }

    SECTION("Test linear interpolation lerp()") {
        auto v1 = Vec3{1, 1, 1};
        auto v2 = Vec3{3, 3, 3};
        REQUIRE(v1.lerp(v2, 0.5) == Vec3{2, 2, 2});
        REQUIRE(v1.lerp(v2, 0.25) == Vec3{1.5, 1.5, 1.5});
        REQUIRE(v1.lerp(v2, 0) == v1);
        REQUIRE(v1.lerp(v2, 1) == v2);
    }

    SECTION("Test distance()") {
        auto v1 = Vec3{};
        auto v2 = Vec3{3, 4, 5};
        REQUIRE(v1.distance(v2) == 7.0710678118654755);
        REQUIRE(v1.distance(v1) == 0);
        REQUIRE(v2.distance(v2) == 0);
    }

    SECTION("Test stringify str()") {
        REQUIRE(Vec3{}.str() == "Vec3{0, 0, 0}");
        REQUIRE(Vec3{1, 2, 3}.str() == "Vec3{1, 2, 3}");
    }
}
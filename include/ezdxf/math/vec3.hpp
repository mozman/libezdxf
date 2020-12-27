// Copyright (c) 2020 Manfred Moitzi
// License: MIT License
// Fast vector library for usage in C only code - header only!
#ifndef EZDXF_MATH_VEC3_HPP
#define EZDXF_MATH_VEC3_HPP

#include <cmath>
#include <ezdxf/math/base.hpp>

namespace ezdxf::math {

    class Vec3 {
        // Immutable 3D vector.

    private:
        double x_;
        double y_;
        double z_;

    public:
        Vec3() = default;

        Vec3(double x, double y, double z) : x_(x), y_(y), z_(z) {};

        ~Vec3() = default;

        [[nodiscard]] double x() const { return x_; }

        [[nodiscard]] double y() const { return y_; }

        [[nodiscard]] double z() const { return z_; }

        Vec3 operator+(const Vec3 &other) const {
            return Vec3(x_ + other.x_, y_ + other.y_, z_ + other.z_);
        };

        Vec3 operator-(const Vec3 &other) const {
            return Vec3(x_ - other.x_, y_ - other.y_, z_ - other.z_);
        };

        Vec3 operator*(double factor) const {
            return Vec3(x_ * factor, y_ * factor, z_ * factor);
        };

        bool operator==(const Vec3 &other) const {
            return is_close(other);
        };

        bool operator!=(const Vec3 &other) const {
            return !is_close(other);
        };

        [[nodiscard]] double magnitude_sqr() const {
            return x_ * x_ + y_ * y_ + z_ * z_;
        };

        [[nodiscard]] double magnitude() const {
            return sqrt(x_ * x_ + y_ * y_ + z_ * z_);
        };

        [[nodiscard]] Vec3 normalize(double length) const {
            double mag = magnitude();
            if (mag == 0.0) return *this;
            return *this * (length / mag);
        };

        [[nodiscard]] Vec3 cross(const Vec3 &other) const {
            return Vec3(
                    y_ * other.z_ - z_ * other.y_,
                    z_ * other.x_ - x_ * other.z_,
                    x_ * other.y_ - y_ * other.x_
            );
        };

        [[nodiscard]] double dot(const Vec3 &other) const {
            return x_ * other.x_ + y_ * other.y_ + z_ * other.z_;
        };

        [[nodiscard]] double distance(const Vec3 &other) const {
            return (*this - other).magnitude();
        };

        [[nodiscard]] Vec3 lerp(const Vec3 &other, double factor) const {
            return *this + (other - *this) * factor;
        };

        [[nodiscard]] bool
        is_close(const Vec3 &other,
                 const double &abs_tol = kAbsTol) const {
            return ezdxf::math::is_close(x_, other.x_, abs_tol) &&
                   ezdxf::math::is_close(y_, other.y_, abs_tol) &&
                   ezdxf::math::is_close(z_, other.z_, abs_tol);
        };
    };
}

#endif  // EZDXF_MATH_VEC3_HPP
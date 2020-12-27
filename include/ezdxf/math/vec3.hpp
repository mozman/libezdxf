// Copyright (c) 2020 Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_MATH_VEC3_HPP
#define EZDXF_MATH_VEC3_HPP

#include <cmath>
#include <ezdxf/math/base.hpp>

namespace ezdxf::math {

    class Vec3 {
        // Immutable 3D vector or vertex.

    private:
        // Can't use const data members x, y and z, because usage as variable
        // would not be possible, see test_math_vec3.cpp.
        //
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

        [[nodiscard]] auto tuple() const { return std::make_tuple(x_, y_, z_); }

        [[nodiscard]] bool
        is_close(const Vec3 &other,
                 const double &abs_tol = kAbsTol) const {
            // General comparisons method between Vec3 objects.
            return ezdxf::math::is_close(x_, other.x_, abs_tol) &&
                   ezdxf::math::is_close(y_, other.y_, abs_tol) &&
                   ezdxf::math::is_close(z_, other.z_, abs_tol);
        }

        [[nodiscard]] bool
        is_close_zero(const double &abs_tol = kAbsTol) const {
            // Specialized and faster test for near zero objects.
            return (fabs(x_) <= abs_tol &&
                    fabs(y_) <= abs_tol &&
                    fabs(z_) <= abs_tol);
        }

        Vec3 operator+(const Vec3 &other) const {
            return Vec3(x_ + other.x_, y_ + other.y_, z_ + other.z_);
        }

        Vec3 operator-(const Vec3 &other) const {
            return Vec3(x_ - other.x_, y_ - other.y_, z_ - other.z_);
        }

        Vec3 operator*(double factor) const {
            return Vec3(x_ * factor, y_ * factor, z_ * factor);
        }

        bool operator==(const Vec3 &other) const {
            return is_close(other);
        }

        bool operator!=(const Vec3 &other) const {
            return !is_close(other);
        }

        [[nodiscard]] double magnitude2() const {
            return x_ * x_ + y_ * y_ + z_ * z_;
        }

        [[nodiscard]] double magnitude() const {
            return sqrt(magnitude2());
        }

        [[nodiscard]] Vec3 normalize(double length = 1.0) const {
            double mag = magnitude();
            if (fabs(mag) < kAbsTol) return *this;
            double f = length / mag;
            return Vec3(x_ * f, y_ * f, z_ * f);
        }

        [[nodiscard]] Vec3 cross(const Vec3 &other) const {
            return Vec3(
                    y_ * other.z_ - z_ * other.y_,
                    z_ * other.x_ - x_ * other.z_,
                    x_ * other.y_ - y_ * other.x_
            );
        }

        [[nodiscard]] double dot(const Vec3 &other) const {
            return x_ * other.x_ + y_ * other.y_ + z_ * other.z_;
        }

        [[nodiscard]] double distance(const Vec3 &other) const {
            return (*this - other).magnitude();
        }

        [[nodiscard]] Vec3 lerp(const Vec3 &other, double factor = 0.5) const {
            return *this + (other - *this) * factor;
        }

    };

    const Vec3 X_Axis{1, 0, 0}; // NOLINT(cert-err58-cpp)
    const Vec3 Y_Axis{0, 1, 0}; // NOLINT(cert-err58-cpp)
    const Vec3 Z_Axis{0, 0, 1}; // NOLINT(cert-err58-cpp)

}

#endif  // EZDXF_MATH_VEC3_HPP
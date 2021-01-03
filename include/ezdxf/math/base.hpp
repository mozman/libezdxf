// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_MATH_BASE_HPP
#define EZDXF_MATH_BASE_HPP

#include <cmath>

namespace ezdxf::math {
    constexpr double kAbsTol = 1e-12;
    constexpr double kRelTol = 1e-9;
    constexpr double kPi = 3.141592653589793;
    constexpr double kPi2 = kPi / 2.0;
    constexpr double kTau = kPi * 2.0;
    constexpr double kRad2Deg = 180.0 / kPi;
    constexpr double kDeg2Rad = kPi / 180.0;

    inline bool is_close(const double a,
                         const double b,
                         const double abs_tol = kAbsTol) {
        // Matches the Python implementation!
        const double diff = fabs(b - a);
        return diff <= fabs(kRelTol * b) ||
               diff <= fabs(kRelTol * a) ||
               diff <= abs_tol;
    }
}
#endif //EZDXF_BASE_HPP

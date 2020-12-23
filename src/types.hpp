// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_TYPES_HPP
#define EZDXF_TYPES_HPP

#include <string>
#include <tuple>

namespace ezdxf {
    typedef long long Int64;
    typedef unsigned long long UInt64;
    typedef double Decimal;
    typedef std::string String;
    // Place holder for ezdxf::math::Vec3
    typedef std::tuple<double, double, double> Vec3;
}

#endif //EZDXF_TYPES_HPP

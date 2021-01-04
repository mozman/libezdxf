// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_UTILS_HPP
#define EZDXF_UTILS_HPP

#include <ezdxf/type.hpp>
#include <optional>

namespace ezdxf::utils {
    void ltrim(String &s);

    void rtrim(String &s);

    void trim(String &s);

    void rtrim_endl(String &s);

    // Important note:
    // This converters are optimized to load DXF tags and nothing else,
    // they are not meant as general purpose functions.

    std::optional<Real> safe_str_to_real(const String &s);

    std::optional<int64_t> safe_str_to_int64(const String &s);

    int safe_group_code(const String &s);

    String hexlify(const Bytes &data);
    Bytes unhexlify(const String &s);

}
#endif //EZDXF_UTILS_HPP

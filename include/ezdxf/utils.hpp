// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_UTILS_HPP
#define EZDXF_UTILS_HPP

#include <ezdxf/type.hpp>

namespace ezdxf::utils {
    void ltrim(String &);

    void rtrim(String &);

    void trim(String &);

    void rtrim_endl(String &);

    std::pair<bool, Real> safe_str_to_real(const String &);

    std::pair<bool, int64_t> safe_str_to_int64(const String &);

    int16_t safe_group_code(const String &);
}
#endif //EZDXF_UTILS_HPP

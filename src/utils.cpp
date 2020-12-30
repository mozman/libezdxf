// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include <ezdxf/utils.hpp>
#include <ezdxf/tag/tag.hpp>

using namespace ezdxf::tag;

namespace ezdxf::utils {

    // Source: https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
    void ltrim(std::string &s) {
        // trim from start (in place)
        s.erase(s.begin(),
                std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }));
    }

    void rtrim(std::string &s) {
        // trim from end (in place)
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    void trim(std::string &s) {
        // trim from both ends (in place)
        ltrim(s);
        rtrim(s);
    }

    void rtrim_endl(std::string &s) {
        // trim <CR><LF> from end (in place)
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return ch != 10 && ch != 13;
        }).base(), s.end());
    }

    std::pair<bool, Real> safe_str_to_real(const String &s) {
        // pair.first: true if an error occurs
        bool err = false;
        Real value = 0;
        try {
            value = stod(s);
        }
        catch (...) {
            err = true;
        }
        return std::make_pair(err, value);
    }

    std::pair<bool, int64_t> safe_str_to_int64(const String &s) {
        // pair.first: true if an error occurs
        // e.g. ProE stores some integer as floating point values!
        bool err = false;
        int64_t value = 0;
        try {
            value = stoll(s);
        }
        catch (...) {
            err = true;
        }
        return std::make_pair(err, value);
    }

    int16_t safe_group_code(const String &s) {
        // Returns kError for invalid group codes.
        auto[err, code] = safe_str_to_int64(s);
        // valid group codes are in the range [0 .. 1071]
        return !err && is_valid_group_code(code) ? code : kError;
    }

}

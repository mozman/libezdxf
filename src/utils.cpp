// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include "ezdxf/utils.hpp"
#include "ezdxf/tag/tag.hpp"

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

    std::optional<Real> safe_str_to_real(const String &s) {
        try {
            return stod(s);
        }
        catch (...) {
            return {};
        }
    }

    std::optional<int64_t> safe_str_to_int64(const String &s) {
        try {
            return stoll(s);
        }
        catch (...) {
            return {};
        }
    }

    int safe_group_code(const String &s) {
        // Returns kError for invalid group codes.
        // valid group codes are in the range [0 .. 1071]
        try {
            auto code = stoi(s);
            return is_valid_group_code(code) ? code : GroupCode::kError;
        }
        catch (...) {
            return GroupCode::kError;
        }
    }

    inline unsigned char _nibble_to_char(unsigned char nibble) {
        return nibble + (nibble < 10 ? 0x30 : 0x37); // "0" : "A" - 10
    }

    String hexlify(const Bytes &data) {
        auto buffer = String();
        // Result requires two chars per data byte:
        buffer.reserve(data.size() * 2);

        for (const unsigned char byte: data) {
            buffer.push_back(_nibble_to_char(byte >> 4));
            buffer.push_back(_nibble_to_char(byte & 0x0f));
        }
        return buffer;
    }

    std::optional<Bytes> unhexlify(const String &s) {
        return {};  // todo
    }

}

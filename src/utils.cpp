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

    inline static unsigned char _nibble_to_char(const unsigned char nibble) {
        // Convert a nibble (0-15) into a char '0'-'9', 'A'-'F'
        return nibble + (nibble < 10 ? 0x30 : 0x37); // '0' : 'A' - 10
    }

    String hexlify(const Bytes &data) {
        // Convert Binary data into a continuous hex string
        // e.g. {0xfe, 0xfe, ...} to "FEFE..."
        //
        // Returns uppercase hex chars.

        auto buffer = String();
        buffer.reserve(data.size() * 2);

        for (const unsigned char byte: data) {
            buffer.push_back(_nibble_to_char(byte >> 4));
            buffer.push_back(_nibble_to_char(byte & 0x0f));
        }
        return buffer;
    }

    inline static char _char_to_nibble(const char c) {
        // Convert an ascii char into a number e.g. 'A' -> 10.
        // Valid chars '0'-'9', 'A'-'F', 'a'-'f'
        //
        // Returns -1 for invalid chars.

        if (c >= '0' && c <= '9')
            return c - 0x30;
        if (c >= 'A' && c <= 'F')
            return c - 0x37;
        if (c >= 'a' && c <= 'f')
            return c - 0x57;
        return -1;  // error
    }

    std::optional<Bytes> unhexlify(const String &s) {
        // Convert a continuous hex string into binary data
        // e.g. "FEFE..." to {0xfe, 0xfe, ...}.
        //
        // If argument `s` contains an uneven count of chars, the last char is
        // ignored!

        Bytes bytes{};
        bytes.reserve(s.size() >> 1);
        bool high_nibble = true;
        unsigned char byte;
        signed char nibble;

        for (const char c : s) {
            nibble = _char_to_nibble(c);
            if (nibble < 0) return {};  // string contains invalid chars
            if (high_nibble) {
                byte = nibble << 4;
            } else {
                bytes.push_back(byte | nibble);
            }
            high_nibble = !high_nibble;
        }
        return bytes;
    }

}

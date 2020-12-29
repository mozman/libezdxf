// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_UTILS_HPP
#define EZDXF_UTILS_HPP

// Source & thx to:
// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

namespace ezdxf::utils {
// trim from start (in place)
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(),
                std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }));
    }

// trim from end (in place)
    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

// trim from both ends (in place)
    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

// trim <CR><LF> from end (in place)
    static inline void rtrim_endl(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return ch != 10 && ch != 13;
        }).base(), s.end());
    }

}

#endif //EZDXF_UTILS_HPP

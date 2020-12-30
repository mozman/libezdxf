// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_TYPE_HPP
#define EZDXF_TYPE_HPP

#include <string>
#include <tuple>
#include <vector>

namespace ezdxf {
    typedef enum {
        // 001-099 - DXF structural errors
        kGenericError = 1,
        kInvalidGroupCodeTag = 2,
        kInvalidIntegerTag = 3,
        kInvalidRealTag = 4,
    } ErrorCode;
    typedef double Real;
    typedef std::string String;
    typedef std::pair<ErrorCode, String> ErrorMessage;
    typedef std::vector<ErrorMessage> ErrorMessages;

}

#endif //EZDXF_TYPE_HPP

// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_TYPE_HPP
#define EZDXF_TYPE_HPP

#include <string>
#include <tuple>
#include <vector>

namespace ezdxf {
    enum class ErrorCode {
        kGenericError=100,
        kInvalidGroupCodeTag,
        kInvalidIntegerTag,
        kInvalidRealTag,
    };
    typedef double Real;
    typedef std::string String;
    typedef std::pair<ErrorCode, String> ErrorMessage;
    typedef std::vector<ErrorMessage> ErrorMessages;

}

#endif //EZDXF_TYPE_HPP

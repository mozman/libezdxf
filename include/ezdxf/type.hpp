// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_TYPE_HPP
#define EZDXF_TYPE_HPP

#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace ezdxf {
    enum class ErrorCode {
        kGenericError = 100,
        kInvalidGroupCodeTag,
        kInvalidIntegerTag,
        kInvalidRealTag,
        kInvalidBinaryTag,
    };

    enum class ResourceType {
        kLayer,
        kLinetype,
        kTextstyle,
        kDimstyle,
        kBlock,
    };

    struct ErrorMessage {
        ErrorCode code;
        std::string message;
        ErrorMessage(ErrorCode code_, std::string msg) :
            code(code_), message(std::move(msg)) {};
    };
    using Real = double;
    using String = std::string;
    using ErrorMessages = std::vector<ErrorMessage>;
    using Bytes = std::vector<unsigned char>;

}

#endif //EZDXF_TYPE_HPP

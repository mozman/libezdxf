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
        kGenericError = 100,
        kInvalidGroupCodeTag,
        kInvalidIntegerTag,
        kInvalidRealTag,
    };
    using Real = double;
    using String = std::string;
    using ErrorMessage = std::pair<ErrorCode, String>;
    using ErrorMessages = std::vector<ErrorMessage>;

}

#endif //EZDXF_TYPE_HPP

// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_TYPE_HPP
#define EZDXF_TYPE_HPP

#include <cassert>
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

    class ACI {
    public:
        [[nodiscard]] int get() const { return aci_; }

        void set(int aci) {
            assert(is_valid_aci(aci));
            aci_ = static_cast<short>(aci);
        };

    private:
        static bool is_valid_aci(const int aci) {
            return aci >= 0 && aci < 258;
        };
        short aci_ = 0;
    };

    enum class Version {
        R9,
        R10,
        R12,
        R13,
        R14,
        R2000,
        R2004,
        R2007,
        R2010,
        R2013,
        R2018,
    };

}

#endif //EZDXF_TYPE_HPP

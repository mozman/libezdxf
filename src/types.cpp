// Copyright (c) 2021, Manfred Moitzi
// License: MIT License
//
#include "ezdxf/type.hpp"
#include "ezdxf/utils.hpp"

namespace ezdxf {
    BinaryData::BinaryData(const String &s) :
            data_(ezdxf::utils::unhexlify(s)) {
    }
}

// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef LIBEZDXF_EZDXF_HPP
#define LIBEZDXF_EZDXF_HPP

#include <string>
#include <utility>
#include "tag_loader.hpp"

namespace ezdxf {
    class Document {
        // Main DXF document
    public:  // functions
        Document() = default;
        bool load(const TagCompiler&);

    public:  // attributes
        std::string filename;
    };

    Document readfile(const std::string&);
}

#endif //LIBEZDXF_EZDXF_HPP

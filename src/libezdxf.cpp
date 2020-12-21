// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//

#include "libezdxf.hpp"


ezdxf::Document readfile(const std::string &filename) {
    auto doc = ezdxf::Document();
    auto string_tags = ezdxf::TagLoader(filename);
    auto tags = ezdxf::TagCompiler(string_tags);
    if (doc.load(tags)) {
        doc.filename = filename;
        return doc;
    } else {
        return ezdxf::Document();
    }
}

bool ezdxf::Document::load(const ezdxf::TagCompiler &tags) {
    return false;
}

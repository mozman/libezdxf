// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//

#include "ezdxf/ezdxf.hpp"


ezdxf::Document readfile(const std::string &filename) {
    auto doc = ezdxf::Document();
    auto string_tags = ezdxf::tag::BasicLoader(filename);
    auto tags = ezdxf::tag::AscLoader(string_tags);
    if (doc.load(tags)) {
        doc.filename = filename;
        return doc;
    } else {
        return ezdxf::Document();
    }
}

bool ezdxf::Document::load(const ezdxf::tag::Loader &tags) {
    return false;
}

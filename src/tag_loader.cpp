// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include <string>
#include "tag_types.hpp"
#include "tag_loader.hpp"

ezdxf::TagLoader::TagLoader(const std::string &filename) {
    // How to open a text stream?
    current = load_next();
}

ezdxf::StringTag ezdxf::TagLoader::take() {
    // Returns the current tag and loads the next tag from stream.
    auto value = current;
    current = load_next();
    return value;
}

ezdxf::StringTag ezdxf::TagLoader::load_next() {
    // How to read int and string from a stream?

    int code = ezdxf::GroupCode::COMMENT;
    std::string value = "Content";

    while (code == ezdxf::GroupCode::COMMENT) {
        code = 0;  // stream.readline();
        // How to check if the end of the stream is reached?
        // if (end of stream) { code = -1; value = "EOF"; }
        value = "Content";  // stream.readline();
    }
    return ezdxf::StringTag(code, value);
    // if stream is empty return StringTag(-1, "")
}

ezdxf::TagType ezdxf::TagCompiler::current_type() const {
    return group_code_type(current.code);
}

ezdxf::StringTag ezdxf::TagCompiler::expect_string() {
    // Returns next tag as StringTag.
    StringTag tag = current;
    load_next_tag();
    return tag;
}

ezdxf::IntegerTag ezdxf::TagCompiler::expect_integer() {
    // Returns next tag as IntegerTag or a tag with group code < 0
    // in case of an error.
    if (current_type() == ezdxf::TagType::INTEGER) {
        // Error handling: ProE stores integers as doubles!
        ezdxf::IntegerTag ret{current.code, std::stol(current.s)};
        load_next_tag();
        return ret;
    }
    return ezdxf::IntegerTag(-1, 0);
}

ezdxf::DoubleTag ezdxf::TagCompiler::expect_double() {
    // Returns next tag as DoubleTag or a tag with group code < 0
    // in case of an error.
    if (current_type() == ezdxf::TagType::DOUBLE) {
        ezdxf::DoubleTag ret{current.code, stod(current.s)};
        load_next_tag();
        return ret;
    }
    return ezdxf::DoubleTag(-1, 0.0);
}

ezdxf::VertexTag ezdxf::TagCompiler::expect_vertex() {
    // Returns next tag as VertexTag or a tag with group code < 0
    // in case of an error.
    double x = 0.0, y = 0.0, z = 0.0;
    if (current_type() == ezdxf::TagType::VERTEX) {
        int code = current.code;
        x = std::stod(current.s);
        load_next_tag();
        if (current.code == code + 10) {
            y = std::stod(current.s);
            load_next_tag();
            if (current.code == code + 20) {
                z = std::stod(current.s);
                load_next_tag();
            }
            return ezdxf::VertexTag(code, x, y, z);
        }
    }
    return ezdxf::VertexTag(-1, x, y, z);
}

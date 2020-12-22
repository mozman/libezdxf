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
    return group_code_type(current.group_code());
}

ezdxf::StringTag ezdxf::TagCompiler::expect_string() {
    // Returns next tag as StringTag.
    StringTag tag = current;
    load_next_tag();
    return tag;
}

ezdxf::IntegerTag ezdxf::TagCompiler::expect_integer() {
    // Returns next tag as IntegerTag or aa error tag with group code < 0.
    if (current_type() == ezdxf::TagType::INTEGER) {
        // Error handling: ProE stores integers as doubles!
        ezdxf::IntegerTag ret{current.group_code(), std::stol(current.str())};
        load_next_tag();
        return ret;
    }
    return ezdxf::IntegerTag(-1, 0);  // error tag
}

ezdxf::DecimalTag ezdxf::TagCompiler::expect_double() {
    // Returns next tag as DecimalTag or an error tag with group code < 0.
    if (current_type() == ezdxf::TagType::DECIMAL) {
        ezdxf::DecimalTag ret{current.group_code(), stod(current.str())};
        load_next_tag();
        return ret;
    }
    return ezdxf::DecimalTag(-1, 0.0);  // error tag
}

ezdxf::VertexTag ezdxf::TagCompiler::expect_vertex() {
    // Returns next tag as VertexTag or an error tag with group code < 0.
    double x = 0.0, y = 0.0, z = 0.0;
    if (current_type() == ezdxf::TagType::VERTEX) {
        int code = current.group_code();
        x = std::stod(current.str());
        load_next_tag();
        if (current.group_code() == code + 10) {
            y = std::stod(current.str());
            load_next_tag();
            if (current.group_code() == code + 20) {
                z = std::stod(current.str());
                load_next_tag();
                return ezdxf::VertexTag(code, x, y, z);
            } else {
                // Does this return a Vertex2Tag or is there an implicit cast
                // to VertexTag?
                return ezdxf::Vertex2Tag(code, x, y);
            }

        }
    }
    return ezdxf::VertexTag(-1, x, y, z);  // error tag
}

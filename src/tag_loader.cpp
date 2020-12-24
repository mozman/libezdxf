// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include "tag_types.hpp"
#include "tag_loader.hpp"

namespace ezdxf {
    Decimal safe_str_to_decimal(const String &s) {
        // TODO: Should return a double for every possible input string!
        return stod(s);
    }

    int64_t safe_str_to_int64(const String &s) {
        // TODO: Should return an int64 for every possible input string!
        // e.g. ProE stores some int64 as floating point values!
        return stoll(s);
    }

    TagLoader::TagLoader(const String &filename) {
        // How to open a text_tag stream?
        current = load_next();
    }

    TextTag TagLoader::take() {
        // Returns the current tag and loads the next tag from stream.
        auto value = current;
        current = load_next();
        return value;
    }

    TextTag TagLoader::load_next() {
        // How to read int and string from a stream?

        short code = GroupCode::COMMENT;
        String value = "Content";

        while (code == GroupCode::COMMENT) {
            code = 0;  // stream.readline();
            // How to check if the end of the stream is reached?
            // if (end of stream) { code = -1; value = "EOF"; }
            value = "Content";  // stream.readline();
        }
        return TextTag(code, value);
        // if stream is empty return TextTag(-1, "")
    }

    TagType TagCompiler::current_type() const {
        return group_code_type(current.group_code());
    }

    TextTag TagCompiler::text_tag() {
        // Returns next tag as TextTag.
        TextTag tag = current;
        load_next_tag();
        return tag;
    }

    IntegerTag TagCompiler::integer_tag() {
        // Returns next tag as IntegerTag or an error tag with group code < 0.
        if (current_type() == TagType::INTEGER) {

            IntegerTag ret{current.group_code(),
                           safe_str_to_int64(current.str())};
            load_next_tag();
            return ret;
        }
        return IntegerTag(GroupCode::ERROR, 0);  // error tag
    }

    DecimalTag TagCompiler::decimal_tag() {
        // Returns next tag as DecimalTag or an error tag with group code < 0.
        if (current_type() == TagType::DECIMAL) {
            DecimalTag ret{current.group_code(),
                           safe_str_to_decimal(current.str())};
            load_next_tag();
            return ret;
        }
        return DecimalTag(GroupCode::ERROR, 0.0);  // error tag
    }

    VertexTag TagCompiler::vertex_tag() {
        // Returns next tag as VertexTag or an error tag with group code < 0.
        double x = 0.0, y = 0.0, z = 0.0;
        if (current_type() == TagType::VERTEX) {
            short code = current.group_code();
            x = safe_str_to_decimal(current.str());
            load_next_tag();
            if (current.group_code() == code + 10) {
                y = safe_str_to_decimal(current.str());
                load_next_tag();
                if (current.group_code() == code + 20) {
                    z = safe_str_to_decimal(current.str());
                    load_next_tag();
                    return VertexTag(code, x, y, z);
                } else {
                    // Does this return a Vertex2Tag or is there an implicit cast
                    // to VertexTag?
                    return Vertex2Tag(code, x, y);
                }

            }
        }
        return VertexTag(GroupCode::ERROR, x, y, z);  // error tag
    }
}
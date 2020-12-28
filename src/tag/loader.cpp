// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include <ezdxf/tag/tag.hpp>
#include <ezdxf/tag/loader.hpp>

namespace ezdxf::tag {
    Real safe_str_to_decimal(const String &s) {
        // TODO: Should return a double for every possible input string!
        return stod(s);
    }

    int64_t safe_str_to_int64(const String &s) {
        // TODO: Should return an integer for every possible input string!
        // e.g. ProE stores some integer as floating point values!
        return stoll(s);
    }

    BasicLoader::BasicLoader(const String &filename) {
        // How to open a text_tag stream?
        current = load_next();
    }

    StringTag BasicLoader::take() {
        // Returns the current tag and loads the next tag from stream.
        auto value = current;
        current = load_next();
        return value;
    }

    StringTag BasicLoader::load_next() {
        // How to read int and string from a stream?

        short code = GroupCode::kComment;
        String value = "Content";

        while (code == GroupCode::kComment) {
            code = 0;  // stream.readline();
            // How to check if the end of the stream is reached?
            // if (end of stream) { code = -1; value = "EOF"; }
            value = "Content";  // stream.readline();
        }
        return StringTag{code, value};
        // if stream is empty return StringTag(-1, "")
    }

    TagType TagLoader::current_type() const {
        return group_code_type(current.group_code());
    }

    StringTag TagLoader::string_tag() {
        // Returns next tag as StringTag.
        StringTag tag = current;
        load_next_tag();
        return tag;
    }

    IntegerTag TagLoader::integer_tag() {
        // Returns next tag as IntegerTag or an error tag with group code < 0.
        if (current_type() == TagType::kInteger) {

            IntegerTag ret{current.group_code(),
                           safe_str_to_int64(current.string())};
            load_next_tag();
            return ret;
        }
        return IntegerTag{GroupCode::kError, 0};  // error tag
    }

    RealTag TagLoader::real_tag() {
        // Returns next tag as RealTag or an error tag with group code < 0.
        if (current_type() == TagType::kReal) {
            RealTag ret{current.group_code(),
                        safe_str_to_decimal(current.string())};
            load_next_tag();
            return ret;
        }
        return RealTag{GroupCode::kError, 0.0};  // error tag
    }

    Vec3Tag TagLoader::vec3_tag() {
        // Returns next tag as Vec3Tag or an error tag with group code < 0.
        double x = 0.0, y = 0.0, z = 0.0;
        if (current_type() == TagType::kVec3) {
            short code = current.group_code();
            x = safe_str_to_decimal(current.string());
            load_next_tag();
            if (current.group_code() == code + 10) {
                y = safe_str_to_decimal(current.string());
                load_next_tag();
                if (current.group_code() == code + 20) {
                    z = safe_str_to_decimal(current.string());
                    load_next_tag();
                    return Vec3Tag{code, x, y, z};
                } else {
                    // Does this return a Vec2Tag or is there an implicit cast
                    // to Vec3Tag?
                    return Vec2Tag{code, x, y};
                }

            }
        }
        return Vec3Tag{GroupCode::kError, x, y, z};  // error tag
    }
}
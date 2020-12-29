// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include <memory>
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

        short code = kComment;
        String value = "Content";

        while (code == kComment) {
            code = 0;  // stream.readline();
            // How to check if the end of the stream is reached?
            // if (end of stream) { code = -1; value = "EOF"; }
            value = "Content";  // stream.readline();
        }
        return StringTag(code, value);
        // if stream is empty return StringTag(-1, "")
    }

    TagType AscLoader::current_type() const {
        return group_code_type(current.group_code());
    }

    pDXFTag AscLoader::string_tag() {
        // Returns next tag as pointer to a StringTag.
        // Returns an error tag if EOF is reached.
        auto ptr = new StringTag(
                current.group_code(),
                current.string()
        );
        load_next_tag();
        return ptr;
    }

    pDXFTag AscLoader::integer_tag() {
        // Returns next tag as pointer to a IntegerTag.
        // Returns an error tag if the next tag is not an IntegerTag or
        // premature EOF is reached.
        // DXF file has to end with a (0, "EOF") StringTag.
        if (current_type() == kInteger) {

            auto ptr = new IntegerTag(
                    current.group_code(),
                    safe_str_to_int64(current.string())
            );
            load_next_tag();
            return ptr;
        }
        return new IntegerTag(kError, 0);  // error tag
    }

    pDXFTag AscLoader::real_tag() {
        // Returns next tag as pointer to a RealTag.
        // Returns an error tag if the next tag is not a RealTag or
        // premature EOF is reached.
        // DXF file has to end with a (0, "EOF") StringTag.
        if (current_type() == kReal) {
            auto ptr = new RealTag(
                    current.group_code(),
                    safe_str_to_decimal(current.string())
            );
            load_next_tag();
            return ptr;
        }
        return new RealTag(kError, 0.0);  // error tag
    }

    pDXFTag AscLoader::vec3_tag() {
        // Returns next tag as pointer to a Vec3Tag/Vec2Tag/RealTag.
        //
        // Returns Vec3Tag: valid 3D vector
        // Returns Vec2Tag: valid 2D vector, z-axis is 0
        // Returns RealTag: Unordered or invalid composed vector, returns first
        // component as RealTag, the following layer may recover unordered
        // vector tags.
        //
        // Returns an error tag if the next tag is not a Vec2Tag/Vec3Tag or
        // premature EOF is reached.
        // DXF file has to end with a (0, "EOF") StringTag.
        //
        Real x = 0.0, y = 0.0, z = 0.0;
        if (current_type() == kVec3) {
            short code = current.group_code();
            x = safe_str_to_decimal(current.string());
            load_next_tag();
            if (current.group_code() == code + 10) {
                y = safe_str_to_decimal(current.string());
                load_next_tag();
                if (current.group_code() == code + 20) {
                    z = safe_str_to_decimal(current.string());
                    load_next_tag();
                    return new Vec3Tag(code, x, y, z);
                } else {
                    // Preserve loading state for 2D only vectors.
                    return new Vec2Tag(code, x, y);
                }
            } else {
                // Unordered or invalid composed DXF vector.
                // Return all vector tags as RealTag: member function code()
                // returns kReal even for group codes which should represent
                // vectors like group code 10.
                return new RealTag(code, x);
            }
        }
        return new Vec3Tag(kError, x, y, z);  // error tag
    }

}
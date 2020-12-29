// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include<sstream>
#include <ezdxf/tag/tag.hpp>
#include <ezdxf/tag/loader.hpp>
#include <ezdxf/utils.hpp>

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

    int16_t safe_group_code(const String &s) {
        auto code = safe_str_to_int64(s);
        // valid group codes are in the range [0 .. 1071]
        return is_valid_group_code(code) ? (int16_t) code : kError;
    }

    BasicLoader::BasicLoader(const String &s) {
        input_stream = new std::basic_istringstream(s);
        if (input_stream) {
            is_stream_owner = true;
            current = load_next();
        }
    }

    BasicLoader::~BasicLoader() {
        if (input_stream && is_stream_owner) {
            delete input_stream;
        }
        input_stream = nullptr;
    }

    StringTag BasicLoader::get() {
        // Returns the current tag and loads the next tag from stream.
        auto value = current;
        current = load_next();
        return value;
    }

    StringTag BasicLoader::load_next() {
        StringTag error{kError}; // EOF marker
        if (!input_stream) {
            return error;
        }
        int16_t code = kComment;
        String value;
        // Skip comment tags with group code 999:
        while (code == kComment) {
            // Read next group code tag or EOF
            input_stream->getline(buffer, kMaxLineBuffer);
            if (input_stream->gcount()) {
                line_number++;
                code = safe_group_code(buffer);
                // Read next value tag or EOF
                input_stream->getline(buffer, kMaxLineBuffer);
                if (input_stream->gcount()) {
                    line_number++;
                    value = String(buffer);
                    if (code == kStructure) {
                        // Remove all whitespace from structure tags:
                        ezdxf::utils::trim(value);
                    } else {
                        // Remove only line endings <CR> and <LF>:
                        ezdxf::utils::rtrim_endl(value);
                    }
                } else {
                    return error;
                }
            } else {
                return error;
            }
        }
        return StringTag(code, value);
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
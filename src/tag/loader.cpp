// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include<sstream>
#include <ezdxf/tag/tag.hpp>
#include <ezdxf/tag/loader.hpp>
#include <ezdxf/utils.hpp>

namespace ezdxf::tag {

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
            if (!input_stream->fail()) {
                line_number++;
                code = utils::safe_group_code(buffer);
                if (code == kError) {
                    log_invalid_group_code();
                }
                // Read next value tag or EOF
                input_stream->getline(buffer, kMaxLineBuffer);
                if (!input_stream->fail()) {
                    line_number++;
                    value = String(buffer);
                    if (code == kStructure) {
                        // Remove all whitespace from structure tags:
                        utils::trim(value);
                    } else {
                        // Remove only line endings <CR> and <LF>:
                        utils::rtrim_endl(value);
                    }
                } else return error;
            } else return error;
        }
        return StringTag(code, value);
    }

    void BasicLoader::log_invalid_group_code() {
        std::ostringstream msg;
        msg << "Invalid group code in line " << get_line_number();
        errors.push_back(ErrorMessage(kInvalidGroupCodeTag, msg.str()));
    }

    void AscLoader::load_next_tag() {
        line_number = loader.get_line_number();
        current = loader.get();
    }

    TagType AscLoader::detect_current_type() const {
        // Detect real tag type defined by the group code for the current tag
        // because current.type() is always kString.
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
        // Returns next tag as pointer to an IntegerTag.
        // Returns an error tag if the next tag is not an IntegerTag or
        // premature EOF is reached.
        // DXF file has to end with a (0, "EOF") StringTag.
        if (detect_current_type() == kInteger) {
            auto[err, value] = utils::safe_str_to_int64(current.string());
            if (!err) {
                auto ptr = new IntegerTag(current.group_code(), value);
                load_next_tag();
                return ptr;
            } else log_invalid_integer_value();
        }
        return new IntegerTag(kError, 0);  // error tag
    }

    pDXFTag AscLoader::real_tag() {
        // Returns next tag as pointer to a RealTag.
        // Returns an error tag if the next tag is not a RealTag or
        // premature EOF is reached.
        // DXF file has to end with a (0, "EOF") StringTag.
        if (detect_current_type() == kReal) {
            auto[err, value] = utils::safe_str_to_real(current.string());
            if (!err) {
                auto ptr = new RealTag(current.group_code(), value);
                load_next_tag();
                return ptr;
            } else log_invalid_real_value();
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

        if (detect_current_type() == kVec3) {
            short code = current.group_code();
            auto[err, value] = utils::safe_str_to_real(current.string());
            if (err) {
                log_invalid_real_value();
                return new Vec3Tag(kError, x, y, z);  // error tag
            } else {
                x = value;
            }
            load_next_tag();
            if (current.group_code() == code + 10) {
                auto[err, value] = utils::safe_str_to_real(current.string());
                if (err) {
                    log_invalid_real_value();
                    return new Vec3Tag(kError, x, y, z);  // error tag
                } else {
                    y = value;
                }
                load_next_tag();
                if (current.group_code() == code + 20) {
                    auto[err, value] = utils::safe_str_to_real(
                            current.string());
                    if (err) {
                        log_invalid_real_value();
                        return new Vec3Tag(kError, x, y, z);  // error tag
                    } else {
                        z = value;
                    }
                    load_next_tag();
                    return new Vec3Tag(code, x, y, z);
                } else {
                    // Preserve loading state for 2D only vectors.
                    return new Vec2Tag(code, x, y);
                }
            } else {
                // Unordered or invalid composed DXF vector.
                // Return all vector tags as RealTag: member function group_code()
                // returns kReal even for group codes which should represent
                // vectors like group code 10.
                return new RealTag(code, x);
            }
        }
        return new Vec3Tag(kError, x, y, z);  // error tag
    }

    void AscLoader::log_invalid_real_value() {
        std::ostringstream msg;
        msg << "Invalid floating point value in line "
            << get_line_number();
        errors.push_back(ErrorMessage(kInvalidRealTag, msg.str()));
    }

    void AscLoader::log_invalid_integer_value() {
        std::ostringstream msg;
        msg << "Invalid integer value in line "
            << get_line_number();
        errors.push_back(ErrorMessage(kInvalidIntegerTag, msg.str()));
    }

}
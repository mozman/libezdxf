// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include<sstream>
#include <ezdxf/tag/tag.hpp>
#include <ezdxf/tag/loader.hpp>
#include <ezdxf/utils.hpp>

namespace ezdxf::tag {
    std::unique_ptr<DXFTag> make_error_tag() {
        // Error tag type is TagType::kUndefined!
        // use member function is_error_tag() to detect error tags.
        return std::make_unique<DXFTag>(GroupCode::kError);
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
        StringTag error{GroupCode::kError}; // EOF marker
        if (!input_stream) {
            return error;
        }
        int code = GroupCode::kComment;
        String value;
        // Skip comment tags with group code 999:
        while (code == GroupCode::kComment) {
            // Read next group code tag or EOF
            input_stream->getline(buffer, kMaxLineBuffer);
            if (input_stream->fail()) {
                return error;
            }
            line_number++;
            code = utils::safe_group_code(buffer);
            if (code == GroupCode::kError) {
                log_invalid_group_code();
                return error;
            }
            // Read next value tag or EOF
            input_stream->getline(buffer, kMaxLineBuffer);
            if (input_stream->fail()) {
                return error;
            }
            line_number++;
            value = String(buffer);
            if (code == GroupCode::kStructure) {
                // Remove all whitespace from structure tags:
                utils::trim(value);
            } else {
                // Remove only line endings <CR> and <LF>:
                utils::rtrim_endl(value);
            }
        }
        return {code, value};
    }

    void BasicLoader::log_invalid_group_code() {
        std::ostringstream msg;
        msg << "Invalid group code in line " << get_line_number();
        errors.emplace_back(ErrorCode::kInvalidGroupCodeTag, msg.str());
    }

    void AscLoader::load_next_tag() {
        line_number = loader.get_line_number();
        current = loader.get();
    }

    TagType AscLoader::detect_current_type() const {
        // Detect real tag type defined by the group code for the current tag
        // because current.type() is always kString.
        // Returns TagType::kUndefined for error tags!
        return group_code_type(current.group_code());
    }

    // Error handling for the following tag loading functions:
    //
    // In most common cases an error tag indicates:
    // DXF structure error or premature end of file and therefore end of
    // processing!
    //
    // A well formed DXF file has to end with a (0, "EOF") StringTag.
    //
    // For recover modes which try to fix errors, the caller has to skip the
    // erroneous current tag by load_next_tag() to continue loading
    // else we are entering an infinite loop!

    std::unique_ptr<DXFTag> AscLoader::string_tag() {
        // Returns next tag as pointer to a StringTag.
        // Returns an error tag if EOF is reached.
        auto ptr = std::make_unique<StringTag>(
                current.group_code(),
                current.string()
        );
        load_next_tag();
        return ptr;
    }

    std::unique_ptr<DXFTag> AscLoader::integer_tag() {
        // Returns next tag as pointer to an IntegerTag.
        // Returns an error tag if the next tag is not an IntegerTag or
        // premature EOF is reached.
        // DXF file has to end with a (0, "EOF") StringTag.
        if (detect_current_type() == TagType::kInteger) {
            auto value = utils::safe_str_to_int64(current.string());
            if (value) {
                auto ptr = std::make_unique<IntegerTag>(
                        current.group_code(), value.value());
                load_next_tag();
                return ptr;
            } else log_invalid_integer_value();
        }
        return make_error_tag();
    }

    std::unique_ptr<DXFTag> AscLoader::real_tag() {
        // Returns next tag as pointer to a RealTag.
        // Returns an error tag if the next tag is not a RealTag or
        // premature EOF is reached.
        // DXF file has to end with a (0, "EOF") StringTag.
        if (detect_current_type() == TagType::kReal) {
            auto value = utils::safe_str_to_real(current.string());
            if (value) {
                auto ptr = std::make_unique<RealTag>(
                        current.group_code(), value.value());
                load_next_tag();
                return ptr;
            } else log_invalid_real_value();
        }
        return make_error_tag();
    }

    std::unique_ptr<DXFTag> AscLoader::vec3_tag() {
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
        if (detect_current_type() != TagType::kVec3) {
            return make_error_tag();
        }
        Real x = 0.0, y = 0.0, z = 0.0;
        int code = current.group_code();
        auto opt_x = utils::safe_str_to_real(current.string());
        if (opt_x) {
            x = opt_x.value();
        } else {
            log_invalid_real_value();
            return make_error_tag();
        }
        if (current.group_code() == code + 10) {
            auto opt_y = utils::safe_str_to_real(current.string());
            if (opt_y) {
                y = opt_y.value();
            } else {
                log_invalid_real_value();
                return make_error_tag();
            }
            load_next_tag();
            if (current.group_code() == code + 20) {
                auto opt_z = utils::safe_str_to_real(current.string());
                if (opt_z) {
                    z = opt_z.value();
                } else {
                    log_invalid_real_value();
                    return make_error_tag();
                }
                load_next_tag();
                return std::make_unique<Vec3Tag>(code, x, y, z);
            } else {
                // Preserve loading state for 2D only vectors.
                return std::make_unique<Vec2Tag>(code, x, y);
            }
        } else {
            // Unordered or invalid composed DXF vector.
            // Return first vector tag as RealTag:
            // The member function group_code() returns kReal, even for
            // group codes which represent vectors like group code 10.
            //
            // Do not log an error here, this should be decided by the
            // caller if this could be fixed or if it's an error.
            return std::make_unique<RealTag>(code, x);
        }
    }

    void AscLoader::log_invalid_real_value() {
        std::ostringstream msg;
        msg << "Invalid floating point value in line "
            << get_line_number();
        errors.emplace_back(ErrorCode::kInvalidRealTag, msg.str());
    }

    void AscLoader::log_invalid_integer_value() {
        std::ostringstream msg;
        msg << "Invalid integer value in line "
            << get_line_number();
        errors.emplace_back(ErrorCode::kInvalidIntegerTag, msg.str());
    }
}
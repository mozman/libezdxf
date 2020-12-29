// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_TAG_TAG_HPP
#define EZDXF_TAG_TAG_HPP

#include <vector>
#include <typeinfo>
#include <ezdxf/type.hpp>
#include <ezdxf/math/vec3.hpp>

using ezdxf::math::Vec3;

namespace ezdxf::tag {
    typedef enum {
        kError = -1,
        kStructure = 0,
        kComment = 999,
    } GroupCode;

    // Type kVec2 is a special type which indicates vectors and vertices
    // which were loaded as 2D points without a z-axis. The tag value is still
    // stored as Vec3 object, it's just meant to preserve the loaded state for
    // rewrite.

    typedef enum {
        kUndefined = 0, kString, kInteger, kReal, kVec3, kVec2
    } TagType;

    class DXFTag {
        // Base tag which provides all possible checks.
        //
        // The basic DXF tag system with the types kString, kInteger and kReal
        // is finally defined. No changes since the first DXF version!
        // The kVec3 and kVec2 types are composed types of 2 or 3 kReal tags.
    private:
        short code = kError;

    public:
        explicit DXFTag(const short code) : code(code) {}

        [[nodiscard]] short group_code() const { return code; }

        [[nodiscard]] virtual TagType type() const {
            return kUndefined;
        }

        [[nodiscard]] inline bool is_error_tag() const {
            // Returns true if tag represents an error tag.
            return code == kError;
        }

        [[nodiscard]] inline bool is_undefined() const {
            // Returns true if tag is undefined.
            return type() == kUndefined;
        }

        [[nodiscard]] inline bool has_string_value() const {
            // Returns true if the tag value type is String.
            return type() == kString;
        }

        [[nodiscard]] inline bool has_real_value() const {
            // Returns true if the tag value type is Real.
            return type() == kReal;
        }

        [[nodiscard]] inline bool has_integer_value() const {
            // Returns true if the tag value type is Integer (int64_t).
            return type() == kInteger;
        }

        [[nodiscard]] inline bool has_vec3_value() const {
            // Returns true if the tag value type is Vec3, which is also
            // true for kVec2!
            return type() == kVec2 || type() == kVec3;
        }

        [[nodiscard]] inline bool export_2d() const {
            // Special mark for vectors loaded without z-axis. The tag value is
            // stored as type Vec3 with z-axis is 0.
            return type() == kVec2;
        }


        // All supported type casts:
        [[nodiscard]] virtual String string() const {
            throw std::bad_cast();
        }

        [[nodiscard]] virtual int64_t integer() const {
            throw std::bad_cast();
        }

        [[nodiscard]] virtual Real real() const {
            throw std::bad_cast();
        }

        [[nodiscard]] virtual Vec3 vec3() const {
            throw std::bad_cast();
        }

        [[nodiscard]] bool is_struct_tag(short code_, const std::string &s) const {
            // Returns true if the stored tag value is a string and matches
            // the given group code and value string.
            //
            // This member function is meant to check for structural tag, which
            // is a common taske for parsing DXF files, without testing the tag
            // type at first.
            return code == code_ &&
                   type() == kString &&
                   s == string();
        }

    };

    class StringTag : public DXFTag {
        // Text is stored as raw data (unencoded cp1252, utf8, ...) String
        // without line endings. White spaces in front and at the end of the
        // string are not striped, because sometimes they are important
        // (e.g. DIMENSION text_tag), except for structure tags with group code 0,
        // for these tags white spaces are obstructive.
    private:
        String s;

    public:
        StringTag(const short code, String value) : DXFTag(code),
                                                    s(std::move(value)) {}

        [[nodiscard]] String string() const override {
            return s;
        }

        [[nodiscard]] TagType type() const override {
            return kString;
        }
    };

    class IntegerTag : public DXFTag {
        // Integer value is stored as signed 64-bit value.
    private:
        int64_t i;

    public:
        IntegerTag(const short code, const int64_t value) : DXFTag(code),
                                                            i(value) {}

        [[nodiscard]] int64_t integer() const override {
            return i;
        }

        [[nodiscard]] TagType type() const override {
            return kInteger;
        }
    };

    class RealTag : public DXFTag {
        // Real value is stored as 64-bit double value.
    private:
        const Real d;

    public:
        RealTag(const short code, const Real value) : DXFTag(code),
                                                      d(value) {};

        [[nodiscard]] Real real() const override {
            return d;
        }

        [[nodiscard]] TagType type() const override {
            return kReal;
        };
    };

    class Vec3Tag : public DXFTag {
    private:
        Vec3 vec3_;

    public:
        Vec3Tag(const short code,
                const Real x,
                const Real y,
                const Real z) :
                DXFTag(code), vec3_{x, y, z} {};

        [[nodiscard]] Vec3 vec3() const override {
            return vec3_;
        }

        [[nodiscard]] TagType type() const override {
            return kVec3;
        };

    };

    // Special class for 2D only vertices is required for a generic DXF tag
    // storage to preserve the vertices as stored in the original DXF document.
    // Some tags have to be written as 2D tags without a z-axis.
    // But otherwise it is completely the same as Vec3Tag.
    class Vec2Tag : public Vec3Tag {
    public:
        Vec2Tag(const short code,
                const Real x,
                const Real y) :
                Vec3Tag(code, x, y, 0.0) {};

        [[nodiscard]] TagType type() const override {
            return kVec2;
        };

    };

    TagType group_code_type(short);

    class Tags {
    private:
        std::vector<DXFTag> tags;
    };
}

#endif //EZDXF_TAG_TAG_HPP

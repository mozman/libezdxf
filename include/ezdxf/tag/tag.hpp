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
        // The abstract base class which is the foundation of the DXF tag type
        // system. The DXFTag type provides already the full functionality, but
        // is itself an undefined (kUndefined) tag and should not be
        // instantiated (no clever tricks uses to prevent this)!
        //
        // The basic DXF tag system with the types kString, kInteger and kReal
        // is finally defined. No changes since the first DXF version!
        // The kVec3 and kVec2 types are composed types of 2 or 3 kReal tags.
        //
        // Every extension of the DXF tag functionality has to be implemented in
        // the DXFTag type, the subclasses only define the tag type by
        // implementing the virtual type() member function and constructor(s)
        // to store the tag value.
        //
    private:
        int16_t code = GroupCode::kError;

    public:
        explicit DXFTag(const int16_t code) : code(code) {}

        [[nodiscard]] int16_t group_code() const { return code; }

        [[nodiscard]] virtual TagType type() const {
            return TagType::kUndefined;
        }

        [[nodiscard]] inline bool is_error_tag() const {
            // Returns true if tag represents an error tag.
            // Every tag type can represent an error tag.
            return code == GroupCode::kError;
        }

        [[nodiscard]] inline bool is_undefined() const {
            // Returns true if tag is undefined.
            return type() == TagType::kUndefined;
        }

        [[nodiscard]] inline bool has_string_value() const {
            // Returns true if the tag value type is String.
            return type() == TagType::kString;
        }

        [[nodiscard]] inline bool has_real_value() const {
            // Returns true if the tag value type is Real.
            return type() == TagType::kReal;
        }

        [[nodiscard]] inline bool has_integer_value() const {
            // Returns true if the tag value type is Integer (int64_t).
            return type() == TagType::kInteger;
        }

        [[nodiscard]] inline bool has_vec3_value() const {
            // Returns true if the tag value type is Vec3, which is also
            // true for kVec2!
            return type() == TagType::kVec2 || type() == TagType::kVec3;
        }

        [[nodiscard]] inline bool export_2d() const {
            // Special flag for vectors loaded without z-axis. The tag value is
            // stored as type Vec3 with z-axis is 0.
            return type() == TagType::kVec2;
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

        [[nodiscard]] bool
        is_string_tag(int16_t code_, const std::string &s) const {
            // Returns true if the stored tag value is a string and matches
            // the given group code and value string.
            //
            // This member function is meant to check for structural tags, which
            // is a common task for parsing DXF files, without testing the tag
            // type at first.
            return code == code_ &&
                   type() == TagType::kString &&
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
        StringTag(const int16_t code, String value) : DXFTag(code),
                                                      s(std::move(value)) {}

        explicit StringTag(const int16_t code) : DXFTag(code), s() {}

        [[nodiscard]] String string() const override {
            return s;
        }

        [[nodiscard]] TagType type() const override {
            return TagType::kString;
        }
    };

    class IntegerTag : public DXFTag {
        // Integer value is stored as signed 64-bit value.
    private:
        int64_t i;

    public:
        IntegerTag(const int16_t code, const int64_t value) : DXFTag(code),
                                                              i(value) {}

        [[nodiscard]] int64_t integer() const override {
            return i;
        }

        [[nodiscard]] TagType type() const override {
            return TagType::kInteger;
        }
    };

    class RealTag : public DXFTag {
        // Real value is stored as 64-bit double value.
    private:
        const Real d;

    public:
        RealTag(const int16_t code, const Real value) : DXFTag(code),
                                                        d(value) {};

        [[nodiscard]] Real real() const override {
            return d;
        }

        [[nodiscard]] TagType type() const override {
            return TagType::kReal;
        };
    };

    class Vec3Tag : public DXFTag {
        // Vectors and vertices are stored as 2- or 3 Real tags of the x-, y-
        // and z-axis.
        // The group codes for the axis tags follow the rule:
        // group code for x = code
        // group code for y = code + 10
        // group code for z = code + 20
    private:
        Vec3 vec3_;

    public:
        Vec3Tag(const int16_t code,
                const Real x,
                const Real y,
                const Real z) :
                DXFTag(code), vec3_{x, y, z} {};

        [[nodiscard]] Vec3 vec3() const override {
            return vec3_;
        }

        [[nodiscard]] TagType type() const override {
            return TagType::kVec3;
        };

    };

    class Vec2Tag : public Vec3Tag {
        // Special class for 2D only vertices is required for a generic DXF tag
        // storage to preserve the vertices as stored in the original DXF
        // document. Some tags have to be written as 2D tags without a z-axis.
        // But otherwise this tag type is fully compatible to the Vec3Tag type.
    public:
        Vec2Tag(const int16_t code,
                const Real x,
                const Real y) :
                Vec3Tag(code, x, y, 0.0) {};

        [[nodiscard]] TagType type() const override {
            return TagType::kVec2;
        };

    };

    TagType group_code_type(int16_t);

    bool is_valid_group_code(int64_t);

    typedef DXFTag *pDXFTag;

    class Tags {
    private:
        std::vector<DXFTag> tags;
    };
}

#endif //EZDXF_TAG_TAG_HPP

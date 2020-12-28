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

    typedef enum {
        kUndefined = 0, kString, kInteger, kReal, kVec3
    } TagType;

    class DXFTag {
    private:
        short code = GroupCode::kError;

    public:
        explicit DXFTag(const short code) : code(code) {}

        [[nodiscard]] short group_code() const { return code; }

        [[nodiscard]] virtual TagType type() const {
            return TagType::kUndefined;
        }

        [[nodiscard]] inline bool is_error_tag() const {
            return code == GroupCode::kError;
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

        [[nodiscard]] bool equals(short code_, const std::string &s) const {
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
        StringTag(const short code, String value) : DXFTag(code),
                                                    s(std::move(value)) {}

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
        IntegerTag(const short code, const int64_t value) : DXFTag(code),
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
        RealTag(const short code, const Real value) : DXFTag(code),
                                                      d(value) {};

        [[nodiscard]] Real real() const override {
            return d;
        }

        [[nodiscard]] TagType type() const override {
            return TagType::kReal;
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
            return TagType::kVec3;
        };

        [[nodiscard]] virtual bool export_z() const { return true; }
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

        [[nodiscard]] bool export_z() const override { return false; }
    };

    TagType group_code_type(short);

    class Tags {
    private:
        std::vector<DXFTag> tags;
    };
}

#endif //EZDXF_TAG_TAG_HPP

// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_TAG_TYPES_HPP
#define EZDXF_TAG_TYPES_HPP

#include <vector>
#include "types.hpp"

namespace ezdxf {
    typedef enum {
        kError = -1,
        kStructure = 0,
        kComment = 999,
    } GroupCode;

    typedef enum {
        kUndefined = 0, kText, kInteger, kDecimal, kVertex
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

    };

    class TextTag : public DXFTag {
        // Text is stored as raw data (unencoded cp1252, utf8, ...) String
        // without line endings. White spaces in front and at the end of the
        // string are not striped, because sometimes they are important
        // (e.g. DIMENSION text_tag), except for structure tags with group code 0,
        // for these tags white spaces are obstructive.
    private:
        String s;

    public:
        TextTag(const short code, String value) : DXFTag(code),
                                                  s(std::move(value)) {}

        [[nodiscard]] String str() const {
            return s;
        }

        [[nodiscard]] TagType type() const override {
            return TagType::kText;
        }
    };

    class IntegerTag : public DXFTag {
        // Integer value is stored as signed 64-bit value.
    private:
        int64_t i;

    public:
        IntegerTag(const short code, const int64_t value) : DXFTag(code),
                                                            i(value) {}

        [[nodiscard]] int64_t int64() const {
            return i;
        }

        [[nodiscard]] TagType type() const override {
            return TagType::kInteger;
        }
    };

    class DecimalTag : public DXFTag {
        // Decimal value is stored as 64-bit double value.
    private:
        const Decimal d;

    public:
        DecimalTag(const short code, const Decimal value) : DXFTag(code),
                                                            d(value) {};

        [[nodiscard]] Decimal decimal() const {
            return d;
        }

        [[nodiscard]] TagType type() const override {
            return TagType::kDecimal;
        };
    };

    class VertexTag : public DXFTag {
        // Vertex axis are stored as 64-bit double values.
    private:
        Decimal x, y, z;

    public:
        VertexTag(const short code,
                  const Decimal x,
                  const Decimal y,
                  const Decimal z) :
                DXFTag(code), x(x), y(y), z(z) {};

        [[nodiscard]] Vec3 vec3() const {
            return Vec3(x, y, z);
        }

        [[nodiscard]] TagType type() const override {
            return TagType::kVertex;
        };

        [[nodiscard]] static bool export_z() { return true; }
    };

    // Special class for 2D only vertices is required for a generic DXF tag
    // storage to preserve the vertices as stored in the original DXF document.
    // Some tags have to be written as 2D tags with out a z-axis.
    // But otherwise it is completely the same as VertexTag.
    class Vertex2Tag : public VertexTag {
    public:
        Vertex2Tag(const short code,
                   const Decimal x,
                   const Decimal y) :
                VertexTag(code, x, y, 0.0) {};

        [[nodiscard]] static bool export_z() { return false; }
    };

    inline bool is_error_tag(const DXFTag &tag) {
        return tag.group_code() < 0;
    }

    TagType group_code_type(short);

    class Tags {
    private:
        std::vector<DXFTag> tags;
    };
}

#endif //EZDXF_TAG_TYPES_HPP

// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef LIBEZDXF_TAG_TYPES_HPP
#define LIBEZDXF_TAG_TYPES_HPP

#include <string>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

namespace ezdxf {
    typedef enum {
        STRUCTURE = 0,
        COMMENT = 999,
    } GroupCode;

    typedef enum {
        STRING, INTEGER, DECIMAL, VERTEX, UNDEFINED
    } TagType;

    // Place holder for ezdxf::math::Vec3
    typedef std::tuple<double, double, double> Vec3;

    class DXFTag {
    private:
        int code = -1;  // error tag

    public:
        explicit DXFTag(const int code) : code(code) {}

        [[nodiscard]] int group_code() const { return code; }

        [[nodiscard]] virtual TagType type() const {
            return TagType::UNDEFINED;
        }

    };

    class StringTag : public DXFTag {
    private:
        std::string s;

    public:
        StringTag(const int code, std::string value) : DXFTag(code),
                                                       s(std::move(value)) {}

        [[nodiscard]] std::string str() const {
            return s;
        }

        [[nodiscard]] TagType type() const override {
            return TagType::STRING;
        }
    };

    class IntegerTag : public DXFTag {
    private:
        long i;
    public:
        IntegerTag(const int code, const long value) : DXFTag(code),
                                                       i(value) {}

        [[nodiscard]] int integer() const {
            return i;
        }

        [[nodiscard]] TagType type() const override {
            return TagType::INTEGER;
        }
    };

    class DecimalTag : public DXFTag {
    private:
        const double d;

    public:
        DecimalTag(const int code, const double value) : DXFTag(code),
                                                         d(value) {};

        [[nodiscard]] double decimal() const {
            return d;
        }

        [[nodiscard]] TagType type() const override {
            return TagType::DECIMAL;
        };
    };

    class VertexTag : public DXFTag {
    private:
        double x, y, z;

    public:
        VertexTag(const int code,
                  const double x,
                  const double y,
                  const double z) :
                DXFTag(code), x(x), y(y), z(z) {};

        [[nodiscard]] Vec3 vec3() const {
            return Vec3(x, y, z);
        }

        [[nodiscard]] TagType type() const override {
            return TagType::VERTEX;
        };

        [[nodiscard]] static bool export_z_axis() { return true; }
    };

    // Special class for 2D only vertices is required for a generic DXF tag
    // storage to preserve the vertices as stored in the original DXF document.
    // Some tags have to be written as 2D tags with out a z-axis.
    // But otherwise it is completely the same as VertexTag.
    class Vertex2Tag : public VertexTag {
    public:
        Vertex2Tag(const int code,
                   const double x,
                   const double y) :
                VertexTag(code, x, y, 0.0) {};

        [[nodiscard]] static bool export_z_axis() { return false; }
    };

    bool is_error_tag(const DXFTag &tag) {
        return tag.group_code() < 0;
    }

    TagType group_code_type(int);

    class Tags {
    private:
        std::vector<DXFTag> tags;
    };
}

#endif //LIBEZDXF_TAG_TYPES_HPP

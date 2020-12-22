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
        STRING, INTEGER, DOUBLE, VERTEX, ERROR
    } TagType;

    class DXFTag {
    public:
        int code;

        explicit DXFTag(const int code) : code(code) {};

        virtual TagType type() { return TagType::ERROR; };

        bool is_error() { return type() == TagType::ERROR; };

    };

    class StringTag : public DXFTag {
    public:
        std::string s;

        StringTag(const int code, std::string value) : DXFTag(code),
                                                       s(std::move(value)) {};

        StringTag(const StringTag &t) : DXFTag(t.code), s(t.s) {};

        TagType type() override {
            return code < 0 ? TagType::ERROR : TagType::STRING;
        };
    };

    class IntegerTag : public DXFTag {
    public:
        long i;

        IntegerTag(const int code, const long value) : DXFTag(code),
                                                       i(value) {};

        TagType type() override {
            return code < 0 ? TagType::ERROR : TagType::INTEGER;
        };
    };

    class DoubleTag : public DXFTag {
    public:
        const double d;

        DoubleTag(const int code, const double value) : DXFTag(code),
                                                        d(value) {};

        TagType type() override {
            return code < 0 ? TagType::ERROR : TagType::DOUBLE;
        };
    };

    class VertexTag : public DXFTag {
    public:
        double x, y, z;

        VertexTag(const int code,
                  const double x,
                  const double y,
                  const double z) :
                DXFTag(code), x(x), y(y), z(z) {};

        TagType type() override {
            return code < 0 ? TagType::ERROR : TagType::VERTEX;
        };
    };

    TagType group_code_type(int code);

    class Tags {
        std::vector<DXFTag> tags;
    };
}

#endif //LIBEZDXF_TAG_TYPES_HPP

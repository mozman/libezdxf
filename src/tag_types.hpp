// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef LIBEZDXF_TAG_TYPES_HPP
#define LIBEZDXF_TAG_TYPES_HPP

#include <string>
#include <tuple>
#include <variant>
#include <vector>

namespace ezdxf {
    typedef enum {
        STRUCTURE = 0,
        COMMENT = 999,
    } GroupCode;

    // enum value is also the position in variant AnyTag
    typedef enum {
        STRING=0, INTEGER=1, DOUBLE=2, VERTEX=3
    } TagType;

    typedef std::tuple<double, double, double> Vertex;

    template <typename T>
    struct DXFTag {
        int code;
        T value;
        DXFTag(int code, T value): code(code), value(value) {};
    };

    typedef DXFTag<std::string> StringTag;
    typedef DXFTag<long> IntegerTag;
    typedef DXFTag<double> DoubleTag;
    typedef DXFTag<Vertex> VertexTag;
    typedef std::variant<StringTag, IntegerTag, DoubleTag, VertexTag> AnyTag;
    TagType group_code_type(int code);

    class Tags {
        std::vector<AnyTag> tags;
    };
}

#endif //LIBEZDXF_TAG_TYPES_HPP

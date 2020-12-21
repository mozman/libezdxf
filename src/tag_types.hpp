// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef LIBEZDXF_TAG_TYPES_HPP
#define LIBEZDXF_TAG_TYPES_HPP

#include <string>
#include <tuple>



namespace ezdxf {
    typedef enum {
        STRUCTURE = 0,
        COMMENT = 999,
    } GroupCode;


    typedef enum {
        STRING, INTEGER, DOUBLE, VERTEX
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

    TagType group_code_type(int code);
}

#endif //LIBEZDXF_TAG_TYPES_HPP

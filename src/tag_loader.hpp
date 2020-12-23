// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_CPP_TAG_LOADER_HPP
#define EZDXF_CPP_TAG_LOADER_HPP

#include "tag_types.hpp"

namespace ezdxf {

    class TagLoader {
    private:
        TextTag current {0, ""};
        TextTag load_next();

    public:
        explicit TagLoader(const String&);
        [[nodiscard]] const TextTag& peek() const {
            return current;
        };
        TextTag take();
        [[nodiscard]] bool is_empty() const {
            return current.group_code() < 0;
        }
    };
    // Usage:
    // while (not loader.is_empty()) {
    //     tag = loader.peek()  // or
    //     tag = loader.next()
    // }

    class TagCompiler {
    private:
        TagLoader& loader;
        TextTag current {0, ""};

        void load_next_tag() {
            current = loader.take();
        };

    public:
        explicit TagCompiler(TagLoader &loader_): loader(loader_) {
            load_next_tag();
        };
        [[nodiscard]] TagType current_type() const;
        [[nodiscard]] bool is_empty() const {
            return ezdxf::is_error_tag(current);
        };
        TextTag text_tag();
        IntegerTag integer_tag();
        DecimalTag decimal_tag();
        VertexTag vertex_tag();
    };
}

#endif //EZDXF_CPP_TAG_LOADER_HPP

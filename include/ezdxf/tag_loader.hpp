// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_CPP_TAG_LOADER_HPP
#define EZDXF_CPP_TAG_LOADER_HPP

#include <ezdxf/tag_types.hpp>

namespace ezdxf {

    class TagLoader {
    private:
        StringTag current {0, ""};
        StringTag load_next();

    public:
        explicit TagLoader(const String&);
        [[nodiscard]] const StringTag& peek() const {
            return current;
        };
        StringTag take();
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
        StringTag current {0, ""};

        void load_next_tag() {
            current = loader.take();
        };

    public:
        explicit TagCompiler(TagLoader &loader_): loader(loader_) {
            load_next_tag();
        };
        [[nodiscard]] TagType current_type() const;
        [[nodiscard]] bool is_empty() const {
            return current.is_error_tag();
        };
        StringTag text_tag();
        IntegerTag integer_tag();
        RealTag decimal_tag();
        Vec3Tag vertex_tag();
    };
}

#endif //EZDXF_CPP_TAG_LOADER_HPP

// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_TAG_LOADER_HPP
#define EZDXF_TAG_LOADER_HPP

#include <ezdxf/tag/tag.hpp>

namespace ezdxf::tag {

    class BasicLoader {
    private:
        StringTag current {0, ""};
        StringTag load_next();

    public:
        explicit BasicLoader(const String&);
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

    class TagLoader {
    private:
        BasicLoader& loader;
        StringTag current {0, ""};

        void load_next_tag() {
            current = loader.take();
        };

    public:
        explicit TagLoader(BasicLoader &loader_): loader(loader_) {
            load_next_tag();
        };
        [[nodiscard]] TagType current_type() const;
        [[nodiscard]] bool is_empty() const {
            return current.is_error_tag();
        };
        StringTag string_tag();
        IntegerTag integer_tag();
        RealTag real_tag();
        Vec3Tag vec3_tag();
    };
}

#endif //EZDXF_TAG_LOADER_HPP

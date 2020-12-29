// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_TAG_LOADER_HPP
#define EZDXF_TAG_LOADER_HPP

#include <ezdxf/tag/tag.hpp>

namespace ezdxf::tag {

    class BasicLoader {
        // Basic string tag loader, returns loaded tags by value!
    private:
        StringTag current{0, ""};

        StringTag load_next();

    public:
        explicit BasicLoader(const String &);

        [[nodiscard]] const StringTag &peek() const {
            return current;
        };

        StringTag take();  // returns loaded string tags by value!

        [[nodiscard]] bool is_empty() const {
            return current.group_code() < 0;
        }
    };
    // Usage:
    // while (not loader.is_empty()) {
    //     tag = loader.peek()  // or
    //     tag = loader.next()
    // }

    class Loader {
        // Abstract base class
    public:
        [[nodiscard]] virtual TagType current_type() const = 0;

        [[nodiscard]] virtual bool eof() const = 0;

        // TODO: return loaded tags as sharded pointers!
        virtual pDXFTag string_tag() = 0;

        virtual pDXFTag integer_tag() = 0;

        virtual pDXFTag real_tag() = 0;

        virtual pDXFTag vec3_tag() = 0;
    };

    class AscLoader : public Loader {
    private:
        BasicLoader &loader;
        StringTag current{0, ""};

        void load_next_tag() {
            current = loader.take();
        };

    public:
        explicit AscLoader(BasicLoader &bl) : loader(bl) {
            load_next_tag();
        };

        [[nodiscard]] TagType current_type() const override;

        [[nodiscard]] bool eof() const override {
            return current.is_error_tag();
        }

        pDXFTag string_tag() override;

        pDXFTag integer_tag() override;

        pDXFTag real_tag() override;

        pDXFTag vec3_tag() override;
    };

}

#endif //EZDXF_TAG_LOADER_HPP

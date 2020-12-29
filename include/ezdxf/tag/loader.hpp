// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_TAG_LOADER_HPP
#define EZDXF_TAG_LOADER_HPP

#include <ezdxf/tag/tag.hpp>
#include <istream>

namespace ezdxf::tag {
    // Quote DXF reference:
    // Group code 0-9: With the introduction of extended symbol names in
    // AutoCAD 2000, the 255-character limit has been increased to 2049
    // single-byte characters without line endings.
    //
    // Every other group code value has a limit of 255 single-byte characters
    // without line endings.
    const size_t kMaxLineBuffer = 2051;  // <CR><LF>

    class BasicLoader {
        // Basic string tag loader, returns loaded tags by value!
    private:
        // Each BasicLoader has its own input buffer:
        // Parallel loading of DXF files should be possible!
        char buffer[kMaxLineBuffer]{};

        // Flag if stream was created by BasicLoader:
        bool is_stream_owner = false;

        // Pointer to a generic char (binary) input stream, should work with all
        // kind of input streams: file, string, ...:
        std::istream *input_stream = nullptr;

        // Current loaded tag -- is an error tag if EOF is reached:
        StringTag current{0};

        StringTag load_next();

    public:
        explicit BasicLoader(const String &);

        ~BasicLoader();

        [[nodiscard]] const StringTag &peek() const {
            return current;
        };

        StringTag get();  // returns loaded string tags by value!

        [[nodiscard]] bool is_empty() const {
            return current.is_error_tag();
        }
    };

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
            current = loader.get();
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

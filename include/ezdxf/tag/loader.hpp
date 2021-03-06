// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_TAG_LOADER_HPP
#define EZDXF_TAG_LOADER_HPP

#include <istream>
#include <vector>
#include "ezdxf/tag/tag.hpp"

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
        StringTag current{GroupCode::kStructure};
        size_t line_number = 0;
        ErrorMessages errors{};

        StringTag load_next();

        void log_invalid_group_code();

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

        [[nodiscard]] size_t get_line_number() const { return line_number; }

        [[nodiscard]] bool has_errors() const { return !errors.empty(); }

        [[nodiscard]] const ErrorMessages &get_errors() const { return errors; }

    };

    class Loader {
        // Abstract base class
    public:
        [[nodiscard]] virtual TagType detect_current_type() const = 0;

        [[nodiscard]] virtual bool eof() const = 0;

        virtual std::unique_ptr<DXFTag> string_tag() = 0;

        virtual std::unique_ptr<DXFTag> binary_tag() = 0;

        virtual std::unique_ptr<DXFTag> integer_tag() = 0;

        virtual std::unique_ptr<DXFTag> real_tag() = 0;

        virtual std::unique_ptr<DXFTag> vec3_tag() = 0;
    };

    class AscLoader : public Loader {
    private:
        BasicLoader &loader;
        StringTag current{GroupCode::kStructure, ""};
        size_t line_number = 0;
        ErrorMessages errors{};

        void load_next_tag();

        [[nodiscard]] size_t get_line_number() const { return line_number; };

        void log_invalid_real_value();

        void log_invalid_integer_value();

        void log_invalid_binary_value();

    public:
        explicit AscLoader(BasicLoader &bl) : loader(bl) {
            load_next_tag();
        };

        [[nodiscard]] TagType detect_current_type() const override;

        [[nodiscard]] bool eof() const override {
            return current.is_error_tag();
        }

        std::unique_ptr<DXFTag> string_tag() override;

        std::unique_ptr<DXFTag> binary_tag() override;

        std::unique_ptr<DXFTag> integer_tag() override;

        std::unique_ptr<DXFTag> real_tag() override;

        std::unique_ptr<DXFTag> vec3_tag() override;

        [[nodiscard]] bool has_errors() const { return !errors.empty(); }

        [[nodiscard]] const ErrorMessages &get_errors() const { return errors; }

    };

}

#endif //EZDXF_TAG_LOADER_HPP

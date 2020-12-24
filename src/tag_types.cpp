// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include "tag_types.hpp"

#define GROUP_CODE_COUNT 1072 // defined by the DXF reference

namespace ezdxf {
    inline bool is_group_code_in_range(const short code) {
        return (code >= 0 && code < GROUP_CODE_COUNT);
    }

    class TagTypeCache {
        TagType cache[GROUP_CODE_COUNT]{};  // init with 0 == TagType::UNDEFINED
    public:
        TagTypeCache() = default;

        TagType get(const short code) {
            return is_group_code_in_range(code) ? cache[code]
                                                : TagType::UNDEFINED;
        }

        void set(const short code, const TagType tag_type) {
            if (is_group_code_in_range(code)) {
                cache[code] = tag_type;
            }
        }
    };

    TagType group_code_type(const short code) {
        static auto cache = TagTypeCache();
        if (!is_group_code_in_range(code)) {
            return TagType::UNDEFINED;
        }
        if (auto tag_type = cache.get(code); tag_type != TagType::UNDEFINED) {
            return tag_type;
        }
        TagType tag_type = TagType::TEXT; // default value
        if ((code >= 10 && code < 19) ||
            (code >= 110 && code < 113) ||
            (code >= 210 && code < 214) ||
            (code >= 1010 && code < 1014)) {
            tag_type = TagType::VERTEX;
        } else if ((code >= 19 && code < 60) ||
                   (code >= 113 && code < 150) ||
                   (code >= 214 && code < 240) ||
                   (code >= 460 && code < 470) ||
                   (code >= 1014 && code < 1060)) {
            tag_type = TagType::DECIMAL;
        } else if ((code >= 60 && code < 80) ||
                   (code >= 90 && code < 100) ||
                   (code >= 160 && code < 180) ||
                   (code >= 270 && code < 290) ||
                   (code >= 370 && code < 390) ||
                   (code >= 400 && code < 410) ||
                   (code >= 420 && code < 430) ||
                   (code >= 440 && code < 460) ||
                   (code >= 1060 && code < 1072)) {
            tag_type = TagType::INTEGER;
        }
        cache.set(code, tag_type);
        return tag_type;
    }
}

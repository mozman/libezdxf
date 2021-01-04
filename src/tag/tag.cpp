// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
#include "ezdxf/tag/tag.hpp"

namespace ezdxf::tag {
    const short kGroupCodeCount = 1072; // defined by the DXF reference

    inline bool is_group_code_in_range(const int code) {
        return (code >= 0 && code < kGroupCodeCount);
    }

    class TypeCache {
        TagType cache[kGroupCodeCount]{}; // init with 0 == TagType::kUndefined
    public:
        TypeCache() = default;

        TagType get(const int code) {
            return is_group_code_in_range(code) ? cache[code]
                                                : TagType::kUndefined;
        }

        void set(const int code, const TagType tag_type) {
            if (is_group_code_in_range(code)) {
                cache[code] = tag_type;
            }
        }
    };

    TagType group_code_type(const int code) {
        static auto cache = TypeCache();
        if (!is_group_code_in_range(code)) {
            return TagType::kUndefined;
        }
        if (auto tag_type = cache.get(code); tag_type != TagType::kUndefined) {
            return tag_type;
        }
        TagType tag_type = TagType::kString; // default value
        if ((code >= 10 && code < 19) ||
            (code >= 110 && code < 113) ||
            (code >= 210 && code < 214) ||
            (code >= 1010 && code < 1014)) {
            tag_type = TagType::kVec3;
        } else if ((code >= 19 && code < 60) ||
                   (code >= 113 && code < 150) ||
                   (code >= 214 && code < 240) ||
                   (code >= 460 && code < 470) ||
                   (code >= 1014 && code < 1060)) {
            tag_type = TagType::kReal;
        } else if ((code >= 60 && code < 80) ||
                   (code >= 90 && code < 100) ||
                   (code >= 160 && code < 180) ||
                   (code >= 270 && code < 290) ||
                   (code >= 370 && code < 390) ||
                   (code >= 400 && code < 410) ||
                   (code >= 420 && code < 430) ||
                   (code >= 440 && code < 460) ||
                   (code >= 1060 && code < 1072)) {
            tag_type = TagType::kInteger;
        } else if ((code >= 310 && code < 320) || code == 1004) {
            tag_type = TagType::kBinaryData;
        }
        cache.set(code, tag_type);
        return tag_type;
    }

    bool is_valid_group_code(int64_t code) {
        return (code >= 0) && (code < kGroupCodeCount);
    }
}

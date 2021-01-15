// Copyright (c) 2021, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_OBJECT_TABLE_HPP
#define EZDXF_OBJECT_TABLE_HPP

#include <stdexcept>
#include <vector>
#include "ezdxf/type.hpp"
#include "ezdxf/acdb/object.hpp"

namespace ezdxf {
    using ezdxf::Handle;
    using ezdxf::acdb::Object;


    template<int N = 12>  // 2^12 = 4096 buckets
    class ObjectTable {
        // Goal: compact and fast enough DXF object lookup by handle.
        // Relationship between Handle and Object is fixed and does not
        // change over the lifetime of a document and DXF objects will also not
        // destroyed, therefore deleting table entries is not required.
        // Table size is fixed and will not grow over runtime.
        using Bucket = std::vector<Object *>;

    private:
        constexpr int count = 1 << N;  // fixed count of buckets as power of 2
        constexpr uint64_t hash_mask = count - 1;
        std::vector<Bucket> buckets{count};

        [[nodiscard]] auto get_bucket(Handle const handle) const {
            return buckets[handle & hash_mask];
        };

    public:
        ObjectTable() = default;

        // "get()" is the most important function here:
        Object *
        get(Handle const handle, Object const *const default_ = nullptr) const {
            // Linear search is fast for small vectors!
            for (auto object_ptr : get_bucket(handle)) {
                if (object_ptr->get_handle() == handle) return object_ptr;
            }
            return default_;
        }

        [[nodiscard]] inline bool has(Handle const handle) const {
            return get(handle) != nullptr;
        }

        void add(Object const *const object) {
            Handle handle = object->get_handle();
            if (!has(handle)) {
                get_bucket(handle).push_pack(object);
            } else throw (std::invalid_argument("handle already exist"));
        }

    }
}
#endif //EZDXF_OBJECT_TABLE_HPP

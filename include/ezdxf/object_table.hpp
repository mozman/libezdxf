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
        // The ObjectTable is the central storage for all DXF objects which
        // have a handle (not DXF Class!). The table store ther objects as
        // unique pointers and therefore owns the objects and destroys all
        // objects at the end of it's lifetime.
        //
        // Goal: A compact and fast enough DXF object lookup by handle.
        // Relationship between handle and object is fixed and does not
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
        ~ObjectTable() {
            for (const auto& bucket: buckets){
                for (auto object_ptr : bucket){
                    // ObjectTable is the owner!
                    delete object_ptr;
                }
            }
        }

        // "get()" is the most important function here:
        Object *
        get(Handle const handle, Object const *const default_ = nullptr) const {
            // Returns a reference to a DXF object.
            // Does not transfer ownership!

            for (auto object_ptr : get_bucket(handle)) {
                // Linear search is fast for small vectors!
                if (object_ptr->get_handle() == handle) return object_ptr;
            }
            return default_;
        }

        [[nodiscard]] inline bool has(Handle const handle) const {
            // Returns true if a DXF object referenced by handle is stored
            // in the object table.

            // The "0" handle is an invalid handle per definition
            return handle != 0 && get(handle) != nullptr;
        }

        [[nodiscard]] inline bool contains(Object const *const object) const {
            // Returns true if the DXF object is stored in the object table.
            return has(object->get_handle());
        }

        void store(Object const *const object) {
            // Transfer ownership of the DXF object to the object table.
            Handle handle = object->get_handle();
            // The "0" handle is an invalid handle per definition
            if (handle == 0)
                throw (std::invalid_argument("object handle 0 is invalid"));
            if (!has(handle)) { // Transfer ownership:
                get_bucket(handle).push_pack(object);
            } else
                throw (std::invalid_argument(
                        "object with same handle already exist"));
        }
    }
}
#endif //EZDXF_OBJECT_TABLE_HPP

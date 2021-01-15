// Copyright (c) 2021, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_OBJECT_HPP
#define EZDXF_OBJECT_HPP

#include <stdexcept>
#include "ezdxf/type.hpp"

namespace ezdxf::acdb {
    using ezdxf::Handle;
    // acdb::Object is the base class for all DXF entities in a DXF Document
    // which have a handle.
    // The handle can only be assigned once!
    class Object {
    private:
        unsigned int status_{0};  // status flags
        Handle handle_{0};  // 0 represents an unassigned handle
        Handle owner_{0}; // 0 represents no owner

    public:
        enum class Status {
            kErased = 1;
        };

        Object() = default;

        virtual ~Object() = default;

        [[nodiscard]] bool is_erased() const {
            return status_ & Status::kErased;
        }

        [[nodiscard]] bool is_alive() const {
            return !is_erased();
        }

        [[nodiscard]] Handle get_handle() const { return handle_; }

        void set_handle(Handle h) {
            if (handle_)  // handle is immutable if != 0
                throw std::invalid_argument("assigned handle is immutable")
            handle_ = h;
        }

        [[nodiscard]] Handle get_owner() const { return owner_; }

        void set_owner(Handle o) { owner_ = o; }

        virtual void erase() {
            // Set object status to erased, DXF objects will not be destroyed at
            // the lifetime of a DXF document!
            status_ |= Status::kErased;
        }
    };

}
#endif //EZDXF_OBJECT_HPP

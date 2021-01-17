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
            kErased = 1
        };

        Object() = default;

        explicit Object(Handle handle, Handle owner = 0) : Object{} {
            set_handle(handle);
            if (owner) set_owner(owner);
        }

        virtual ~Object() = default;

        // Returns the DXF type as specified in the DXF file:
        // Object is not a real DXF object, it is the base class for all
        // DXF entities.
        virtual DXFType dxf_type() { return DXFType::None; }

        // Returns the corresponding ObjectARX® type:
        virtual ARXType arx_type() { return ARXType::AcDbObject; }

        [[nodiscard]] bool is_erased() const {
            return status_ & static_cast<unsigned int>(Status::kErased);
        }

        [[nodiscard]] bool is_alive() const {
            return !is_erased();
        }

        [[nodiscard]] Handle get_handle() const { return handle_; }

        void set_handle(Handle h) {
            if (handle_)  // handle is immutable if != 0
                throw std::invalid_argument("assigned handle is immutable");
            handle_ = h;
        }

        [[nodiscard]] Handle get_owner() const { return owner_; }

        void set_owner(Handle o) { owner_ = o; }

        virtual void erase() {
            // Set object status to erased, DXF objects will not be destroyed at
            // the lifetime of a DXF document!
            status_ |= static_cast<unsigned int>(Status::kErased);
        }
    };

}
#endif //EZDXF_OBJECT_HPP

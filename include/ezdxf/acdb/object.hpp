// Copyright (c) 2021, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_OBJECT_HPP
#define EZDXF_OBJECT_HPP

#include "ezdxf/type.hpp"

namespace ezdxf::acdb {
    using ezdxf::Handle;
    // AcDbObject is the base class for all DXF entities in a DXF Document.

    class Object {
    private:
        unsigned int status_{0};  // status flags
        Handle handle_{0};
        Handle owner_{0};

    public:
        enum class Status {
            kErased = 1;
        };

        Object() = default;
        virtual ~Object() = default;

        [[nodiscard]] bool is_alive() const {
            return !(status_ & Status::kErased);
        }
        [[nodiscard]] Handle get_handle() const { return handle_; }

        void set_handle(Handle h) { handle_ = h; }

        [[nodiscard]] Handle get_owner() const { return owner_; }

        void set_owner(Handle o) { owner_ = o; }

        virtual void erase() {
            // Set object status to erased, DXF objects ill not destroyed at
            // the lifetime of aDXF Document!
            status_ |= Status::kErased;
        }
    };

}
#endif //EZDXF_OBJECT_HPP

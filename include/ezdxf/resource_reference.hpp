// Copyright (c) 2021, Manfred Moitzi
// License: MIT License
//
#ifndef EZDXF_RESOURCE_REFERENCE_HPP
#define EZDXF_RESOURCE_REFERENCE_HPP

#include <stdexcept>
#include <utility>
#include <cassert>
#include "ezdxf/type.hpp"

namespace ezdxf {
    struct DXFEntity {
        [[nodiscard]] uint64_t get_handle() const { return 0; }
    };  // just a placeholder

    // Abstract base class for DXF references:
    // Store references as handle or name at the loading stage.
    // Store references as pointer to a DXF entity at runtime (after loading stage)

    class ResourceReference : {

    public:
        [[nodiscard]] virtual bool is_none() const { return false; };

        [[nodiscard]] virtual bool has_ptr() const { return false; };

        [[nodiscard]] virtual bool has_handle() const { return false; };

        [[nodiscard]] virtual bool has_name() const { return false; };

        [[nodiscard]] virtual DXFEntity *
        operator->() const { throw std::badcast; };

        [[nodiscard]] virtual uint64_t
        get_handle() const { throw std::badcast; };

        [[nodiscard]] virtual std::pair<ResourceType, String>
        get_name() const { throw std::badcast; };
    };

    // Reference nothing
    class NoneReference : ResourceReference {
    public:
        NoneReference() = default;
        [[nodiscard]] bool is_none() const override { return true; };
    };

    // ResourceHandle: Store references to other DXF entities as integer
    // handle as stored in the DXF file, this handles should be resolved into
    // ReferencePtr objects.

    class ResourceHandle : public ResourceReference {
    public:
        ResourceHandle() = delete;

        explicit ResourceHandle(const uint64_t handle) : handle_(handle) {};

        [[nodiscard]] bool has_handle() const override { return true; };

        [[nodiscard]] uint64_t get_handle() const override { return handle_; }

    private:
        uint64_t handle_{0};
    };

    // ResourceName: Store references to other DXF entities by name as string.
    // This resource names should be resolved into ReferencePtr objects.

    class ResourceName : public ResourceReference {
    public:
        ResourceName() = delete;

        explicit ResourceName(const ResourceType t, String n) :
                type_(t), name_(std::move(n)) {};

        [[nodiscard]] bool has_name() const override { return true; };

        [[nodiscard]] std::pair<ResourceType, String>
        get_name() const override { return {type_, name_}};

    private:
        ResourceType type_{};
        String name_{};
    };

    // ReferencePtr: Store references to other DXF entities as reference pointer.
    // Guarantees the referenced entity exist - maybe in deleted state.

    class ReferencePtr : public ResourceReference {
    public:
        ReferencePtr() = delete;

        explicit ReferencePtr(const DXFEntity *entity_ptr) :
                entity_ptr_(entity_ptr) {
            assert((entity_ptr_ != nullptr));  // null pointers not allowed
        };

        [[nodiscard]] bool has_ptr() const override { return true; }

        [[nodiscard]] DXFEntity *
        operator->() const override { return entity_ptr_; }

        [[nodiscard]] bool has_handle() const override { return true; }

        [[nodiscard]] uint64_t
        get_handle() const override { return entity_ptr_->get_handel(); }

    private:
        DXFEntity *entity_ptr_{nullptr};
    };

}

#endif //EZDXF_RESOURCE_REFERENCE_HPP

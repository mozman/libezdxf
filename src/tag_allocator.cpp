// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//

#include "tag_allocator.hpp"

namespace ezdxf {
    DXFTagAllocator dxf_tag_allocator() {
        // Returns the global DXF tag allocator.
        static DXFTagAllocator allocator = DXFTagAllocator();
        return allocator;
    }
}
// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//

#include <ezdxf/tag/allocator.hpp>

namespace ezdxf::tag {
    DXFTagAllocator allocator() {
        // Returns the global DXF tag allocator.
        static DXFTagAllocator allocator_ = DXFTagAllocator();
        return allocator_;
    }
}
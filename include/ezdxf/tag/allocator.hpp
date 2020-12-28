// Copyright (c) 2020, Manfred Moitzi
// License: MIT License
//
// DXF tags will "float" through the loading process.
// The current implementation uses stack allocated instances, passed by values,
// even C++ compilers are optimized for such a usage, it would be better to
// allocate DXF tags on the heap and pass them as shared_pointers.
//
// The heap allocated tags are immutable and could use public const member
// attributes, which was not possible for the stack allocated objects.
//
// Some tags are used multiple times in the same configuration
// (e.g. structure tags like (0, "LINE"), property tags like (8, layer name)
// and some basic values (10, 0.0), (20, 0.0) ...) and could be reused because
// they are immutable. This is only for a selected count of group codes and
// each group code could get it's own caching hash map.
//
// After loading the DXF file some tags has to remain in memory if used in
// storages to preserve DXF content (DXFTagStorage), others could be
// freed -- should be detectable by the reference counter of the
// shared_pointer.
//
// This tag allocator can be a global object, because the repeated tags are
// often the same across DXF files.

#ifndef EZDXF_TAG_ALLOCATOR_HPP
#define EZDXF_TAG_ALLOCATOR_HPP

#include <ezdxf/tag/allocator.hpp>

namespace ezdxf::tag {

    class DXFTagAllocator {
    };

    DXFTagAllocator allocator();

}

#endif //EZDXF_TAG_ALLOCATOR_HPP

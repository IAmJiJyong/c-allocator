#ifndef __ARENA_ALLOCATOR_H__
#define __ARENA_ALLOCATOR_H__

#include "allocator.h"

typedef struct ArenaRegion ArenaRegion;
struct ArenaRegion
{
        void*        buffer;
        size_t       capacity;
        size_t       offset;
        ArenaRegion* next;
};

typedef struct ArenaAllocator
{
        AllocatorInterface* interface;
        void*               child_allocator;
        ArenaRegion*        head;
        size_t              region_size;
} ArenaAllocator;

void
arena_allocator_init(ArenaAllocator* self, void* backing_interface, size_t region_size);

#endif

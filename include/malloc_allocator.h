#ifndef __MALLOC_ALLOCATOR_H__
#define __MALLOC_ALLOCATOR_H__

#include "allocator.h"
typedef struct MallocAllocator
{
        AllocatorInterface* interface;
} MallocAllocator;

void
malloc_allocator_init(MallocAllocator* self);

#endif

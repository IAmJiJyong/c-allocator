#ifndef __POOL_ALLOCATOR_H__
#define __POOL_ALLOCATOR_H__

#include "allocator.h"

typedef struct PoolAllocator
{
        AllocatorInterface* interface;
        void*               free_list;
        void*               chunk;
        size_t              element_size;
        size_t              pool_size;
} PoolAllocator;

void
pool_allocator_init(PoolAllocator* self, size_t element_size, size_t pool_size);
void
pool_allocator_expand(PoolAllocator* self, size_t new_pool_size);

#endif

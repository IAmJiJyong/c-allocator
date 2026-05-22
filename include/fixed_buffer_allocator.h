#ifndef __FIX_BUFFER_ALLOCATOR_H__
#define __FIX_BUFFER_ALLOCATOR_H__

#include "allocator.h"
typedef struct FixedBufferAllocator
{
        AllocatorInterface* interface;
        void*               buffer;
        size_t              capacity;
        size_t              offset;
} FixedBufferAllocator;

void
fixed_buffer_allocator_init(FixedBufferAllocator* self, void* buffer, size_t size);
void
fixed_buffer_allocator_reset(FixedBufferAllocator* self);

#endif

#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include "stddef.h"

typedef void* (*alloc_fn)(void* self, size_t size);
typedef void* (*resize_fn)(void* self, void* ptr, size_t size);
typedef void (*free_fn)(void* self, void* ptr);
typedef void (*destroy_fn)(void* self);

typedef struct AllocatorInterface AllocatorInterface;
struct AllocatorInterface
{
        alloc_fn   alloc;
        resize_fn  resize;
        free_fn    free;
        destroy_fn destroy;
};

static inline void*
allocator_alloc(void* self, size_t size)
{
        if (!self || !size)
                return NULL;
        AllocatorInterface* interface = *(AllocatorInterface**)self;
        return interface->alloc(self, size);
}

static inline void*
allocator_resize(void* self, void* ptr, size_t size)
{
        if (!self || !size || !ptr)
                return NULL;
        AllocatorInterface* interface = *(AllocatorInterface**)self;
        return (interface->resize) ? interface->resize(self, ptr, size) : NULL;
}

static inline void
allocator_free(void* self, void* ptr)
{
        if (!self || !ptr)
                return;
        AllocatorInterface* interface = *(AllocatorInterface**)self;
        if (interface->free)
                interface->free(self, ptr);
}

static inline void
allocator_destroy(void* self)
{
        if (!self)
                return;
        AllocatorInterface* interface = *(AllocatorInterface**)self;
        interface->destroy(self);
}

static inline size_t
aligned_mem(size_t size, size_t align)
{
        return (size + (align - 1)) & ~(align - 1);
}

#endif

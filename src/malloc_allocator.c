#include "malloc_allocator.h"
#include "allocator.h"
#include <stdlib.h>

static void*
malloc_allocator_alloc(MallocAllocator* self, size_t size)
{
        (void)self;
        if (size)
                return malloc(size);
        return NULL;
}

static void*
malloc_allocator_resize(MallocAllocator* self, void* ptr, size_t size)
{
        (void)self;
        return realloc(ptr, size);
}

static void
malloc_allocator_free(MallocAllocator* self, void* ptr)
{
        (void)self;
        free(ptr);
}

static void
malloc_allocator_destory(MallocAllocator* self)
{
        (void)self;
}

static AllocatorInterface malloc_allocator_interface = { .alloc   = (alloc_fn)malloc_allocator_alloc,
                                                         .resize  = (resize_fn)malloc_allocator_resize,
                                                         .free    = (free_fn)malloc_allocator_free,
                                                         .destroy = (destroy_fn)malloc_allocator_destory };

void
malloc_allocator_init(MallocAllocator* self)
{
        self->interface = &malloc_allocator_interface;
}

#include "pool_allocator.h"
#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>

#define CHUNK_AT(base, index, size) ((void*)((char*)(base) + (size) * (index)))
#define LIST_NEXT(ptr)              (*(void**)(ptr))

static void*
pool_allocator_alloc(PoolAllocator* self, size_t size)
{
        (void)size;
        if (!self)
                return NULL;

        void* node = self->free_list;
        if (!node)
                return NULL;

        self->free_list = LIST_NEXT(node);
        return node;
}

static void
pool_allocator_free(PoolAllocator* self, void* ptr)
{
        if (!self || !ptr) {
                return;
        }
        LIST_NEXT(ptr)  = self->free_list;
        self->free_list = ptr;
}

static void
pool_allocator_destroy(PoolAllocator* self)
{
        if (!self) {
                return;
        }
        free(self->chunk);
}

static AllocatorInterface pool_allocator_interface = { .alloc   = (alloc_fn)pool_allocator_alloc,
                                                       .free    = (free_fn)pool_allocator_free,
                                                       .destroy = (destroy_fn)pool_allocator_destroy };

void
pool_allocator_init(PoolAllocator* self, size_t element_size, size_t pool_size)
{
        if (!self || !element_size || !pool_size) {
                return;
        }
        element_size       = aligned_mem(element_size, 16);

        self->element_size = element_size;
        self->pool_size    = pool_size;
        self->chunk        = malloc(element_size * pool_size);
        if (!self->chunk) {
                free(self->chunk);
                fprintf(stderr, "Initialize pool allocator failded.\n");
                return;
        }

        char* base = self->chunk;
        for (size_t i = 0; i < pool_size - 1; i++) {
                char* curr      = CHUNK_AT(base, i, self->element_size);
                char* next      = CHUNK_AT(base, i + 1, self->element_size);
                LIST_NEXT(curr) = next;
        }

        char* last      = base + (self->pool_size - 1) * self->element_size;
        LIST_NEXT(last) = NULL;

        self->free_list = self->chunk;
        self->interface = &pool_allocator_interface;
}

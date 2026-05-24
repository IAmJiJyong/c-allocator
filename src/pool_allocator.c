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

        if (!self->free_list) {
                pool_allocator_expand(self, self->pool_size);
                if (!self->free_list)
                        return NULL;
        }

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

        PoolChunk* curr = self->chunks;
        while (curr) {
                PoolChunk* next = curr->next;
                free(curr->chunk);
                free(curr);
                curr = next;
        }

        self->chunks    = NULL;
        self->free_list = NULL;
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

        element_size       = (sizeof(void*) > element_size) ? sizeof(void*) : element_size;
        element_size       = aligned_mem(element_size, 16);

        self->element_size = element_size;
        self->pool_size    = pool_size;
        self->chunks       = NULL;
        self->free_list    = NULL;
        self->interface    = &pool_allocator_interface;
        pool_allocator_expand(self, pool_size);
}

void
pool_allocator_expand(PoolAllocator* self, size_t expand_size)
{
        if (!self || !expand_size) {
                return;
        }

        size_t chunk_size = self->element_size * expand_size;
        void*  chunk      = malloc(chunk_size);
        if (!chunk) {
                fprintf(stderr, "Pool expand failed.\n");
                return;
        }

        PoolChunk* chunk_node = malloc(sizeof(*chunk_node));
        if (!chunk_node) {
                free(chunk);
                return;
        }

        chunk_node->chunk = chunk;
        chunk_node->next  = self->chunks;
        self->chunks      = chunk_node;

        char* base        = chunk;

        for (size_t i = 0; i < expand_size - 1; i++) {
                void* curr      = CHUNK_AT(base, i, self->element_size);
                void* next      = CHUNK_AT(base, i + 1, self->element_size);
                LIST_NEXT(curr) = next;
        }
        void* last      = CHUNK_AT(base, expand_size - 1, self->element_size);
        LIST_NEXT(last) = self->free_list;

        self->free_list = chunk;
}

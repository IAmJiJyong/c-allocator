#include "arena_allocator.h"
#include "allocator.h"
#include <stdlib.h>

static void*
arena_allocator_alloc(ArenaAllocator* self, size_t size)
{
        if (!self)
                return NULL;

        size = aligned_mem(size, 8);

        if (!self->head || self->head->offset + size > self->head->capacity) {
                size_t alloc_size = (size > self->region_size) ? size : self->region_size;

                void*  new_buf    = allocator_alloc(self->child_allocator, alloc_size);
                if (!new_buf)
                        return NULL;

                ArenaRegion* region = malloc(sizeof(*region));
                if (!region) {
                        allocator_free(self->child_allocator, new_buf);
                        return NULL;
                }

                region->buffer   = new_buf;
                region->capacity = alloc_size;
                region->offset   = 0;
                region->next     = self->head;
                self->head       = region;
        }

        void* ptr = (char*)self->head->buffer + self->head->offset;
        self->head->offset += size;
        return ptr;
}

static void
arena_allocator_free(ArenaAllocator* self, void* ptr)
{
        (void)self;
        (void)ptr;
}

void
arena_allocator_destroy(ArenaAllocator* self)
{
        if (!self)
                return;

        ArenaRegion* curr = self->head;
        while (curr) {
                ArenaRegion* next = curr->next;
                allocator_free(self->child_allocator, curr->buffer);
                free(curr);
                curr = next;
        }
}

static AllocatorInterface arena_allocator_interface = { .alloc   = (alloc_fn)arena_allocator_alloc,
                                                        .free    = (free_fn)arena_allocator_free,
                                                        .destroy = (destroy_fn)arena_allocator_destroy,
                                                        .resize  = NULL };

void
arena_allocator_init(ArenaAllocator* self, void* child, size_t region_size)
{
        self->interface       = &arena_allocator_interface;
        self->child_allocator = child;
        self->head            = NULL;
        self->region_size     = region_size;
}

#include "fixed_buffer_allocator.h"
#include "allocator.h"

static void*
fixed_buffer_allocator_alloc(FixedBufferAllocator* self, size_t size)
{
        if (size == 0)
                return NULL;

        // 為了安全與硬體效能，強制將當前游標向上對齊到 8 位元組
        size_t aligned_offset = aligned_mem(self->offset, 8);

        if (aligned_offset + size > self->capacity) {
                return NULL; // 空間不足，直接回傳 NULL (OutOfMemory)
        }

        void* ptr    = (char*)self->buffer + aligned_offset;

        // 更新游標位置
        self->offset = aligned_offset + size;

        return ptr;
}

static void
fixed_buffer_allocator_free(FixedBufferAllocator* self, void* ptr)
{
        (void)self;
        (void)ptr;
}

static void
fixed_buffer_allocator_destroy(FixedBufferAllocator* self)
{
        self->interface = NULL;
        self->buffer    = NULL;
        self->capacity  = 0;
        self->offset    = 0;
}

static AllocatorInterface fixed_buffer_allocator_interface = { .alloc   = (alloc_fn)fixed_buffer_allocator_alloc,
                                                               .free    = (free_fn)fixed_buffer_allocator_free,
                                                               .destroy = (destroy_fn)fixed_buffer_allocator_destroy };

void
fixed_buffer_allocator_init(FixedBufferAllocator* self, void* buffer, size_t size)
{
        self->interface = &fixed_buffer_allocator_interface;
        self->buffer    = buffer;
        self->capacity  = size;
        self->offset    = 0;
}

void
fixed_buffer_allocator_reset(FixedBufferAllocator* self)
{
        self->offset = 0;
}

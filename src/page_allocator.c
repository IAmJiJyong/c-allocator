#include "page_allocator.h"
#include "allocator.h"
#include "pool_allocator.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#ifndef GOLDEN_RATIO_64
#        define GOLDEN_RATIO_64 0x9e3779b97f4a7c15ULL
#endif

static inline uint32_t
__page_hash(void* ptr, size_t hash_bits)
{
        uintptr_t val = (uintptr_t)ptr;
        val *= GOLDEN_RATIO_64;
        return (uint32_t)(val >> (64 - hash_bits));
}

static inline size_t
page_shift(size_t page_size)
{
        if (page_size == 0 || (page_size & (page_size - 1))) {
                fprintf(stderr, "Page size must be 2^n.");
                return 0;
        }
        return __builtin_ctzll(page_size);
}

static inline size_t
page_size(void)
{
        return (size_t)sysconf(_SC_PAGESIZE);
}

static void*
page_allocator_alloc(PageAllocator* self, size_t size)
{
        size       = aligned_mem(size, self->page_size);

        void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (addr == MAP_FAILED) {
                return NULL;
        }

        Page* page               = allocator_alloc(self->page_pool, size);
        page->addr               = addr;
        page->size               = size;
        page->next               = NULL;

        size_t idx               = __page_hash(addr, PAGE_TABLE_BITS);
        page->next               = self->table.buckets[idx];

        self->table.buckets[idx] = page;

        return addr;
}

static void
page_allocator_free(PageAllocator* self, void* ptr)
{
        size_t idx  = __page_hash(ptr, PAGE_TABLE_BITS);
        Page** curr = &self->table.buckets[idx];

        while (*curr) {
                if ((*curr)->addr == ptr) {
                        Page* page = *curr;
                        *curr      = page->next;
                        munmap(page->addr, page->size);
                        allocator_free(self->page_pool, page);
                        return;
                }
                curr = &(*curr)->next;
        }
}

static void
page_allocator_destroy(PageAllocator* self)
{
        self->page_size  = 0;
        self->page_shift = 0;
        allocator_destroy(self->page_pool);
        free(self->page_pool);
}

static AllocatorInterface page_allocator_interface = { .alloc   = (alloc_fn)page_allocator_alloc,
                                                       .resize  = NULL,
                                                       .free    = (free_fn)page_allocator_free,
                                                       .destroy = (destroy_fn)page_allocator_destroy };

void
page_allocator_init(PageAllocator* self)
{
        if (!self)
                return;
        self->page_size  = page_size();
        self->page_shift = page_shift(self->page_size);
        self->interface  = &page_allocator_interface;
        self->page_pool  = calloc(1, sizeof(PoolAllocator));
        pool_allocator_init(self->page_pool, sizeof(Page), PAGE_POOL_SIZE);
}

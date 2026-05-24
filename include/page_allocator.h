#ifndef __PAGE_ALLOCATOR_H__
#define __PAGE_ALLOCATOR_H__

#include "allocator.h"
#include "pool_allocator.h"
#include <stdint.h>
#include <stdio.h>

#define PAGE_TABLE_BITS    16
#define PAGE_TABLE_BUCKETS (1U << PAGE_TABLE_BITS)
#define PAGE_POOL_SIZE     1024

typedef struct Page Page;
struct Page
{
        void*  addr;
        size_t size;
        Page*  next;
};

typedef struct PageTable
{
        Page* buckets[PAGE_TABLE_BUCKETS];
} PageTable;

typedef struct PageAllocator
{
        AllocatorInterface* interface;
        PageTable           table;
        PoolAllocator*      page_pool;
        size_t              page_size;
        size_t              page_shift;
} PageAllocator;

void
page_allocator_init(PageAllocator* self);

#endif

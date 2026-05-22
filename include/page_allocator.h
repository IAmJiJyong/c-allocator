#ifndef __PAGE_ALLOCATOR_H__
#define __PAGE_ALLOCATOR_H__

#include "allocator.h"

typedef struct
{
        AllocatorInterface* interface;
        size_t              page_size;
} PageAllocator;

typedef struct
{
        size_t size;
} PageHeader;

void
page_allocator_init(PageAllocator* self);

#endif

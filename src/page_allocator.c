#include "page_allocator.h"
#include "allocator.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

static inline size_t
page_size(void)
{
        return (size_t)sysconf(_SC_PAGESIZE);
}

static void*
page_alloc(void* self, size_t size)
{
        PageAllocator* pa = (PageAllocator*)self;

        if (!pa || size == 0)
                return NULL;

        size_t      pages     = (size + pa->page_size - 1) / pa->page_size;
        size_t      total     = pages * pa->page_size;

        size_t      full_size = total + sizeof(PageHeader);

        PageHeader* header    = mmap(NULL, full_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (header == MAP_FAILED)
                return NULL;

        header->size = full_size;

        return (void*)(header + 1);
}

static void
page_free(void* self, void* ptr)
{
}

static void
page_destroy(void* self)
{
}

static AllocatorInterface page_allocator_interface = { .alloc   = (alloc_fn)page_alloc,
                                                       .resize  = NULL,
                                                       .free    = (free_fn)page_free,
                                                       .destroy = (destroy_fn)page_destroy };

void
page_allocator_init(PageAllocator* self)
{
        if (self) {
                self->page_size = page_size();
                self->interface = &page_allocator_interface;
        }
}

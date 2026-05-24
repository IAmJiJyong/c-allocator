// #include "page_allocator.h"
// #include "allocator.h"
// #include <stdint.h>
// #include <stdlib.h>
// #include <sys/mman.h>
// #include <unistd.h>
//
// #ifndef GOLDEN_RATIO_64
// #        define GOLDEN_RATIO_64 0x9e3779b97f4a7c15ULL
// #endif
//
// inline uint32_t
// __page_hash(void* ptr, size_t hash_bits)
// {
//         uintptr_t val = (uintptr_t)ptr;
//         val *= GOLDEN_RATIO_64;
//         return (uint32_t)(val >> (64 - hash_bits));
// }
//
// static inline size_t
// page_shift(size_t page_size)
// {
//         if (page_size == 0 || (page_size & (page_size - 1))) {
//                 fprintf(stderr, "Page size must be 2^n.");
//                 return 0;
//         }
//         return __builtin_ctzll(page_size);
// }
//
// static inline size_t
// page_size(void)
// {
//         return (size_t)sysconf(_SC_PAGESIZE);
// }
//
// static void*
// page_alloc(void* self, size_t size)
// {
//         PageAllocator* pa = (PageAllocator*)self;
//
//         if (!pa || size == 0)
//                 return NULL;
//
//         size_t      pages     = (size + pa->page_size - 1) / pa->page_size;
//         size_t      total     = pages * pa->page_size;
//
//         size_t      full_size = total + sizeof(PageHeader);
//
//         PageHeader* header    = mmap(NULL, full_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
//
//         if (header == MAP_FAILED)
//                 return NULL;
//
//         header->size = full_size;
//
//         return (void*)(header + 1);
// }
//
// static void
// page_free(void* self, void* ptr)
// {
// }
//
// static void
// page_destroy(void* self)
// {
// }
//
// static AllocatorInterface page_allocator_interface = { .alloc   = (alloc_fn)page_alloc,
//                                                        .resize  = NULL,
//                                                        .free    = (free_fn)page_free,
//                                                        .destroy = (destroy_fn)page_destroy };
//
// void
// page_allocator_init(PageAllocator* self)
// {
//         if (self) {
//                 self->page_size = page_size();
//                 self->interface = &page_allocator_interface;
//         }
// }

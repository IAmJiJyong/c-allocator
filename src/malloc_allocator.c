/**
 * @file malloc_allocator.c
 * @brief Implements the MallocAllocator, utilizing standard library malloc/free for memory operations.
 */
#include "malloc_allocator.h"
#include "allocator.h"
#include <stdlib.h>

/**
 * @brief Allocates a block of memory using `malloc`.
 *
 * @param self A pointer to the MallocAllocator instance (unused in this implementation).
 * @param size The size of the memory block to allocate.
 * @return A pointer to the allocated memory block, or NULL if the allocation fails.
 */
static void*
malloc_allocator_alloc(MallocAllocator* self, size_t size)
{
        (void)self; /* Unused parameter to conform to function signature */
        if (size)
                return malloc(size);
        return NULL;
}

/**
 * @brief Resizes a previously allocated memory block using `realloc`.
 *
 * @param self A pointer to the MallocAllocator instance (unused in this implementation).
 * @param ptr A pointer to the memory block to resize.
 * @param size The new size of the memory block.
 * @return A pointer to the reallocated memory block, or NULL if reallocation fails.
 */
static void*
malloc_allocator_resize(MallocAllocator* self, void* ptr, size_t size)
{
        (void)self; /* Unused parameter to conform to function signature */
        return realloc(ptr, size);
}

/**
 * @brief Frees a previously allocated memory block using `free`.
 *
 * @param self A pointer to the MallocAllocator instance (unused in this implementation).
 * @param ptr A pointer to the memory block to free.
 */
static void
malloc_allocator_free(MallocAllocator* self, void* ptr)
{
        (void)self; /* Unused parameter to conform to function signature */
        free(ptr);
}

/**
 * @brief Destroys the MallocAllocator instance (no operation needed for this implementation).
 *
 * @param self A pointer to the MallocAllocator instance (unused in this implementation).
 */
static void
malloc_allocator_destory(MallocAllocator* self)
{
        (void)self; /* Unused parameter to conform to function signature */
}

/**
 * @brief Static instance of AllocatorInterface for MallocAllocator.
 *
 * This interface maps the generic allocator functions to the MallocAllocator's
 * specific implementations that wrap `malloc`, `realloc`, and `free`.
 */
static AllocatorInterface malloc_allocator_interface = { .alloc   = (alloc_fn)malloc_allocator_alloc,
                                                         .resize  = (resize_fn)malloc_allocator_resize,
                                                         .free    = (free_fn)malloc_allocator_free,
                                                         .destroy = (destroy_fn)malloc_allocator_destory };

void
malloc_allocator_init(MallocAllocator* self)
{
        self->interface = &malloc_allocator_interface;
}

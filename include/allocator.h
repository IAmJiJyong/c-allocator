/**
 * @file allocator.h
 * @brief Defines the AllocatorInterface abstraction for memory allocation.
 *
 * This header provides a generic allocator interface that can be implemented
 * by different memory allocation strategies, such as:
 * - malloc allocator (malloc/free)
 * - arena allocator
 *
 * The purpose of this abstraction is to decouple memory management from
 * data structures and algorithms, allowing flexible swapping of allocation
 * strategies without modifying higher-level code.
 *
 * @note All allocator implementations must conform to the AllocatorInterface
 *       function signature requirements.
 *
 * @see AllocatorInterface
 */
#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include "stddef.h"

/**
 * @brief Function pointer for memory allocation.
 *
 * @param self A pointer to the allocator instance.
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or NULL if allocation fails.
 */
typedef void* (*alloc_fn)(void* self, size_t size);

/**
 * @brief Resizes a previously allocated block of memory using the provided allocator interface.
 *
 * This is a convenience function that dispatches the reallocation call to the
 * actual `resize` function pointer within the `AllocatorInterface`.
 *
 * @param self A pointer to the allocator instance.
 * @param ptr A pointer to the memory block to resize.
 * @param size The new size in bytes for the memory block.
 * @return A pointer to the resized memory block, or NULL if resizing fails or inputs are invalid.
 */
typedef void* (*resize_fn)(void* self, void* ptr, size_t size);

/**
 * @brief Function pointer for freeing allocated memory.
 *
 * @param self A pointer to the allocator instance.
 * @param ptr A pointer to the memory block to free.
 */
typedef void (*free_fn)(void* self, void* ptr);

/**
 * @brief Function pointer for destroying the allocator instance.
 *
 * @param self A pointer to the allocator instance to destroy.
 */
typedef void (*destroy_fn)(void* self);

typedef struct AllocatorInterface AllocatorInterface;

/**
 * @brief Defines the interface for a generic memory allocator.
 *
 * This structure provides a set of function pointers that define the core
 * operations of a memory allocator: allocation, reallocation, deallocation,
 * and destruction. This allows for polymorphic use of different allocator
 * implementations.
 */
struct AllocatorInterface
{
        /**
         * @brief Allocate memory block
         */
        alloc_fn   alloc;
        /**
         * @brief Resize memory block
         */
        resize_fn  resize;
        /**
         * @brief Free memory block
         */
        free_fn    free;
        /**
         * @brief Destroy allocator instance
         */
        destroy_fn destroy;
};

/**
 * @brief Allocates memory using the provided allocator interface.
 *
 * This is a convenience function that dispatches the allocation call to the
 * actual `alloc` function pointer within the `AllocatorInterface`.
 *
 * @param self A pointer to the allocator instance (e.g., `(void*)&my_allocator_instance`).
 *             The first member of the actual allocator instance must be a pointer
 *             to its `AllocatorInterface` struct.
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or NULL if allocation fails or inputs are invalid.
 */
static inline void*
allocator_alloc(void* self, size_t size)
{
        if (!self || !size)
                return NULL;
        AllocatorInterface* interface = *(AllocatorInterface**)self;
        return interface->alloc(self, size);
}

/**
 * @brief Resizes a previously allocated block of memory using the provided allocator interface.
 *
 * This is a convenience function that dispatches the reallocation call to the
 * actual `resize` function pointer within the `AllocatorInterface`.
 *
 * @param self A pointer to the allocator instance.
 * @param ptr A pointer to the memory block to resize.
 * @param size The new size in bytes for the memory block.
 * @return A pointer to the resized memory block, or NULL if resizing fails or inputs are invalid.
 */
static inline void*
allocator_resize(void* self, void* ptr, size_t size)
{
        if (!self || !size || !ptr)
                return NULL;
        AllocatorInterface* interface = *(AllocatorInterface**)self;
        return (interface->resize) ? interface->resize(self, ptr, size) : NULL;
}

/**
 * @brief Frees a previously allocated block of memory using the provided allocator interface.
 *
 * This is a convenience function that dispatches the deallocation call to the
 * actual `free` function pointer within the `AllocatorInterface`.
 *
 * @param self A pointer to the allocator instance.
 * @param ptr A pointer to the memory block to free.
 */
static inline void
allocator_free(void* self, void* ptr)
{
        if (!self || !ptr)
                return;
        AllocatorInterface* interface = *(AllocatorInterface**)self;
        if (interface->free)
                interface->free(self, ptr);
}

/**
 * @brief Destroys the allocator instance using the provided allocator interface.
 *
 * This is a convenience function that dispatches the destruction call to the
 * actual `destroy` function pointer within the `AllocatorInterface`.
 *
 * @param self A pointer to the allocator instance to destroy.
 */
static inline void
allocator_destroy(void* self)
{
        if (!self)
                return;
        AllocatorInterface* interface = *(AllocatorInterface**)self;
        interface->destroy(self);
}

/**
 * @brief Calculates the size of memory required to be aligned to a specified boundary.
 *
 * @param size The original size of the memory block.
 * @param align The desired alignment boundary (must be a power of 2).
 * @return The aligned size.
 */
static inline size_t
aligned_mem(size_t size, size_t align)
{
        return (size + (align - 1)) & ~(align - 1);
}

#endif

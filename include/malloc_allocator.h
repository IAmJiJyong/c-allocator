/**
 * @file malloc_allocator.h
 * @brief Defines the MallocAllocator, an allocator that uses standard library malloc/free.
 */

#ifndef __MALLOC_ALLOCATOR_H__
#define __MALLOC_ALLOCATOR_H__

#include "allocator.h"

/**
 * @brief Represents a memory allocator that uses the standard C library's malloc and free functions.
 *
 * This allocator delegates memory management to the system's default `malloc` and `free`
 * implementations, conforming to the AllocatorInterface.
 */
typedef struct MallocAllocator
{
        AllocatorInterface* interface; /**< @brief Pointer to the generic allocator interface. */
} MallocAllocator;

/**
 * @brief Initializes a MallocAllocator instance.
 *
 * This function sets up the MallocAllocator, typically by assigning its
 * interface methods to wrappers around `malloc` and `free`.
 *
 * @param self A pointer to the MallocAllocator instance to be initialized.
 */
void
malloc_allocator_init(MallocAllocator* self);

#endif

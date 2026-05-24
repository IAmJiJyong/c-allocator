# C Memory Allocators Library

A lightweight, polymorphic memory management library implemented in C.

This project provides a unified interface (`AllocatorInterface`) for various memory allocation strategies. It allows developers to swap or compose different allocators—such as Arena, Pool, or Fixed Buffer allocators—depending on specific application needs (e.g., performance, memory constraints, or object lifetime management) without changing high-level logic.

## Key Features

- **Polymorphic Interface**: All allocators implement a common `AllocatorInterface` (`alloc`, `resize`, `free`, `destroy`), enabling generic memory management code.
- **Multiple Allocation Strategies**:
  - **Arena Allocator**: Fast, bulk allocation with single-step deallocation. Ideal for short-lived task-based memory.
  - **Pool Allocator**: O(1) allocation and deallocation for fixed-size objects, minimizing fragmentation.
  - **Fixed Buffer Allocator**: Allocates within a pre-defined buffer (e.g., on the stack or a global array). No dynamic heap usage.
  - **Malloc Allocator**: A thin wrapper around the standard library's `malloc` and `free`.
  - **Page Allocator**: Manages memory in OS-aligned pages.
- **Composition**: Easily use one allocator as a backing store for another (e.g., an Arena backed by a Malloc or Page allocator).
- **Debug-Friendly**: Integrated support for AddressSanitizer and unit testing.

## Project Structure

- `include/`: Header files defining the core interface and specific allocator implementations.
- `src/`: Implementation of the allocation logic.
- `tests/`: Comprehensive unit tests for every allocator.
- `build/`: Output directory for object files, static libraries (`.a`), and shared libraries (`.so`).

## Building and Testing

The project uses `make` for building and testing.

### Build the Library

```bash
# Default build (release mode)
make all

# Debug build (with symbols and AddressSanitizer)
make debug
```

This generates `libmemalloc.a` and `libmemalloc.so` in the `build/` directory.

### Run Tests

```bash
make test
```

## Quick Start

The following example demonstrates how to use the `ArenaAllocator` backed by the standard `MallocAllocator`.

```c
#include "arena_allocator.h"
#include "malloc_allocator.h"
#include <stdio.h>

int main() {
    // 1. Initialize a backing allocator (standard malloc)
    MallocAllocator backing;
    malloc_allocator_init(&backing);

    // 2. Initialize an Arena with a 4KB region size, using the malloc allocator
    ArenaAllocator arena;
    arena_allocator_init(&arena, &backing, 4096);

    // 3. Use the generic interface to allocate memory
    void* p1 = allocator_alloc(&arena, 128);
    void* p2 = allocator_alloc(&arena, 512);

    // Note: With an Arena, you don't need to free individual pointers.
    // Calling destroy on the arena will release all managed memory at once.
    
    allocator_destroy(&arena);
    return 0;
}
```

## License

This project is licensed under the [MIT License](LICENSE).

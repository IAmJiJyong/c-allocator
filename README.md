# Memory Allocators Project

This project provides a collection of custom memory allocators implemented in C. It aims to offer various strategies for memory management, suitable for different use cases and performance requirements.

## Features

- **Arena Allocator**: Efficient allocation and deallocation of memory in a large pre-allocated buffer. Ideal for scenarios where many small objects are allocated and deallocated together.
- **Fixed Buffer Allocator**: Manages memory within a fixed-size buffer, providing fast allocations for objects of a predefined size.
- **Malloc Allocator**: A wrapper around the standard `malloc`/`free` functions, providing a consistent `AllocatorInterface`.
- **Page Allocator**: (Potentially) An allocator that manages memory at the page level. (Further details would require examining `page_allocator.c` and `page_allocator.h`).

The project includes a generic `AllocatorInterface` to allow for polymorphic use of different allocator implementations.

## Building the Project

The project uses a `Makefile` for compilation.

To build the static and shared libraries:

```bash
make all
```

This will generate `libmemalloc.a` (static library) and `libmemalloc.so` (shared library) in the `build/` directory.

## Running Tests

To build and run the test suite:

```bash
make test
```

This will compile the test executables and run them. The tests are designed to verify the correctness and functionality of each allocator implementation.

## Directory Structure

- `include/`: Header files for the allocator interfaces and implementations.
- `src/`: Source code files for the allocator implementations.
- `tests/`: Unit tests for each allocator.
- `build/`: Output directory for compiled objects, libraries, and test executables.
- `docs/`: Doxygen generated documentation (html and latex)

## Usage

To use the allocators in your project, include the relevant header files from the `include/` directory and link against `libmemalloc.a` or `libmemalloc.so`.

Example (assuming you want to use the `malloc_allocator`):

```c
#include "malloc_allocator.h"
#include "allocator.h" // For AllocatorInterface and convenience functions

int main() {
    malloc_allocator_t my_allocator;
    malloc_allocator_init(&my_allocator);

    // Use the generic AllocatorInterface
    void* data = allocator_alloc((void*)&my_allocator, 100);
    // ... use data ...
    allocator_free((void*)&my_allocator, data);

    malloc_allocator_destroy(&my_allocator);
    return 0;
}
```

(Note: The example usage might need adjustment based on the exact API of each allocator, especially regarding initialization and destruction.)

## License

MIT License

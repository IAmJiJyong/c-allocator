# C Memory Allocators Library

A lightweight, polymorphic memory management library implemented in C.

This project provides a unified interface for various memory allocation strategies, allowing for easy swapping and composition of allocators in C applications.

## Key Features

- **Polymorphic Interface (`AllocatorInterface`)**: Decouples memory management strategies from application logic.
- **Support for Multiple Allocators**:
  - `ArenaAllocator`: Efficient bulk allocation and destruction.
  - `FixedBufferAllocator`: Fast allocation within a fixed-size buffer.
  - `MallocAllocator`: Standard heap allocation wrapper.
  - `PageAllocator`: Page-aligned memory management.

## Project Structure

- `include/`: Header files defining the `AllocatorInterface` and specific allocator implementations.
- `src/`: Source code for allocator implementations.
- `tests/`: Unit test suite to verify correctness.
- `build/`: Output directory for compiled libraries and test binaries.

## Building and Testing

The project requires `make` and a C compiler.

### Build Library

```bash
make all
```

Generates `libmemalloc.a` (static) and `libmemalloc.so` (shared) in the `build/` directory.

### Run Tests

```bash
make test
```

Compiles and executes the test suite to verify functionality.

## Usage

The library uses a common interface for all allocators. All allocator structures must start with a pointer to their `AllocatorInterface`.

### Example: Using the Malloc Allocator

```c
#include "malloc_allocator.h"
#include "allocator.h"
#include <stdio.h>

int main() {
    MallocAllocator allocator;
    malloc_allocator_init(&allocator);

    // Allocating memory using the generic interface
    void* ptr = allocator_alloc(&allocator, 128);

    if (!ptr) {
        fprintf(stderr, "allocation failed\n");
        return 1;
    }
    // Use the allocated memory
    //...
    allocator_free(&allocator, ptr);

    allocator_destroy(&allocator);
    return 0;
}
```

## Contributing

Contributions are welcome. Please ensure that all new allocator implementations conform to the `AllocatorInterface` signature.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

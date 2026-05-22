#include "allocator.h"        // 包含統一 Allocator 介面
#include "malloc_allocator.h" // 包含你的 MallocAllocator 實作
#include "unittest.h"         // 你提供的微型測試框架
#include <stdio.h>
#include <string.h>

int
test_basic_alloc_and_free(void)
{
        MallocAllocator allocator = { 0 };
        malloc_allocator_init(&allocator);

        size_t size = 100;
        char*  ptr  = (char*)allocator_alloc(&allocator, size);

        EXPECT(ptr != NULL);

        memset(ptr, 'A', size - 1);
        ptr[size - 1] = '\0';
        EXPECT(ptr[0] == 'A');
        EXPECT(strlen(ptr) == size - 1);

        allocator_free(&allocator, ptr);

        return SUCCESS;
}

int
test_multiple_allocations(void)
{
        MallocAllocator allocator = { 0 };
        malloc_allocator_init(&allocator);

        int* arr1 = (int*)allocator_alloc(&allocator, 50 * sizeof(int));
        int* arr2 = (int*)allocator_alloc(&allocator, 50 * sizeof(int));

        EXPECT(arr1 != NULL);
        EXPECT(arr2 != NULL);

        EXPECT(arr1 != arr2);

        arr1[0] = 123;
        arr2[0] = 456;
        EXPECT(arr1[0] == 123);
        EXPECT(arr2[0] == 456);

        allocator_free(&allocator, arr1);
        allocator_free(&allocator, arr2);

        return SUCCESS;
}

int
test_large_allocation(void)
{
        MallocAllocator allocator = { 0 };
        malloc_allocator_init(&allocator);

        size_t large_size = 2 * 1024 * 1024;
        char*  large_ptr  = (char*)allocator_alloc(&allocator, large_size);

        EXPECT(large_ptr != NULL);

        large_ptr[large_size - 1] = 'Z';
        EXPECT(large_ptr[large_size - 1] == 'Z');

        allocator_free(&allocator, large_ptr);

        return SUCCESS;
}

int
test_zero_alloc(void)
{
        MallocAllocator allocator = { 0 };
        malloc_allocator_init(&allocator);

        void* ptr = allocator_alloc(&allocator, 0);
        EXPECT(ptr == NULL);

        allocator_free(&allocator, NULL);

        return SUCCESS;
}

int
main(void)
{
        printf("=========================================\n");
        printf("        Malloc Allocator Unittests       \n");
        printf("=========================================\n");

        // 呼叫你的框架巨集執行各項測試
        RUN_TEST(test_basic_alloc_and_free);
        RUN_TEST(test_multiple_allocations);
        RUN_TEST(test_large_allocation);
        RUN_TEST(test_zero_alloc);

        printf("=========================================\n");
        return 0;
}

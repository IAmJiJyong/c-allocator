#include "arena_allocator.h"
#include "page_allocator.h"
#include "unittest.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int
test_page_arena_monotonic(void)
{
        PageAllocator  pa    = { 0 };
        ArenaAllocator arena = { 0 };

        page_allocator_init(&pa);
        arena_allocator_init(&arena, &pa, pa.page_size);

        void* a = allocator_alloc(&arena, 64);
        void* b = allocator_alloc(&arena, 128);

        EXPECT(a != NULL);
        EXPECT(b != NULL);
        EXPECT(a != b);

        allocator_destroy(&arena);
        return SUCCESS;
}

int
test_page_arena_alignment(void)
{
        PageAllocator  pa    = { 0 };
        ArenaAllocator arena = { 0 };

        page_allocator_init(&pa);
        arena_allocator_init(&arena, &pa, pa.page_size);

        void* p = allocator_alloc(&arena, 1);

        EXPECT(p != NULL);
        EXPECT(((uintptr_t)p % 16) == 0);

        allocator_destroy(&arena);

        return SUCCESS;
}

int
test_page_arena_growth(void)
{
        PageAllocator  pa    = { 0 };
        ArenaAllocator arena = { 0 };

        page_allocator_init(&pa);
        arena_allocator_init(&arena, &pa, pa.page_size);

        for (int i = 0; i < 100; i++) {
                void* p = allocator_alloc(&arena, 128);
                EXPECT(p != NULL);
        }

        allocator_destroy(&arena);
        return SUCCESS;
}

int
test_stress_alloc_free_cycle(void)
{
        PageAllocator  pa    = { 0 };
        ArenaAllocator arena = { 0 };

        page_allocator_init(&pa);
        arena_allocator_init(&arena, &pa, pa.page_size);

        void* pages[32];

        for (int i = 0; i < 1000; i++) {

                /* allocate all */
                for (int j = 0; j < 32; j++) {
                        pages[j] = allocator_alloc(&arena, sizeof(int));
                        EXPECT(pages[j] != NULL);
                }

                /* free all */
                for (int j = 0; j < 32; j++) {
                        allocator_free(&arena, pages[j]);
                }
        }

        allocator_destroy(&arena);

        return SUCCESS;
}

int
main(void)
{
        printf("=========================================\n");
        printf("      Page + Arena Hybrid Unittests      \n");
        printf("=========================================\n");

        RUN_TEST(test_page_arena_monotonic);
        RUN_TEST(test_page_arena_alignment);
        RUN_TEST(test_page_arena_growth);
        RUN_TEST(test_stress_alloc_free_cycle);

        printf("=========================================\n");
        return 0;
}

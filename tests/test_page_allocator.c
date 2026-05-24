#include "page_allocator.h"
#include "unittest.h"

int
test_page_alloc_init(void)
{
        PageAllocator page;
        page_allocator_init(&page);
        EXPECT(page.page_size == 4096);
        allocator_destroy(&page);

        return SUCCESS;
}

int
test_alloc_unique_pages(void)
{
        PageAllocator page = { 0 };

        page_allocator_init(&page);

        void* pages[8];

        for (int i = 0; i < 8; i++) {
                pages[i] = allocator_alloc(&page, sizeof(int));
                EXPECT(pages[i] != NULL);
        }

        /* all pointers must be unique */
        for (int i = 0; i < 8; i++) {
                for (int j = i + 1; j < 8; j++) {
                        EXPECT(pages[i] != pages[j]);
                }
        }

        allocator_destroy(&page);

        return SUCCESS;
}

int
test_exhaustion(void)
{
        PageAllocator page = { 0 };

        page_allocator_init(&page);

        void* p[4];

        for (int i = 0; i < 4; i++) {
                p[i] = allocator_alloc(&page, sizeof(int));
                EXPECT(p[i] != NULL);
        }

        /* should be exhausted now */
        void* overflow = allocator_alloc(&page, sizeof(int));
        EXPECT(overflow != NULL);

        allocator_destroy(&page);

        return SUCCESS;
}

int
test_stress_alloc_free_cycle(void)
{
        PageAllocator page = { 0 };

        page_allocator_init(&page);

        void* pages[32];

        for (int i = 0; i < 1000; i++) {

                /* allocate all */
                for (int j = 0; j < 32; j++) {
                        pages[j] = allocator_alloc(&page, sizeof(int));
                        EXPECT(pages[j] != NULL);
                }

                /* free all */
                for (int j = 0; j < 32; j++) {
                        allocator_free(&page, pages[j]);
                }
        }

        allocator_destroy(&page);

        return SUCCESS;
}

int
main(void)
{

        printf("=========================================\n");
        printf("         Page Allocator Unittests        \n");
        printf("=========================================\n");

        RUN_TEST(test_page_alloc_init);
        RUN_TEST(test_alloc_unique_pages);
        RUN_TEST(test_exhaustion);
        RUN_TEST(test_stress_alloc_free_cycle);

        printf("=========================================\n");
        return 0;
}

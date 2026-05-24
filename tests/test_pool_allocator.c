#include "allocator.h"
#include "pool_allocator.h"
#include "unittest.h"

typedef struct Node
{
        int x;
} Node;

int
test_pool_init(void)
{
        PoolAllocator pool;
        size_t        count = 8;

        pool_allocator_init(&pool, sizeof(Node), count);

        EXPECT(pool.free_list != NULL);
        void* curr = pool.free_list;

        for (size_t i = 0; i < count - 1; i++) {
                void* next = *(void**)curr;
                EXPECT(next != NULL);
                curr = next;
        }

        allocator_destroy(&pool);
        return SUCCESS;
}

int
test_pool_alloc_all(void)
{
        PoolAllocator pool;
        size_t        count = 4;

        pool_allocator_init(&pool, sizeof(Node), count);

        void* ptrs[4];
        for (int i = 0; i < 4; i++) {
                ptrs[i] = allocator_alloc(&pool, sizeof(Node));
                EXPECT(ptrs[i] != NULL);
        }

        // should be empty now
        void* p = allocator_alloc(&pool, sizeof(Node));
        EXPECT(p == NULL);

        allocator_destroy(&pool);
        return SUCCESS;
}

int
test_pool_free_reuse(void)
{

        PoolAllocator pool;
        size_t        count = 4;

        pool_allocator_init(&pool, sizeof(Node), count);

        void* a = allocator_alloc(&pool, sizeof(Node));
        void* b = allocator_alloc(&pool, sizeof(Node));

        allocator_free(&pool, a);
        void* c = allocator_alloc(&pool, sizeof(Node));

        // LIFO: c should be same as a
        EXPECT(c == a);

        allocator_destroy(&pool);
        return SUCCESS;
}

int
test_pool_memory_write(void)
{
        PoolAllocator pool;
        size_t        count = 4;

        pool_allocator_init(&pool, sizeof(Node), count);

        Node* n1 = allocator_alloc(&pool, sizeof(Node));
        Node* n2 = allocator_alloc(&pool, sizeof(Node));

        n1->x    = 123;
        n2->x    = 456;

        EXPECT(n1->x == 123);
        EXPECT(n2->x == 456);

        allocator_destroy(&pool);
        return SUCCESS;
}

int
test_pool_free(void)
{

        PoolAllocator pool;
        size_t        count = 4;

        pool_allocator_init(&pool, sizeof(Node), count);

        void* a = allocator_alloc(&pool, sizeof(Node));
        void* b = allocator_alloc(&pool, sizeof(Node));

        allocator_free(&pool, a);
        allocator_free(&pool, b);

        void* x = allocator_alloc(&pool, sizeof(Node));
        void* y = allocator_alloc(&pool, sizeof(Node));

        EXPECT(x != NULL);
        EXPECT(y != NULL);

        allocator_destroy(&pool);
        return SUCCESS;
}

int
main(void)
{
        printf("=========================================\n");
        printf("         Pool Allocator Unittests        \n");
        printf("=========================================\n");

        RUN_TEST(test_pool_init);
        RUN_TEST(test_pool_alloc_all);
        RUN_TEST(test_pool_free_reuse);
        RUN_TEST(test_pool_memory_write);
        RUN_TEST(test_pool_free);

        printf("=========================================\n");
        return 0;
}

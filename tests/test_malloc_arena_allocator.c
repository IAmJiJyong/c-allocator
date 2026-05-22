#include "allocator.h"        // 統一 Allocator 介面
#include "arena_allocator.h"  // 你的 ArenaAllocator
#include "malloc_allocator.h" // 你的 MallocAllocator
#include "unittest.h"         // 微型測試框架
#include <stdio.h>
#include <string.h>

// 測試案例 1：Arena 配合 Malloc 進行基本小物件切分
int
test_arena_basic_subdivision(void)
{
        MallocAllocator backing = { 0 };
        malloc_allocator_init(&backing);

        ArenaAllocator arena = { 0 };
        arena_allocator_init(&arena, &backing, 1024);

        EXPECT(&arena != NULL);
        // 3. 密集分配多個小物件
        char* ptr1 = (char*)allocator_alloc(&arena, 200);
        char* ptr2 = (char*)allocator_alloc(&arena, 300);
        char* ptr3 = (char*)allocator_alloc(&arena, 100);

        EXPECT(ptr1 != NULL);
        EXPECT(ptr2 != NULL);
        EXPECT(ptr3 != NULL);

        // 驗證：位址不能重疊，且都在同一個大 Region 內（位址差距應該很近）
        EXPECT(ptr1 != ptr2);
        EXPECT(ptr2 != ptr3);

        // 驗證：可讀寫性
        memset(ptr1, 'A', 200);
        memset(ptr2, 'B', 300);
        EXPECT(ptr1[0] == 'A');
        EXPECT(ptr2[0] == 'B');

        // 4. 單獨對 Arena 呼叫 free 應該是安全空操作
        allocator_free(&arena, ptr1);
        allocator_free(&arena, ptr2);

        // 5. 摧毀 Arena，一鍵還給 Malloc
        allocator_destroy(&arena);

        return SUCCESS;
}

// 測試案例 2：自動向後串接測試（跨 Region 分配）
int
test_arena_cross_region_growth(void)
{
        MallocAllocator backing = { 0 };
        malloc_allocator_init(&backing);

        // 故意給很小的批發價 64 位元組，來強迫觸發 Region 串接
        ArenaAllocator arena = { 0 };
        arena_allocator_init(&arena, &backing, 64);

        // 第一個分配 40 bytes，此時第一個 Region 剩下 24 bytes 剩餘
        char* p1 = (char*)allocator_alloc(&arena, 40);
        EXPECT(p1 != NULL);

        // 第二個要求 50 bytes，第一個 Region 塞不下了！
        // Arena 內部必須自動向 Malloc 批發第二個 64 bytes 的 Region 並串接
        char* p2 = (char*)allocator_alloc(&arena, 50);
        EXPECT(p2 != NULL);

        // 關鍵驗證：即使產生了新的 Region，第一個 Region 的資料 p1 絕對不能壞掉
        p1[0] = 'H';
        p2[0] = 'Y';
        EXPECT(p1[0] == 'H');
        EXPECT(p2[0] == 'Y');

        allocator_destroy(&arena);
        return SUCCESS;
}

// 測試案例 3：巨大分配（大於預設批發尺寸）
int
test_arena_huge_allocation(void)
{
        MallocAllocator backing = { 0 };
        malloc_allocator_init(&backing);

        // 預設批發 1KB
        ArenaAllocator arena = { 0 };
        arena_allocator_init(&arena, &backing, 1024);

        // 突然大開口要 5000 bytes (5KB)
        // Arena 內部必須能動態向底層追加一個 >= 5000 空間的特殊 Region
        char* huge_ptr = (char*)allocator_alloc(&arena, 5000);
        EXPECT(huge_ptr != NULL);

        huge_ptr[4999] = 'X';
        EXPECT(huge_ptr[4999] == 'X');

        allocator_destroy(&arena);
        return SUCCESS;
}

// 測試案例 4：邊界條件（零分配）
int
test_arena_zero_alloc(void)
{
        MallocAllocator backing = { 0 };
        malloc_allocator_init(&backing);

        ArenaAllocator arena = { 0 };
        arena_allocator_init(&arena, &backing, 1024);

        // 仿 Zig 與安全的 Malloc 定位，分配 0 應該穩定拿到 NULL 且不崩潰
        void* ptr = allocator_alloc(&arena, 0);
        EXPECT(ptr == NULL);

        allocator_destroy(&arena);
        return SUCCESS;
}

int
main(void)
{
        printf("=========================================\n");
        printf("        Arena Allocator Unittests        \n");
        printf("=========================================\n");

        RUN_TEST(test_arena_basic_subdivision);
        RUN_TEST(test_arena_cross_region_growth);
        RUN_TEST(test_arena_huge_allocation);
        RUN_TEST(test_arena_zero_alloc);

        printf("=========================================\n");
        return 0;
}

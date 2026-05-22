#include "arena_allocator.h"        // 你的 ArenaAllocator 標頭檔
#include "fixed_buffer_allocator.h" // 你的 FixedBufferAllocator 標頭檔
#include "unittest.h"               // 微型測試框架
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// 模擬一個密集的業務資料結構
typedef struct TaskNode
{
        int              id;
        double           weight; // 8 Byte 對齊成員
        struct TaskNode* next;
} TaskNode;

// 測試案例 1：在有限的固定緩衝內進行多層級分配與對齊
int
test_hybrid_basic_and_alignment(void)
{
        // 1. 在 Stack 棧上準備一塊 1KB (1024 Byte) 的實體記憶體作為絕對邊界
        char                 stack_pool[1024];

        FixedBufferAllocator fba;
        fixed_buffer_allocator_init(&fba, stack_pool, sizeof(stack_pool));

        // 2. 建立 Arena，設定每次空間不夠時，向底層的 FBA 批發 256 Byte 的 Region 大水庫
        ArenaAllocator arena;
        arena_allocator_init(&arena, &fba, 256);

        // 3. 透過 Arena 密集分配物件
        TaskNode* t1 = (TaskNode*)allocator_alloc(&arena, sizeof(TaskNode));
        TaskNode* t2 = (TaskNode*)allocator_alloc(&arena, sizeof(TaskNode));

        EXPECT(t1 != NULL);
        EXPECT(t2 != NULL);

        // 驗證：位址滿足 8 Byte 對齊，且彼此獨立不重疊
        EXPECT(((uintptr_t)t1 % 8) == 0);
        EXPECT(((uintptr_t)t2 % 8) == 0);
        EXPECT(t1 != t2);

        // 驗證可寫入性
        t1->id     = 101;
        t1->weight = 99.9;
        t2->id     = 102;
        t2->weight = 88.8;
        EXPECT(t1->id == 101);
        EXPECT(t2->weight == 88.8);

        // 4. 清理 Arena 戰場
        allocator_destroy(&arena);

        // 5. 關鍵驗證：Arena 清理完後，FBA 應該要能重置並繼續給其他人使用
        fixed_buffer_allocator_reset(&fba);
        EXPECT(fba.offset == 0);

        return SUCCESS;
}

// 測試案例 2：自動觸發跨 Region 串接（強迫 Arena 向 FBA 批發多次）
int
test_hybrid_multi_region_growth(void)
{
        // 總容量 512 Byte
        char                 stack_pool[512];
        FixedBufferAllocator fba;
        fixed_buffer_allocator_init(&fba, stack_pool, sizeof(stack_pool));

        // Arena 每次只批發 128 Byte 的極小 Region
        ArenaAllocator arena;
        arena_allocator_init(&arena, &fba, 128);

        // 連續分配多個大物件，逼迫 Arena 內部觸發 Region 鏈結串接
        // 第一次分配 80 Byte -> 佔用第 1 個 Region (128B)
        char* p1 = (char*)allocator_alloc(&arena, 80);
        // 第二次分配 80 Byte -> 第 1 個 Region 塞不下，自動向 FBA 批發第 2 個 Region (128B)
        char* p2 = (char*)allocator_alloc(&arena, 80);
        // 第三次分配 80 Byte -> 自動向 FBA 批發第 3 個 Region (128B)
        char* p3 = (char*)allocator_alloc(&arena, 80);

        EXPECT(p1 != NULL);
        EXPECT(p2 != NULL);
        EXPECT(p3 != NULL);

        // 驗證跨 Region 後，最老的第一個 Region 資料 (p1) 絕對不能壞掉或被覆蓋
        memset(p1, 'A', 80);
        memset(p3, 'C', 80);
        EXPECT(p1[0] == 'A');
        EXPECT(p3[0] == 'C');

        // 驗證此時 FBA 游標確實有往前推進（至少推進了 3 個 Region 的批發量）
        EXPECT(fba.offset >= 128 * 3);

        allocator_destroy(&arena);
        return SUCCESS;
}

// 測試案例 3：嚴格的實體上限攔截測試（OutOfMemory）
int
test_hybrid_strict_oom(void)
{
        // 池子死死卡在 128 Byte
        char                 stack_pool[128];
        FixedBufferAllocator fba;
        fixed_buffer_allocator_init(&fba, stack_pool, sizeof(stack_pool));

        // Arena 預設批發 64 Byte
        ArenaAllocator arena;
        arena_allocator_init(&arena, &fba, 64);

        // 瘋狂索求空間，直到徹底把 128 Byte 土地榨乾
        void* ptr;
        int   alloc_count = 0;
        for (int i = 0; i < 20; i++) {
                ptr = allocator_alloc(&arena, 20);
                if (ptr == NULL) {
                        alloc_count = i;
                        break;
                }
        }

        // 關鍵驗證：到了極限時，Arena 必須回傳 NULL（因為底層 FBA 會拒絕發放新土地）
        // 程式絕對不能發生段錯誤崩潰，必須优雅攔截
        EXPECT(ptr == NULL);
        EXPECT(alloc_count < 20); // 證實確實中途被 FBA 卡截

        allocator_destroy(&arena);
        return SUCCESS;
}

int
main(void)
{
        printf("=========================================\n");
        printf("  FixedBuffer + Arena Hybrid Unittests   \n");
        printf("=========================================\n");

        RUN_TEST(test_hybrid_basic_and_alignment);
        RUN_TEST(test_hybrid_multi_region_growth);
        RUN_TEST(test_hybrid_strict_oom);

        printf("=========================================\n");
        return 0;
}

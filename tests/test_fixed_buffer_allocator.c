#include "fixed_buffer_allocator.h" // 你的 FixedBufferAllocator 標頭檔
#include "unittest.h"               // 你提供的微型測試框架
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// 測試案例 1：順序分配、可讀寫性與 8 位元組對齊驗證
int
test_fba_basic_and_alignment(void)
{
        // 準備一塊 64 Byte 的靜態陣列作為記憶體池
        char                 pool[64];

        FixedBufferAllocator allocator;
        fixed_buffer_allocator_init(&allocator, pool, sizeof(pool));

        // 第一次分配：11 Byte (故意給奇數)
        char* p1 = (char*)allocator_alloc(&allocator, 11);
        EXPECT(p1 != NULL);
        strcpy(p1, "Hello!"); // 驗證可寫入

        // 第二次分配：4 Byte
        // 由於 p1 佔了 11 Byte，游標在 11。為了滿足 8  alignment，
        // 分配器內部必須自動將游標向上對齊到 16，再切出 4 Byte。
        int* p2 = (int*)allocator_alloc(&allocator, sizeof(int));
        EXPECT(p2 != NULL);
        *p2 = 42;

        // 關鍵驗證：p2 的記憶體位址必須能被 8 整除
        EXPECT(((uintptr_t)p2 % 8) == 0);

        // 驗證 p1 的資料沒有被 p2 的對齊挪動給蓋掉
        EXPECT(strcmp(p1, "Hello!") == 0);
        EXPECT(*p2 == 42);

        return SUCCESS;
}

// 測試案例 2：空間剛好用盡與記憶體不足（OOM）攔截
int
test_fba_out_of_memory(void)
{
        char                 pool[32]; // 故意給一個極小的 32 Byte 池子

        FixedBufferAllocator allocator;
        fixed_buffer_allocator_init(&allocator, pool, sizeof(pool));

        // 1. 剛好分配合法的最大上限
        void* p1 = allocator_alloc(&allocator, 32);
        EXPECT(p1 != NULL);

        // 2. 池子已經乾了，再次申請哪怕只有 1 Byte，也必須安全回傳 NULL，不能崩潰
        void* p2 = allocator_alloc(&allocator, 1);
        EXPECT(p2 == NULL);

        return SUCCESS;
}

// 測試案例 3：一秒重置與重複利用測試
int
test_fba_reset_and_reuse(void)
{
        char                 pool[16];

        FixedBufferAllocator allocator;
        fixed_buffer_allocator_init(&allocator, pool, sizeof(pool));

        // 榨乾這 16 Byte
        void* p1 = allocator_alloc(&allocator, 16);
        EXPECT(p1 != NULL);

        // 此時再要空間必然失敗
        void* p2 = allocator_alloc(&allocator, 8);
        EXPECT(p2 == NULL);

        // 執行核心特有功能：Reset
        fixed_buffer_allocator_reset(&allocator);
        EXPECT(allocator.offset == 0); // 內部游標必須歸零

        // 重置後，原本爆滿的池子應該要能再次順利配置
        void* p3 = allocator_alloc(&allocator, 8);
        EXPECT(p3 != NULL);

        return SUCCESS;
}

// 測試案例 4：零分配邊界防禦與 Free 容錯
int
test_fba_zero_and_free(void)
{
        char                 pool[32];

        FixedBufferAllocator allocator;
        fixed_buffer_allocator_init(&allocator, pool, sizeof(pool));

        // 依據安全規範，要求 0 Byte 必須回傳 NULL
        void* p1 = allocator_alloc(&allocator, 0);
        EXPECT(p1 == NULL);
        EXPECT(allocator.offset == 0); // 游標絕對不能移動

        // FixedBufferAllocator 的 free 是空操作，傳入有效指標或 NULL 都必須安全容錯、不崩潰
        allocator_free(&allocator, pool);
        allocator_free(&allocator, NULL);

        return SUCCESS;
}

int
main(void)
{
        printf("=========================================\n");
        printf("    Fixed Buffer Allocator Unittests     \n");
        printf("=========================================\n");

        RUN_TEST(test_fba_basic_and_alignment);
        RUN_TEST(test_fba_out_of_memory);
        RUN_TEST(test_fba_reset_and_reuse);
        RUN_TEST(test_fba_zero_and_free);

        printf("=========================================\n");
        return 0;
}

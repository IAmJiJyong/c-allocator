#ifndef __UNITTEST_H__
#define __UNITTEST_H__

#include <stdio.h>
#include <time.h>

#define TEST_NAME_WIDTH 35

#define SUCCESS         1
#define FAILED          0

#define RUN_TEST(test)                                                                                                 \
        do {                                                                                                           \
                printf("Running %-*s", TEST_NAME_WIDTH, #test);                                                        \
                fflush(stdout);                                                                                        \
                                                                                                                       \
                clock_t start_time = clock();                                                                          \
                int     result     = test();                                                                           \
                clock_t end_time   = clock();                                                                          \
                                                                                                                       \
                double  elapsed_ms = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000.0;                      \
                                                                                                                       \
                if (result)                                                                                            \
                        printf(" [ \x1b[1;32mPASSED\x1b[0m ] (\x1b[1;33m%.3f ms\x1b[0m)\n", elapsed_ms);               \
                else                                                                                                   \
                        printf(" [ \x1b[1;31mFAILED\x1b[0m ] (\x1b[1;33m%.3f ms\x1b[0m)\n", elapsed_ms);               \
        } while (0)

#define EXPECT(cond)                                                                                                   \
        do {                                                                                                           \
                if (!(cond)) {                                                                                         \
                        fprintf(stderr, "\n%s:%d EXPECT FAILED: %s\n", __FILE__, __LINE__, #cond);                     \
                        return FAILED;                                                                                 \
                }                                                                                                      \
        } while (0)

#endif

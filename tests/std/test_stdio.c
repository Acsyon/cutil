#include "unity.h"

#include <string.h>

#include <cutil/std/stdio.h>

static void
_test_sfclose(void)
{
    /* Arrange */
    FILE *const ins[] = {NULL, stdin, stdout, stderr};
    const size_t num = sizeof ins / sizeof *ins;

    for (size_t i = 0; i < num; ++i) {
        FILE *const in = ins[i];

        /* Act */
        const int res = cutil_sfclose(in);
        
        /* Assert */
        TEST_ASSERT_EQUAL_INT(0, res);
    }
}

static void
_test_sfclose_on_tmpfile(void)
{
    /* Arrange */
    FILE *const in = tmpfile();

    /* Act */
    const int res = cutil_sfclose(in);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void
_test_get_filesize(void)
{
    /* Arrange */
    char str[64] = {0};
    memset(str, '1', sizeof str);

    long sizes[] = {0, 1, 2, 4, 8, 16, 32, 64};
    const size_t num = sizeof sizes / sizeof *sizes;

    for (size_t i = 0; i < num; ++i) {
        const long size = sizes[i];
        FILE *const in = tmpfile();
        fwrite(str, 1UL, size, in);

        /* Act */
        const long fsize = cutil_get_filesize(in);
        
        /* Assert */
        TEST_ASSERT_EQUAL(size + 1, fsize);

        /* Cleanup */
        fclose(in);
    }
}

void
setUp(void)
{}

void
tearDown(void)
{}

int
main(void)
{
    UNITY_BEGIN();
    RUN_TEST(_test_sfclose);
    RUN_TEST(_test_sfclose_on_tmpfile);
    RUN_TEST(_test_get_filesize);
    return UNITY_END();
}

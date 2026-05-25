#include "unity.h"
#include <cutil/core/std/stdio.h>

#include <cutil/core/std/string.h>
#include <cutil/core/util/macro.h>

static void
test_should_notClose_when_haveNullOrStdStreams(void)
{
    /* Arrange */
    FILE *const INS[] = {NULL, stdin, stdout, stderr};
    const size_t NUM = CUTIL_GET_NATIVE_ARRAY_SIZE(INS);

    for (size_t i = 0; i < NUM; ++i) {
        FILE *const in = INS[i];

        /* Act */
        const int res = cutil_sfclose(in);

        /* Assert */
        TEST_ASSERT_EQUAL_INT(0, res);
    }
}

static void
test_should_close_when_haveTmpfile(void)
{
    /* Arrange */
    FILE *const in = tmpfile();

    /* Act */
    const int res = cutil_sfclose(in);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void
test_should_returnCorrectSize_when_haveTmpfile(void)
{
    /* Arrange */
    char str[64] = {0};
    memset(str, '1', sizeof str);

    const long SIZES[] = {0, 1, 2, 4, 8, 16, 32, 64};
    const size_t NUM = CUTIL_GET_NATIVE_ARRAY_SIZE(SIZES);

    for (size_t i = 0; i < NUM; ++i) {
        const long size = SIZES[i];
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

    RUN_TEST(test_should_notClose_when_haveNullOrStdStreams);
    RUN_TEST(test_should_close_when_haveTmpfile);
    RUN_TEST(test_should_returnCorrectSize_when_haveTmpfile);

    return UNITY_END();
}

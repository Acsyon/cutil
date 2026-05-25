#include "unity.h"
#include <cutil/core/std/string.h>

#include <cutil/core/util/macro.h>

static const char *const SHORT_STRINGS[] = {
  "This", "is", "a", "short", "string",
};
static const size_t NUM_SHORT = CUTIL_GET_NATIVE_ARRAY_SIZE(SHORT_STRINGS);

static const char *const LONG_STRINGS[] = {
  "This is a very long string for testing purposes",
  "THIS IS ANOTHER, EVEN LONGER STRING THAT FULFILS THE SAME OBJECTIVE",
  "Since two is a very pitiful number for test cases, here's another one",
};
static const size_t NUM_LONG = CUTIL_GET_NATIVE_ARRAY_SIZE(LONG_STRINGS);

static void
test_should_returnStrlen_when_stringIsShorterThanMaxlen(void)
{
    /* Arrange */
    const size_t maxlen = 10;

    for (size_t i = 0; i < NUM_SHORT; ++i) {
        const char *const str = SHORT_STRINGS[i];

        /* Act */
        const size_t res = cutil_strnlen(str, maxlen);

        /* Assert */
        TEST_ASSERT_EQUAL_size_t(strlen(str), res);
    }
}

static void
test_should_returnMaxlen_when_stringIsLongerThanMaxlen(void)
{
    /* Arrange */
    const size_t maxlen = 10;

    for (size_t i = 0; i < NUM_LONG; ++i) {
        const char *const str = LONG_STRINGS[i];

        /* Act */
        const size_t res = cutil_strnlen(str, maxlen);

        /* Assert */
        TEST_ASSERT_EQUAL_size_t(maxlen, res);
    }
}

static void
sf_strdup_test_fnc_aux(size_t num, const char *const strs[])
{
    for (size_t i = 0; i < num; ++i) {
        const char *const str = strs[i];

        /* Act */
        char *const res = cutil_strdup(str);

        /* Assert */
        TEST_ASSERT_EQUAL_STRING(str, res);

        /* Cleanup */
        free(res);
    }
}

static void
test_should_duplicateStringCorrectly_when_useStrdup(void)
{
    /* Act */
    /* Assert */
    sf_strdup_test_fnc_aux(NUM_SHORT, SHORT_STRINGS);
    sf_strdup_test_fnc_aux(NUM_LONG, LONG_STRINGS);
}

static void
test_should_duplicateFullString_when_stringIsShorterThanMaxlen(void)
{
    /* Arrange */
    const size_t maxlen = 10;

    for (size_t i = 0; i < NUM_SHORT; ++i) {
        const char *const str = SHORT_STRINGS[i];

        /* Act */
        char *const res = cutil_strndup(str, maxlen);

        /* Assert */
        TEST_ASSERT_EQUAL_STRING(str, res);

        /* Cleanup */
        free(res);
    }
}

static void
test_should_duplicateFullString_when_stringIsLongerThanMaxlen(void)
{
    /* Arrange */
    const size_t maxlen = 10;

    for (size_t i = 0; i < NUM_LONG; ++i) {
        const char *const str = LONG_STRINGS[i];

        /* Act */
        char *const res = cutil_strndup(str, maxlen);

        /* Assert */
        TEST_ASSERT_EQUAL_STRING_LEN(str, res, maxlen);
        TEST_ASSERT_EQUAL_CHAR('\0', res[maxlen]);

        /* Cleanup */
        free(res);
    }
}

static void
test_should_returnNull_when_strdupReceivesNull(void)
{
    /* Act */
    char *const res = cutil_strdup(NULL);

    /* Assert */
    TEST_ASSERT_NULL(res);
}

static void
test_should_returnNull_when_strndupReceivesNull(void)
{
    /* Act */
    char *const res = cutil_strndup(NULL, 10);

    /* Assert */
    TEST_ASSERT_NULL(res);
}

static void
test_should_returnNull_when_memdupReceivesNull(void)
{
    /* Act */
    void *const res = cutil_memdup(NULL, sizeof(int), 4);

    /* Assert */
    TEST_ASSERT_NULL(res);
}

static void
test_should_duplicateIntArray_when_useMemdup(void)
{
    /* Arrange */
    const int src[] = {1, 2, 3, 4, 5};
    const size_t num = CUTIL_GET_NATIVE_ARRAY_SIZE(src);

    /* Act */
    int *const res = cutil_memdup(src, sizeof *src, num);

    /* Assert */
    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_INT_ARRAY(src, res, num);

    /* Cleanup */
    free(res);
}

static void
test_should_duplicateByteArray_when_useMemdup(void)
{
    /* Arrange */
    const unsigned char src[] = {0x00, 0xFF, 0xAB, 0x42};
    const size_t num = CUTIL_GET_NATIVE_ARRAY_SIZE(src);

    /* Act */
    unsigned char *const res = cutil_memdup(src, sizeof *src, num);

    /* Assert */
    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(src, res, num);

    /* Cleanup */
    free(res);
}

static void
test_should_produceSeparateCopy_when_useMemdup(void)
{
    /* Arrange */
    int src[] = {10, 20, 30};
    const size_t num = CUTIL_GET_NATIVE_ARRAY_SIZE(src);

    /* Act */
    int *const res = cutil_memdup(src, sizeof *src, num);

    /* Assert */
    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_NOT_EQUAL(src, res); /* different pointers */

    /* Mutating src must not affect the copy */
    src[0] = 99;
    TEST_ASSERT_EQUAL_INT(10, res[0]);

    /* Cleanup */
    free(res);
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

    RUN_TEST(test_should_returnStrlen_when_stringIsShorterThanMaxlen);
    RUN_TEST(test_should_returnMaxlen_when_stringIsLongerThanMaxlen);
    RUN_TEST(test_should_duplicateStringCorrectly_when_useStrdup);
    RUN_TEST(test_should_duplicateFullString_when_stringIsShorterThanMaxlen);
    RUN_TEST(test_should_duplicateFullString_when_stringIsLongerThanMaxlen);
    RUN_TEST(test_should_returnNull_when_strdupReceivesNull);
    RUN_TEST(test_should_returnNull_when_strndupReceivesNull);
    RUN_TEST(test_should_returnNull_when_memdupReceivesNull);
    RUN_TEST(test_should_duplicateIntArray_when_useMemdup);
    RUN_TEST(test_should_duplicateByteArray_when_useMemdup);
    RUN_TEST(test_should_produceSeparateCopy_when_useMemdup);

    return UNITY_END();
}

#include "unity.h"

#include <cutil/std/string.h>

static const char *SHORT_STRINGS[] = {"This", "is", "a", "short", "string"};

static const char *LONG_STRINGS[] = {
  "This is a very long string for testing purposes",
  "THIS IS ANOTHER, EVEN LONGER STRING THAT FULFILS THE SAME OBJECTIVE",
  "Since two is a very pitiful number for test cases, here's another one"
};

static void
_should_returnStrlen_when_stringIsShorterThanMaxlen(void)
{
    /* Arrange */
    const size_t maxlen = 10;
    const size_t num = sizeof SHORT_STRINGS / sizeof *SHORT_STRINGS;

    for (size_t i = 0; i < num; ++i) {
        const char *const str = SHORT_STRINGS[i];

        /* Act */
        const size_t res = cutil_strnlen(str, maxlen);

        /* Assert */
        TEST_ASSERT_EQUAL_size_t(strlen(str), res);
    }
}

static void
_should_returnMaxlen_when_stringIsLongerThanMaxlen(void)
{
    /* Arrange */
    const size_t maxlen = 10;
    const size_t num = sizeof LONG_STRINGS / sizeof *LONG_STRINGS;

    for (size_t i = 0; i < num; ++i) {
        const char *const str = LONG_STRINGS[i];

        /* Act */
        const size_t res = cutil_strnlen(str, maxlen);

        /* Assert */
        TEST_ASSERT_EQUAL_size_t(maxlen, res);
    }
}

static void
_strdup_test_fnc_aux(size_t num, const char *strs[])
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
_should_duplicateStringCorrectly_when_useStrdup(void)
{
    /* Arrange */
    const size_t num_short = sizeof SHORT_STRINGS / sizeof *SHORT_STRINGS;
    const size_t num_long = sizeof LONG_STRINGS / sizeof *LONG_STRINGS;

    /* Act */
    /* Assert */
    _strdup_test_fnc_aux(num_short, SHORT_STRINGS);
    _strdup_test_fnc_aux(num_long, LONG_STRINGS);
}

static void
_should_duplicateFullString_when_stringIsShorterThanMaxlen(void)
{
    /* Arrange */
    const size_t maxlen = 10;
    const size_t num = sizeof SHORT_STRINGS / sizeof *SHORT_STRINGS;

    for (size_t i = 0; i < num; ++i) {
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
_should_duplicateFullString_when_stringIsLongerThanMaxlen(void)
{
    /* Arrange */
    const size_t maxlen = 10;
    const size_t num = sizeof LONG_STRINGS / sizeof *LONG_STRINGS;

    for (size_t i = 0; i < num; ++i) {
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
    RUN_TEST(_should_returnStrlen_when_stringIsShorterThanMaxlen);
    RUN_TEST(_should_returnMaxlen_when_stringIsLongerThanMaxlen);
    RUN_TEST(_should_duplicateStringCorrectly_when_useStrdup);
    RUN_TEST(_should_duplicateFullString_when_stringIsShorterThanMaxlen);
    RUN_TEST(_should_duplicateFullString_when_stringIsLongerThanMaxlen);
    return UNITY_END();
}

#include <string.h>

#include "unity.h"

#include <cutil/stringbuilder.h>

#define LONG_STRING                                                            \
    "This is a very long string that exceeds the initial and default capacity"

static void
_should_constructBuilder_when_useSize(void)
{
    /* Arrange */
    const char *const assert_str = "";

    /* Act */
    cutil_StringBuilder *const sb = cutil_StringBuilder_alloc(0);

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(strlen(assert_str), len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);
    TEST_ASSERT_GREATER_THAN_size_t(0, sb->capacity);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_constructBuilder_when_useString(void)
{
    /* Arrange */
    const char *const assert_str = LONG_STRING;

    /* Act */
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(assert_str);

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(strlen(assert_str), len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_constructBuilder_when_useFile(void)
{
    /* Arrange */
    const char *const assert_str = LONG_STRING;
    FILE *const in = tmpfile();
    fwrite(assert_str, 1UL, sizeof LONG_STRING, in);

    /* Act */
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_file(in);

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(strlen(assert_str), len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_duplicateBuilder(void)
{
    /* Arrange */
    const char *const assert_str = LONG_STRING;
    cutil_StringBuilder *const sb_assert
      = cutil_StringBuilder_from_string(assert_str);

    /* Act */
    cutil_StringBuilder *const sb = cutil_StringBuilder_duplicate(sb_assert);

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(strlen(assert_str), len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);

    TEST_ASSERT_EQUAL_size_t(sb_assert->length, sb->length);
    TEST_ASSERT_EQUAL_STRING(sb_assert->str, sb->str);
    TEST_ASSERT_EQUAL_size_t(sb_assert->capacity, sb->capacity);
    TEST_ASSERT_EQUAL_size_t(sb_assert->bufsiz, sb->bufsiz);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_clearCorrectly(void)
{
    /* Arrange */
    const char *const append_str = LONG_STRING;
    cutil_StringBuilder *const sb_assert = cutil_StringBuilder_create();
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(append_str);

    /* Act */
    cutil_StringBuilder_clear(sb);

    /* Assert */
    const size_t assert_len = cutil_StringBuilder_length(sb_assert);
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(assert_len, len);

    const char *const assert_str = cutil_StringBuilder_get_string(sb_assert);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);

    TEST_ASSERT_EQUAL_size_t(sb_assert->capacity, sb->capacity);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_copyCorrectly(void)
{
    /* Arrange */
    const char *const assert_str = LONG_STRING;
    cutil_StringBuilder *const sb_assert
      = cutil_StringBuilder_from_string(assert_str);
    cutil_StringBuilder *const sb = cutil_StringBuilder_create();

    /* Act */
    cutil_StringBuilder_copy(sb, sb_assert);

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(strlen(assert_str), len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);

    TEST_ASSERT_EQUAL_size_t(sb_assert->length, sb->length);
    TEST_ASSERT_EQUAL_STRING(sb_assert->str, sb->str);
    TEST_ASSERT_EQUAL_size_t(sb_assert->capacity, sb->capacity);
    TEST_ASSERT_EQUAL_size_t(sb_assert->bufsiz, sb->bufsiz);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_appendCorrectly_when_haveSingleString(void)
{
    /* Arrange */
    const char *const assert_str = "Hello";
    cutil_StringBuilder *const sb = cutil_StringBuilder_create();

    /* Act */
    cutil_StringBuilder_append(sb, assert_str);

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(strlen(assert_str), len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_appendCorrectly_when_haveMultipleStrings(void)
{
    /* Arrange */
    const char *const input_str_1 = "Hello";
    const char *const input_str_2 = ", World!";
    const char *const assert_str = "Hello, World!";
    cutil_StringBuilder *const sb = cutil_StringBuilder_create();

    /* Act */
    cutil_StringBuilder_append(sb, input_str_1);
    cutil_StringBuilder_append(sb, input_str_2);

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(strlen(assert_str), len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_appendCorrectly_when_haveEmptyString(void)
{
    /* Arrange */
    const char *const assert_str = "Hello, World!";
    const char *const append_str = "";
    cutil_StringBuilder *const sb = cutil_StringBuilder_create();

    /* Act */
    cutil_StringBuilder_append(sb, assert_str);
    cutil_StringBuilder_append(sb, append_str);

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(strlen(assert_str), len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_expandCorrectly_when_appendString(void)
{
    /* Arrange */
    const char *const assert_str = LONG_STRING;
    cutil_StringBuilder *const sb = cutil_StringBuilder_create();
    const size_t init_capacity = sb->capacity;

    /* Act */
    cutil_StringBuilder_append(sb, assert_str);

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(strlen(assert_str), len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);
    TEST_ASSERT_GREATER_THAN_size_t(init_capacity, sb->capacity);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_appendCorrectly_when_haveHugeNumberOfStrings(void)
{
    /* Arrange */
    const size_t num_appends = 1023;
    const char *const append_str = "x";
    cutil_StringBuilder *const sb = cutil_StringBuilder_create();

    /* Act */
    for (size_t i = 0; i < num_appends; ++i) {
        cutil_StringBuilder_append(sb, append_str);
    }

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(num_appends, len);
    TEST_ASSERT_GREATER_THAN_size_t(num_appends + 1, sb->capacity);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_insertCorrectly(void)
{
    /* Arrange */
    const char *const input_str_1 = "Hello, World!";
    const char *const input_str_2 = " Beautiful";
    const char *const assert_str = "Hello, Beautiful World!";
    cutil_StringBuilder *const sb
      = cutil_StringBuilder_from_string(input_str_1);

    /* Act */
    cutil_StringBuilder_insert(sb, 6, input_str_2);

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(strlen(assert_str), len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_shrink_when_resize(void)
{
    /* Arrange */
    const char *const assert_str = LONG_STRING;
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(assert_str);
    const size_t before_length = cutil_StringBuilder_length(sb);
    const size_t before_capacity = sb->capacity;
    const size_t before_bufsiz = sb->bufsiz;
    const int resize_flags
      = CUTIL_RESIZE_FLAG_STRING | CUTIL_RESIZE_FLAG_BUFFER;

    const size_t SIZES[] = {64, 56, 48, 32, 24, 16, 8};
    const size_t NUM_SIZES = (sizeof SIZES) / (sizeof SIZES[0]);
    for (size_t i = 0; i < NUM_SIZES; ++i) {
        /* Act */
        cutil_StringBuilder_resize(sb, SIZES[i], resize_flags);

        /* Assert */
        TEST_ASSERT_TRUE(before_length >= cutil_StringBuilder_length(sb));
        TEST_ASSERT_TRUE(before_capacity >= sb->capacity);
        TEST_ASSERT_TRUE(before_bufsiz >= sb->bufsiz);
    }

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_shrinkCorrectly_when_resizeWithForce(void)
{
    /* Arrange */
    const char *const assert_str = LONG_STRING;
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(assert_str);
    const int resize_flags = CUTIL_RESIZE_FLAG_STRING | CUTIL_RESIZE_FLAG_BUFFER
      | CUTIL_RESIZE_FLAG_FORCE;

    const size_t SIZES[] = {64, 56, 48, 32, 24, 16, 8};
    const size_t NUM_SIZES = (sizeof SIZES) / (sizeof SIZES[0]);
    for (size_t i = 0; i < NUM_SIZES; ++i) {
        const size_t size = SIZES[i];

        /* Act */
        cutil_StringBuilder_resize(sb, size, resize_flags);

        /* Assert */
        TEST_ASSERT_EQUAL_size_t(size, cutil_StringBuilder_length(sb));
        TEST_ASSERT_EQUAL_size_t(size, sb->capacity);
        TEST_ASSERT_EQUAL_size_t(size, sb->bufsiz);
    }

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_expand_when_resize(void)
{
    /* Arrange */
    const char *const assert_str = "Hello, World!";
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(assert_str);
    const size_t before_length = cutil_StringBuilder_length(sb);
    const size_t before_capacity = sb->capacity;
    const size_t before_bufsiz = sb->bufsiz;
    const int resize_flags
      = CUTIL_RESIZE_FLAG_STRING | CUTIL_RESIZE_FLAG_BUFFER;

    const size_t SIZES[] = {64, 128, 256, 512};
    const size_t NUM_SIZES = (sizeof SIZES) / (sizeof SIZES[0]);
    for (size_t i = 0; i < NUM_SIZES; ++i) {
        const size_t size = SIZES[i];

        /* Act */
        cutil_StringBuilder_resize(sb, size, resize_flags);

        /* Assert */
        TEST_ASSERT_TRUE(before_length == cutil_StringBuilder_length(sb));
        TEST_ASSERT_TRUE(before_capacity < sb->capacity);
        if (size >= before_bufsiz) {
            TEST_ASSERT_TRUE(before_bufsiz <= sb->bufsiz);
        }
    }

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_expandCorrectly_when_resizeWithForce(void)
{
    /* Arrange */
    const char *const assert_str = "Hello, World!";
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(assert_str);
    const size_t before_length = cutil_StringBuilder_length(sb);
    const int resize_flags = CUTIL_RESIZE_FLAG_STRING | CUTIL_RESIZE_FLAG_BUFFER
      | CUTIL_RESIZE_FLAG_FORCE;

    const size_t SIZES[] = {64, 128, 256, 512};
    const size_t NUM_SIZES = (sizeof SIZES) / (sizeof SIZES[0]);
    for (size_t i = 0; i < NUM_SIZES; ++i) {
        const size_t size = SIZES[i];

        /* Act */
        cutil_StringBuilder_resize(sb, size, resize_flags);

        /* Assert */
        TEST_ASSERT_EQUAL_size_t(before_length, cutil_StringBuilder_length(sb));
        TEST_ASSERT_EQUAL_size_t(size, sb->capacity);
        TEST_ASSERT_EQUAL_size_t(size, sb->bufsiz);
    }

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_resizeCorrectly_when_shrinkToFit(void)
{
    /* Arrange */
    const char *const assert_str = "Hello, World!";
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(assert_str);
    const size_t before_length = cutil_StringBuilder_length(sb);

    /* Act */
    cutil_StringBuilder_shrink_to_fit(sb);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(before_length, cutil_StringBuilder_length(sb));
    TEST_ASSERT_EQUAL_size_t(before_length + 1, sb->capacity);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_duplicateStringCorrectly(void)
{
    /* Arrange */
    const char *const assert_str = "Hello, World!";
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(assert_str);

    /* Act */
    char *const cpy = cutil_StringBuilder_duplicate_string(sb);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING(assert_str, cpy);

    /* Cleanup */
    free(cpy);
    cutil_StringBuilder_free(sb);
}

static void
_should_deleteCorrectly()
{
    /* Arrange */
    const char *const input_str = "Hello, Beautiful World!";
    const char *const assert_str = "Hello, World!";
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(input_str);

    /* Act */
    cutil_StringBuilder_delete(sb, 6, 10);

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(strlen(assert_str), len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_deleteCorrectly_when_boundsExceedSize()
{
    /* Arrange */
    const char *const input_str = "Hello, Beautiful World!";
    const char *const assert_str = "Hello,";
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(input_str);

    /* Act */
    cutil_StringBuilder_delete(sb, 6, 100);

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(strlen(assert_str), len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_deleteCorrectly_when_useFromTo()
{
    /* Arrange */
    const char *const input_str = "Hello, Beautiful World!";
    const char *const assert_str = "Hello, World!";
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(input_str);

    /* Act */
    cutil_StringBuilder_delete_from_to(sb, 6, 15);

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(strlen(assert_str), len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_deleteFromToCorrectly_when_boundsExceedSize()
{
    /* Arrange */
    const char *const input_str = "Hello, Beautiful World!";
    const char *const assert_str = "Hello,";
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(input_str);

    /* Act */
    cutil_StringBuilder_delete_from_to(sb, 6, 100);

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(strlen(assert_str), len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
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
    RUN_TEST(_should_constructBuilder_when_useSize);
    RUN_TEST(_should_constructBuilder_when_useString);
    RUN_TEST(_should_constructBuilder_when_useFile);
    RUN_TEST(_should_duplicateBuilder);
    RUN_TEST(_should_clearCorrectly);
    RUN_TEST(_should_copyCorrectly);
    RUN_TEST(_should_appendCorrectly_when_haveSingleString);
    RUN_TEST(_should_appendCorrectly_when_haveMultipleStrings);
    RUN_TEST(_should_appendCorrectly_when_haveEmptyString);
    RUN_TEST(_should_expandCorrectly_when_appendString);
    RUN_TEST(_should_appendCorrectly_when_haveHugeNumberOfStrings);
    RUN_TEST(_should_insertCorrectly);
    RUN_TEST(_should_shrink_when_resize);
    RUN_TEST(_should_shrinkCorrectly_when_resizeWithForce);
    RUN_TEST(_should_expand_when_resize);
    RUN_TEST(_should_expandCorrectly_when_resizeWithForce);
    RUN_TEST(_should_resizeCorrectly_when_shrinkToFit);
    RUN_TEST(_should_duplicateStringCorrectly);
    RUN_TEST(_should_deleteCorrectly);
    RUN_TEST(_should_deleteCorrectly_when_boundsExceedSize);
    RUN_TEST(_should_deleteCorrectly_when_useFromTo);
    RUN_TEST(_should_deleteFromToCorrectly_when_boundsExceedSize);
    return UNITY_END();
}

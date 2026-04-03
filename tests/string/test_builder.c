#include <cutil/string/builder.h>

#include "unity.h"

#include <cutil/data/generic/type.h>
#include <cutil/std/string.h>
#include <cutil/string/builder.h>
#include <cutil/util/hash.h>
#include <cutil/util/macro.h>

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
_should_preserveNulInvariant_when_constructEmpty(void)
{
    /* Act */
    cutil_StringBuilder *const sb = cutil_StringBuilder_alloc(0);

    /* Assert: invariant str[length] == '\0' on an empty builder */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(0, len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_CHAR('\0', str[len]);

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
    cutil_StringBuilder_free(sb_assert);
    cutil_StringBuilder_free(sb);
}

static void
_should_clearCorrectly(void)
{
    /* Arrange */
    const char *const append_str = LONG_STRING;
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(append_str);

    /* Act */
    cutil_StringBuilder_clear(sb);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0, cutil_StringBuilder_length(sb));
    TEST_ASSERT_NULL(cutil_StringBuilder_get_string(sb));
    TEST_ASSERT_EQUAL_size_t(0, sb->capacity);

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
    cutil_StringBuilder_free(sb_assert);
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
    /* Invariant: str[length] must be NUL after middle insert */
    TEST_ASSERT_EQUAL_CHAR('\0', str[len]);

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
    const size_t NUM_SIZES = CUTIL_GET_NATIVE_ARRAY_SIZE(SIZES);
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
    const size_t NUM_SIZES = CUTIL_GET_NATIVE_ARRAY_SIZE(SIZES);
    for (size_t i = 0; i < NUM_SIZES; ++i) {
        const size_t size = SIZES[i];

        /* Act */
        cutil_StringBuilder_resize(sb, size, resize_flags);

        /* Assert */
        TEST_ASSERT_EQUAL_size_t(size, cutil_StringBuilder_length(sb));
        /* capacity = size + 1: resize allocates one extra byte for null
         * terminator */
        TEST_ASSERT_EQUAL_size_t(size + 1, sb->capacity);
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
    const size_t NUM_SIZES = CUTIL_GET_NATIVE_ARRAY_SIZE(SIZES);
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
    const size_t NUM_SIZES = CUTIL_GET_NATIVE_ARRAY_SIZE(SIZES);
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
    /* Invariant: duplicated string is NUL-terminated */
    TEST_ASSERT_EQUAL_CHAR('\0', cpy[strlen(assert_str)]);

    /* Cleanup */
    free(cpy);
    cutil_StringBuilder_free(sb);
}

static void
_should_copyStringCorrectly_when_bufferIsLargeEnough(void)
{
    /* Arrange */
    const char assert_str[] = "Hello, World!";
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(assert_str);
    const size_t buflen = sizeof assert_str;
    char *const buf = malloc(buflen * sizeof *buf);

    /* Act */
    const size_t len = cutil_StringBuilder_copy_string_to_buf(sb, buflen, buf);
    TEST_ASSERT_EQUAL_size_t(buflen - 1, len);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING(assert_str, buf);
    /* Invariant: buf is NUL-terminated on success */
    TEST_ASSERT_EQUAL_CHAR('\0', buf[len]);

    /* Cleanup */
    free(buf);
    cutil_StringBuilder_free(sb);
}

static void
_should_notCopyString_when_bufferIsTooSmall(void)
{
    /* Arrange */
    const char *const assert_str = "Hello, World!";
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(assert_str);

    const char small_str[] = "Hello";
    const size_t buflen = sizeof small_str;
    char *const buf = cutil_strdup(small_str);

    /* Act */
    const size_t len = cutil_StringBuilder_copy_string_to_buf(sb, buflen, buf);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING(small_str, buf);
    TEST_ASSERT_EQUAL_size_t(0, len);

    /* Cleanup */
    free(buf);
    cutil_StringBuilder_free(sb);
}

static void
_should_copyStringCorrectly_when_bufferIsNull(void)
{
    /* Arrange */
    const char assert_str[] = "Hello, World!";
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(assert_str);
    size_t buflen = 0;
    char *buf = NULL;

    /* Act */
    const size_t newlen = cutil_StringBuilder_copy_string(sb, &buflen, &buf);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING(assert_str, buf);
    TEST_ASSERT_EQUAL_size_t(sizeof assert_str, buflen);
    TEST_ASSERT_EQUAL_size_t(sizeof assert_str - 1, newlen);

    /* Cleanup */
    free(buf);
    cutil_StringBuilder_free(sb);
}

static void
_should_copyStringAndRetainPointer_when_bufferIsLargeEnough(void)
{
    /* Arrange */
    const char assert_str[] = "Hello, World!";
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(assert_str);

    size_t buflen = sizeof assert_str + 1;
    char *buf = malloc(buflen * sizeof *buf);
    char *const old_buf = buf;
    const size_t old_buflen = buflen;

    /* Act */
    const size_t newlen = cutil_StringBuilder_copy_string(sb, &buflen, &buf);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING(assert_str, buf);
    TEST_ASSERT_EQUAL_PTR(old_buf, buf);
    TEST_ASSERT_EQUAL_size_t(old_buflen, buflen);
    TEST_ASSERT_EQUAL_size_t(sizeof assert_str - 1, newlen);

    /* Cleanup */
    free(buf);
    cutil_StringBuilder_free(sb);
}

static void
_should_copyStringCorrectly_when_buflenIsNull(void)
{
    /* Arrange */
    const char assert_str[] = "Hello, World!";
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(assert_str);
    char *buf = NULL;

    /* Act */
    const size_t newlen = cutil_StringBuilder_copy_string(sb, NULL, &buf);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING(assert_str, buf);
    TEST_ASSERT_EQUAL_size_t(sizeof assert_str - 1, newlen);

    /* Cleanup */
    free(buf);
    cutil_StringBuilder_free(sb);
}

static void
_should_deleteCorrectly(void)
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
_should_deleteCorrectly_when_numLessThanRemainder(void)
{
    /* Arrange: delete 1 char where the remaining suffix (7 chars) exceeds num.
     * The old memmove(num) bug would fail to move the suffix and NUL. */
    const char *const input_str = "Hello, World!";
    const char *const assert_str = "Hello World!";
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(input_str);

    /* Act: delete ',' at pos 5 (num=1, remainder=7 > num) */
    cutil_StringBuilder_delete(sb, 5, 1);

    /* Assert */
    const size_t len = cutil_StringBuilder_length(sb);
    TEST_ASSERT_EQUAL_size_t(strlen(assert_str), len);
    const char *const str = cutil_StringBuilder_get_string(sb);
    TEST_ASSERT_EQUAL_STRING(assert_str, str);
    /* Invariant: str[length] must be NUL — catches the memmove(num) bug */
    TEST_ASSERT_EQUAL_CHAR('\0', str[len]);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_deleteCorrectly_when_boundsExceedSize(void)
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
_should_deleteCorrectly_when_useFromTo(void)
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
_should_deleteFromToCorrectly_when_boundsExceedSize(void)
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

static void
_should_returnTrue_when_deepEqualsGenericCalledOnEqualStrings(void)
{
    /* Arrange */
    cutil_StringBuilder *const lhs = cutil_StringBuilder_from_string("hello");
    cutil_StringBuilder *const rhs = cutil_StringBuilder_from_string("hello");

    /* Act */
    const cutil_Bool result = cutil_StringBuilder_deep_equals_generic(lhs, rhs);

    /* Assert */
    TEST_ASSERT_TRUE(result);

    /* Cleanup */
    cutil_StringBuilder_free(lhs);
    cutil_StringBuilder_free(rhs);
}

static void
_should_returnFalse_when_deepEqualsGenericCalledOnDifferentStrings(void)
{
    /* Arrange */
    cutil_StringBuilder *const lhs = cutil_StringBuilder_from_string("hello");
    cutil_StringBuilder *const rhs = cutil_StringBuilder_from_string("world");

    /* Act */
    const cutil_Bool result = cutil_StringBuilder_deep_equals_generic(lhs, rhs);

    /* Assert */
    TEST_ASSERT_FALSE(result);

    /* Cleanup */
    cutil_StringBuilder_free(lhs);
    cutil_StringBuilder_free(rhs);
}

static void
_should_returnTrue_when_deepEqualsGenericCalledOnSamePointer(void)
{
    /* Arrange */
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string("hello");

    /* Act */
    const cutil_Bool result = cutil_StringBuilder_deep_equals_generic(sb, sb);

    /* Assert */
    TEST_ASSERT_TRUE(result);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_returnFalse_when_deepEqualsGenericCalledOnNullAndNonNull(void)
{
    /* Arrange */
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string("hello");

    /* Act / Assert */
    TEST_ASSERT_FALSE(cutil_StringBuilder_deep_equals_generic(NULL, sb));
    TEST_ASSERT_FALSE(cutil_StringBuilder_deep_equals_generic(sb, NULL));

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_returnTrue_when_deepEqualsGenericCalledOnTwoNulls(void)
{
    /* Act */
    const cutil_Bool result
      = cutil_StringBuilder_deep_equals_generic(NULL, NULL);

    /* Assert */
    TEST_ASSERT_TRUE(result);
}

static void
_should_returnZero_when_compareGenericCalledOnEqualStrings(void)
{
    /* Arrange */
    cutil_StringBuilder *const lhs = cutil_StringBuilder_from_string("hello");
    cutil_StringBuilder *const rhs = cutil_StringBuilder_from_string("hello");

    /* Act */
    const int result = cutil_StringBuilder_compare_generic(lhs, rhs);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, result);

    /* Cleanup */
    cutil_StringBuilder_free(lhs);
    cutil_StringBuilder_free(rhs);
}

static void
_should_returnNegative_when_compareGenericCalledOnLesserFirst(void)
{
    /* Arrange */
    cutil_StringBuilder *const lhs = cutil_StringBuilder_from_string("abc");
    cutil_StringBuilder *const rhs = cutil_StringBuilder_from_string("xyz");

    /* Act */
    const int result = cutil_StringBuilder_compare_generic(lhs, rhs);

    /* Assert */
    TEST_ASSERT_TRUE(result < 0);

    /* Cleanup */
    cutil_StringBuilder_free(lhs);
    cutil_StringBuilder_free(rhs);
}

static void
_should_returnPositive_when_compareGenericCalledOnGreaterFirst(void)
{
    /* Arrange */
    cutil_StringBuilder *const lhs = cutil_StringBuilder_from_string("xyz");
    cutil_StringBuilder *const rhs = cutil_StringBuilder_from_string("abc");

    /* Act */
    const int result = cutil_StringBuilder_compare_generic(lhs, rhs);

    /* Assert */
    TEST_ASSERT_TRUE(result > 0);

    /* Cleanup */
    cutil_StringBuilder_free(lhs);
    cutil_StringBuilder_free(rhs);
}

static void
_should_returnNegative_when_compareGenericCalledOnNullFirst(void)
{
    /* Arrange */
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string("hello");

    /* Act */
    const int result = cutil_StringBuilder_compare_generic(NULL, sb);

    /* Assert */
    TEST_ASSERT_TRUE(result < 0);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_returnNonZeroHash_when_hashGenericCalledOnNonEmptyBuilder(void)
{
    /* Arrange */
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string("hello");

    /* Act */
    const cutil_hash_t hash = cutil_StringBuilder_hash_generic(sb);

    /* Assert */
    TEST_ASSERT_TRUE(hash != CUTIL_HASH_C(0));

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_returnZeroHash_when_hashGenericCalledOnEmptyBuilder(void)
{
    /* Arrange */
    cutil_StringBuilder *const sb = cutil_StringBuilder_create();

    /* Act */
    const cutil_hash_t hash = cutil_StringBuilder_hash_generic(sb);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT64(CUTIL_HASH_C(0), hash);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_returnZeroHash_when_hashGenericCalledOnNullBuilder(void)
{
    /* Act */
    const cutil_hash_t hash = cutil_StringBuilder_hash_generic(NULL);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT64(CUTIL_HASH_C(0), hash);
}

static void
_should_writeNullString_when_toStringGenericCalledOnNull(void)
{
    /* Arrange */
    char buf[16];

    /* Act */
    const size_t len
      = cutil_StringBuilder_to_string_generic(NULL, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(4, len);
    TEST_ASSERT_EQUAL_STRING("NULL", buf);
}

static void
_should_returnZero_when_toStringGenericCalledWithTooSmallBuffer(void)
{
    /* Arrange */
    cutil_StringBuilder *const sb
      = cutil_StringBuilder_from_string("hello world");
    char buf[4];

    /* Act */
    const size_t len
      = cutil_StringBuilder_to_string_generic(sb, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0, len);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_writeContent_when_toStringGenericCalledWithAdequateBuffer(void)
{
    /* Arrange */
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string("hello");
    char buf[16];

    /* Act */
    const size_t len
      = cutil_StringBuilder_to_string_generic(sb, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(5, len);
    TEST_ASSERT_EQUAL_STRING("hello", buf);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_clearBuilder_when_clearGenericCalled(void)
{
    /* Arrange */
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string("hello");

    /* Act */
    cutil_StringBuilder_clear_generic(sb);

    /* Assert */
    TEST_ASSERT_NULL(sb->str);
    TEST_ASSERT_EQUAL_size_t(0, sb->length);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
}

static void
_should_copyBuilder_when_copyGenericCalled(void)
{
    /* Arrange */
    cutil_StringBuilder *const src = cutil_StringBuilder_from_string("hello");
    cutil_StringBuilder *const dst = cutil_StringBuilder_create();

    /* Act */
    cutil_StringBuilder_copy_generic(dst, src);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING(
      cutil_StringBuilder_get_string(src), cutil_StringBuilder_get_string(dst)
    );
    TEST_ASSERT_EQUAL_size_t(
      cutil_StringBuilder_length(src), cutil_StringBuilder_length(dst)
    );

    /* Cleanup */
    cutil_StringBuilder_free(src);
    cutil_StringBuilder_free(dst);
}

static void
_should_beValid_when_descriptorIsChecked(void)
{
    /* Act */
    const cutil_GenericType *const type = CUTIL_GENERIC_TYPE_STRING_BUILDER;

    /* Assert */
    TEST_ASSERT_TRUE(cutil_GenericType_is_valid(type));
    TEST_ASSERT_EQUAL_STRING(
      "cutil_StringBuilder", cutil_GenericType_get_name(type)
    );
    TEST_ASSERT_EQUAL_size_t(
      sizeof(cutil_StringBuilder), cutil_GenericType_get_size(type)
    );
    TEST_ASSERT_NOT_NULL(type->init);
    TEST_ASSERT_NOT_NULL(type->clear);
    TEST_ASSERT_NOT_NULL(type->copy);
    TEST_ASSERT_NOT_NULL(type->deep_equals);
    TEST_ASSERT_NOT_NULL(type->comp);
    TEST_ASSERT_NOT_NULL(type->hash);
    TEST_ASSERT_NOT_NULL(type->to_string);
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
    RUN_TEST(_should_preserveNulInvariant_when_constructEmpty);
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
    RUN_TEST(_should_copyStringCorrectly_when_bufferIsLargeEnough);
    RUN_TEST(_should_notCopyString_when_bufferIsTooSmall);
    RUN_TEST(_should_copyStringCorrectly_when_bufferIsNull);
    RUN_TEST(_should_copyStringAndRetainPointer_when_bufferIsLargeEnough);
    RUN_TEST(_should_copyStringCorrectly_when_buflenIsNull);
    RUN_TEST(_should_deleteCorrectly);
    RUN_TEST(_should_deleteCorrectly_when_numLessThanRemainder);
    RUN_TEST(_should_deleteCorrectly_when_boundsExceedSize);
    RUN_TEST(_should_deleteCorrectly_when_useFromTo);
    RUN_TEST(_should_deleteFromToCorrectly_when_boundsExceedSize);

    RUN_TEST(_should_returnTrue_when_deepEqualsGenericCalledOnEqualStrings);
    RUN_TEST(
      _should_returnFalse_when_deepEqualsGenericCalledOnDifferentStrings
    );
    RUN_TEST(_should_returnTrue_when_deepEqualsGenericCalledOnSamePointer);
    RUN_TEST(_should_returnFalse_when_deepEqualsGenericCalledOnNullAndNonNull);
    RUN_TEST(_should_returnTrue_when_deepEqualsGenericCalledOnTwoNulls);
    RUN_TEST(_should_returnZero_when_compareGenericCalledOnEqualStrings);
    RUN_TEST(_should_returnNegative_when_compareGenericCalledOnLesserFirst);
    RUN_TEST(_should_returnPositive_when_compareGenericCalledOnGreaterFirst);
    RUN_TEST(_should_returnNegative_when_compareGenericCalledOnNullFirst);
    RUN_TEST(_should_returnNonZeroHash_when_hashGenericCalledOnNonEmptyBuilder);
    RUN_TEST(_should_returnZeroHash_when_hashGenericCalledOnEmptyBuilder);
    RUN_TEST(_should_returnZeroHash_when_hashGenericCalledOnNullBuilder);
    RUN_TEST(_should_writeNullString_when_toStringGenericCalledOnNull);
    RUN_TEST(_should_returnZero_when_toStringGenericCalledWithTooSmallBuffer);
    RUN_TEST(_should_writeContent_when_toStringGenericCalledWithAdequateBuffer);
    RUN_TEST(_should_clearBuilder_when_clearGenericCalled);
    RUN_TEST(_should_copyBuilder_when_copyGenericCalled);
    RUN_TEST(_should_beValid_when_descriptorIsChecked);

    return UNITY_END();
}

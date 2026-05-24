#include "unity.h"
#include <cutil/core/string/type.h>

#include <cutil/core/status.h>
#include <cutil/core/util/hash.h>
#include <cutil/data/generic/type.h>

static void
test_should_haveNullStr_when_createdFromNullString(void)
{
    /* Act */
    cutil_String *const s = cutil_String_from_string(NULL);

    /* Assert */
    TEST_ASSERT_NULL(s->str);
    TEST_ASSERT_EQUAL_size_t(0, s->length);

    /* Cleanup */
    cutil_String_free(s);
}

static void
test_should_duplicateString_when_createdFromNonNullString(void)
{
    /* Arrange */
    const char input[] = "hello";

    /* Act */
    cutil_String *const s = cutil_String_from_string(input);

    /* Assert */
    TEST_ASSERT_NOT_NULL(s->str);
    TEST_ASSERT_EQUAL_STRING("hello", s->str);
    TEST_ASSERT_EQUAL_size_t(sizeof input - 1UL, s->length);
    TEST_ASSERT_NOT_EQUAL(input, s->str);

    /* Cleanup */
    cutil_String_free(s);
}

static void
test_should_respectMaxlen_when_createdFromNstring(void)
{
    /* Arrange */
    const size_t maxlen = 3;
    const char *const input = "hello";

    /* Act */
    cutil_String *const s = cutil_String_from_nstring(input, maxlen);

    /* Assert */
    TEST_ASSERT_NOT_NULL(s->str);
    TEST_ASSERT_EQUAL_STRING("hel", s->str);
    TEST_ASSERT_EQUAL_size_t(maxlen, s->length);

    /* Cleanup */
    cutil_String_free(s);
}

static void
test_should_setNullAndZeroLength_when_cleared(void)
{
    /* Arrange */
    cutil_String *const s = cutil_String_from_string("hello");

    /* Act */
    cutil_String_clear(s);

    /* Assert */
    TEST_ASSERT_NULL(s->str);
    TEST_ASSERT_EQUAL_size_t(0, s->length);

    /* Cleanup */
    cutil_String_free(s);
}

static void
test_should_copyString_when_copyCalledOnInitializedDst(void)
{
    /* Arrange */
    cutil_String *const src = cutil_String_from_string("hello");
    cutil_String *const dst = cutil_String_from_string("world");

    /* Act */
    cutil_String_copy(dst, src);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("hello", dst->str);
    TEST_ASSERT_EQUAL_size_t(src->length, dst->length);
    TEST_ASSERT_NOT_EQUAL(src->str, dst->str);

    /* Cleanup */
    cutil_String_free(src);
    cutil_String_free(dst);
}

static void
test_should_returnTrue_when_deepEqualsCalledOnEqualStrings(void)
{
    /* Arrange */
    cutil_String *const lhs = cutil_String_from_string("hello");
    cutil_String *const rhs = cutil_String_from_string("hello");

    /* Act */
    const cutil_Bool result = cutil_String_deep_equals(lhs, rhs);

    /* Assert */
    TEST_ASSERT_TRUE(result);

    /* Cleanup */
    cutil_String_free(lhs);
    cutil_String_free(rhs);
}

static void
test_should_returnFalse_when_deepEqualsCalledOnDifferentStrings(void)
{
    /* Arrange */
    cutil_String *const lhs = cutil_String_from_string("hello");
    cutil_String *const rhs = cutil_String_from_string("world");

    /* Act */
    const cutil_Bool result = cutil_String_deep_equals(lhs, rhs);

    /* Assert */
    TEST_ASSERT_FALSE(result);

    /* Cleanup */
    cutil_String_free(lhs);
    cutil_String_free(rhs);
}

static void
test_should_returnZero_when_compareCalledOnEqualStrings(void)
{
    /* Arrange */
    cutil_String *const lhs = cutil_String_from_string("hello");
    cutil_String *const rhs = cutil_String_from_string("hello");

    /* Act */
    const int result = cutil_String_compare(lhs, rhs);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, result);

    /* Cleanup */
    cutil_String_free(lhs);
    cutil_String_free(rhs);
}

static void
test_should_returnNonZeroHash_when_hashCalledOnNonEmpty(void)
{
    /* Arrange */
    cutil_String *const s = cutil_String_from_string("hello");

    /* Act */
    const cutil_hash_t hash = cutil_String_hash(s);

    /* Assert */
    TEST_ASSERT_TRUE(hash != CUTIL_HASH_C(0));

    /* Cleanup */
    cutil_String_free(s);
}

static void
test_should_returnZeroHash_when_hashCalledOnNullStr(void)
{
    /* Arrange */
    cutil_String *const s = cutil_String_from_string(NULL);

    /* Act */
    const cutil_hash_t hash = cutil_String_hash(s);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT64(CUTIL_HASH_C(0), hash);

    /* Cleanup */
    cutil_String_free(s);
}

static void
test_should_writeContent_when_toStringCalledWithAdequateBuffer(void)
{
    /* Arrange */
    const char input[] = "hello";
    cutil_String *const s = cutil_String_from_string(input);
    char buf[16];

    /* Act */
    const size_t len = cutil_String_to_string(s, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(sizeof input - 1UL, len);
    TEST_ASSERT_EQUAL_STRING(input, buf);

    /* Cleanup */
    cutil_String_free(s);
}

static void
test_should_writeNullString_when_toStringCalledOnNullStr(void)
{
    /* Arrange */
    cutil_String *const s = cutil_String_from_string(NULL);
    char buf[16];

    /* Act */
    const size_t len = cutil_String_to_string(s, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(4, len);
    TEST_ASSERT_EQUAL_STRING("NULL", buf);

    /* Cleanup */
    cutil_String_free(s);
}

static void
test_should_returnZero_when_toStringCalledWithTooSmallBuffer(void)
{
    /* Arrange */
    cutil_String *const s = cutil_String_from_string("hello world");
    char buf[4];

    /* Act */
    const size_t len = cutil_String_to_string(s, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0, len);

    /* Cleanup */
    cutil_String_free(s);
}

static void
test_should_clearViaGeneric_when_clearGenericCalled(void)
{
    /* Arrange */
    cutil_String *const s = cutil_String_from_string("hello");

    /* Act */
    cutil_String_clear_generic(s);

    /* Assert */
    TEST_ASSERT_NULL(s->str);
    TEST_ASSERT_EQUAL_size_t(0, s->length);

    /* Cleanup */
    cutil_String_free(s);
}

static void
test_should_copyViaGeneric_when_copyGenericCalled(void)
{
    /* Arrange */
    cutil_String *const src = cutil_String_from_string("hello");
    cutil_String *const dst = cutil_String_from_string("world");

    /* Act */
    cutil_String_copy_generic(dst, src);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("hello", dst->str);
    TEST_ASSERT_EQUAL_size_t(src->length, dst->length);

    /* Cleanup */
    cutil_String_free(src);
    cutil_String_free(dst);
}

static void
test_should_matchDeepEqualsGeneric_when_calledWithSameArgs(void)
{
    /* Arrange */
    cutil_String *const lhs = cutil_String_from_string("hello");
    cutil_String *const rhs = cutil_String_from_string("hello");

    /* Act */
    const cutil_Bool typed = cutil_String_deep_equals(lhs, rhs);
    const cutil_Bool generic = cutil_String_deep_equals_generic(lhs, rhs);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(typed, generic);

    /* Cleanup */
    cutil_String_free(lhs);
    cutil_String_free(rhs);
}

static void
test_should_matchCompareGeneric_when_calledWithSameArgs(void)
{
    /* Arrange */
    cutil_String *const lhs = cutil_String_from_string("abc");
    cutil_String *const rhs = cutil_String_from_string("xyz");

    /* Act */
    const int typed = cutil_String_compare(lhs, rhs);
    const int generic = cutil_String_compare_generic(lhs, rhs);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(typed, generic);

    /* Cleanup */
    cutil_String_free(lhs);
    cutil_String_free(rhs);
}

/* ========================================================================= */
/* cutil_StringView tests                                                    */
/* ========================================================================= */

static void
test_should_haveNullStr_when_viewCreatedFromNullString(void)
{
    /* Act */
    cutil_StringView *const sv = cutil_StringView_from_string(NULL);

    /* Assert */
    TEST_ASSERT_NULL(sv->str);
    TEST_ASSERT_EQUAL_size_t(0, sv->length);

    /* Cleanup */
    cutil_StringView_free(sv);
}

static void
test_should_storePointer_when_viewCreatedFromNonNullString(void)
{
    /* Arrange */
    const char input[] = "hello";

    /* Act */
    cutil_StringView *const sv = cutil_StringView_from_string(input);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(input, sv->str);
    TEST_ASSERT_EQUAL_size_t(sizeof input - 1UL, sv->length);

    /* Cleanup */
    cutil_StringView_free(sv);
}

static void
test_should_respectMaxlen_when_viewCreatedFromNstring(void)
{
    /* Arrange */
    const char *const input = "hello";
    const size_t maxlen = 3;

    /* Act */
    cutil_StringView *const sv = cutil_StringView_from_nstring(input, maxlen);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(input, sv->str);
    TEST_ASSERT_EQUAL_size_t(maxlen, sv->length);

    /* Cleanup */
    cutil_StringView_free(sv);
}

static void
test_should_setNullAndZeroLength_when_viewCleared(void)
{
    /* Arrange */
    cutil_StringView *const sv = cutil_StringView_from_string("hello");

    /* Act */
    cutil_StringView_clear(sv);

    /* Assert */
    TEST_ASSERT_NULL(sv->str);
    TEST_ASSERT_EQUAL_size_t(0, sv->length);

    /* Cleanup */
    cutil_StringView_free(sv);
}

static void
test_should_shallowCopy_when_viewCopyCalled(void)
{
    /* Arrange */
    const char *const input = "hello";
    cutil_StringView *const src = cutil_StringView_from_string(input);
    cutil_StringView *const dst = cutil_StringView_from_string("world");

    /* Act */
    cutil_StringView_copy(dst, src);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(src->str, dst->str);
    TEST_ASSERT_EQUAL_size_t(src->length, dst->length);

    /* Cleanup */
    cutil_StringView_free(src);
    cutil_StringView_free(dst);
}

static void
test_should_returnTrue_when_viewDeepEqualsCalledOnEqualStrings(void)
{
    /* Arrange */
    cutil_StringView *const lhs = cutil_StringView_from_string("hello");
    cutil_StringView *const rhs = cutil_StringView_from_string("hello");

    /* Act */
    const cutil_Bool result = cutil_StringView_deep_equals(lhs, rhs);

    /* Assert */
    TEST_ASSERT_TRUE(result);

    /* Cleanup */
    cutil_StringView_free(lhs);
    cutil_StringView_free(rhs);
}

static void
test_should_returnFalse_when_viewDeepEqualsCalledOnDifferentStrings(void)
{
    /* Arrange */
    cutil_StringView *const lhs = cutil_StringView_from_string("hello");
    cutil_StringView *const rhs = cutil_StringView_from_string("world");

    /* Act */
    const cutil_Bool result = cutil_StringView_deep_equals(lhs, rhs);

    /* Assert */
    TEST_ASSERT_FALSE(result);

    /* Cleanup */
    cutil_StringView_free(lhs);
    cutil_StringView_free(rhs);
}

static void
test_should_returnZero_when_viewCompareCalledOnEqualStrings(void)
{
    /* Arrange */
    cutil_StringView *const lhs = cutil_StringView_from_string("hello");
    cutil_StringView *const rhs = cutil_StringView_from_string("hello");

    /* Act */
    const int result = cutil_StringView_compare(lhs, rhs);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, result);

    /* Cleanup */
    cutil_StringView_free(lhs);
    cutil_StringView_free(rhs);
}

static void
test_should_returnNonZeroHash_when_viewHashCalledOnNonEmpty(void)
{
    /* Arrange */
    cutil_StringView *const sv = cutil_StringView_from_string("hello");

    /* Act */
    const cutil_hash_t hash = cutil_StringView_hash(sv);

    /* Assert */
    TEST_ASSERT_TRUE(hash != CUTIL_HASH_C(0));

    /* Cleanup */
    cutil_StringView_free(sv);
}

static void
test_should_returnZeroHash_when_viewHashCalledOnNullStr(void)
{
    /* Arrange */
    cutil_StringView *const sv = cutil_StringView_from_string(NULL);

    /* Act */
    const cutil_hash_t hash = cutil_StringView_hash(sv);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT64(CUTIL_HASH_C(0), hash);

    /* Cleanup */
    cutil_StringView_free(sv);
}

static void
test_should_writeContent_when_viewToStringCalledWithAdequateBuffer(void)
{
    /* Arrange */
    const char input[] = "hello";
    cutil_StringView *const sv = cutil_StringView_from_string(input);
    char buf[16];

    /* Act */
    const size_t len = cutil_StringView_to_string(sv, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(sizeof input - 1UL, len);
    TEST_ASSERT_EQUAL_STRING(input, buf);

    /* Cleanup */
    cutil_StringView_free(sv);
}

static void
test_should_writeNullString_when_viewToStringCalledOnNullStr(void)
{
    /* Arrange */
    cutil_StringView *const sv = cutil_StringView_from_string(NULL);
    char buf[16];

    /* Act */
    const size_t len = cutil_StringView_to_string(sv, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(4, len);
    TEST_ASSERT_EQUAL_STRING("NULL", buf);

    /* Cleanup */
    cutil_StringView_free(sv);
}

static void
test_should_returnZero_when_viewToStringCalledWithTooSmallBuffer(void)
{
    /* Arrange */
    cutil_StringView *const sv = cutil_StringView_from_string("hello world");
    char buf[4];

    /* Act */
    const size_t len = cutil_StringView_to_string(sv, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(CUTIL_ERROR_SIZE, len);

    /* Cleanup */
    cutil_StringView_free(sv);
}

static void
test_should_respectViewLength_when_viewToStringCalledAfterNstring(void)
{
    /* Arrange */
    const char *const input = "hello";
    const size_t maxlen = 3;
    cutil_StringView *const sv = cutil_StringView_from_nstring(input, maxlen);
    char buf[16];

    /* Act */
    const size_t len = cutil_StringView_to_string(sv, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(maxlen, len);
    TEST_ASSERT_EQUAL_STRING("hel", buf);

    /* Cleanup */
    cutil_StringView_free(sv);
}

static void
test_should_clearViaGeneric_when_viewClearGenericCalled(void)
{
    /* Arrange */
    cutil_StringView *const sv = cutil_StringView_from_string("hello");

    /* Act */
    cutil_StringView_clear_generic(sv);

    /* Assert */
    TEST_ASSERT_NULL(sv->str);
    TEST_ASSERT_EQUAL_size_t(0, sv->length);

    /* Cleanup */
    cutil_StringView_free(sv);
}

static void
test_should_copyViaGeneric_when_viewCopyGenericCalled(void)
{
    /* Arrange */
    const char *const input = "hello";
    cutil_StringView *const src = cutil_StringView_from_string(input);
    cutil_StringView *const dst = cutil_StringView_from_string("world");

    /* Act */
    cutil_StringView_copy_generic(dst, src);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(input, dst->str);
    TEST_ASSERT_EQUAL_size_t(src->length, dst->length);

    /* Cleanup */
    cutil_StringView_free(src);
    cutil_StringView_free(dst);
}

static void
test_should_matchDeepEqualsGeneric_when_viewCalledWithSameArgs(void)
{
    /* Arrange */
    cutil_StringView *const lhs = cutil_StringView_from_string("hello");
    cutil_StringView *const rhs = cutil_StringView_from_string("hello");

    /* Act */
    const cutil_Bool typed = cutil_StringView_deep_equals(lhs, rhs);
    const cutil_Bool generic = cutil_StringView_deep_equals_generic(lhs, rhs);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(typed, generic);

    /* Cleanup */
    cutil_StringView_free(lhs);
    cutil_StringView_free(rhs);
}

static void
test_should_matchCompareGeneric_when_viewCalledWithSameArgs(void)
{
    /* Arrange */
    cutil_StringView *const lhs = cutil_StringView_from_string("abc");
    cutil_StringView *const rhs = cutil_StringView_from_string("xyz");

    /* Act */
    const int typed = cutil_StringView_compare(lhs, rhs);
    const int generic = cutil_StringView_compare_generic(lhs, rhs);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(typed, generic);

    /* Cleanup */
    cutil_StringView_free(lhs);
    cutil_StringView_free(rhs);
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

    /* cutil_String */
    RUN_TEST(test_should_haveNullStr_when_createdFromNullString);
    RUN_TEST(test_should_duplicateString_when_createdFromNonNullString);
    RUN_TEST(test_should_respectMaxlen_when_createdFromNstring);
    RUN_TEST(test_should_setNullAndZeroLength_when_cleared);
    RUN_TEST(test_should_copyString_when_copyCalledOnInitializedDst);
    RUN_TEST(test_should_returnTrue_when_deepEqualsCalledOnEqualStrings);
    RUN_TEST(test_should_returnFalse_when_deepEqualsCalledOnDifferentStrings);
    RUN_TEST(test_should_returnZero_when_compareCalledOnEqualStrings);
    RUN_TEST(test_should_returnNonZeroHash_when_hashCalledOnNonEmpty);
    RUN_TEST(test_should_returnZeroHash_when_hashCalledOnNullStr);
    RUN_TEST(test_should_writeContent_when_toStringCalledWithAdequateBuffer);
    RUN_TEST(test_should_writeNullString_when_toStringCalledOnNullStr);
    RUN_TEST(test_should_returnZero_when_toStringCalledWithTooSmallBuffer);
    RUN_TEST(test_should_clearViaGeneric_when_clearGenericCalled);
    RUN_TEST(test_should_copyViaGeneric_when_copyGenericCalled);
    RUN_TEST(test_should_matchDeepEqualsGeneric_when_calledWithSameArgs);
    RUN_TEST(test_should_matchCompareGeneric_when_calledWithSameArgs);

    /* cutil_StringView */
    RUN_TEST(test_should_haveNullStr_when_viewCreatedFromNullString);
    RUN_TEST(test_should_storePointer_when_viewCreatedFromNonNullString);
    RUN_TEST(test_should_respectMaxlen_when_viewCreatedFromNstring);
    RUN_TEST(test_should_setNullAndZeroLength_when_viewCleared);
    RUN_TEST(test_should_shallowCopy_when_viewCopyCalled);
    RUN_TEST(test_should_returnTrue_when_viewDeepEqualsCalledOnEqualStrings);
    RUN_TEST(test_should_returnFalse_when_viewDeepEqualsCalledOnDifferentStrings);
    RUN_TEST(test_should_returnZero_when_viewCompareCalledOnEqualStrings);
    RUN_TEST(test_should_returnNonZeroHash_when_viewHashCalledOnNonEmpty);
    RUN_TEST(test_should_returnZeroHash_when_viewHashCalledOnNullStr);
    RUN_TEST(test_should_writeContent_when_viewToStringCalledWithAdequateBuffer);
    RUN_TEST(test_should_writeNullString_when_viewToStringCalledOnNullStr);
    RUN_TEST(test_should_returnZero_when_viewToStringCalledWithTooSmallBuffer);
    RUN_TEST(test_should_respectViewLength_when_viewToStringCalledAfterNstring);
    RUN_TEST(test_should_clearViaGeneric_when_viewClearGenericCalled);
    RUN_TEST(test_should_copyViaGeneric_when_viewCopyGenericCalled);
    RUN_TEST(test_should_matchDeepEqualsGeneric_when_viewCalledWithSameArgs);
    RUN_TEST(test_should_matchCompareGeneric_when_viewCalledWithSameArgs);

    return UNITY_END();
}

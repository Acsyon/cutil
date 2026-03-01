#include "unity.h"
#include <cutil/data/native/bitarray.h>

#include <cutil/util/macro.h>

static void
_should_constructArray_when_provideSizeAndCapacity(void)
{
    /* Arrange */
    const size_t CAPACITY[] = {1, 5, 10, 50, 100};
    const size_t NUM_CAPACITIES = CUTIL_GET_NATIVE_ARRAY_SIZE(CAPACITY);

    for (size_t i_capacity = 0; i_capacity < NUM_CAPACITIES; ++i_capacity) {
        const size_t capacity = CAPACITY[i_capacity];

        /* Act */
        cutil_BitArray *const arr = cutil_BitArray_alloc(capacity);

        /* Assert */
        TEST_ASSERT_NOT_NULL(arr);
        TEST_ASSERT_GREATER_OR_EQUAL_size_t(
          capacity, cutil_BitArray_get_capacity(arr)
        );
        TEST_ASSERT_NOT_NULL(arr->data);

        /* Cleanup */
        cutil_BitArray_free(arr);
    }
}

static void
_should_clearArray_when_callClearArray(void)
{
    /* Arrange */
    const size_t CAPACITY[] = {1, 5, 10, 50, 100};
    const size_t NUM_CAPACITIES = CUTIL_GET_NATIVE_ARRAY_SIZE(CAPACITY);

    for (size_t i_capacity = 0; i_capacity < NUM_CAPACITIES; ++i_capacity) {
        const size_t capacity = CAPACITY[i_capacity];
        cutil_BitArray *const arr = cutil_BitArray_alloc(capacity);

        /* Act */
        cutil_BitArray_clear(arr);

        /* Assert */
        TEST_ASSERT_NOT_NULL(arr);
        TEST_ASSERT_EQUAL_size_t(0UL, cutil_BitArray_get_capacity(arr));
        TEST_ASSERT_NULL(arr->data);

        /* Cleanup */
        cutil_BitArray_free(arr);
    }
}

static void
_should_setAndGetIndividualElements(void)
{
    /* Arrange */
    const size_t capacity = 128;
    cutil_BitArray *const arr = cutil_BitArray_alloc(capacity);
    TEST_ASSERT_EQUAL_size_t(capacity, cutil_BitArray_get_capacity(arr));

    for (size_t i = 0; i < capacity; ++i) {
        /* Act */
        /* Assert */
        TEST_ASSERT_EQUAL_CHAR(0, cutil_BitArray_get(arr, i));
        cutil_BitArray_set(arr, i);
        TEST_ASSERT_NOT_EQUAL_CHAR(0, cutil_BitArray_get(arr, i));
        cutil_BitArray_unset(arr, i);
        TEST_ASSERT_EQUAL_CHAR(0, cutil_BitArray_get(arr, i));
    }

    /* Cleanup */
    cutil_BitArray_free(arr);
}

static void
_should_copyArrayCorrectly(void)
{
    /* Arrange */
    const size_t capacity = 8;
    cutil_BitArray *const src = cutil_BitArray_alloc(capacity);
    cutil_BitArray *const dst = cutil_BitArray_alloc(capacity);

    for (size_t i = 0; i < capacity; i += 2) {
        cutil_BitArray_set(src, i);
    }

    /* Act */
    cutil_BitArray_copy(dst, src);

    /* Assert */
    for (size_t i = 0; i < capacity; ++i) {
        const char src_val = cutil_BitArray_get(src, i);
        const char dst_val = cutil_BitArray_get(dst, i);
        TEST_ASSERT_EQUAL_CHAR(src_val, dst_val);
    }

    /* Cleanup */
    cutil_BitArray_free(src);
    cutil_BitArray_free(dst);
}

static void
_should_duplicateArrayCorrectly(void)
{
    /* Arrange */
    const size_t capacity = 24;
    cutil_BitArray *const orig = cutil_BitArray_alloc(capacity);

    for (size_t i = 0; i < capacity; i += 2) {
        cutil_BitArray_set(orig, i);
    }

    /* Act */
    cutil_BitArray *const dup = cutil_BitArray_duplicate(orig);

    /* Assert */
    TEST_ASSERT_NOT_NULL(dup);
    TEST_ASSERT_NOT_EQUAL(orig, dup);
    TEST_ASSERT_EQUAL_size_t(
      cutil_BitArray_get_capacity(orig), cutil_BitArray_get_capacity(dup)
    );

    for (size_t i = 0; i < capacity; ++i) {
        const unsigned char orig_val = cutil_BitArray_get(orig, i);
        const unsigned char dup_val = cutil_BitArray_get(dup, i);
        TEST_ASSERT_EQUAL_CHAR(orig_val, dup_val);
    }

    /* Cleanup */
    cutil_BitArray_free(orig);
    cutil_BitArray_free(dup);
}

static void
_should_resizeArrayCorrectly(void)
{
    /* Arrange */
    const size_t initial_capacity = 25;
    const size_t new_capacity = 45;
    cutil_BitArray *const arr = cutil_BitArray_alloc(initial_capacity);

    unsigned char *const assert_arr
      = calloc(initial_capacity, sizeof *assert_arr);

    for (size_t i = 1; i < initial_capacity; i += 2) {
        cutil_BitArray_set(arr, i);
        assert_arr[i] = 1;
    }

    /* Act */
    cutil_BitArray_resize(arr, new_capacity);

    /* Assert */
    TEST_ASSERT_GREATER_OR_EQUAL_size_t(
      new_capacity, cutil_BitArray_get_capacity(arr)
    );
    TEST_ASSERT_NOT_NULL(arr->data);

    for (size_t i = 0; i < initial_capacity; ++i) {
        TEST_ASSERT_EQUAL_CHAR(assert_arr[i], !!cutil_BitArray_get(arr, i));
    }
    for (size_t i = initial_capacity; i < new_capacity; ++i) {
        TEST_ASSERT_EQUAL_CHAR(0, cutil_BitArray_get(arr, i));
    }

    cutil_BitArray_set(arr, new_capacity - 1);
    TEST_ASSERT_NOT_EQUAL_CHAR(0, cutil_BitArray_get(arr, new_capacity - 1));

    /* Cleanup */
    cutil_BitArray_free(arr);
    free(assert_arr);
}

static void
_should_handleEdgeCases(void)
{
    /* Test with zero capacity */
    {
        cutil_BitArray *const arr = cutil_BitArray_alloc(0);
        TEST_ASSERT_NOT_NULL(arr);
        TEST_ASSERT_EQUAL_size_t(0, cutil_BitArray_get_capacity(arr));
        TEST_ASSERT_NULL(arr->data);
        cutil_BitArray_free(arr);
    }

    /* Test resize to zero */
    {
        cutil_BitArray *const arr = cutil_BitArray_alloc(5);
        cutil_BitArray_resize(arr, 0);
        TEST_ASSERT_EQUAL_size_t(0, cutil_BitArray_get_capacity(arr));
        TEST_ASSERT_NULL(arr->data);
        cutil_BitArray_free(arr);
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

    RUN_TEST(_should_constructArray_when_provideSizeAndCapacity);
    RUN_TEST(_should_clearArray_when_callClearArray);
    RUN_TEST(_should_setAndGetIndividualElements);
    RUN_TEST(_should_copyArrayCorrectly);
    RUN_TEST(_should_duplicateArrayCorrectly);
    RUN_TEST(_should_resizeArrayCorrectly);
    RUN_TEST(_should_handleEdgeCases);

    return UNITY_END();
}

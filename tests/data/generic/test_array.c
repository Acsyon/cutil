#include "unity.h"

#include <cutil/data/generic/array.h>
#include <cutil/util/hash.h>
#include <cutil/util/macro.h>

#include <stdio.h>

static void
_should_constructArray_when_provideSizeAndCapacity(void)
{
    /* Arrange */
    const size_t SIZES[] = {1, 5, 10, 50, 100};
    const size_t NUM_SIZES = CUTIL_GET_NATIVE_ARRAY_SIZE(SIZES);
    const size_t CAPACITIES[] = {1, 5, 10, 50, 100};
    const size_t NUM_CAPACITIES = CUTIL_GET_NATIVE_ARRAY_SIZE(CAPACITIES);

    for (size_t i_size = 0; i_size < NUM_SIZES; ++i_size) {
        for (size_t i_capacity = 0; i_capacity < NUM_CAPACITIES; ++i_capacity) {
            const size_t size = SIZES[i_size];
            const size_t capacity = CAPACITIES[i_capacity];
            const cutil_GenericType type = {.name = "name", .size = size};

            /* Act */
            cutil_Array *const arr = cutil_Array_alloc(&type, capacity);

            /* Assert */
            TEST_ASSERT_NOT_NULL(arr);
            TEST_ASSERT_EQUAL_size_t(size, cutil_Array_get_size(arr));
            TEST_ASSERT_EQUAL_size_t(capacity, cutil_Array_get_capacity(arr));
            TEST_ASSERT_NOT_NULL(arr->data);

            /* Cleanup */
            cutil_Array_free(arr);
        }
    }
}

static void
_should_clearArray_when_callClearArray(void)
{
    /* Arrange */
    const size_t SIZES[] = {1, 5, 10, 50, 100};
    const size_t NUM_SIZES = CUTIL_GET_NATIVE_ARRAY_SIZE(SIZES);
    const size_t CAPACITIES[] = {1, 5, 10, 50, 100};
    const size_t NUM_CAPACITIES = CUTIL_GET_NATIVE_ARRAY_SIZE(CAPACITIES);

    for (size_t i_size = 0; i_size < NUM_SIZES; ++i_size) {
        for (size_t i_capacity = 0; i_capacity < NUM_CAPACITIES; ++i_capacity) {
            const size_t size = SIZES[i_size];
            const size_t capacity = CAPACITIES[i_capacity];
            const cutil_GenericType type = {.name = "name", .size = size};
            cutil_Array *const arr = cutil_Array_alloc(&type, capacity);

            /* Act */
            cutil_Array_clear(arr);

            /* Assert */
            TEST_ASSERT_NOT_NULL(arr);
            TEST_ASSERT_EQUAL_size_t(size, cutil_Array_get_size(arr));
            TEST_ASSERT_EQUAL_size_t(0UL, cutil_Array_get_capacity(arr));
            TEST_ASSERT_NULL(arr->data);

            /* Cleanup */
            cutil_Array_free(arr);
        }
    }
}

static void
_should_setMultCorrectly(void)
{
    /* Arrange */
    const size_t SIZES[] = {sizeof(int), sizeof(double), 16};
    const size_t NUM_SIZES = CUTIL_GET_NATIVE_ARRAY_SIZE(SIZES);
    const size_t CAPACITIES[] = {5, 10, 20};
    const size_t NUM_CAPACITIES = CUTIL_GET_NATIVE_ARRAY_SIZE(CAPACITIES);

    for (size_t i_size = 0; i_size < NUM_SIZES; ++i_size) {
        for (size_t i_capacity = 0; i_capacity < NUM_CAPACITIES; ++i_capacity) {
            const size_t size = SIZES[i_size];
            const size_t capacity = CAPACITIES[i_capacity];
            const cutil_GenericType type = {.name = "name", .size = size};

            for (size_t idx = 0; idx < capacity - 2; ++idx) {
                for (size_t num = 1; num <= 3 && (idx + num) <= capacity; ++num)
                {
                    char *const test_data = malloc(size * num);
                    char *const verify_data = malloc(size * num);

                    for (size_t i = 0; i < size * num; ++i) {
                        test_data[i] = (char) (i % 256);
                        verify_data[i] = 0;
                    }

                    cutil_Array *const arr = cutil_Array_alloc(&type, capacity);

                    /* Act */
                    cutil_Array_set_mult(arr, idx, num, test_data);

                    /* Assert */
                    cutil_Array_get_mult(arr, idx, num, verify_data);
                    TEST_ASSERT_EQUAL_MEMORY(
                      test_data, verify_data, size * num
                    );

                    /* Cleanup */
                    cutil_Array_free(arr);
                    free(test_data);
                    free(verify_data);
                }
            }
        }
    }
}

static void
_should_setAndGetIndividualElements(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(int)};
    const size_t capacity = 10;
    cutil_Array *const arr = cutil_Array_alloc(&type, capacity);

    for (size_t i = 0; i < capacity; ++i) {
        int value = i * 10;
        int result = 0;

        /* Act */
        cutil_Array_set(arr, i, &value);
        cutil_Array_get(arr, i, &result);

        /* Assert */
        TEST_ASSERT_EQUAL_INT(value, result);
    }

    /* Cleanup */
    cutil_Array_free(arr);
}

static void
_should_copyArrayCorrectly(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(double)};
    const size_t capacity = 8;
    cutil_Array *const src = cutil_Array_alloc(&type, capacity);
    cutil_Array *const dst = cutil_Array_alloc(&type, capacity);

    for (size_t i = 0; i < capacity; ++i) {
        const double value = i * 1.5;
        cutil_Array_set(src, i, &value);
    }

    /* Act */
    cutil_Array_copy(dst, src);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(
      cutil_Array_get_size(src), cutil_Array_get_size(dst)
    );
    TEST_ASSERT_EQUAL_size_t(
      cutil_Array_get_capacity(src), cutil_Array_get_capacity(dst)
    );

    for (size_t i = 0; i < capacity; ++i) {
        double src_val, dst_val;
        cutil_Array_get(src, i, &src_val);
        cutil_Array_get(dst, i, &dst_val);
        TEST_ASSERT_EQUAL_FLOAT(src_val, dst_val);
    }

    /* Cleanup */
    cutil_Array_free(src);
    cutil_Array_free(dst);
}

static void
_should_duplicateArrayCorrectly(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(char)};
    const size_t capacity = 20;
    cutil_Array *const orig = cutil_Array_alloc(&type, capacity);

    for (size_t i = 0; i < capacity; ++i) {
        const char value = 'A' + (char) (i % 26);
        cutil_Array_set(orig, i, &value);
    }

    /* Act */
    cutil_Array *const dup = cutil_Array_duplicate(orig);

    /* Assert */
    TEST_ASSERT_NOT_NULL(dup);
    TEST_ASSERT_NOT_EQUAL(orig, dup);
    TEST_ASSERT_EQUAL_size_t(
      cutil_Array_get_size(orig), cutil_Array_get_size(dup)
    );
    TEST_ASSERT_EQUAL_size_t(
      cutil_Array_get_capacity(orig), cutil_Array_get_capacity(dup)
    );

    for (size_t i = 0; i < capacity; ++i) {
        char orig_val, dup_val;
        cutil_Array_get(orig, i, &orig_val);
        cutil_Array_get(dup, i, &dup_val);
        TEST_ASSERT_EQUAL_CHAR(orig_val, dup_val);
    }

    /* Cleanup */
    cutil_Array_free(orig);
    cutil_Array_free(dup);
}

static void
_should_resizeArrayCorrectly(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(float)};
    const size_t initial_capacity = 5;
    const size_t new_capacity = 15;
    cutil_Array *const arr = cutil_Array_alloc(&type, initial_capacity);

    for (size_t i = 0; i < initial_capacity; ++i) {
        const float value = i * 2.0f;
        cutil_Array_set(arr, i, &value);
    }

    /* Act */
    cutil_Array_resize(arr, new_capacity);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(type.size, cutil_Array_get_size(arr));
    TEST_ASSERT_EQUAL_size_t(new_capacity, cutil_Array_get_capacity(arr));
    TEST_ASSERT_NOT_NULL(arr->data);

    for (size_t i = 0; i < initial_capacity; ++i) {
        float expected = i * 2.0f;
        float actual;
        cutil_Array_get(arr, i, &actual);
        TEST_ASSERT_EQUAL_FLOAT(expected, actual);
    }

    float new_value = 99.9f;
    cutil_Array_set(arr, new_capacity - 1, &new_value);
    float verify_value;
    cutil_Array_get(arr, new_capacity - 1, &verify_value);
    TEST_ASSERT_EQUAL_FLOAT(new_value, verify_value);

    /* Cleanup */
    cutil_Array_free(arr);
}

static void
_should_handleEdgeCases(void)
{
    /* Test with NULL type */
    {
        cutil_Array *const arr = cutil_Array_alloc(NULL, 10);
        TEST_ASSERT_NULL(arr);
    }

    /* Test with zero size element */
    {
        const cutil_GenericType type = {.name = "name", .size = 0};
        cutil_Array *const arr = cutil_Array_alloc(&type, 10);
        TEST_ASSERT_NULL(arr);
    }

    /* Test with NULL name */
    {
        const cutil_GenericType type = {.size = 10};
        cutil_Array *const arr = cutil_Array_alloc(&type, 10);
        TEST_ASSERT_NULL(arr);
    }

    /* Test with zero capacity */
    {
        const cutil_GenericType type = {.name = "name", .size = sizeof(int)};
        cutil_Array *const arr = cutil_Array_alloc(&type, 0);
        TEST_ASSERT_NOT_NULL(arr);
        TEST_ASSERT_EQUAL_size_t(0, cutil_Array_get_capacity(arr));
        TEST_ASSERT_NULL(arr->data);
        cutil_Array_free(arr);
    }

    /* Test resize to zero */
    {
        const cutil_GenericType type = {.name = "name", .size = sizeof(double)};
        cutil_Array *const arr = cutil_Array_alloc(&type, 5);
        cutil_Array_resize(arr, 0);
        TEST_ASSERT_EQUAL_size_t(0, cutil_Array_get_capacity(arr));
        TEST_ASSERT_NULL(arr->data);
        cutil_Array_free(arr);
    }
}

static void
_should_getPointerCorrectly(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(long)};
    const size_t capacity = 7;
    cutil_Array *const arr = cutil_Array_alloc(&type, capacity);

    for (size_t i = 0; i < capacity; ++i) {
        const long value = i * 100L;
        cutil_Array_set(arr, i, &value);
    }

    for (size_t i = 0; i < capacity; ++i) {
        /* Act */
        const void *ptr = cutil_Array_get_ptr(arr, i);

        /* Assert */
        TEST_ASSERT_NOT_NULL(ptr);

        const char *const expected
          = cutil_void_array_get_elem_const(type.size, arr->data, i);
        TEST_ASSERT_EQUAL_PTR(expected, ptr);

        long expected_value = i * 100L;
        long actual_value = *(const long *) ptr;
        TEST_ASSERT_EQUAL_INT64(expected_value, actual_value);
    }

    /* Cleanup */
    cutil_Array_free(arr);
}

static inline size_t
_int_to_string(const void *data, char *buf, size_t buflen)
{
    return CUTIL_GENERIC_TYPE_INT->to_string(data, buf, buflen);
}

static void
_should_returnTrue_when_bothArraysNull(void)
{
    /* Act */
    /* Assert */
    TEST_ASSERT_TRUE(cutil_Array_deep_equals(NULL, NULL));
}

static void
_should_returnFalse_when_oneArrayIsNull(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(int)};
    cutil_Array *const arr = cutil_Array_alloc(&type, 3);

    /* Act */
    /* Assert */
    TEST_ASSERT_FALSE(cutil_Array_deep_equals(arr, NULL));
    TEST_ASSERT_FALSE(cutil_Array_deep_equals(NULL, arr));

    /* Cleanup */
    cutil_Array_free(arr);
}

static void
_should_returnTrue_when_samePointer(void)
{
    /* Arrange*/
    const cutil_GenericType type = {.name = "name", .size = sizeof(int)};
    cutil_Array *const arr = cutil_Array_alloc(&type, 3);

    /* Act */
    /* Assert */
    TEST_ASSERT_TRUE(cutil_Array_deep_equals(arr, arr));

    /* Cleanup */
    cutil_Array_free(arr);
}

static void
_should_returnTrue_when_arraysHaveSameContent(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(int)};
    const size_t capacity = 5;
    cutil_Array *const lhs = cutil_Array_alloc(&type, capacity);
    cutil_Array *const rhs = cutil_Array_alloc(&type, capacity);

    /* Act */
    for (size_t i = 0; i < capacity; ++i) {
        const int val = (int) (i * 3);
        cutil_Array_set(lhs, i, &val);
        cutil_Array_set(rhs, i, &val);
    }

    /* Assert */
    TEST_ASSERT_TRUE(cutil_Array_deep_equals(lhs, rhs));

    /* Cleanup */
    cutil_Array_free(lhs);
    cutil_Array_free(rhs);
}

static void
_should_returnFalse_when_arraysHaveDifferentData(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(int)};
    const size_t capacity = 4;
    cutil_Array *const lhs = cutil_Array_alloc(&type, capacity);
    cutil_Array *const rhs = cutil_Array_alloc(&type, capacity);

    /* Act */
    for (size_t i = 0; i < capacity; ++i) {
        const int lval = (int) i;
        const int rval = (int) (i + 1);
        cutil_Array_set(lhs, i, &lval);
        cutil_Array_set(rhs, i, &rval);
    }

    /* Assert */
    TEST_ASSERT_FALSE(cutil_Array_deep_equals(lhs, rhs));

    /* Cleanup */
    cutil_Array_free(lhs);
    cutil_Array_free(rhs);
}

static void
_should_returnFalse_when_arraysHaveDifferentCapacity(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(int)};
    cutil_Array *const lhs = cutil_Array_alloc(&type, 3);
    cutil_Array *const rhs = cutil_Array_alloc(&type, 5);

    /* Act */
    /* Assert */
    TEST_ASSERT_FALSE(cutil_Array_deep_equals(lhs, rhs));

    /* Cleanup */
    cutil_Array_free(lhs);
    cutil_Array_free(rhs);
}

static void
_should_returnFalse_when_arraysHaveDifferentTypes(void)
{
    /* Arrange */
    const cutil_GenericType type_int = {.name = "int", .size = sizeof(int)};
    const cutil_GenericType type_long = {.name = "long", .size = sizeof(long)};
    cutil_Array *const lhs = cutil_Array_alloc(&type_int, 3);
    cutil_Array *const rhs = cutil_Array_alloc(&type_long, 3);

    /* Act */
    /* Assert */
    TEST_ASSERT_FALSE(cutil_Array_deep_equals(lhs, rhs));

    /* Cleanup */
    cutil_Array_free(lhs);
    cutil_Array_free(rhs);
}

static void
_should_compareReturnZero_when_samePointer(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(int)};
    cutil_Array *const arr = cutil_Array_alloc(&type, 3);

    /* Act */
    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, cutil_Array_compare(arr, arr));

    /* Cleanup */
    cutil_Array_free(arr);
}

static void
_should_compareReturnZero_when_bothNull(void)
{
    /* Act */
    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, cutil_Array_compare(NULL, NULL));
}

static void
_should_compareReturnNegative_when_lhsIsNull(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(int)};
    cutil_Array *const arr = cutil_Array_alloc(&type, 3);

    /* Act */
    /* Assert */
    TEST_ASSERT_LESS_THAN_INT(0, cutil_Array_compare(NULL, arr));

    /* Cleanup */
    cutil_Array_free(arr);
}

static void
_should_compareReturnPositive_when_rhsIsNull(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(int)};
    cutil_Array *const arr = cutil_Array_alloc(&type, 3);

    /* Act */
    /* Assert */
    TEST_ASSERT_GREATER_THAN_INT(0, cutil_Array_compare(arr, NULL));

    /* Cleanup */
    cutil_Array_free(arr);
}

static void
_should_compareReturnZero_when_arraysHaveSameContent(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(int)};
    const size_t capacity = 5;
    cutil_Array *const lhs = cutil_Array_alloc(&type, capacity);
    cutil_Array *const rhs = cutil_Array_alloc(&type, capacity);

    for (size_t i = 0; i < capacity; ++i) {
        const int val = (int) (i * 7);
        cutil_Array_set(lhs, i, &val);
        cutil_Array_set(rhs, i, &val);
    }

    /* Act */
    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, cutil_Array_compare(lhs, rhs));

    /* Cleanup */
    cutil_Array_free(lhs);
    cutil_Array_free(rhs);
}

static void
_should_compareReturnNonZero_when_arraysHaveDifferentData(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(int)};
    const size_t capacity = 3;
    cutil_Array *const lhs = cutil_Array_alloc(&type, capacity);
    cutil_Array *const rhs = cutil_Array_alloc(&type, capacity);

    for (size_t i = 0; i < capacity; ++i) {
        const int lval = 0;
        cutil_Array_set(lhs, i, &lval);
        const int rval = 1;
        cutil_Array_set(rhs, i, &rval);
    }

    /* Act */
    /* Assert */
    TEST_ASSERT_NOT_EQUAL(0, cutil_Array_compare(lhs, rhs));
    /* Ordering must be consistent: compare(a,b) and compare(b,a) differ in
       sign. */
    const int fwd = cutil_Array_compare(lhs, rhs);
    const int rev = cutil_Array_compare(rhs, lhs);
    TEST_ASSERT_TRUE((fwd < 0 && rev > 0) || (fwd > 0 && rev < 0));

    /* Cleanup */
    cutil_Array_free(lhs);
    cutil_Array_free(rhs);
}

static void
_should_hashReturnZero_when_arrayIsNull(void)
{
    /* Act */
    /* Assert */
    TEST_ASSERT_EQUAL_UINT64(CUTIL_HASH_C(0), cutil_Array_hash(NULL));
}

static void
_should_hashReturnZero_when_arrayIsEmpty(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(int)};
    cutil_Array *const arr = cutil_Array_alloc(&type, 0);

    /* Act */
    /* Assert */
    TEST_ASSERT_EQUAL_UINT64(CUTIL_HASH_C(0), cutil_Array_hash(arr));

    /* Cleanup */
    cutil_Array_free(arr);
}

static void
_should_hashConsistently(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(int)};
    const size_t capacity = 4;
    cutil_Array *const arr = cutil_Array_alloc(&type, capacity);

    for (size_t i = 0; i < capacity; ++i) {
        const int val = (int) (i * 11);
        cutil_Array_set(arr, i, &val);
    }

    /* Act */
    const cutil_hash_t h1 = cutil_Array_hash(arr);
    const cutil_hash_t h2 = cutil_Array_hash(arr);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT64(h1, h2);

    /* Cleanup */
    cutil_Array_free(arr);
}

static void
_should_hashDifferently_when_contentsChanges(void)
{
    /* Arrange */
    const cutil_GenericType type = {.name = "name", .size = sizeof(int)};
    const size_t capacity = 4;
    cutil_Array *const arr = cutil_Array_alloc(&type, capacity);

    int val = 0;
    for (size_t i = 0; i < capacity; ++i) {
        cutil_Array_set(arr, i, &val);
    }

    /* Act */
    const cutil_hash_t h_before = cutil_Array_hash(arr);

    val = 999;
    cutil_Array_set(arr, 0, &val);
    const cutil_hash_t h_after = cutil_Array_hash(arr);

    /* Assert */
    TEST_ASSERT_NOT_EQUAL(h_before, h_after);

    /* Cleanup */
    cutil_Array_free(arr);
}

static void
_should_toStringSerializeNull(void)
{
    /* Arrange */
    char buf[16];

    /* Act */
    const size_t n = cutil_Array_to_string(NULL, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("NULL", buf);
    TEST_ASSERT_EQUAL_size_t(4UL, n); /* strlen("NULL") */
}

static void
_should_toStringSerializeEmptyArray(void)
{
    /* Arrange */
    const cutil_GenericType type
      = {.name = "name", .size = sizeof(int), .to_string = &_int_to_string};
    cutil_Array *const arr = cutil_Array_alloc(&type, 0);

    char buf[8];

    /* Act */
    const size_t n = cutil_Array_to_string(arr, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("[]", buf);
    TEST_ASSERT_EQUAL_size_t(2UL, n);

    /* Cleanup */
    cutil_Array_free(arr);
}

static void
_should_toStringSerializeElements(void)
{
    /* Arrange */
    const cutil_GenericType type
      = {.name = "name", .size = sizeof(int), .to_string = &_int_to_string};
    const size_t capacity = 3;
    cutil_Array *const arr = cutil_Array_alloc(&type, capacity);

    const int vals[] = {1, 2, 3};
    for (size_t i = 0; i < capacity; ++i) {
        cutil_Array_set(arr, i, &vals[i]);
    }

    char buf[32];

    /* Act */
    const size_t n = cutil_Array_to_string(arr, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("[1,2,3]", buf);
    TEST_ASSERT_EQUAL_size_t(7UL, n); /* strlen("[1,2,3]") */

    /* Cleanup */
    cutil_Array_free(arr);
}

static void
_should_toStringReturnRequiredSize_when_bufIsNull(void)
{
    /* Arrange */
    const cutil_GenericType type
      = {.name = "name", .size = sizeof(int), .to_string = &_int_to_string};
    const size_t capacity = 3;
    cutil_Array *const arr = cutil_Array_alloc(&type, capacity);

    const int vals[] = {10, 200, 3000};
    for (size_t i = 0; i < capacity; ++i) {
        cutil_Array_set(arr, i, &vals[i]);
    }

    /* Act */
    /* "[10,200,3000]" — 13 chars, NUL is the 14th byte */
    const size_t needed = cutil_Array_to_string(arr, NULL, 0UL);
    TEST_ASSERT_EQUAL_size_t(13UL, needed);

    /* The returned size must be exactly enough to hold the string + NUL. */
    char *buf = malloc(needed + 1);
    const size_t written = cutil_Array_to_string(arr, buf, needed + 1);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(needed, written);
    TEST_ASSERT_EQUAL_STRING("[10,200,3000]", buf);
    /* Verify NUL terminator was written at position `written`. */
    TEST_ASSERT_EQUAL_CHAR('\0', buf[written]);

    /* Cleanup */
    free(buf);
    cutil_Array_free(arr);
}

static void
_should_toStringReturnZero_when_bufferTooSmall(void)
{
    /* Arrange */
    const cutil_GenericType type
      = {.name = "name", .size = sizeof(int), .to_string = &_int_to_string};
    const size_t capacity = 2;
    cutil_Array *const arr = cutil_Array_alloc(&type, capacity);

    const int vals[] = {1, 2};
    for (size_t i = 0; i < capacity; ++i) {
        cutil_Array_set(arr, i, &vals[i]);
    }
    /* "[1,2]" needs 6 bytes including NUL; pass only 4. */
    char buf[4];

    /* Act */
    const size_t n = cutil_Array_to_string(arr, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0UL, n);

    /* Cleanup */
    cutil_Array_free(arr);
}

static void
_should_toStringWriteNulTerminator(void)
{
    /* Arrange */
    const cutil_GenericType type
      = {.name = "name", .size = sizeof(int), .to_string = &_int_to_string};
    const size_t capacity = 2;
    cutil_Array *const arr = cutil_Array_alloc(&type, capacity);

    const int vals[] = {7, 8};
    for (size_t i = 0; i < capacity; ++i) {
        cutil_Array_set(arr, i, &vals[i]);
    }
    /* "[7,8]" = 5 chars; allocate exactly 6 bytes. */
    char buf[6];
    buf[5] = 'X'; /* sentinel */

    /* Act */
    const size_t n = cutil_Array_to_string(arr, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(5UL, n);
    TEST_ASSERT_EQUAL_CHAR('\0', buf[5]); /* NUL was written */
    TEST_ASSERT_EQUAL_STRING("[7,8]", buf);

    /* Cleanup */
    cutil_Array_free(arr);
}

/* Tests for Array iterator */

static void
_should_returnNonNull_when_getConstIteratorCalledOnArray(void)
{
    /* Arrange */
    cutil_Array *const arr = cutil_Array_alloc(CUTIL_GENERIC_TYPE_INT, 3);

    /* Act */
    cutil_ConstIterator *const it = cutil_Array_get_const_iterator(arr);

    /* Assert */
    TEST_ASSERT_NOT_NULL(it);

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Array_free(arr);
}

static void
_should_returnNonNull_when_getIteratorCalledOnArray(void)
{
    /* Arrange */
    cutil_Array *const arr = cutil_Array_alloc(CUTIL_GENERIC_TYPE_INT, 3);

    /* Act */
    cutil_Iterator *const it = cutil_Array_get_iterator(arr);

    /* Assert */
    TEST_ASSERT_NOT_NULL(it);

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Array_free(arr);
}

static void
_should_traverseAllElements_when_iteratingArray(void)
{
    /* Arrange */
    cutil_Array *const arr = cutil_Array_alloc(CUTIL_GENERIC_TYPE_INT, 4);
    const int vals[] = {100, 200, 300, 400};
    for (size_t i = 0; i < 4; ++i) {
        cutil_Array_set(arr, i, &vals[i]);
    }

    cutil_ConstIterator *const it = cutil_Array_get_const_iterator(arr);
    TEST_ASSERT_NOT_NULL(it);

    /* Act */
    int count = 0;
    while (cutil_ConstIterator_next(it)) {
        const int *const elem_ptr = cutil_ConstIterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(elem_ptr);
        TEST_ASSERT_EQUAL_INT(vals[count], *elem_ptr);
        ++count;
    }

    /* Assert */
    TEST_ASSERT_EQUAL_INT(4, count);

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Array_free(arr);
}

static void
_should_copyElementIntoBuffer_when_getCalledOnArrayIterator(void)
{
    /* Arrange */
    cutil_Array *const arr = cutil_Array_alloc(CUTIL_GENERIC_TYPE_INT, 4);
    const int vals[] = {100, 200, 300, 400};
    for (size_t i = 0; i < 4; ++i) {
        cutil_Array_set(arr, i, &vals[i]);
    }

    cutil_ConstIterator *const it = cutil_Array_get_const_iterator(arr);
    TEST_ASSERT_NOT_NULL(it);

    /* Act / Assert */
    int buf = 0;
    int count = 0;
    while (cutil_ConstIterator_next(it)) {
        const int status = cutil_ConstIterator_get(it, &buf);
        TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, status);
        TEST_ASSERT_EQUAL_INT(vals[count], buf);
        ++count;
    }
    TEST_ASSERT_EQUAL_INT(4, count);

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Array_free(arr);
}

static void
_should_overwriteElement_when_setCalledOnArrayIterator(void)
{
    /* Arrange */
    cutil_Array *const arr = cutil_Array_alloc(CUTIL_GENERIC_TYPE_INT, 2);
    const int v0 = 10;
    const int v1 = 20;
    cutil_Array_set(arr, 0, &v0);
    cutil_Array_set(arr, 1, &v1);

    cutil_Iterator *const it = cutil_Array_get_iterator(arr);
    TEST_ASSERT_NOT_NULL(it);

    /* Act */
    TEST_ASSERT_TRUE(cutil_Iterator_next(it));
    const int new_val = 99;
    const int status = cutil_Iterator_set(it, &new_val);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, status);
    int buf = 0;
    cutil_Array_get(arr, 0, &buf);
    TEST_ASSERT_EQUAL_INT(99, buf);

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Array_free(arr);
}

static void
_should_returnFailure_when_removeCalledOnArrayIterator(void)
{
    /* Arrange */
    cutil_Array *const arr = cutil_Array_alloc(CUTIL_GENERIC_TYPE_INT, 2);
    const int v = 1;
    cutil_Array_set(arr, 0, &v);

    cutil_Iterator *const it = cutil_Array_get_iterator(arr);
    TEST_ASSERT_NOT_NULL(it);
    TEST_ASSERT_TRUE(cutil_Iterator_next(it));

    /* Act */
    const int result = cutil_Iterator_remove(it);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, result);

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Array_free(arr);
}

static void
_should_traverseSingleElement_when_arrayHasCapacityOne(void)
{
    /* Arrange */
    cutil_Array *const arr = cutil_Array_alloc(CUTIL_GENERIC_TYPE_INT, 1);
    const int val = 42;
    cutil_Array_set(arr, 0, &val);

    cutil_ConstIterator *const it = cutil_Array_get_const_iterator(arr);
    TEST_ASSERT_NOT_NULL(it);

    /* Act */
    TEST_ASSERT_TRUE(cutil_ConstIterator_next(it));
    const int *const elem_ptr = cutil_ConstIterator_get_ptr(it);
    TEST_ASSERT_NOT_NULL(elem_ptr);
    TEST_ASSERT_EQUAL_INT(42, *elem_ptr);
    TEST_ASSERT_FALSE(cutil_ConstIterator_next(it));

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Array_free(arr);
}

static void
_should_returnFalse_when_nextCalledOnCapacityZeroArray(void)
{
    /* Arrange */
    cutil_Array *const arr = cutil_Array_alloc(CUTIL_GENERIC_TYPE_INT, 0);
    TEST_ASSERT_NOT_NULL(arr);

    cutil_ConstIterator *const it = cutil_Array_get_const_iterator(arr);
    TEST_ASSERT_NOT_NULL(it);

    /* Act / Assert */
    TEST_ASSERT_FALSE(cutil_ConstIterator_next(it));

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Array_free(arr);
}

static void
_should_traverseAllElements_when_constIteratorRewound(void)
{
    /* Arrange */
    cutil_Array *const arr = cutil_Array_alloc(CUTIL_GENERIC_TYPE_INT, 4);
    const int vals[] = {10, 20, 30, 40};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < N; ++i) {
        cutil_Array_set(arr, i, &vals[i]);
    }
    cutil_ConstIterator *const it = cutil_Array_get_const_iterator(arr);
    TEST_ASSERT_NOT_NULL(it);

    /* Act */
    int first[4] = {0};
    int count_first = 0;
    while (cutil_ConstIterator_next(it)) {
        const int *const p = cutil_ConstIterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(p);
        first[count_first++] = *p;
    }

    cutil_ConstIterator_rewind(it);

    int second[4] = {0};
    int count_second = 0;
    while (cutil_ConstIterator_next(it)) {
        const int *const p = cutil_ConstIterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(p);
        second[count_second++] = *p;
    }

    /* Assert */
    TEST_ASSERT_EQUAL_INT((int) N, count_first);
    TEST_ASSERT_EQUAL_INT((int) N, count_second);
    for (int i = 0; i < (int) N; ++i) {
        TEST_ASSERT_EQUAL_INT(first[i], second[i]);
    }

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Array_free(arr);
}

static void
_should_traverseAllElements_when_iteratorRewound(void)
{
    /* Arrange */
    cutil_Array *const arr = cutil_Array_alloc(CUTIL_GENERIC_TYPE_INT, 4);
    const int vals[] = {10, 20, 30, 40};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < N; ++i) {
        cutil_Array_set(arr, i, &vals[i]);
    }
    cutil_Iterator *const it = cutil_Array_get_iterator(arr);
    TEST_ASSERT_NOT_NULL(it);

    /* Act */
    int first[4] = {0};
    int count_first = 0;
    while (cutil_Iterator_next(it)) {
        const int *const p = cutil_Iterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(p);
        first[count_first++] = *p;
    }

    cutil_Iterator_rewind(it);

    int second[4] = {0};
    int count_second = 0;
    while (cutil_Iterator_next(it)) {
        const int *const p = cutil_Iterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(p);
        second[count_second++] = *p;
    }

    /* Assert */
    TEST_ASSERT_EQUAL_INT((int) N, count_first);
    TEST_ASSERT_EQUAL_INT((int) N, count_second);
    for (int i = 0; i < (int) N; ++i) {
        TEST_ASSERT_EQUAL_INT(first[i], second[i]);
    }

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Array_free(arr);
}

static void
_should_returnFalse_when_nextCalledAfterExhaustionOnConstIterator(void)
{
    /* Arrange */
    cutil_Array *const arr = cutil_Array_alloc(CUTIL_GENERIC_TYPE_INT, 3);
    const int vals[] = {1, 2, 3};
    for (size_t i = 0; i < 3; ++i) {
        cutil_Array_set(arr, i, &vals[i]);
    }
    cutil_ConstIterator *const it = cutil_Array_get_const_iterator(arr);
    TEST_ASSERT_NOT_NULL(it);

    while (cutil_ConstIterator_next(it)) {
    }

    /* Assert stable-next */
    TEST_ASSERT_FALSE(cutil_ConstIterator_next(it));
    TEST_ASSERT_FALSE(cutil_ConstIterator_next(it));
    TEST_ASSERT_FALSE(cutil_ConstIterator_next(it));
    TEST_ASSERT_NULL(cutil_ConstIterator_get_ptr(it));

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Array_free(arr);
}

static void
_should_returnFalse_when_nextCalledAfterExhaustionOnIterator(void)
{
    /* Arrange */
    cutil_Array *const arr = cutil_Array_alloc(CUTIL_GENERIC_TYPE_INT, 3);
    const int vals[] = {1, 2, 3};
    for (size_t i = 0; i < 3; ++i) {
        cutil_Array_set(arr, i, &vals[i]);
    }
    cutil_Iterator *const it = cutil_Array_get_iterator(arr);
    TEST_ASSERT_NOT_NULL(it);

    while (cutil_Iterator_next(it)) {
    }

    /* Assert stable-next */
    TEST_ASSERT_FALSE(cutil_Iterator_next(it));
    TEST_ASSERT_FALSE(cutil_Iterator_next(it));
    TEST_ASSERT_FALSE(cutil_Iterator_next(it));

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Array_free(arr);
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
    RUN_TEST(_should_setMultCorrectly);
    RUN_TEST(_should_setAndGetIndividualElements);
    RUN_TEST(_should_copyArrayCorrectly);
    RUN_TEST(_should_duplicateArrayCorrectly);
    RUN_TEST(_should_resizeArrayCorrectly);
    RUN_TEST(_should_handleEdgeCases);
    RUN_TEST(_should_getPointerCorrectly);

    /* deep_equals */
    RUN_TEST(_should_returnTrue_when_bothArraysNull);
    RUN_TEST(_should_returnFalse_when_oneArrayIsNull);
    RUN_TEST(_should_returnTrue_when_samePointer);
    RUN_TEST(_should_returnTrue_when_arraysHaveSameContent);
    RUN_TEST(_should_returnFalse_when_arraysHaveDifferentData);
    RUN_TEST(_should_returnFalse_when_arraysHaveDifferentCapacity);
    RUN_TEST(_should_returnFalse_when_arraysHaveDifferentTypes);

    /* compare */
    RUN_TEST(_should_compareReturnZero_when_samePointer);
    RUN_TEST(_should_compareReturnZero_when_bothNull);
    RUN_TEST(_should_compareReturnNegative_when_lhsIsNull);
    RUN_TEST(_should_compareReturnPositive_when_rhsIsNull);
    RUN_TEST(_should_compareReturnZero_when_arraysHaveSameContent);
    RUN_TEST(_should_compareReturnNonZero_when_arraysHaveDifferentData);

    /* hash */
    RUN_TEST(_should_hashReturnZero_when_arrayIsNull);
    RUN_TEST(_should_hashReturnZero_when_arrayIsEmpty);
    RUN_TEST(_should_hashConsistently);
    RUN_TEST(_should_hashDifferently_when_contentsChanges);

    /* to_string */
    RUN_TEST(_should_toStringSerializeNull);
    RUN_TEST(_should_toStringSerializeEmptyArray);
    RUN_TEST(_should_toStringSerializeElements);
    RUN_TEST(_should_toStringReturnRequiredSize_when_bufIsNull);
    RUN_TEST(_should_toStringReturnZero_when_bufferTooSmall);
    RUN_TEST(_should_toStringWriteNulTerminator);

    /* iterator */
    RUN_TEST(_should_returnNonNull_when_getConstIteratorCalledOnArray);
    RUN_TEST(_should_returnNonNull_when_getIteratorCalledOnArray);
    RUN_TEST(_should_traverseAllElements_when_iteratingArray);
    RUN_TEST(_should_copyElementIntoBuffer_when_getCalledOnArrayIterator);
    RUN_TEST(_should_overwriteElement_when_setCalledOnArrayIterator);
    RUN_TEST(_should_returnFailure_when_removeCalledOnArrayIterator);
    RUN_TEST(_should_traverseSingleElement_when_arrayHasCapacityOne);
    RUN_TEST(_should_returnFalse_when_nextCalledOnCapacityZeroArray);
    RUN_TEST(_should_traverseAllElements_when_constIteratorRewound);
    RUN_TEST(_should_traverseAllElements_when_iteratorRewound);
    RUN_TEST(_should_returnFalse_when_nextCalledAfterExhaustionOnConstIterator);
    RUN_TEST(_should_returnFalse_when_nextCalledAfterExhaustionOnIterator);

    return UNITY_END();
}

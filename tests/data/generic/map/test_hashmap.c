#include "unity.h"

#include <cutil/data/generic/map/hashmap.h>
#include <cutil/data/generic/type.h>

#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

/* Tests for cutil_HashMap_alloc */
static void
_should_allocateHashMap_when_createdWithValidKeyType(void)
{
    /* Arrange */
    const cutil_GenericType *const key_type = CUTIL_GENERIC_TYPE_INT;
    const cutil_GenericType *const val_type = CUTIL_GENERIC_TYPE_DOUBLE;

    /* Act */
    cutil_Map *const map = cutil_HashMap_alloc(key_type, val_type);

    /* Assert */
    TEST_ASSERT_NOT_NULL(map);
    TEST_ASSERT_NOT_NULL(map->vtable);
    TEST_ASSERT_NOT_NULL(map->data);
    TEST_ASSERT_EQUAL_STRING("cutil_HashMap", map->vtable->name);

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_allocateHashMap_when_createdWithValidValueType(void)
{
    /* Arrange */
    const cutil_GenericType *const key_type = CUTIL_GENERIC_TYPE_U32;
    const cutil_GenericType *const val_type = CUTIL_GENERIC_TYPE_FLOAT;

    /* Act */
    cutil_Map *const map = cutil_HashMap_alloc(key_type, val_type);

    /* Assert */
    TEST_ASSERT_NOT_NULL(map);
    TEST_ASSERT_NOT_NULL(map->vtable);
    TEST_ASSERT_NOT_NULL(map->data);

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_allocateHashMap_withVariousKeyTypes(void)
{
    /* Arrange */
    const cutil_GenericType *const key_types[] = {
      CUTIL_GENERIC_TYPE_CHAR,   CUTIL_GENERIC_TYPE_INT,
      CUTIL_GENERIC_TYPE_I32,    CUTIL_GENERIC_TYPE_U64,
      CUTIL_GENERIC_TYPE_DOUBLE,
    };
    const size_t NUM_TYPES = CUTIL_GET_NATIVE_ARRAY_SIZE(key_types);

    for (size_t i = 0; i < NUM_TYPES; ++i) {
        /* Act */
        cutil_Map *const map
          = cutil_HashMap_alloc(key_types[i], CUTIL_GENERIC_TYPE_INT);

        /* Assert */
        TEST_ASSERT_NOT_NULL(map);
        TEST_ASSERT_NOT_NULL(map->vtable);
        TEST_ASSERT_NOT_NULL(map->data);

        /* Cleanup */
        cutil_Map_free(map);
    }
}

static void
_should_allocateHashMap_withVariousValueTypes(void)
{
    /* Arrange */
    const cutil_GenericType *const val_types[] = {
      CUTIL_GENERIC_TYPE_CHAR,   CUTIL_GENERIC_TYPE_INT,
      CUTIL_GENERIC_TYPE_I32,    CUTIL_GENERIC_TYPE_U64,
      CUTIL_GENERIC_TYPE_DOUBLE,
    };
    const size_t NUM_TYPES = CUTIL_GET_NATIVE_ARRAY_SIZE(val_types);

    for (size_t i = 0; i < NUM_TYPES; ++i) {
        /* Act */
        cutil_Map *const map
          = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, val_types[i]);

        /* Assert */
        TEST_ASSERT_NOT_NULL(map);
        TEST_ASSERT_NOT_NULL(map->vtable);
        TEST_ASSERT_NOT_NULL(map->data);

        /* Cleanup */
        cutil_Map_free(map);
    }
}

/* Tests for cutil_Map_get_key_type */
static void
_should_returnKeyType_when_queried(void)
{
    /* Arrange */
    const cutil_GenericType *const expected_key_type = CUTIL_GENERIC_TYPE_INT;
    const cutil_GenericType *const val_type = CUTIL_GENERIC_TYPE_DOUBLE;
    cutil_Map *const map = cutil_HashMap_alloc(expected_key_type, val_type);

    /* Act */
    const cutil_GenericType *const result = cutil_Map_get_key_type(map);

    /* Assert */
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING(
      cutil_GenericType_get_name(expected_key_type),
      cutil_GenericType_get_name(result)
    );
    TEST_ASSERT_EQUAL_size_t(
      cutil_GenericType_get_size(expected_key_type),
      cutil_GenericType_get_size(result)
    );

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_returnKeyType_forVariousTypes(void)
{
    /* Arrange */
    const cutil_GenericType *const key_types[] = {
      CUTIL_GENERIC_TYPE_I8,     CUTIL_GENERIC_TYPE_I32, CUTIL_GENERIC_TYPE_I64,
      CUTIL_GENERIC_TYPE_U8,     CUTIL_GENERIC_TYPE_U32, CUTIL_GENERIC_TYPE_U64,
      CUTIL_GENERIC_TYPE_DOUBLE,
    };
    const size_t NUM_TYPES = CUTIL_GET_NATIVE_ARRAY_SIZE(key_types);

    for (size_t i = 0; i < NUM_TYPES; ++i) {
        const cutil_GenericType *const key_type = key_types[i];

        /* Act */
        cutil_Map *const map
          = cutil_HashMap_alloc(key_type, CUTIL_GENERIC_TYPE_INT);
        const cutil_GenericType *const result = cutil_Map_get_key_type(map);

        /* Assert */
        TEST_ASSERT_NOT_NULL(result);
        TEST_ASSERT_EQUAL_STRING(
          cutil_GenericType_get_name(key_type),
          cutil_GenericType_get_name(result)
        );

        /* Cleanup */
        cutil_Map_free(map);
    }
}

/* Tests for cutil_Map_get_val_type */
static void
_should_returnValueType_when_queried(void)
{
    /* Arrange */
    const cutil_GenericType *const key_type = CUTIL_GENERIC_TYPE_INT;
    const cutil_GenericType *const expected_val_type
      = CUTIL_GENERIC_TYPE_DOUBLE;
    cutil_Map *const map = cutil_HashMap_alloc(key_type, expected_val_type);

    /* Act */
    const cutil_GenericType *const result = cutil_Map_get_val_type(map);

    /* Assert */
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING(
      cutil_GenericType_get_name(expected_val_type),
      cutil_GenericType_get_name(result)
    );
    TEST_ASSERT_EQUAL_size_t(
      cutil_GenericType_get_size(expected_val_type),
      cutil_GenericType_get_size(result)
    );

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_returnValueType_forVariousTypes(void)
{
    /* Arrange */
    const cutil_GenericType *const val_types[] = {
      CUTIL_GENERIC_TYPE_I8,    CUTIL_GENERIC_TYPE_I32, CUTIL_GENERIC_TYPE_I64,
      CUTIL_GENERIC_TYPE_U8,    CUTIL_GENERIC_TYPE_U32, CUTIL_GENERIC_TYPE_U64,
      CUTIL_GENERIC_TYPE_FLOAT,
    };
    const size_t NUM_TYPES = CUTIL_GET_NATIVE_ARRAY_SIZE(val_types);

    for (size_t i = 0; i < NUM_TYPES; ++i) {
        const cutil_GenericType *const val_type = val_types[i];

        /* Act */
        cutil_Map *const map
          = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, val_type);
        const cutil_GenericType *const result = cutil_Map_get_val_type(map);

        /* Assert */
        TEST_ASSERT_NOT_NULL(result);
        TEST_ASSERT_EQUAL_STRING(
          cutil_GenericType_get_name(val_type),
          cutil_GenericType_get_name(result)
        );

        /* Cleanup */
        cutil_Map_free(map);
    }
}

/* Tests for HashMap with different key and value type combinations */
static void
_should_preserveKeyAndValueTypes_whenCreatedWithDifferentPairs(void)
{
    /* Arrange */
    const struct {
        const cutil_GenericType *const key_type;
        const cutil_GenericType *const val_type;
    } TYPE_PAIRS[] = {
      {CUTIL_GENERIC_TYPE_I8, CUTIL_GENERIC_TYPE_I32},
      {CUTIL_GENERIC_TYPE_I32, CUTIL_GENERIC_TYPE_U64},
      {CUTIL_GENERIC_TYPE_U32, CUTIL_GENERIC_TYPE_DOUBLE},
      {CUTIL_GENERIC_TYPE_DOUBLE, CUTIL_GENERIC_TYPE_FLOAT},
    };
    const size_t NUM_PAIRS = CUTIL_GET_NATIVE_ARRAY_SIZE(TYPE_PAIRS);

    for (size_t i = 0; i < NUM_PAIRS; ++i) {
        const cutil_GenericType *const key_type = TYPE_PAIRS[i].key_type;
        const cutil_GenericType *const val_type = TYPE_PAIRS[i].val_type;

        /* Act */
        cutil_Map *const map = cutil_HashMap_alloc(key_type, val_type);
        const cutil_GenericType *const retrieved_key_type
          = cutil_Map_get_key_type(map);
        const cutil_GenericType *const retrieved_val_type
          = cutil_Map_get_val_type(map);

        /* Assert */
        TEST_ASSERT_EQUAL_STRING(
          cutil_GenericType_get_name(key_type),
          cutil_GenericType_get_name(retrieved_key_type)
        );
        TEST_ASSERT_EQUAL_STRING(
          cutil_GenericType_get_name(val_type),
          cutil_GenericType_get_name(retrieved_val_type)
        );

        /* Cleanup */
        cutil_Map_free(map);
    }
}

/* Tests for HashMap iterator */

static void
_should_returnNonNull_when_getConstIteratorCalledOnHashMap(void)
{
    /* Arrange */
    cutil_Map *const map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);

    /* Act */
    cutil_ConstIterator *const it = cutil_Map_get_const_iterator(map);

    /* Assert */
    TEST_ASSERT_NOT_NULL(it);

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Map_free(map);
}

static void
_should_returnNonNull_when_getIteratorCalledOnHashMap(void)
{
    /* Arrange */
    cutil_Map *const map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);

    /* Act */
    cutil_Iterator *const it = cutil_Map_get_iterator(map);

    /* Assert */
    TEST_ASSERT_NOT_NULL(it);

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Map_free(map);
}

static void
_should_returnFalse_when_nextCalledOnEmptyHashMapIterator(void)
{
    /* Arrange */
    cutil_Map *const map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
    cutil_ConstIterator *const it = cutil_Map_get_const_iterator(map);
    TEST_ASSERT_NOT_NULL(it);

    /* Act / Assert */
    TEST_ASSERT_FALSE(cutil_ConstIterator_next(it));

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Map_free(map);
}

static void
_should_traverseAllKeys_when_iteratingHashMap(void)
{
    /* Arrange */
    cutil_Map *const map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
    const int keys[] = {10, 20, 30, 40, 50};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(keys);
    int dummy_val = 0;
    for (size_t i = 0; i < N; ++i) {
        cutil_Map_set(map, &keys[i], &dummy_val);
    }

    cutil_ConstIterator *const it = cutil_Map_get_const_iterator(map);
    TEST_ASSERT_NOT_NULL(it);

    /* Act */
    int found[] = {0, 0, 0, 0, 0};
    int count = 0;
    while (cutil_ConstIterator_next(it)) {
        const int *key_ptr = cutil_ConstIterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(key_ptr);
        for (size_t i = 0; i < N; ++i) {
            if (*key_ptr == keys[i]) {
                found[i] = 1;
            }
        }
        ++count;
    }

    /* Assert */
    TEST_ASSERT_EQUAL_INT((int) N, count);
    for (size_t i = 0; i < N; ++i) {
        TEST_ASSERT_TRUE(found[i]);
    }

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Map_free(map);
}

static void
_should_copyKeyIntoBuffer_when_getCalledOnHashMapIterator(void)
{
    /* Arrange */
    cutil_Map *const map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
    const int key = 42;
    int dummy_val = 0;
    cutil_Map_set(map, &key, &dummy_val);

    cutil_ConstIterator *const it = cutil_Map_get_const_iterator(map);
    TEST_ASSERT_NOT_NULL(it);

    /* Act */
    TEST_ASSERT_TRUE(cutil_ConstIterator_next(it));
    int buf = 0;
    const int status = cutil_ConstIterator_get(it, &buf);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, status);
    TEST_ASSERT_EQUAL_INT(42, buf);

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Map_free(map);
}

static void
_should_removeCurrentKey_when_removeCalledOnHashMapIterator(void)
{
    /* Arrange */
    cutil_Map *const map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
    const int insert_keys[] = {1, 2, 3};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(insert_keys);
    int dummy_val = 0;
    for (size_t i = 0; i < N; ++i) {
        cutil_Map_set(map, &insert_keys[i], &dummy_val);
    }

    cutil_Iterator *const it = cutil_Map_get_iterator(map);
    TEST_ASSERT_NOT_NULL(it);

    /* Act */
    TEST_ASSERT_TRUE(cutil_Iterator_next(it));
    const int removed_key = *(const int *) cutil_Iterator_get_ptr(it);
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, cutil_Iterator_remove(it));

    int remaining_count = 0;
    while (cutil_Iterator_next(it)) {
        ++remaining_count;
    }

    /* Assert */
    TEST_ASSERT_EQUAL_INT(2, remaining_count);
    TEST_ASSERT_EQUAL_size_t(2UL, cutil_Map_get_count(map));
    TEST_ASSERT_FALSE(cutil_Map_contains(map, &removed_key));

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Map_free(map);
}

static void
_should_returnFailure_when_setCalledOnHashMapIterator(void)
{
    /* Arrange */
    cutil_Map *const map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
    const int key = 7;
    int dummy_val = 0;
    cutil_Map_set(map, &key, &dummy_val);

    cutil_Iterator *const it = cutil_Map_get_iterator(map);
    TEST_ASSERT_NOT_NULL(it);
    TEST_ASSERT_TRUE(cutil_Iterator_next(it));

    /* Act */
    int new_val = 99;
    const int result = cutil_Iterator_set(it, &new_val);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, result);

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Map_free(map);
}

static void
_should_traverseAllKeys_when_constIteratorRewound(void)
{
    /* Arrange */
    cutil_Map *const map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
    const int keys[] = {10, 20, 30, 40};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(keys);
    int dummy_val = 0;
    for (size_t i = 0; i < N; ++i) {
        cutil_Map_set(map, &keys[i], &dummy_val);
    }
    cutil_ConstIterator *const it = cutil_Map_get_const_iterator(map);
    TEST_ASSERT_NOT_NULL(it);

    /* First pass */
    int seen1[4] = {0};
    int count1 = 0;
    while (cutil_ConstIterator_next(it)) {
        const int *kp = (const int *) cutil_ConstIterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(kp);
        for (size_t i = 0; i < N; ++i) {
            if (*kp == keys[i]) {
                seen1[i] = 1;
            }
        }
        ++count1;
    }

    /* Rewind */
    cutil_ConstIterator_rewind(it);

    /* Second pass */
    int seen2[4] = {0};
    int count2 = 0;
    while (cutil_ConstIterator_next(it)) {
        const int *kp = (const int *) cutil_ConstIterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(kp);
        for (size_t i = 0; i < N; ++i) {
            if (*kp == keys[i]) {
                seen2[i] = 1;
            }
        }
        ++count2;
    }

    /* Assert */
    TEST_ASSERT_EQUAL_INT((int) N, count1);
    TEST_ASSERT_EQUAL_INT((int) N, count2);
    for (size_t i = 0; i < N; ++i) {
        TEST_ASSERT_TRUE(seen1[i]);
        TEST_ASSERT_TRUE(seen2[i]);
    }

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Map_free(map);
}

static void
_should_traverseAllKeys_when_iteratorRewound(void)
{
    /* Arrange */
    cutil_Map *const map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
    const int keys[] = {10, 20, 30, 40};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(keys);
    int dummy_val = 0;
    for (size_t i = 0; i < N; ++i) {
        cutil_Map_set(map, &keys[i], &dummy_val);
    }
    cutil_Iterator *const it = cutil_Map_get_iterator(map);
    TEST_ASSERT_NOT_NULL(it);

    /* First pass */
    int seen1[4] = {0};
    int count1 = 0;
    while (cutil_Iterator_next(it)) {
        const int *kp = (const int *) cutil_Iterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(kp);
        for (size_t i = 0; i < N; ++i) {
            if (*kp == keys[i]) {
                seen1[i] = 1;
            }
        }
        ++count1;
    }

    /* Rewind */
    cutil_Iterator_rewind(it);

    /* Second pass */
    int seen2[4] = {0};
    int count2 = 0;
    while (cutil_Iterator_next(it)) {
        const int *kp = (const int *) cutil_Iterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(kp);
        for (size_t i = 0; i < N; ++i) {
            if (*kp == keys[i]) {
                seen2[i] = 1;
            }
        }
        ++count2;
    }

    /* Assert */
    TEST_ASSERT_EQUAL_INT((int) N, count1);
    TEST_ASSERT_EQUAL_INT((int) N, count2);
    for (size_t i = 0; i < N; ++i) {
        TEST_ASSERT_TRUE(seen1[i]);
        TEST_ASSERT_TRUE(seen2[i]);
    }

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Map_free(map);
}

static void
_should_returnFalse_when_nextCalledAfterExhaustionOnConstIterator(void)
{
    /* Arrange */
    cutil_Map *const map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
    const int keys[] = {1, 2, 3};
    int dummy_val = 0;
    for (size_t i = 0; i < 3; ++i) {
        cutil_Map_set(map, &keys[i], &dummy_val);
    }
    cutil_ConstIterator *const it = cutil_Map_get_const_iterator(map);
    TEST_ASSERT_NOT_NULL(it);

    /* Drain */
    while (cutil_ConstIterator_next(it)) {}

    /* Assert stable-next */
    TEST_ASSERT_FALSE(cutil_ConstIterator_next(it));
    TEST_ASSERT_FALSE(cutil_ConstIterator_next(it));
    TEST_ASSERT_FALSE(cutil_ConstIterator_next(it));

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Map_free(map);
}

static void
_should_returnFalse_when_nextCalledAfterExhaustionOnIterator(void)
{
    /* Arrange */
    cutil_Map *const map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
    const int keys[] = {1, 2, 3};
    int dummy_val = 0;
    for (size_t i = 0; i < 3; ++i) {
        cutil_Map_set(map, &keys[i], &dummy_val);
    }
    cutil_Iterator *const it = cutil_Map_get_iterator(map);
    TEST_ASSERT_NOT_NULL(it);

    /* Drain */
    while (cutil_Iterator_next(it)) {}

    /* Assert stable-next */
    TEST_ASSERT_FALSE(cutil_Iterator_next(it));
    TEST_ASSERT_FALSE(cutil_Iterator_next(it));
    TEST_ASSERT_FALSE(cutil_Iterator_next(it));

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Map_free(map);
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

    RUN_TEST(_should_allocateHashMap_when_createdWithValidKeyType);
    RUN_TEST(_should_allocateHashMap_when_createdWithValidValueType);
    RUN_TEST(_should_allocateHashMap_withVariousKeyTypes);
    RUN_TEST(_should_allocateHashMap_withVariousValueTypes);
    RUN_TEST(_should_returnKeyType_when_queried);
    RUN_TEST(_should_returnKeyType_forVariousTypes);
    RUN_TEST(_should_returnValueType_when_queried);
    RUN_TEST(_should_returnValueType_forVariousTypes);
    RUN_TEST(_should_preserveKeyAndValueTypes_whenCreatedWithDifferentPairs);

    /* Iterator tests */
    RUN_TEST(_should_returnNonNull_when_getConstIteratorCalledOnHashMap);
    RUN_TEST(_should_returnNonNull_when_getIteratorCalledOnHashMap);
    RUN_TEST(_should_returnFalse_when_nextCalledOnEmptyHashMapIterator);
    RUN_TEST(_should_traverseAllKeys_when_iteratingHashMap);
    RUN_TEST(_should_copyKeyIntoBuffer_when_getCalledOnHashMapIterator);
    RUN_TEST(_should_removeCurrentKey_when_removeCalledOnHashMapIterator);
    RUN_TEST(_should_returnFailure_when_setCalledOnHashMapIterator);
    RUN_TEST(_should_traverseAllKeys_when_constIteratorRewound);
    RUN_TEST(_should_traverseAllKeys_when_iteratorRewound);
    RUN_TEST(_should_returnFalse_when_nextCalledAfterExhaustionOnConstIterator);
    RUN_TEST(_should_returnFalse_when_nextCalledAfterExhaustionOnIterator);

    return UNITY_END();
}

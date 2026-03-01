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

    return UNITY_END();
}

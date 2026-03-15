#include "unity.h"

#include <cutil/data/generic/set/hashset.h>
#include <cutil/data/generic/type.h>

#include <cutil/std/stdlib.h>
#include <cutil/util/macro.h>

/* Tests for cutil_HashSet_alloc */
static void
_should_allocateHashSet_when_createdWithValidElemType(void)
{
    /* Arrange */
    const cutil_GenericType *const elem_type = CUTIL_GENERIC_TYPE_INT;

    /* Act */
    cutil_Set *const set = cutil_HashSet_alloc(elem_type);

    /* Assert */
    TEST_ASSERT_NOT_NULL(set);
    TEST_ASSERT_NOT_NULL(set->vtable);
    TEST_ASSERT_NOT_NULL(set->data);
    TEST_ASSERT_EQUAL_STRING("cutil_HashSet", set->vtable->name);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_allocateHashSet_withVariousElemTypes(void)
{
    /* Arrange */
    const cutil_GenericType *const elem_types[] = {
      CUTIL_GENERIC_TYPE_CHAR,   CUTIL_GENERIC_TYPE_INT,
      CUTIL_GENERIC_TYPE_I32,    CUTIL_GENERIC_TYPE_U64,
      CUTIL_GENERIC_TYPE_DOUBLE,
    };
    const size_t NUM_TYPES = CUTIL_GET_NATIVE_ARRAY_SIZE(elem_types);

    for (size_t i = 0; i < NUM_TYPES; ++i) {
        /* Act */
        cutil_Set *const set = cutil_HashSet_alloc(elem_types[i]);

        /* Assert */
        TEST_ASSERT_NOT_NULL(set);
        TEST_ASSERT_NOT_NULL(set->vtable);
        TEST_ASSERT_NOT_NULL(set->data);

        /* Cleanup */
        cutil_Set_free(set);
    }
}

/* Tests for vtable pointer identity */
static void
_should_haveCorrectVtablePointer_when_created(void)
{
    /* Arrange / Act */
    cutil_Set *const set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);

    /* Assert */
    TEST_ASSERT_NOT_NULL(CUTIL_SET_TYPE_HASHSET);
    TEST_ASSERT_EQUAL_PTR(CUTIL_SET_TYPE_HASHSET, set->vtable);

    /* Cleanup */
    cutil_Set_free(set);
}

/* Tests for cutil_Set_get_elem_type */
static void
_should_returnElemType_when_queried(void)
{
    /* Arrange */
    const cutil_GenericType *const expected = CUTIL_GENERIC_TYPE_INT;
    cutil_Set *const set = cutil_HashSet_alloc(expected);

    /* Act */
    const cutil_GenericType *const result = cutil_Set_get_elem_type(set);

    /* Assert */
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING(
      cutil_GenericType_get_name(expected), cutil_GenericType_get_name(result)
    );
    TEST_ASSERT_EQUAL_size_t(
      cutil_GenericType_get_size(expected), cutil_GenericType_get_size(result)
    );

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_returnElemType_forVariousTypes(void)
{
    /* Arrange */
    const cutil_GenericType *const elem_types[] = {
      CUTIL_GENERIC_TYPE_I8,     CUTIL_GENERIC_TYPE_I32, CUTIL_GENERIC_TYPE_I64,
      CUTIL_GENERIC_TYPE_U8,     CUTIL_GENERIC_TYPE_U32, CUTIL_GENERIC_TYPE_U64,
      CUTIL_GENERIC_TYPE_DOUBLE,
    };
    const size_t NUM_TYPES = CUTIL_GET_NATIVE_ARRAY_SIZE(elem_types);

    for (size_t i = 0; i < NUM_TYPES; ++i) {
        const cutil_GenericType *const elem_type = elem_types[i];

        /* Act */
        cutil_Set *const set = cutil_HashSet_alloc(elem_type);
        const cutil_GenericType *const result = cutil_Set_get_elem_type(set);

        /* Assert */
        TEST_ASSERT_NOT_NULL(result);
        TEST_ASSERT_EQUAL_STRING(
          cutil_GenericType_get_name(elem_type),
          cutil_GenericType_get_name(result)
        );

        /* Cleanup */
        cutil_Set_free(set);
    }
}

static void
_should_preserveElemType_whenCreatedWithDifferentTypes(void)
{
    /* Arrange */
    struct {
        const cutil_GenericType *elem_type;
    } TYPES[] = {
      {CUTIL_GENERIC_TYPE_INT},
      {CUTIL_GENERIC_TYPE_I32},
      {CUTIL_GENERIC_TYPE_U32},
      {CUTIL_GENERIC_TYPE_DOUBLE},
    };
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(TYPES);

    for (size_t i = 0; i < N; ++i) {
        const cutil_GenericType *const elem_type = TYPES[i].elem_type;

        /* Act */
        cutil_Set *const set = cutil_HashSet_alloc(elem_type);
        const cutil_GenericType *const result = cutil_Set_get_elem_type(set);

        /* Assert */
        TEST_ASSERT_NOT_NULL(result);
        TEST_ASSERT_EQUAL_STRING(
          cutil_GenericType_get_name(elem_type),
          cutil_GenericType_get_name(result)
        );

        /* Cleanup */
        cutil_Set_free(set);
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
    RUN_TEST(_should_allocateHashSet_when_createdWithValidElemType);
    RUN_TEST(_should_allocateHashSet_withVariousElemTypes);
    RUN_TEST(_should_haveCorrectVtablePointer_when_created);
    RUN_TEST(_should_returnElemType_when_queried);
    RUN_TEST(_should_returnElemType_forVariousTypes);
    RUN_TEST(_should_preserveElemType_whenCreatedWithDifferentTypes);
    return UNITY_END();
}

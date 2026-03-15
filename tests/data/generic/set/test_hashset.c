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

/* Tests for HashSet iterator */

static void
_should_returnNonNull_when_getConstIteratorCalledOnHashSet(void)
{
    /* Arrange */
    cutil_Set *const set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);

    /* Act */
    cutil_ConstIterator *const it = cutil_Set_get_const_iterator(set);

    /* Assert */
    TEST_ASSERT_NOT_NULL(it);

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Set_free(set);
}

static void
_should_returnNonNull_when_getIteratorCalledOnHashSet(void)
{
    /* Arrange */
    cutil_Set *const set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);

    /* Act */
    cutil_Iterator *const it = cutil_Set_get_iterator(set);

    /* Assert */
    TEST_ASSERT_NOT_NULL(it);

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Set_free(set);
}

static void
_should_returnFalse_when_nextCalledOnEmptyHashSetIterator(void)
{
    /* Arrange */
    cutil_Set *const set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
    cutil_ConstIterator *const it = cutil_Set_get_const_iterator(set);
    TEST_ASSERT_NOT_NULL(it);

    /* Act / Assert */
    TEST_ASSERT_FALSE(cutil_ConstIterator_next(it));

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Set_free(set);
}

static void
_should_traverseAllElements_when_iteratingHashSet(void)
{
    /* Arrange */
    cutil_Set *const set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
    const int elems[] = {5, 10, 15, 20};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(elems);
    for (size_t i = 0; i < N; ++i) {
        cutil_Set_add(set, &elems[i]);
    }

    cutil_ConstIterator *const it = cutil_Set_get_const_iterator(set);
    TEST_ASSERT_NOT_NULL(it);

    /* Act */
    int found[] = {0, 0, 0, 0};
    int count = 0;
    while (cutil_ConstIterator_next(it)) {
        const int *const elem_ptr = cutil_ConstIterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(elem_ptr);
        for (size_t i = 0; i < N; ++i) {
            if (*elem_ptr == elems[i]) {
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
    cutil_Set_free(set);
}

static void
_should_removeCurrentElement_when_removeCalledOnHashSetIterator(void)
{
    /* Arrange */
    cutil_Set *const set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
    const int insert_elems[] = {1, 2, 3};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(insert_elems);
    for (size_t i = 0; i < N; ++i) {
        cutil_Set_add(set, &insert_elems[i]);
    }

    cutil_Iterator *const it = cutil_Set_get_iterator(set);
    TEST_ASSERT_NOT_NULL(it);

    /* Act */
    TEST_ASSERT_TRUE(cutil_Iterator_next(it));
    const int removed_elem = *(const int *) cutil_Iterator_get_ptr(it);
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, cutil_Iterator_remove(it));

    int remaining_count = 0;
    while (cutil_Iterator_next(it)) {
        ++remaining_count;
    }

    /* Assert */
    TEST_ASSERT_EQUAL_INT(2, remaining_count);
    TEST_ASSERT_EQUAL_size_t(2UL, cutil_Set_get_count(set));
    TEST_ASSERT_FALSE(cutil_Set_contains(set, &removed_elem));

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Set_free(set);
}

static void
_should_returnFailure_when_setCalledOnHashSetIterator(void)
{
    /* Arrange */
    cutil_Set *const set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
    const int elem = 7;
    cutil_Set_add(set, &elem);

    cutil_Iterator *const it = cutil_Set_get_iterator(set);
    TEST_ASSERT_NOT_NULL(it);
    TEST_ASSERT_TRUE(cutil_Iterator_next(it));

    /* Act */
    int new_val = 99;
    const int result = cutil_Iterator_set(it, &new_val);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, result);

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Set_free(set);
}

static void
_should_traverseAllElements_when_constIteratorRewound(void)
{
    /* Arrange */
    cutil_Set *const set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
    const int elems[] = {10, 20, 30, 40};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(elems);
    for (size_t i = 0; i < N; ++i) {
        cutil_Set_add(set, &elems[i]);
    }
    cutil_ConstIterator *const it = cutil_Set_get_const_iterator(set);
    TEST_ASSERT_NOT_NULL(it);

    /* Act */
    int seen1[4] = {0};
    int count1 = 0;
    while (cutil_ConstIterator_next(it)) {
        const int *const ep = cutil_ConstIterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(ep);
        for (size_t i = 0; i < N; ++i) {
            if (*ep == elems[i]) {
                seen1[i] = 1;
            }
        }
        ++count1;
    }

    cutil_ConstIterator_rewind(it);

    int seen2[4] = {0};
    int count2 = 0;
    while (cutil_ConstIterator_next(it)) {
        const int *const ep = cutil_ConstIterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(ep);
        for (size_t i = 0; i < N; ++i) {
            if (*ep == elems[i]) {
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
    cutil_Set_free(set);
}

static void
_should_traverseAllElements_when_iteratorRewound(void)
{
    /* Arrange */
    cutil_Set *const set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
    const int elems[] = {10, 20, 30, 40};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(elems);
    for (size_t i = 0; i < N; ++i) {
        cutil_Set_add(set, &elems[i]);
    }
    cutil_Iterator *const it = cutil_Set_get_iterator(set);
    TEST_ASSERT_NOT_NULL(it);

    /* Act */
    int seen1[4] = {0};
    int count1 = 0;
    while (cutil_Iterator_next(it)) {
        const int *const ep = cutil_Iterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(ep);
        for (size_t i = 0; i < N; ++i) {
            if (*ep == elems[i]) {
                seen1[i] = 1;
            }
        }
        ++count1;
    }

    cutil_Iterator_rewind(it);

    int seen2[4] = {0};
    int count2 = 0;
    while (cutil_Iterator_next(it)) {
        const int *const ep = cutil_Iterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(ep);
        for (size_t i = 0; i < N; ++i) {
            if (*ep == elems[i]) {
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
    cutil_Set_free(set);
}

static void
_should_returnFalse_when_nextCalledAfterExhaustionOnConstIterator(void)
{
    /* Arrange */
    cutil_Set *const set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
    const int elems[] = {1, 2, 3};
    for (size_t i = 0; i < 3; ++i) {
        cutil_Set_add(set, &elems[i]);
    }
    cutil_ConstIterator *const it = cutil_Set_get_const_iterator(set);
    TEST_ASSERT_NOT_NULL(it);

    while (cutil_ConstIterator_next(it)) {
    }

    /* Assert stable-next */
    TEST_ASSERT_FALSE(cutil_ConstIterator_next(it));
    TEST_ASSERT_FALSE(cutil_ConstIterator_next(it));
    TEST_ASSERT_FALSE(cutil_ConstIterator_next(it));

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Set_free(set);
}

static void
_should_returnFalse_when_nextCalledAfterExhaustionOnIterator(void)
{
    /* Arrange */
    cutil_Set *const set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
    const int elems[] = {1, 2, 3};
    for (size_t i = 0; i < 3; ++i) {
        cutil_Set_add(set, &elems[i]);
    }
    cutil_Iterator *const it = cutil_Set_get_iterator(set);
    TEST_ASSERT_NOT_NULL(it);

    while (cutil_Iterator_next(it)) {
    }

    /* Assert stable-next */
    TEST_ASSERT_FALSE(cutil_Iterator_next(it));
    TEST_ASSERT_FALSE(cutil_Iterator_next(it));
    TEST_ASSERT_FALSE(cutil_Iterator_next(it));

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Set_free(set);
}

/* Generic-type descriptor tests */

static void
_should_haveCorrectMetadata_for_hashset_int_descriptor(void)
{
    /* Assert */
    TEST_ASSERT_NOT_NULL(CUTIL_GENERIC_TYPE_HASHSET_INT);
    TEST_ASSERT_TRUE(
      cutil_GenericType_is_valid(CUTIL_GENERIC_TYPE_HASHSET_INT)
    );
    TEST_ASSERT_EQUAL_STRING(
      "cutil_HashSet<int>",
      cutil_GenericType_get_name(CUTIL_GENERIC_TYPE_HASHSET_INT)
    );
    TEST_ASSERT_EQUAL_size_t(
      sizeof(cutil_Set),
      cutil_GenericType_get_size(CUTIL_GENERIC_TYPE_HASHSET_INT)
    );
    TEST_ASSERT_NOT_NULL(CUTIL_GENERIC_TYPE_HASHSET_INT->init);
    TEST_ASSERT_NOT_NULL(CUTIL_GENERIC_TYPE_HASHSET_INT->clear);
    TEST_ASSERT_NOT_NULL(CUTIL_GENERIC_TYPE_HASHSET_INT->copy);
    TEST_ASSERT_NOT_NULL(CUTIL_GENERIC_TYPE_HASHSET_INT->deep_equals);
    TEST_ASSERT_NOT_NULL(CUTIL_GENERIC_TYPE_HASHSET_INT->comp);
    TEST_ASSERT_NOT_NULL(CUTIL_GENERIC_TYPE_HASHSET_INT->hash);
    TEST_ASSERT_NOT_NULL(CUTIL_GENERIC_TYPE_HASHSET_INT->to_string);
}

static void
_should_initHashSet_when_initCalledOnRawMemory(void)
{
    /* Arrange */
    void *const raw_mem = calloc(1, sizeof(cutil_Set));
    TEST_ASSERT_NOT_NULL(raw_mem);

    /* Act */
    cutil_GenericType_apply_init(CUTIL_GENERIC_TYPE_HASHSET_INT, raw_mem);
    cutil_Set *const set = raw_mem;

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(CUTIL_SET_TYPE_HASHSET, set->vtable);
    TEST_ASSERT_EQUAL_size_t(0UL, cutil_Set_get_count(set));
    TEST_ASSERT_EQUAL_STRING(
      cutil_GenericType_get_name(CUTIL_GENERIC_TYPE_INT),
      cutil_GenericType_get_name(cutil_Set_get_elem_type(set))
    );

    /* Cleanup */
    cutil_GenericType_apply_clear(CUTIL_GENERIC_TYPE_HASHSET_INT, set);
    free(set);
}

static void
_should_clearHashSet_when_clearCalledThroughDescriptor(void)
{
    /* Arrange */
    void *const raw_mem = calloc(1, sizeof(cutil_Set));
    TEST_ASSERT_NOT_NULL(raw_mem);
    cutil_GenericType_apply_init(CUTIL_GENERIC_TYPE_HASHSET_INT, raw_mem);
    cutil_Set *const set = raw_mem;
    const int vals[] = {1, 2, 3};
    for (size_t i = 0; i < 3; ++i) {
        cutil_Set_add(set, &vals[i]);
    }
    TEST_ASSERT_EQUAL_size_t(3UL, cutil_Set_get_count(set));
    TEST_ASSERT_NOT_NULL(set->data);

    /* Act */
    cutil_GenericType_apply_clear(CUTIL_GENERIC_TYPE_HASHSET_INT, set);

    /* Assert */
    TEST_ASSERT_NULL(set->data);

    /* Cleanup */
    free(set);
}

static void
_should_copyHashSet_when_copyCalledThroughDescriptor(void)
{
    /* Arrange */
    cutil_Set *const src = CUTIL_CALLOC_OBJECT(src);
    cutil_Set *const dst = CUTIL_CALLOC_OBJECT(dst);
    TEST_ASSERT_NOT_NULL(src);
    TEST_ASSERT_NOT_NULL(dst);
    cutil_GenericType_apply_init(CUTIL_GENERIC_TYPE_HASHSET_INT, src);
    cutil_GenericType_apply_init(CUTIL_GENERIC_TYPE_HASHSET_INT, dst);
    const int src_vals[] = {10, 20, 30};
    for (size_t i = 0; i < 3; ++i) {
        cutil_Set_add(src, &src_vals[i]);
    }

    /* Act */
    cutil_GenericType_apply_copy(CUTIL_GENERIC_TYPE_HASHSET_INT, dst, src);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(3UL, cutil_Set_get_count(dst));
    const int v10 = 10, v20 = 20, v30 = 30;
    TEST_ASSERT_TRUE(cutil_Set_contains(dst, &v10));
    TEST_ASSERT_TRUE(cutil_Set_contains(dst, &v20));
    TEST_ASSERT_TRUE(cutil_Set_contains(dst, &v30));
    /* Independence: modifying dst must not affect src */
    const int v99 = 99;
    cutil_Set_add(dst, &v99);
    TEST_ASSERT_FALSE(cutil_Set_contains(src, &v99));

    /* Cleanup */
    cutil_GenericType_apply_clear(CUTIL_GENERIC_TYPE_HASHSET_INT, src);
    cutil_GenericType_apply_clear(CUTIL_GENERIC_TYPE_HASHSET_INT, dst);
    free(src);
    free(dst);
}

/* Multiple variants */

static void
_should_haveCorrectElemType_for_multipleDescriptorVariants(void)
{
    struct {
        const cutil_GenericType *desc;
        const cutil_GenericType *expected_elem_type;
    } VARIANTS[] = {
      {CUTIL_GENERIC_TYPE_HASHSET_CHAR, CUTIL_GENERIC_TYPE_CHAR},
      {CUTIL_GENERIC_TYPE_HASHSET_I64, CUTIL_GENERIC_TYPE_I64},
      {CUTIL_GENERIC_TYPE_HASHSET_DOUBLE, CUTIL_GENERIC_TYPE_DOUBLE},
    };
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(VARIANTS);

    for (size_t i = 0; i < N; ++i) {
        const cutil_GenericType *const desc = VARIANTS[i].desc;
        const cutil_GenericType *const expected_elem
          = VARIANTS[i].expected_elem_type;

        TEST_ASSERT_NOT_NULL(desc);
        TEST_ASSERT_NOT_NULL(desc->init);

        cutil_Set *const set = CUTIL_CALLOC_OBJECT(set);
        TEST_ASSERT_NOT_NULL(set);
        cutil_GenericType_apply_init(desc, set);

        TEST_ASSERT_EQUAL_STRING(
          cutil_GenericType_get_name(expected_elem),
          cutil_GenericType_get_name(cutil_Set_get_elem_type(set))
        );

        cutil_GenericType_apply_clear(desc, set);
        free(set);
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

    /* Iterator tests */
    RUN_TEST(_should_returnNonNull_when_getConstIteratorCalledOnHashSet);
    RUN_TEST(_should_returnNonNull_when_getIteratorCalledOnHashSet);
    RUN_TEST(_should_returnFalse_when_nextCalledOnEmptyHashSetIterator);
    RUN_TEST(_should_traverseAllElements_when_iteratingHashSet);
    RUN_TEST(_should_removeCurrentElement_when_removeCalledOnHashSetIterator);
    RUN_TEST(_should_returnFailure_when_setCalledOnHashSetIterator);
    RUN_TEST(_should_traverseAllElements_when_constIteratorRewound);
    RUN_TEST(_should_traverseAllElements_when_iteratorRewound);
    RUN_TEST(_should_returnFalse_when_nextCalledAfterExhaustionOnConstIterator);
    RUN_TEST(_should_returnFalse_when_nextCalledAfterExhaustionOnIterator);

    /* Generic-type descriptor tests */
    RUN_TEST(_should_haveCorrectMetadata_for_hashset_int_descriptor);
    RUN_TEST(_should_initHashSet_when_initCalledOnRawMemory);
    RUN_TEST(_should_clearHashSet_when_clearCalledThroughDescriptor);
    RUN_TEST(_should_copyHashSet_when_copyCalledThroughDescriptor);

    /* Multiple variants */
    RUN_TEST(_should_haveCorrectElemType_for_multipleDescriptorVariants);

    return UNITY_END();
}

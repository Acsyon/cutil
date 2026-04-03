#include "unity.h"

#include <cutil/data/generic/array.h>
#include <cutil/data/generic/list.h>
#include <cutil/data/generic/list/arraylist.h>
#include <cutil/data/generic/type.h>
#include <cutil/std/stdlib.h>
#include <cutil/util/macro.h>

static void
_should_haveZeroCapacity_when_newlyAllocated(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);

    /* Act */
    const size_t cap = cutil_ArrayList_get_capacity(list);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0UL, cap);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_increaseCapacity_when_firstElementAppended(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);

    /* Act */
    const int v = 1;
    cutil_List_append(list, &v);

    /* Assert */
    TEST_ASSERT_GREATER_THAN_size_t(0UL, cutil_ArrayList_get_capacity(list));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_haveCapacityGtOrEqLength_when_elementsAppended(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);

    /* Act / Assert */
    for (int i = 0; i < 20; ++i) {
        cutil_List_append(list, &i);
        TEST_ASSERT_GREATER_OR_EQUAL_size_t(
          cutil_List_get_count(list), cutil_ArrayList_get_capacity(list)
        );
    }

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_doubleCapacity_when_belowThreshold(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    cutil_ArrayList_resize(list, 1UL);
    const size_t cap_before = cutil_ArrayList_get_capacity(list);
    const int v = 0;
    for (size_t i = 0; i < cap_before; ++i) {
        cutil_List_append(list, &v);
    }

    /* Act */
    cutil_List_append(list, &v);
    const size_t cap_after = cutil_ArrayList_get_capacity(list);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(cap_before * 2UL, cap_after);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_growLinearly_when_atOrAboveThreshold(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    const size_t threshold = 1024UL * 1024UL;
    cutil_ArrayList_resize(list, threshold);
    TEST_ASSERT_EQUAL_size_t(0UL, cutil_List_get_count(list));
    TEST_ASSERT_EQUAL_size_t(threshold, cutil_ArrayList_get_capacity(list));

    const int v = 0;
    for (size_t i = 0; i < threshold; ++i) {
        cutil_List_append(list, &v);
    }

    /* Act */
    cutil_List_append(list, &v);
    const size_t cap_after = cutil_ArrayList_get_capacity(list);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(2UL * threshold, cap_after);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_setCapacityExactly_when_resizeCalled(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);

    /* Act */
    cutil_ArrayList_resize(list, 10UL);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(10UL, cutil_ArrayList_get_capacity(list));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_preserveElements_when_growingViaResize(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    const int vals[] = {1, 2, 3};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    cutil_ArrayList_resize(list, 10UL);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(1, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(2, *(const int *) cutil_List_get_ptr(list, 1UL));
    TEST_ASSERT_EQUAL_INT(3, *(const int *) cutil_List_get_ptr(list, 2UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_truncateElements_when_shrinkingViaResize(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    const int vals[] = {1, 2, 3, 4, 5};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    cutil_ArrayList_resize(list, 2UL);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(2UL, cutil_List_get_count(list));
    TEST_ASSERT_EQUAL_INT(1, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(2, *(const int *) cutil_List_get_ptr(list, 1UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnSuccess_when_resizeCalledWithZero(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    const int v = 7;
    cutil_List_append(list, &v);

    /* Act */
    const int rc = cutil_ArrayList_resize(list, 0UL);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, rc);
    TEST_ASSERT_EQUAL_size_t(0UL, cutil_List_get_count(list));
    TEST_ASSERT_EQUAL_size_t(0UL, cutil_ArrayList_get_capacity(list));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_matchCapacityToLength_when_shrinkToFitCalled(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    /* Force several growth events */
    for (int i = 0; i < 10; ++i) {
        cutil_List_append(list, &i);
    }

    /* Act */
    cutil_ArrayList_shrink_to_fit(list);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(
      cutil_List_get_count(list), cutil_ArrayList_get_capacity(list)
    );

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_setZeroCapacity_when_shrinkCalledOnEmptyList(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    cutil_ArrayList_resize(list, 100UL);

    /* Act */
    cutil_ArrayList_shrink_to_fit(list);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0UL, cutil_ArrayList_get_capacity(list));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_preserveElements_when_shrinkToFitCalled(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    const int vals[] = {10, 20, 30};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }
    cutil_ArrayList_resize(list, 50UL);

    /* Act */
    cutil_ArrayList_shrink_to_fit(list);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(3UL, cutil_List_get_count(list));
    TEST_ASSERT_EQUAL_INT(10, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(20, *(const int *) cutil_List_get_ptr(list, 1UL));
    TEST_ASSERT_EQUAL_INT(30, *(const int *) cutil_List_get_ptr(list, 2UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_removeCurrentElement_when_removeCalledOnIterator(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    const int vals[] = {1, 2, 3};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }
    cutil_Iterator *const it = cutil_List_get_iterator(list);
    cutil_Iterator_next(it); /* advance to index 0 */

    /* Act */
    cutil_Iterator_remove(it);

    int remaining[2] = {0};
    int idx = 0;
    while (cutil_Iterator_next(it)) {
        remaining[idx++] = *(const int *) cutil_Iterator_get_ptr(it);
    }
    cutil_Iterator_free(it);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(2UL, cutil_List_get_count(list));
    TEST_ASSERT_EQUAL_INT(2, remaining[0]);
    TEST_ASSERT_EQUAL_INT(3, remaining[1]);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_removeAllElements_when_iteratorRemoveCalledRepeatedly(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    for (int i = 0; i < 5; ++i) {
        cutil_List_append(list, &i);
    }
    cutil_Iterator *const it = cutil_List_get_iterator(list);

    /* Act */
    while (cutil_Iterator_next(it)) {
        cutil_Iterator_remove(it);
    }
    cutil_Iterator_free(it);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0UL, cutil_List_get_count(list));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_removeMiddleElement_when_iteratorRemoveCalled(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    const int vals[] = {10, 20, 30, 40};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }
    cutil_Iterator *const it = cutil_List_get_iterator(list);
    cutil_Iterator_next(it);
    cutil_Iterator_next(it);

    /* Act */
    cutil_Iterator_remove(it);

    int remaining[3] = {0};
    int idx = 0;

    remaining[0] = *(const int *) cutil_List_get_ptr(list, 0UL);
    while (cutil_Iterator_next(it)) {
        remaining[1 + idx] = *(const int *) cutil_Iterator_get_ptr(it);
        ++idx;
    }
    cutil_Iterator_free(it);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(3UL, cutil_List_get_count(list));
    TEST_ASSERT_EQUAL_INT(10, remaining[0]);
    TEST_ASSERT_EQUAL_INT(30, remaining[1]);
    TEST_ASSERT_EQUAL_INT(40, remaining[2]);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_skipNoElements_when_removingDuringTraversal(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    for (int i = 0; i < 10; ++i) {
        cutil_List_append(list, &i);
    }
    cutil_Iterator *const it = cutil_List_get_iterator(list);

    /* Act */
    while (cutil_Iterator_next(it)) {
        const int *const p = (const int *) cutil_Iterator_get_ptr(it);
        if (*p % 2 == 0) {
            cutil_Iterator_remove(it);
        }
    }
    cutil_Iterator_free(it);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(5UL, cutil_List_get_count(list));
    const int expected[] = {1, 3, 5, 7, 9};
    const size_t num_expected = CUTIL_GET_NATIVE_ARRAY_SIZE(expected);
    for (size_t i = 0; i < num_expected; ++i) {
        TEST_ASSERT_EQUAL_INT(
          expected[i], *(const int *) cutil_List_get_ptr(list, i)
        );
    }

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnTrue_when_identicalListsCompared(void)
{
    /* Arrange */
    cutil_List *const list_a = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    cutil_List *const list_b = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    const int vals[] = {1, 2, 3};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list_a, &vals[i]);
        cutil_List_append(list_b, &vals[i]);
    }

    /* Act */
    const cutil_Bool result = cutil_List_deep_equals(list_a, list_b);

    /* Assert */
    TEST_ASSERT_TRUE(result);

    /* Cleanup */
    cutil_List_free(list_a);
    cutil_List_free(list_b);
}

static void
_should_returnFalse_when_listsWithDifferentOrderCompared(void)
{
    /* Arrange */
    cutil_List *const list_a = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    cutil_List *const list_b = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    const int a_vals[] = {1, 2, 3};
    const int b_vals[] = {3, 2, 1};
    for (int i = 0; i < 3; ++i) {
        cutil_List_append(list_a, &a_vals[i]);
        cutil_List_append(list_b, &b_vals[i]);
    }

    /* Act */
    const cutil_Bool result = cutil_List_deep_equals(list_a, list_b);

    /* Assert */
    TEST_ASSERT_FALSE(result);

    /* Cleanup */
    cutil_List_free(list_a);
    cutil_List_free(list_b);
}

static void
_should_returnFalse_when_listsWithDifferentLengthsCompared(void)
{
    /* Arrange */
    cutil_List *const list_a = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    cutil_List *const list_b = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    const int vals[] = {1, 2, 3};
    for (int i = 0; i < 2; ++i) {
        cutil_List_append(list_a, &vals[i]);
    }
    for (int i = 0; i < 3; ++i) {
        cutil_List_append(list_b, &vals[i]);
    }

    /* Act */
    const cutil_Bool result = cutil_List_deep_equals(list_a, list_b);

    /* Assert */
    TEST_ASSERT_FALSE(result);

    /* Cleanup */
    cutil_List_free(list_a);
    cutil_List_free(list_b);
}

static void
_smoke_test_arraylist_generic_type(const cutil_GenericType *gt)
{
    /* Arrange */
    static const size_t NUM_SLOTS = 2UL;

    /* Act */
    cutil_Array *const arr = cutil_Array_alloc(gt, NUM_SLOTS);

    /* Assert */
    TEST_ASSERT_NOT_NULL(arr);
    TEST_ASSERT_NOT_NULL(gt);
    TEST_ASSERT_NOT_NULL(gt->name);
    TEST_ASSERT_EQUAL_size_t(sizeof(cutil_List), gt->size);
    for (size_t i = 0; i < NUM_SLOTS; ++i) {
        const cutil_List *const list = cutil_Array_get_ptr(arr, i);
        TEST_ASSERT_NOT_NULL(list->vtable);
        TEST_ASSERT_EQUAL_STRING("cutil_ArrayList", list->vtable->name);
    }

    /* Cleanup */
    cutil_Array_free(arr);
}

static void
_should_initAndClearAll_when_genericTypeSingletonsUsed(void)
{
    const cutil_GenericType *const ARRAYLIST_TYPES[] = {
      CUTIL_GENERIC_TYPE_ARRAYLIST_CHAR,   CUTIL_GENERIC_TYPE_ARRAYLIST_SHORT,
      CUTIL_GENERIC_TYPE_ARRAYLIST_INT,    CUTIL_GENERIC_TYPE_ARRAYLIST_LONG,
      CUTIL_GENERIC_TYPE_ARRAYLIST_LLONG,  CUTIL_GENERIC_TYPE_ARRAYLIST_UCHAR,
      CUTIL_GENERIC_TYPE_ARRAYLIST_USHORT, CUTIL_GENERIC_TYPE_ARRAYLIST_UINT,
      CUTIL_GENERIC_TYPE_ARRAYLIST_ULONG,  CUTIL_GENERIC_TYPE_ARRAYLIST_ULLONG,
      CUTIL_GENERIC_TYPE_ARRAYLIST_I8,     CUTIL_GENERIC_TYPE_ARRAYLIST_I16,
      CUTIL_GENERIC_TYPE_ARRAYLIST_I32,    CUTIL_GENERIC_TYPE_ARRAYLIST_I64,
      CUTIL_GENERIC_TYPE_ARRAYLIST_U8,     CUTIL_GENERIC_TYPE_ARRAYLIST_U16,
      CUTIL_GENERIC_TYPE_ARRAYLIST_U32,    CUTIL_GENERIC_TYPE_ARRAYLIST_U64,
      CUTIL_GENERIC_TYPE_ARRAYLIST_SIZET,  CUTIL_GENERIC_TYPE_ARRAYLIST_HASHT,
      CUTIL_GENERIC_TYPE_ARRAYLIST_FLOAT,  CUTIL_GENERIC_TYPE_ARRAYLIST_DOUBLE,
      CUTIL_GENERIC_TYPE_ARRAYLIST_LDOUBLE
    };
    static const size_t NUM_ARRAYLIST_TYPES
      = CUTIL_GET_NATIVE_ARRAY_SIZE(ARRAYLIST_TYPES);

    for (size_t i = 0; i < NUM_ARRAYLIST_TYPES; ++i) {
        _smoke_test_arraylist_generic_type(ARRAYLIST_TYPES[i]);
    }
}

static void
_should_appendToNestedList_when_arrayListGenericTypeUsed(void)
{
    /* Arrange */
    const cutil_GenericType *const elem_type = CUTIL_GENERIC_TYPE_ARRAYLIST_INT;
    cutil_Array *const outer = cutil_Array_alloc(elem_type, 1UL);
    cutil_List *const inner = CUTIL_CONST_CAST(cutil_Array_get_ptr(outer, 0UL));
    const int val = 42;

    /* Act */
    cutil_List_append(inner, &val);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(1UL, cutil_List_get_count(inner));
    TEST_ASSERT_EQUAL_INT(val, *(const int *) cutil_List_get_ptr(inner, 0UL));

    /* Cleanup */
    cutil_Array_free(outer);
}

static void
_should_preserveCapacity_when_setCalled(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    cutil_ArrayList_resize(list, 10UL);
    const int v = 5;
    cutil_List_append(list, &v);
    const size_t cap_before = cutil_ArrayList_get_capacity(list);

    /* Act */
    const int new_val = 99;
    cutil_List_set(list, 0UL, &new_val);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(cap_before, cutil_ArrayList_get_capacity(list));
    TEST_ASSERT_EQUAL_size_t(1UL, cutil_List_get_count(list));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_setFirstElement_when_indexIsZero(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    const int vals[] = {1, 2, 3};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    const int new_val = 100;
    cutil_List_set(list, 0UL, &new_val);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(100, *(const int *) cutil_List_get_ptr(list, 0UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_setLastElement_when_indexIsLengthMinusOne(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    const int vals[] = {1, 2, 3};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    const int new_val = 200;
    cutil_List_set(list, 2UL, &new_val);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(200, *(const int *) cutil_List_get_ptr(list, 2UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_swapBidirectionally_when_memswapElemCalled(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    const int v_a = 7;
    cutil_List_append(list, &v_a);
    int v_b = 13;

    /* Act */
    const int rc = cutil_List_memswap_elem(list, 0UL, &v_b);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, rc);
    TEST_ASSERT_EQUAL_INT(13, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(7, v_b);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnFailure_when_memswapElemOutOfBounds(void)
{
    /* Arrange */
    cutil_List *const list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
    const int val = 5;
    cutil_List_append(list, &val);
    int buf = 99;

    /* Act */
    const int rc = cutil_List_memswap_elem(list, 1UL, &buf);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, rc);
    TEST_ASSERT_EQUAL_INT(val, *(const int *) cutil_List_get_ptr(list, 0UL));

    /* Cleanup */
    cutil_List_free(list);
}

/* ==========================================================================
 * main
 * ========================================================================== */

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

    RUN_TEST(_should_haveZeroCapacity_when_newlyAllocated);
    RUN_TEST(_should_increaseCapacity_when_firstElementAppended);
    RUN_TEST(_should_haveCapacityGtOrEqLength_when_elementsAppended);

    RUN_TEST(_should_doubleCapacity_when_belowThreshold);
    RUN_TEST(_should_growLinearly_when_atOrAboveThreshold);

    RUN_TEST(_should_setCapacityExactly_when_resizeCalled);
    RUN_TEST(_should_preserveElements_when_growingViaResize);
    RUN_TEST(_should_truncateElements_when_shrinkingViaResize);
    RUN_TEST(_should_returnSuccess_when_resizeCalledWithZero);

    RUN_TEST(_should_matchCapacityToLength_when_shrinkToFitCalled);
    RUN_TEST(_should_setZeroCapacity_when_shrinkCalledOnEmptyList);
    RUN_TEST(_should_preserveElements_when_shrinkToFitCalled);

    RUN_TEST(_should_removeCurrentElement_when_removeCalledOnIterator);
    RUN_TEST(_should_removeAllElements_when_iteratorRemoveCalledRepeatedly);
    RUN_TEST(_should_removeMiddleElement_when_iteratorRemoveCalled);
    RUN_TEST(_should_skipNoElements_when_removingDuringTraversal);

    RUN_TEST(_should_returnTrue_when_identicalListsCompared);
    RUN_TEST(_should_returnFalse_when_listsWithDifferentOrderCompared);
    RUN_TEST(_should_returnFalse_when_listsWithDifferentLengthsCompared);

    RUN_TEST(_should_initAndClearAll_when_genericTypeSingletonsUsed);
    RUN_TEST(_should_appendToNestedList_when_arrayListGenericTypeUsed);

    RUN_TEST(_should_preserveCapacity_when_setCalled);
    RUN_TEST(_should_setFirstElement_when_indexIsZero);
    RUN_TEST(_should_setLastElement_when_indexIsLengthMinusOne);
    RUN_TEST(_should_swapBidirectionally_when_memswapElemCalled);
    RUN_TEST(_should_returnFailure_when_memswapElemOutOfBounds);

    return UNITY_END();
}

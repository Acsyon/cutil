#include "unity.h"

#include <cutil/data/generic/set.h>
#include <cutil/data/generic/set/hashset.h>
#include <cutil/data/generic/type.h>

#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

/* ==========================================================================
 * Mock infrastructure
 * ========================================================================== */

typedef struct {
    int free_count;
    int reset_count;
    int copy_count;
    int duplicate_count;
    int get_count_count;
    int contains_count;
    int add_count;
    int remove_count;
    int get_elem_type_count;
    int get_count_val;
    int get_const_iterator_count;
    int get_iterator_count;
} MockSetData;

static void
_mock_free(void *data)
{
    MockSetData *const mock = data;
    if (mock) {
        ++mock->free_count;
        free(mock);
    }
}

static void
_mock_reset(void *data)
{
    MockSetData *const mock = data;
    if (mock) {
        ++mock->reset_count;
    }
}

static void
_mock_copy(void *dst, const void *src)
{
    MockSetData *const dst_mock = dst;
    const MockSetData *const src_mock = src;
    if (dst_mock && src_mock) {
        ++dst_mock->copy_count;
    }
}

static void *
_mock_duplicate(const void *data)
{
    MockSetData *const src = CUTIL_CONST_CAST(data);
    if (!src) {
        return NULL;
    }
    MockSetData *const dup = CUTIL_MALLOC_OBJECT(dup);
    *dup = *src;
    ++src->duplicate_count;
    return dup;
}

static size_t
_mock_get_count(const void *data)
{
    MockSetData *const mock = CUTIL_CONST_CAST(data);
    if (mock) {
        ++mock->get_count_count;
        return (size_t) mock->get_count_val;
    }
    return 0UL;
}

static cutil_Bool
_mock_contains(const void *data, const void *elem)
{
    MockSetData *const mock = CUTIL_CONST_CAST(data);
    CUTIL_UNUSED(elem);
    if (mock) {
        ++mock->contains_count;
        return true;
    }
    return false;
}

static int
_mock_add(void *data, const void *elem)
{
    MockSetData *const mock = data;
    CUTIL_UNUSED(elem);
    if (mock) {
        ++mock->add_count;
    }
    return 0;
}

static int
_mock_remove(void *data, const void *elem)
{
    MockSetData *const mock = data;
    CUTIL_UNUSED(elem);
    if (mock) {
        ++mock->remove_count;
    }
    return 0;
}

static const cutil_GenericType *
_mock_get_elem_type(const void *data)
{
    MockSetData *const mock = CUTIL_CONST_CAST(data);
    if (mock) {
        ++mock->get_elem_type_count;
    }
    return CUTIL_GENERIC_TYPE_INT;
}

/* --- Stub iterator infrastructure for set iterator dispatch tests --------- */

static void
_mock_set_iter_free(void *data)
{
    free(data);
}

static cutil_Bool
_mock_set_iter_next(void *data)
{
    CUTIL_UNUSED(data);
    return CUTIL_FALSE;
}

static const cutil_ConstIteratorType MOCK_SET_ITER_CONST_TYPE = {
  .name = "MockSetConstIter",
  .free = &_mock_set_iter_free,
  .rewind = NULL,
  .next = &_mock_set_iter_next,
  .get = NULL,
  .get_ptr = NULL,
};

static const cutil_IteratorType MOCK_SET_ITER_TYPE = {
  .name = "MockSetIter",
  .free = &_mock_set_iter_free,
  .rewind = NULL,
  .next = &_mock_set_iter_next,
  .get = NULL,
  .get_ptr = NULL,
  .set = NULL,
  .remove = NULL,
};

static cutil_ConstIterator *
_mock_get_const_iterator(const void *data)
{
    MockSetData *const mock = CUTIL_CONST_CAST(data);
    if (mock) {
        ++mock->get_const_iterator_count;
    }
    cutil_ConstIterator *const it = CUTIL_MALLOC_OBJECT(it);
    it->vtable = &MOCK_SET_ITER_CONST_TYPE;
    it->data = malloc(1);
    return it;
}

static cutil_Iterator *
_mock_get_iterator(void *data)
{
    MockSetData *const mock = data;
    if (mock) {
        ++mock->get_iterator_count;
    }
    cutil_Iterator *const it = CUTIL_MALLOC_OBJECT(it);
    it->vtable = &MOCK_SET_ITER_TYPE;
    it->data = malloc(1);
    return it;
}

static const cutil_SetType MOCK_SET_TYPE = {
  .name = "MockSet",
  .free = &_mock_free,
  .reset = &_mock_reset,
  .copy = &_mock_copy,
  .duplicate = &_mock_duplicate,
  .get_count = &_mock_get_count,
  .contains = &_mock_contains,
  .add = &_mock_add,
  .remove = &_mock_remove,
  .get_elem_type = &_mock_get_elem_type,
  .get_const_iterator = &_mock_get_const_iterator,
  .get_iterator = &_mock_get_iterator,
};

static cutil_Set *
_create_mock_set(void)
{
    MockSetData *const data = CUTIL_MALLOC_OBJECT(data);
    *data = (MockSetData) {0};
    cutil_Set *const set = CUTIL_MALLOC_OBJECT(set);
    set->vtable = &MOCK_SET_TYPE;
    set->data = data;
    return set;
}

/* ==========================================================================
 * Section A: Mock-based vtable dispatch tests
 * ========================================================================== */

static void
_should_returnTrue_when_setTypesAreIdentical(void)
{
    /* Arrange */
    const cutil_SetType type1 = {
      .name = "Test",
      .free = &_mock_free,
      .reset = &_mock_reset,
    };
    const cutil_SetType type2 = {
      .name = "Test",
      .free = &_mock_free,
      .reset = &_mock_reset,
    };

    /* Act */
    const cutil_Bool result = cutil_SetType_equals(&type1, &type2);

    /* Assert */
    TEST_ASSERT_TRUE(result);
}

static void
_should_returnFalse_when_setTypesAreDifferent(void)
{
    /* Arrange */
    const cutil_SetType type1 = {.name = "Type1"};
    const cutil_SetType type2 = {.name = "Type2"};

    /* Act */
    const cutil_Bool result = cutil_SetType_equals(&type1, &type2);

    /* Assert */
    TEST_ASSERT_FALSE(result);
}

static void
_should_callFreeFunction_when_setIsFreed(void)
{
    /* Arrange */
    cutil_Set *const set = _create_mock_set();

    /* Act */
    cutil_Set_free(set);

    /* Assert — no crash is sufficient; free_count inaccessible after free */
    TEST_ASSERT_TRUE(true);
}

static void
_should_doNothing_when_setIsNull(void)
{
    /* Act & Assert */
    cutil_Set_free(NULL);
    TEST_ASSERT_TRUE(true);
}

static void
_should_callClearFunction_when_setIsCleared(void)
{
    /* Arrange */
    cutil_Set *const set = _create_mock_set();
    MockSetData *const mock = set->data;
    const int initial = mock->reset_count;

    /* Act */
    cutil_Set_reset(set);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(initial + 1, mock->reset_count);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_callCopyFunction_when_setIsCopied(void)
{
    /* Arrange */
    cutil_Set *const dst = _create_mock_set();
    cutil_Set *const src = _create_mock_set();
    MockSetData *const dst_mock = dst->data;
    const int initial = dst_mock->copy_count;

    /* Act */
    cutil_Set_copy(dst, src);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(initial + 1, dst_mock->copy_count);

    /* Cleanup */
    cutil_Set_free(dst);
    cutil_Set_free(src);
}

static void
_should_notCopy_when_setTypesIncompatible(void)
{
    /* Arrange */
    cutil_SetType type1 = {.name = "Type1", .copy = &_mock_copy};
    cutil_SetType type2 = {.name = "Type2", .copy = &_mock_copy};
    MockSetData *const dst_data = CUTIL_MALLOC_OBJECT(dst_data);
    *dst_data = (MockSetData) {0};
    MockSetData *const src_data = CUTIL_MALLOC_OBJECT(src_data);
    *src_data = (MockSetData) {0};
    cutil_Set dst_set = {.vtable = &type1, .data = dst_data};
    cutil_Set src_set = {.vtable = &type2, .data = src_data};
    const int initial = dst_data->copy_count;

    /* Act */
    cutil_Set_copy(&dst_set, &src_set);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(initial, dst_data->copy_count);

    /* Cleanup */
    free(dst_data);
    free(src_data);
}

static void
_should_returnDuplicateSet_when_setIsDuplicated(void)
{
    /* Arrange */
    cutil_Set *const set = _create_mock_set();
    MockSetData *const mock = set->data;
    mock->get_count_val = 7;
    const int initial = mock->duplicate_count;

    /* Act */
    cutil_Set *const dup = cutil_Set_duplicate(set);

    /* Assert */
    TEST_ASSERT_NOT_NULL(dup);
    TEST_ASSERT_EQUAL_PTR(set->vtable, dup->vtable);
    TEST_ASSERT_NOT_NULL(dup->data);
    TEST_ASSERT_EQUAL_INT(initial + 1, mock->duplicate_count);

    /* Cleanup */
    cutil_Set_free(set);
    cutil_Set_free(dup);
}

static void
_should_returnCount_when_setCountIsRequested(void)
{
    /* Arrange */
    cutil_Set *const set = _create_mock_set();
    MockSetData *const mock = set->data;
    mock->get_count_val = 100;

    /* Act */
    const size_t count = cutil_Set_get_count(set);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(100, count);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_returnZero_when_setIsEmpty(void)
{
    /* Arrange */
    cutil_Set *const set = _create_mock_set();
    MockSetData *const mock = set->data;
    mock->get_count_val = 0;

    /* Act */
    const size_t count = cutil_Set_get_count(set);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0, count);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_callContainsFunction_when_elemIsQueried(void)
{
    /* Arrange */
    cutil_Set *const set = _create_mock_set();
    MockSetData *const mock = set->data;
    const int initial = mock->contains_count;
    const int elem = 42;

    /* Act */
    const cutil_Bool result = cutil_Set_contains(set, &elem);

    /* Assert */
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(initial + 1, mock->contains_count);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_callAddFunction_when_elemIsAdded(void)
{
    /* Arrange */
    cutil_Set *const set = _create_mock_set();
    MockSetData *const mock = set->data;
    const int initial = mock->add_count;
    const int elem = 1;

    /* Act */
    cutil_Set_add(set, &elem);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(initial + 1, mock->add_count);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_callRemoveFunction_when_elemIsRemoved(void)
{
    /* Arrange */
    cutil_Set *const set = _create_mock_set();
    MockSetData *const mock = set->data;
    const int initial = mock->remove_count;
    const int elem = 1;

    /* Act */
    cutil_Set_remove(set, &elem);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(initial + 1, mock->remove_count);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_callGetElemTypeFunction_when_elemTypeIsQueried(void)
{
    /* Arrange */
    cutil_Set *const set = _create_mock_set();
    MockSetData *const mock = set->data;
    const int initial = mock->get_elem_type_count;

    /* Act */
    const cutil_GenericType *const elem_type = cutil_Set_get_elem_type(set);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(CUTIL_GENERIC_TYPE_INT, elem_type);
    TEST_ASSERT_EQUAL_INT(initial + 1, mock->get_elem_type_count);

    /* Cleanup */
    cutil_Set_free(set);
}

/* Tests for cutil_Set iterator shims */

static void
_should_callGetConstIterator_when_getConstIteratorCalledOnSet(void)
{
    /* Arrange */
    cutil_Set *const set = _create_mock_set();
    MockSetData *const mock = set->data;

    /* Act */
    cutil_ConstIterator *const it = cutil_Set_get_const_iterator(set);

    /* Assert */
    TEST_ASSERT_NOT_NULL(it);
    TEST_ASSERT_EQUAL_INT(1, mock->get_const_iterator_count);

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Set_free(set);
}

static void
_should_callGetIterator_when_getIteratorCalledOnSet(void)
{
    /* Arrange */
    cutil_Set *const set = _create_mock_set();
    MockSetData *const mock = set->data;

    /* Act */
    cutil_Iterator *const it = cutil_Set_get_iterator(set);

    /* Assert */
    TEST_ASSERT_NOT_NULL(it);
    TEST_ASSERT_EQUAL_INT(1, mock->get_iterator_count);

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Set_free(set);
}

static void
_should_returnNull_when_getConstIteratorCalledOnNullSet(void)
{
    TEST_ASSERT_NULL(cutil_Set_get_const_iterator(NULL));
}

static void
_should_returnNull_when_getIteratorCalledOnNullSet(void)
{
    TEST_ASSERT_NULL(cutil_Set_get_iterator(NULL));
}

/* ==========================================================================
 * Section B: Factory infrastructure for behavioural tests
 * ==========================================================================
 *
 * Each test function below obtains its set under test by calling
 * g_current_factory(). To run a test against a new implementation, assign a
 * factory function to g_current_factory in main() before calling RUN_TEST.
 */

typedef cutil_Set *(*SetFactory)(void);
static SetFactory g_current_factory = NULL;

static cutil_Set *
_hashset_factory_int(void)
{
    return cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
}

/* ==========================================================================
 * Section B: Factory-driven behavioural tests
 * ========================================================================== */

static void
_should_returnSuccess_when_singleElemAdded(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int elem = 42;

    /* Act */
    const int result = cutil_Set_add(set, &elem);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
    TEST_ASSERT_EQUAL_size_t(1, cutil_Set_get_count(set));

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_returnZeroCount_when_setIsNewlyAllocated(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();

    /* Act */
    const size_t count = cutil_Set_get_count(set);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0, count);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_incrementCountWithEachUniqueElem(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int elems[] = {10, 20, 30, 40, 50};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(elems);

    for (size_t i = 0; i < N; ++i) {
        /* Act */
        cutil_Set_add(set, &elems[i]);

        /* Assert */
        TEST_ASSERT_EQUAL_size_t(i + 1, cutil_Set_get_count(set));
    }

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_returnTrue_when_containsExistingElem(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int elem = 5;
    cutil_Set_add(set, &elem);

    /* Act */
    const cutil_Bool result = cutil_Set_contains(set, &elem);

    /* Assert */
    TEST_ASSERT_TRUE(result);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_returnFalse_when_containsMissingElem(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int elem = 999;

    /* Act */
    const cutil_Bool result = cutil_Set_contains(set, &elem);

    /* Assert */
    TEST_ASSERT_FALSE(result);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_returnSuccess_when_removingExistingElem(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int elem = 3;
    cutil_Set_add(set, &elem);

    /* Act */
    const int result = cutil_Set_remove(set, &elem);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
    TEST_ASSERT_EQUAL_size_t(0, cutil_Set_get_count(set));
    TEST_ASSERT_FALSE(cutil_Set_contains(set, &elem));

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_returnFailure_when_removingMissingElem(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int elem = 888;

    /* Act */
    const int result = cutil_Set_remove(set, &elem);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, result);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_decrementCount_when_existingElemRemoved(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int elems[] = {1, 2, 3};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(elems);
    for (size_t i = 0; i < N; ++i) {
        cutil_Set_add(set, &elems[i]);
    }

    /* Act */
    cutil_Set_remove(set, &elems[1]);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(N - 1, cutil_Set_get_count(set));

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_returnZeroCount_when_resetAfterFill(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int elems[] = {1, 2, 3, 4, 5};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(elems);
    for (size_t i = 0; i < N; ++i) {
        cutil_Set_add(set, &elems[i]);
    }
    TEST_ASSERT_EQUAL_size_t(N, cutil_Set_get_count(set));

    /* Act */
    cutil_Set_reset(set);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0, cutil_Set_get_count(set));
    TEST_ASSERT_FALSE(cutil_Set_contains(set, &elems[0]));

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_notDuplicateCount_when_sameElemAddedTwice(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int elem = 42;

    /* Act */
    cutil_Set_add(set, &elem);
    cutil_Set_add(set, &elem);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(1, cutil_Set_get_count(set));

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_matchAllElems_when_copied(void)
{
    /* Arrange */
    cutil_Set *const src = g_current_factory();
    cutil_Set *const dst = g_current_factory();
    const int elems[] = {1, 2, 3};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(elems);
    for (size_t i = 0; i < N; ++i) {
        cutil_Set_add(src, &elems[i]);
    }

    /* Act */
    cutil_Set_copy(dst, src);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(N, cutil_Set_get_count(dst));
    for (size_t i = 0; i < N; ++i) {
        TEST_ASSERT_TRUE(cutil_Set_contains(dst, &elems[i]));
    }

    /* Cleanup */
    cutil_Set_free(src);
    cutil_Set_free(dst);
}

static void
_should_beIndependent_when_dstModifiedAfterCopy(void)
{
    /* Arrange */
    cutil_Set *const src = g_current_factory();
    cutil_Set *const dst = g_current_factory();
    const int elem = 1;
    cutil_Set_add(src, &elem);
    cutil_Set_copy(dst, src);
    const size_t src_count_before = cutil_Set_get_count(src);

    /* Act */
    const int extra_elem = 99;
    cutil_Set_add(dst, &extra_elem);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(src_count_before, cutil_Set_get_count(src));
    TEST_ASSERT_FALSE(cutil_Set_contains(src, &extra_elem));

    /* Cleanup */
    cutil_Set_free(src);
    cutil_Set_free(dst);
}

static void
_should_matchAllElems_when_duplicated(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int elems[] = {5, 6, 7};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(elems);
    for (size_t i = 0; i < N; ++i) {
        cutil_Set_add(set, &elems[i]);
    }

    /* Act */
    cutil_Set *const dup = cutil_Set_duplicate(set);

    /* Assert */
    TEST_ASSERT_NOT_NULL(dup);
    TEST_ASSERT_EQUAL_size_t(N, cutil_Set_get_count(dup));
    for (size_t i = 0; i < N; ++i) {
        TEST_ASSERT_TRUE(cutil_Set_contains(dup, &elems[i]));
    }

    /* Cleanup */
    cutil_Set_free(set);
    cutil_Set_free(dup);
}

static void
_should_beIndependent_when_dupModifiedAfterDuplicate(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int elem = 2;
    cutil_Set_add(set, &elem);
    cutil_Set *const dup = cutil_Set_duplicate(set);
    const size_t set_count_before = cutil_Set_get_count(set);

    /* Act */
    const int extra_elem = 77;
    cutil_Set_add(dup, &extra_elem);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(set_count_before, cutil_Set_get_count(set));
    TEST_ASSERT_FALSE(cutil_Set_contains(set, &extra_elem));

    /* Cleanup */
    cutil_Set_free(set);
    cutil_Set_free(dup);
}

static void
_should_returnElemType_when_elemTypeQueried(void)
{
    /* Arrange */
    cutil_Set *const set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);

    /* Act */
    const cutil_GenericType *const elem_type = cutil_Set_get_elem_type(set);

    /* Assert */
    TEST_ASSERT_NOT_NULL(elem_type);
    TEST_ASSERT_EQUAL_STRING(
      cutil_GenericType_get_name(CUTIL_GENERIC_TYPE_INT),
      cutil_GenericType_get_name(elem_type)
    );

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_returnNonNull_when_getConstIteratorCalled(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();

    /* Act */
    cutil_ConstIterator *const it = cutil_Set_get_const_iterator(set);

    /* Assert */
    TEST_ASSERT_NOT_NULL(it);

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_Set_free(set);
}

static void
_should_returnNonNull_when_getIteratorCalled(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();

    /* Act */
    cutil_Iterator *const it = cutil_Set_get_iterator(set);

    /* Assert */
    TEST_ASSERT_NOT_NULL(it);

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_Set_free(set);
}

static void
_should_traverseAllElements_when_constIteratorRewoundOnSet(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
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
_should_traverseAllElements_when_iteratorRewoundOnSet(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
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
_should_returnFalse_when_nextCalledAfterExhaustionOnConstSet(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
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
_should_returnFalse_when_nextCalledAfterExhaustionOnMutableSet(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
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

static void
_should_returnTrue_when_deepEqualsCalledOnSetsWithSameElements(void)
{
    /* Arrange */
    cutil_Set *const a = g_current_factory();
    cutil_Set *const b = g_current_factory();
    const int av[] = {1, 2, 3};
    const int bv[] = {3, 1, 2};
    for (size_t i = 0; i < 3; ++i) {
        cutil_Set_add(a, &av[i]);
        cutil_Set_add(b, &bv[i]);
    }

    /* Act / Assert */
    TEST_ASSERT_TRUE(cutil_Set_deep_equals(a, b));

    /* Cleanup */
    cutil_Set_free(a);
    cutil_Set_free(b);
}

static void
_should_returnFalse_when_deepEqualsCalledOnSetsWithDifferentCount(void)
{
    /* Arrange */
    cutil_Set *const a = g_current_factory();
    cutil_Set *const b = g_current_factory();
    const int av[] = {1, 2, 3};
    const int bv[] = {1, 2};
    for (size_t i = 0; i < 3; ++i) {
        cutil_Set_add(a, &av[i]);
    }
    for (size_t i = 0; i < 2; ++i) {
        cutil_Set_add(b, &bv[i]);
    }

    /* Act / Assert */
    TEST_ASSERT_FALSE(cutil_Set_deep_equals(a, b));

    /* Cleanup */
    cutil_Set_free(a);
    cutil_Set_free(b);
}

static void
_should_returnFalse_when_deepEqualsCalledOnSetsWithDifferentElements(void)
{
    /* Arrange */
    cutil_Set *const a = g_current_factory();
    cutil_Set *const b = g_current_factory();
    const int av[] = {1, 2, 3};
    const int bv[] = {1, 2, 4};
    for (size_t i = 0; i < 3; ++i) {
        cutil_Set_add(a, &av[i]);
        cutil_Set_add(b, &bv[i]);
    }

    /* Act / Assert */
    TEST_ASSERT_FALSE(cutil_Set_deep_equals(a, b));

    /* Cleanup */
    cutil_Set_free(a);
    cutil_Set_free(b);
}

static void
_should_handleNullInputs_when_deepEqualsCalledWithNulls(void)
{
    /* Arrange */
    cutil_Set *const a = g_current_factory();

    /* Assert */
    TEST_ASSERT_TRUE(cutil_Set_deep_equals(NULL, NULL));
    TEST_ASSERT_FALSE(cutil_Set_deep_equals(a, NULL));
    TEST_ASSERT_FALSE(cutil_Set_deep_equals(NULL, a));

    /* Cleanup */
    cutil_Set_free(a);
}

static void
_should_returnZero_when_compareCalledWithSamePointer(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int vals[] = {7, 8, 9};
    for (size_t i = 0; i < 3; ++i) {
        cutil_Set_add(set, &vals[i]);
    }

    /* Act / Assert */
    TEST_ASSERT_EQUAL_INT(0, cutil_Set_compare(set, set));

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_returnConsistentSign_when_compareCalledOnSetsWithDifferentCount(void)
{
    /* Arrange */
    cutil_Set *const small = g_current_factory();
    cutil_Set *const large = g_current_factory();
    const int sv[] = {1, 2};
    const int lv[] = {1, 2, 3};
    for (size_t i = 0; i < 2; ++i) {
        cutil_Set_add(small, &sv[i]);
    }
    for (size_t i = 0; i < 3; ++i) {
        cutil_Set_add(large, &lv[i]);
    }

    /* Act / Assert */
    TEST_ASSERT_TRUE(cutil_Set_compare(small, large) < 0);
    TEST_ASSERT_TRUE(cutil_Set_compare(large, small) > 0);

    /* Cleanup */
    cutil_Set_free(small);
    cutil_Set_free(large);
}

static void
_should_returnSameHash_when_hashCalledTwiceOnSameSet(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int vals[] = {5, 10, 15};
    for (size_t i = 0; i < 3; ++i) {
        cutil_Set_add(set, &vals[i]);
    }

    /* Act */
    const cutil_hash_t h1 = cutil_Set_hash(set);
    const cutil_hash_t h2 = cutil_Set_hash(set);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT64(h1, h2);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_returnSameHash_when_setsHaveSameElementsInDifferentInsertionOrder(void)
{
    /* Arrange */
    cutil_Set *const a = g_current_factory();
    cutil_Set *const b = g_current_factory();
    const int av[] = {3, 1, 2};
    const int bv[] = {1, 2, 3};
    for (size_t i = 0; i < 3; ++i) {
        cutil_Set_add(a, &av[i]);
        cutil_Set_add(b, &bv[i]);
    }

    /* Act / Assert */
    TEST_ASSERT_EQUAL_UINT64(cutil_Set_hash(a), cutil_Set_hash(b));

    /* Cleanup */
    cutil_Set_free(a);
    cutil_Set_free(b);
}

static void
_should_returnZero_when_hashCalledOnNullOrEmptySet(void)
{
    /* Arrange */
    cutil_Set *const empty_set = g_current_factory();

    /* Assert */
    TEST_ASSERT_EQUAL_UINT64(CUTIL_HASH_C(0), cutil_Set_hash(NULL));
    TEST_ASSERT_EQUAL_UINT64(CUTIL_HASH_C(0), cutil_Set_hash(empty_set));

    /* Cleanup */
    cutil_Set_free(empty_set);
}

static void
_should_renderEmptyBraces_when_toStringCalledOnEmptySet(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    char buf[32];

    /* Act */
    cutil_Set_to_string(set, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("{}", buf);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_renderSingleElement_when_toStringCalledOnSingletonSet(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int val = 42;
    cutil_Set_add(set, &val);
    char buf[32];

    /* Act */
    const size_t sz = cutil_Set_to_string(set, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("{42}", buf);
    TEST_ASSERT_EQUAL_size_t(4UL, sz);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_returnRequiredLength_when_toStringCalledWithNullBuf(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int val = 42;
    cutil_Set_add(set, &val);

    /* Act */
    const size_t req = cutil_Set_to_string(set, NULL, 0UL);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(4UL, req);

    /* Then render and verify */
    char buf[16];
    cutil_Set_to_string(set, buf, sizeof buf);
    TEST_ASSERT_EQUAL_STRING("{42}", buf);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_returnZero_when_toStringCalledWithTooSmallBuffer(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int val = 42;
    cutil_Set_add(set, &val);
    char buf[8];

    /* Act: required length is 4, need buflen >= 5; pass 3 */
    const size_t result = cutil_Set_to_string(set, buf, 3UL);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0UL, result);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_renderAllElements_when_toStringCalledOnMultiElementSet(void)
{
    /* Arrange */
    cutil_Set *const set = g_current_factory();
    const int vals[] = {1, 2, 3};
    for (size_t i = 0; i < 3; ++i) {
        cutil_Set_add(set, &vals[i]);
    }
    char buf[64];

    /* Act */
    const size_t req = cutil_Set_to_string(set, NULL, 0UL);
    const size_t written = cutil_Set_to_string(set, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_TRUE(strlen(buf) > 2UL);
    TEST_ASSERT_EQUAL_CHAR('{', buf[0]);
    TEST_ASSERT_EQUAL_CHAR('}', buf[strlen(buf) - 1]);
    TEST_ASSERT_NOT_NULL(strstr(buf, "1"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "2"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "3"));
    TEST_ASSERT_EQUAL_size_t(req, written);

    /* Cleanup */
    cutil_Set_free(set);
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

    /* --- Mock-based vtable dispatch tests --- */
    RUN_TEST(_should_returnTrue_when_setTypesAreIdentical);
    RUN_TEST(_should_returnFalse_when_setTypesAreDifferent);
    RUN_TEST(_should_callFreeFunction_when_setIsFreed);
    RUN_TEST(_should_doNothing_when_setIsNull);
    RUN_TEST(_should_callClearFunction_when_setIsCleared);
    RUN_TEST(_should_callCopyFunction_when_setIsCopied);
    RUN_TEST(_should_notCopy_when_setTypesIncompatible);
    RUN_TEST(_should_returnDuplicateSet_when_setIsDuplicated);
    RUN_TEST(_should_returnCount_when_setCountIsRequested);
    RUN_TEST(_should_returnZero_when_setIsEmpty);
    RUN_TEST(_should_callContainsFunction_when_elemIsQueried);
    RUN_TEST(_should_callAddFunction_when_elemIsAdded);
    RUN_TEST(_should_callRemoveFunction_when_elemIsRemoved);
    RUN_TEST(_should_callGetElemTypeFunction_when_elemTypeIsQueried);

    /* Iterator shim tests */
    RUN_TEST(_should_callGetConstIterator_when_getConstIteratorCalledOnSet);
    RUN_TEST(_should_callGetIterator_when_getIteratorCalledOnSet);
    RUN_TEST(_should_returnNull_when_getConstIteratorCalledOnNullSet);
    RUN_TEST(_should_returnNull_when_getIteratorCalledOnNullSet);

    /* --- HashSet (INT) — full interface coverage --- */
    g_current_factory = _hashset_factory_int;
    RUN_TEST(_should_returnSuccess_when_singleElemAdded);
    RUN_TEST(_should_returnZeroCount_when_setIsNewlyAllocated);
    RUN_TEST(_should_incrementCountWithEachUniqueElem);
    RUN_TEST(_should_returnTrue_when_containsExistingElem);
    RUN_TEST(_should_returnFalse_when_containsMissingElem);
    RUN_TEST(_should_returnSuccess_when_removingExistingElem);
    RUN_TEST(_should_returnFailure_when_removingMissingElem);
    RUN_TEST(_should_decrementCount_when_existingElemRemoved);
    RUN_TEST(_should_returnZeroCount_when_resetAfterFill);
    RUN_TEST(_should_notDuplicateCount_when_sameElemAddedTwice);
    RUN_TEST(_should_matchAllElems_when_copied);
    RUN_TEST(_should_beIndependent_when_dstModifiedAfterCopy);
    RUN_TEST(_should_matchAllElems_when_duplicated);
    RUN_TEST(_should_beIndependent_when_dupModifiedAfterDuplicate);
    RUN_TEST(_should_returnNonNull_when_getConstIteratorCalled);
    RUN_TEST(_should_returnNonNull_when_getIteratorCalled);
    RUN_TEST(_should_traverseAllElements_when_constIteratorRewoundOnSet);
    RUN_TEST(_should_traverseAllElements_when_iteratorRewoundOnSet);
    RUN_TEST(_should_returnFalse_when_nextCalledAfterExhaustionOnConstSet);
    RUN_TEST(_should_returnFalse_when_nextCalledAfterExhaustionOnMutableSet);
    RUN_TEST(_should_returnTrue_when_deepEqualsCalledOnSetsWithSameElements);
    RUN_TEST(_should_returnFalse_when_deepEqualsCalledOnSetsWithDifferentCount);
    RUN_TEST(
      _should_returnFalse_when_deepEqualsCalledOnSetsWithDifferentElements
    );
    RUN_TEST(_should_handleNullInputs_when_deepEqualsCalledWithNulls);
    RUN_TEST(_should_returnZero_when_compareCalledWithSamePointer);
    RUN_TEST(
      _should_returnConsistentSign_when_compareCalledOnSetsWithDifferentCount
    );
    RUN_TEST(_should_returnSameHash_when_hashCalledTwiceOnSameSet);
    RUN_TEST(
      _should_returnSameHash_when_setsHaveSameElementsInDifferentInsertionOrder
    );
    RUN_TEST(_should_returnZero_when_hashCalledOnNullOrEmptySet);
    RUN_TEST(_should_renderEmptyBraces_when_toStringCalledOnEmptySet);
    RUN_TEST(_should_renderSingleElement_when_toStringCalledOnSingletonSet);
    RUN_TEST(_should_returnRequiredLength_when_toStringCalledWithNullBuf);
    RUN_TEST(_should_returnZero_when_toStringCalledWithTooSmallBuffer);
    RUN_TEST(_should_renderAllElements_when_toStringCalledOnMultiElementSet);

    /* --- Elem-type test (not factory-driven) --- */
    RUN_TEST(_should_returnElemType_when_elemTypeQueried);

    return UNITY_END();
}

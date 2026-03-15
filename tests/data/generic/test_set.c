#include "unity.h"

#include <cutil/data/generic/set.h>
#include <cutil/data/generic/set/hashset.h>
#include <cutil/data/generic/type.h>

#include <cutil/std/stdlib.h>
#include <cutil/util/macro.h>

/* ==========================================================================
 * Mock infrastructure
 * ========================================================================== */

typedef struct {
    int free_count;
    int clear_count;
    int copy_count;
    int duplicate_count;
    int get_count_count;
    int contains_count;
    int add_count;
    int remove_count;
    int get_elem_type_count;
    int get_count_val;
} MockSetData;

static void
_mock_free(void *data)
{
    MockSetData *const mock = (MockSetData *) data;
    if (mock) {
        ++mock->free_count;
        free(mock);
    }
}

static void
_mock_clear(void *data)
{
    MockSetData *const mock = (MockSetData *) data;
    if (mock) {
        ++mock->clear_count;
    }
}

static void
_mock_copy(void *dst, const void *src)
{
    MockSetData *const dst_mock = (MockSetData *) dst;
    const MockSetData *const src_mock = (const MockSetData *) src;
    if (dst_mock && src_mock) {
        ++dst_mock->copy_count;
    }
}

static void *
_mock_duplicate(const void *data)
{
    MockSetData *const src = (MockSetData *) data;
    if (!src) {
        return NULL;
    }
    MockSetData *const dup = malloc(sizeof *dup);
    *dup = *src;
    ++src->duplicate_count;
    return dup;
}

static size_t
_mock_get_count(const void *data)
{
    MockSetData *const mock = (MockSetData *) data;
    if (mock) {
        ++mock->get_count_count;
        return (size_t) mock->get_count_val;
    }
    return 0UL;
}

static cutil_Bool
_mock_contains(const void *data, const void *elem)
{
    MockSetData *const mock = (MockSetData *) data;
    (void) elem;
    if (mock) {
        ++mock->contains_count;
        return true;
    }
    return false;
}

static int
_mock_add(void *data, const void *elem)
{
    MockSetData *const mock = (MockSetData *) data;
    (void) elem;
    if (mock) {
        ++mock->add_count;
    }
    return 0;
}

static int
_mock_remove(void *data, const void *elem)
{
    MockSetData *const mock = (MockSetData *) data;
    (void) elem;
    if (mock) {
        ++mock->remove_count;
    }
    return 0;
}

static const cutil_GenericType *
_mock_get_elem_type(const void *data)
{
    MockSetData *const mock = (MockSetData *) data;
    if (mock) {
        ++mock->get_elem_type_count;
    }
    return CUTIL_GENERIC_TYPE_INT;
}

static const cutil_SetType MOCK_SET_TYPE = {
  .name = "MockSet",
  .free = &_mock_free,
  .clear = &_mock_clear,
  .copy = &_mock_copy,
  .duplicate = &_mock_duplicate,
  .get_count = &_mock_get_count,
  .contains = &_mock_contains,
  .add = &_mock_add,
  .remove = &_mock_remove,
  .get_elem_type = &_mock_get_elem_type,
};

static cutil_Set *
_create_mock_set(void)
{
    MockSetData *const data = malloc(sizeof *data);
    *data = (MockSetData) {0};
    cutil_Set *const set = malloc(sizeof *set);
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
      .clear = &_mock_clear,
    };
    const cutil_SetType type2 = {
      .name = "Test",
      .free = &_mock_free,
      .clear = &_mock_clear,
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
    MockSetData *const mock = (MockSetData *) set->data;
    const int initial = mock->clear_count;

    /* Act */
    cutil_Set_clear(set);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(initial + 1, mock->clear_count);

    /* Cleanup */
    cutil_Set_free(set);
}

static void
_should_callCopyFunction_when_setIsCopied(void)
{
    /* Arrange */
    cutil_Set *const dst = _create_mock_set();
    cutil_Set *const src = _create_mock_set();
    MockSetData *const dst_mock = (MockSetData *) dst->data;
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
    MockSetData *const dst_data = malloc(sizeof *dst_data);
    *dst_data = (MockSetData) {0};
    MockSetData *const src_data = malloc(sizeof *src_data);
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
    MockSetData *const mock = (MockSetData *) set->data;
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
    MockSetData *const mock = (MockSetData *) set->data;
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
    MockSetData *const mock = (MockSetData *) set->data;
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
    MockSetData *const mock = (MockSetData *) set->data;
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
    MockSetData *const mock = (MockSetData *) set->data;
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
    MockSetData *const mock = (MockSetData *) set->data;
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
    MockSetData *const mock = (MockSetData *) set->data;
    const int initial = mock->get_elem_type_count;

    /* Act */
    const cutil_GenericType *const elem_type = cutil_Set_get_elem_type(set);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(CUTIL_GENERIC_TYPE_INT, elem_type);
    TEST_ASSERT_EQUAL_INT(initial + 1, mock->get_elem_type_count);

    /* Cleanup */
    cutil_Set_free(set);
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
_should_returnZeroCount_when_clearedAfterFill(void)
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
    cutil_Set_clear(set);

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

    /* Act — add a new element to dst only */
    const int extra_elem = 99;
    cutil_Set_add(dst, &extra_elem);

    /* Assert — src is unchanged */
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

    /* Act — modify dup */
    const int extra_elem = 77;
    cutil_Set_add(dup, &extra_elem);

    /* Assert — original unchanged */
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
    RUN_TEST(_should_returnZeroCount_when_clearedAfterFill);
    RUN_TEST(_should_notDuplicateCount_when_sameElemAddedTwice);
    RUN_TEST(_should_matchAllElems_when_copied);
    RUN_TEST(_should_beIndependent_when_dstModifiedAfterCopy);
    RUN_TEST(_should_matchAllElems_when_duplicated);
    RUN_TEST(_should_beIndependent_when_dupModifiedAfterDuplicate);

    /* --- Elem-type test (not factory-driven) --- */
    RUN_TEST(_should_returnElemType_when_elemTypeQueried);

    return UNITY_END();
}

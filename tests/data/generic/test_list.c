#include "unity.h"
#include <cutil/data/generic/list.h>

#include <cutil/data/generic/list/arraylist.h>

#include <cutil/data/generic/type.h>
#include <cutil/std/stdlib.h>
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
    int get_count;
    int get_count_val;
    int get_ptr_count;
    int locate_count;
    size_t locate_ret_val;
    int set_count;
    int append_count;
    int insert_mult_count;
    int remove_mult_count;
    int sort_custom_count;
    int get_elem_type_count;
    int get_const_iterator_count;
    int get_iterator_count;
} MockListData;

static void
_mock_list_free(void *data)
{
    MockListData *const mock = data;
    if (mock) {
        ++mock->free_count;
        free(mock);
    }
}

static void
_mock_list_reset(void *data)
{
    MockListData *const mock = data;
    if (mock) {
        ++mock->reset_count;
    }
}

static void
_mock_list_copy(void *dst, const void *src)
{
    MockListData *const dst_mock = dst;
    const MockListData *const src_mock = src;
    if (dst_mock && src_mock) {
        ++dst_mock->copy_count;
    }
}

static void *
_mock_list_duplicate(const void *data)
{
    MockListData *const src = CUTIL_CONST_CAST(data);
    if (!src) {
        return NULL;
    }
    MockListData *const dup = CUTIL_MALLOC_OBJECT(dup);
    *dup = *src;
    ++src->duplicate_count;
    return dup;
}

static size_t
_mock_list_get_count(const void *data)
{
    MockListData *const mock = CUTIL_CONST_CAST(data);
    if (mock) {
        ++mock->get_count_count;
        return (size_t) mock->get_count_val;
    }
    return 0UL;
}

static int
_mock_list_get(const void *data, size_t idx, void *out)
{
    CUTIL_UNUSED(data);
    CUTIL_UNUSED(idx);
    CUTIL_UNUSED(out);
    return CUTIL_STATUS_FAILURE;
}

static const void *
_mock_list_get_ptr(const void *data, size_t idx)
{
    MockListData *const mock = CUTIL_CONST_CAST(data);
    CUTIL_UNUSED(idx);
    if (mock) {
        ++mock->get_ptr_count;
    }
    return NULL;
}

static size_t
_mock_list_locate(const void *data, const void *elem)
{
    MockListData *const mock = CUTIL_CONST_CAST(data);
    CUTIL_UNUSED(elem);
    if (mock) {
        ++mock->locate_count;
        return mock->locate_ret_val;
    }
    return CUTIL_LIST_NPOS;
}

static int
_mock_list_set(void *data, size_t idx, const void *elem)
{
    MockListData *const mock = data;
    CUTIL_UNUSED(idx);
    CUTIL_UNUSED(elem);
    if (mock) {
        ++mock->set_count;
    }
    return CUTIL_STATUS_SUCCESS;
}

static int
_mock_list_append(void *data, const void *elem)
{
    MockListData *const mock = data;
    CUTIL_UNUSED(elem);
    if (mock) {
        ++mock->append_count;
    }
    return CUTIL_STATUS_SUCCESS;
}

static int
_mock_list_insert_mult(void *data, size_t pos, size_t num, const void *elems)
{
    MockListData *const mock = data;
    CUTIL_UNUSED(pos);
    CUTIL_UNUSED(num);
    CUTIL_UNUSED(elems);
    if (mock) {
        ++mock->insert_mult_count;
    }
    return CUTIL_STATUS_SUCCESS;
}

static int
_mock_list_remove_mult(void *data, size_t pos, size_t num)
{
    MockListData *const mock = data;
    CUTIL_UNUSED(pos);
    CUTIL_UNUSED(num);
    if (mock) {
        ++mock->remove_mult_count;
    }
    return CUTIL_STATUS_SUCCESS;
}

static void
_mock_list_sort_custom(void *data, cutil_CompFunc *comp)
{
    MockListData *const mock = data;
    CUTIL_UNUSED(comp);
    if (mock) {
        ++mock->sort_custom_count;
    }
}

static const cutil_GenericType *
_mock_list_get_elem_type(const void *data)
{
    MockListData *const mock = CUTIL_CONST_CAST(data);
    if (mock) {
        ++mock->get_elem_type_count;
    }
    return CUTIL_GENERIC_TYPE_INT;
}

/* --- Stub iterator infrastructure ---------------------------------------- */

static void
_mock_list_iter_free(void *data)
{
    free(data);
}

static cutil_Bool
_mock_list_iter_next(void *data)
{
    CUTIL_UNUSED(data);
    return CUTIL_FALSE;
}

static const cutil_ConstIteratorType MOCK_LIST_ITER_CONST_TYPE = {
  .name = "MockListConstIter",
  .free = &_mock_list_iter_free,
  .rewind = NULL,
  .next = &_mock_list_iter_next,
  .get = NULL,
  .get_ptr = NULL,
};

static const cutil_IteratorType MOCK_LIST_ITER_TYPE = {
  .name = "MockListIter",
  .free = &_mock_list_iter_free,
  .rewind = NULL,
  .next = &_mock_list_iter_next,
  .get = NULL,
  .get_ptr = NULL,
  .set = NULL,
  .remove = NULL,
};

static cutil_ConstIterator *
_mock_get_const_iterator(const void *data)
{
    MockListData *const mock = CUTIL_CONST_CAST(data);
    if (mock) {
        ++mock->get_const_iterator_count;
    }
    cutil_ConstIterator *const it = CUTIL_MALLOC_OBJECT(it);
    it->vtable = &MOCK_LIST_ITER_CONST_TYPE;
    it->data = malloc(1);
    return it;
}

static cutil_Iterator *
_mock_get_iterator(void *data)
{
    MockListData *const mock = data;
    if (mock) {
        ++mock->get_iterator_count;
    }
    cutil_Iterator *const it = CUTIL_MALLOC_OBJECT(it);
    it->vtable = &MOCK_LIST_ITER_TYPE;
    it->data = malloc(1);
    return it;
}

static const cutil_ListType MOCK_LIST_TYPE = {
  .name = "MockList",
  .free = &_mock_list_free,
  .reset = &_mock_list_reset,
  .copy = &_mock_list_copy,
  .duplicate = &_mock_list_duplicate,
  .get_count = &_mock_list_get_count,
  .get = &_mock_list_get,
  .get_ptr = &_mock_list_get_ptr,
  .locate = &_mock_list_locate,
  .set = &_mock_list_set,
  .append = &_mock_list_append,
  .insert_mult = &_mock_list_insert_mult,
  .remove_mult = &_mock_list_remove_mult,
  .sort_custom = &_mock_list_sort_custom,
  .get_elem_type = &_mock_list_get_elem_type,
  .get_const_iterator = &_mock_get_const_iterator,
  .get_iterator = &_mock_get_iterator,
};

static cutil_List *
_create_mock_list(void)
{
    MockListData *const data = CUTIL_MALLOC_OBJECT(data);
    *data = (MockListData) {0};
    cutil_List *const list = CUTIL_MALLOC_OBJECT(list);
    list->vtable = &MOCK_LIST_TYPE;
    list->data = data;
    return list;
}

/* ==========================================================================
 * Mock-based vtable dispatch tests
 * ========================================================================== */

static void
_should_returnTrue_when_twoListTypesAreEqual(void)
{
    /* Act */
    const cutil_Bool result = cutil_ListType_equals(
      CUTIL_LIST_TYPE_ARRAYLIST, CUTIL_LIST_TYPE_ARRAYLIST
    );

    /* Assert */
    TEST_ASSERT_TRUE(result);
}

static void
_should_returnFalse_when_listTypesMismatch(void)
{
    /* Act */
    const cutil_Bool result
      = cutil_ListType_equals(CUTIL_LIST_TYPE_ARRAYLIST, NULL);

    /* Assert */
    TEST_ASSERT_FALSE(result);
}

static void
_should_dispatchFree_when_listClearCalled(void)
{
    /* Arrange */
    cutil_List *const list = _create_mock_list();

    /* Act */
    cutil_List_clear(list);

    /* Assert */
    TEST_ASSERT_NULL(list->data);

    /* Cleanup */
    free(list);
}

static void
_should_dispatchReset_when_listResetCalled(void)
{
    /* Arrange */
    cutil_List *const list = _create_mock_list();
    MockListData *const mock = list->data;

    /* Act */
    cutil_List_reset(list);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(1, mock->reset_count);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_dispatchCopy_when_listCopyCalled(void)
{
    /* Arrange */
    cutil_List *const dst = _create_mock_list();
    cutil_List *const src = _create_mock_list();
    MockListData *const dst_mock = dst->data;

    /* Act */
    cutil_List_copy(dst, src);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(1, dst_mock->copy_count);

    /* Cleanup */
    cutil_List_free(dst);
    cutil_List_free(src);
}

static void
_should_dispatchDuplicate_when_listDuplicateCalled(void)
{
    /* Arrange */
    cutil_List *const list = _create_mock_list();
    MockListData *const mock = list->data;

    /* Act */
    cutil_List *const dup = cutil_List_duplicate(list);

    /* Assert */
    TEST_ASSERT_NOT_NULL(dup);
    TEST_ASSERT_EQUAL_INT(1, mock->duplicate_count);

    /* Cleanup */
    cutil_List_free(dup);
    cutil_List_free(list);
}

static void
_should_dispatch_when_setCalled(void)
{
    /* Arrange */
    cutil_List *const list = _create_mock_list();
    MockListData *const mock = list->data;
    const int val = 42;

    /* Act */
    cutil_List_set(list, 0UL, &val);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(1, mock->set_count);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_dispatchGetConstIterator_when_getConstIteratorCalled(void)
{
    /* Arrange */
    cutil_List *const list = _create_mock_list();
    MockListData *const mock = list->data;

    /* Act */
    cutil_ConstIterator *const it = cutil_List_get_const_iterator(list);

    /* Assert */
    TEST_ASSERT_NOT_NULL(it);
    TEST_ASSERT_EQUAL_INT(1, mock->get_const_iterator_count);

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_List_free(list);
}

static void
_should_dispatchGetIterator_when_getIteratorCalled(void)
{
    /* Arrange */
    cutil_List *const list = _create_mock_list();
    MockListData *const mock = list->data;

    /* Act */
    cutil_Iterator *const it = cutil_List_get_iterator(list);

    /* Assert */
    TEST_ASSERT_NOT_NULL(it);
    TEST_ASSERT_EQUAL_INT(1, mock->get_iterator_count);

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_List_free(list);
}

static void
_should_returnNull_when_getConstIteratorCalledOnNullList(void)
{
    TEST_ASSERT_NULL(cutil_List_get_const_iterator(NULL));
}

static void
_should_returnNull_when_getIteratorCalledOnNullList(void)
{
    TEST_ASSERT_NULL(cutil_List_get_iterator(NULL));
}

/* ==========================================================================
 * Factory infrastructure for behavioural tests
 * ==========================================================================
 *
 * Each test function below obtains its list under test by calling
 * g_current_factory(). To run a test against a new implementation, assign a
 * factory function to g_current_factory in main() before calling RUN_TEST.
 */

typedef cutil_List *(*ListFactory)(void);
static ListFactory g_current_factory = NULL;

static cutil_List *
_arraylist_factory_int(void)
{
    return cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
}

static void
_should_notCrash_when_listIsFreed(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();

    /* Act / Assert */
    cutil_List_free(list);
    TEST_ASSERT_TRUE(true);
}

static void
_should_clearAllElements_when_listIsReset(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {1, 2, 3};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    cutil_List_reset(list);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0UL, cutil_List_get_count(list));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_preserveElements_when_listIsCopied(void)
{
    /* Arrange */
    cutil_List *const src = g_current_factory();
    cutil_List *const dst = g_current_factory();
    const int vals[] = {1, 2, 3};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(src, &vals[i]);
    }

    /* Act */
    cutil_List_copy(dst, src);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(num_vals, cutil_List_get_count(dst));
    for (size_t i = 0; i < num_vals; ++i) {
        TEST_ASSERT_EQUAL_INT(
          vals[i], *(const int *) cutil_List_get_ptr(dst, i)
        );
    }

    /* Cleanup */
    cutil_List_free(src);
    cutil_List_free(dst);
}

static void
_should_returnIndependentCopy_when_listIsDuplicated(void)
{
    /* Arrange */
    cutil_List *const src = g_current_factory();
    const int vals[] = {10, 20, 30};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(src, &vals[i]);
    }

    /* Act */
    cutil_List *const dup = cutil_List_duplicate(src);
    const int extra = 99;
    cutil_List_append(src, &extra);

    /* Assert */
    TEST_ASSERT_NOT_NULL(dup);
    TEST_ASSERT_EQUAL_size_t(num_vals, cutil_List_get_count(dup));
    for (size_t i = 0; i < num_vals; ++i) {
        TEST_ASSERT_EQUAL_INT(
          vals[i], *(const int *) cutil_List_get_ptr(dup, i)
        );
    }

    /* Cleanup */
    cutil_List_free(src);
    cutil_List_free(dup);
}

static void
_should_returnZeroCount_when_listIsEmpty(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();

    /* Act */
    const size_t count = cutil_List_get_count(list);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0UL, count);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnCorrectCount_when_elementsAppended(void)
{
    /* Arrange */
    const int num_elems = 5;
    cutil_List *const list = g_current_factory();

    /* Act */
    for (int i = 0; i < num_elems; ++i) {
        cutil_List_append(list, &i);
    }

    /* Assert */
    TEST_ASSERT_EQUAL_size_t((size_t) num_elems, cutil_List_get_count(list));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnElement_when_getCalledWithValidIndex(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {10, 20, 30};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    int val = 0;
    const int rc = cutil_List_get(list, 1UL, &val);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, rc);
    TEST_ASSERT_EQUAL_INT(20, val);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnFailure_when_getIndexOutOfBounds(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();

    /* Act */
    int val = 0;
    const int rc = cutil_List_get(list, 100UL, &val);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, rc);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnNonNull_when_getPtrCalledWithValidIndex(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int v = 42;
    cutil_List_append(list, &v);

    /* Act */
    const void *const ptr = cutil_List_get_ptr(list, 0UL);

    /* Assert */
    TEST_ASSERT_NOT_NULL(ptr);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnMatchingValue_when_getPtrCalledWithValidIndex(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {10, 20, 30};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    const int *const ptr = (const int *) cutil_List_get_ptr(list, 2UL);

    /* Assert */
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_EQUAL_INT(30, *ptr);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnNull_when_getPtrIndexOutOfBounds(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();

    /* Act */
    const void *const ptr = cutil_List_get_ptr(list, 100UL);

    /* Assert */
    TEST_ASSERT_NULL(ptr);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_locateElement_when_present(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {10, 20, 30};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }
    const int target = 20;

    /* Act */
    const size_t idx = cutil_List_locate(list, &target);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(1UL, idx);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnNotFound_when_elementAbsent(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {10, 20, 30};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }
    const int target = 99;

    /* Act */
    const size_t idx = cutil_List_locate(list, &target);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(CUTIL_LIST_NPOS, idx);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnFirstOccurrence_when_duplicateElementPresent(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int v = 5;
    cutil_List_append(list, &v);
    cutil_List_append(list, &v);
    cutil_List_append(list, &v);

    /* Act */
    const size_t idx = cutil_List_locate(list, &v);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0UL, idx);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnTrue_when_containsExistingElement(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {10, 20, 30};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }
    const int target = 20;

    /* Act */
    const cutil_Bool result = cutil_List_contains(list, &target);

    /* Assert */
    TEST_ASSERT_TRUE(result);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnFalse_when_containsAbsentElement(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {10, 20, 30};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }
    const int target = 99;

    /* Act */
    const cutil_Bool result = cutil_List_contains(list, &target);

    /* Assert */
    TEST_ASSERT_FALSE(result);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnSuccess_when_setValidIndex(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int v = 10;
    cutil_List_append(list, &v);

    /* Act */
    const int new_val = 99;
    const int rc = cutil_List_set(list, 0UL, &new_val);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, rc);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnFailure_when_setOutOfBounds(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();

    /* Act */
    const int val = 42;
    const int rc = cutil_List_set(list, 100UL, &val);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, rc);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_overwriteValue_when_setCalled(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {1, 2, 3};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    const int new_val = 99;
    cutil_List_set(list, 1UL, &new_val);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(99, *(const int *) cutil_List_get_ptr(list, 1UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_swapElements_when_memswapElemCalled(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int initial = 10;
    cutil_List_append(list, &initial);
    int external = 99;

    /* Act */
    cutil_List_memswap_elem(list, 0UL, &external);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(99, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(10, external);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnFailure_when_memswapElemOutOfBounds(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    int buf = 0;

    /* Act */
    const int rc = cutil_List_memswap_elem(list, 0UL, &buf);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, rc);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_preserveOrder_when_elementsAppended(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();

    /* Act */
    const int vals[] = {1, 2, 3};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Assert */
    TEST_ASSERT_EQUAL_INT(1, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(2, *(const int *) cutil_List_get_ptr(list, 1UL));
    TEST_ASSERT_EQUAL_INT(3, *(const int *) cutil_List_get_ptr(list, 2UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_shiftElements_when_insertedAtFront(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int v2 = 2, v3 = 3, v1 = 1;
    cutil_List_append(list, &v2);
    cutil_List_append(list, &v3);

    /* Act */
    cutil_List_insert(list, 0UL, &v1);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(3UL, cutil_List_get_count(list));
    TEST_ASSERT_EQUAL_INT(1, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(2, *(const int *) cutil_List_get_ptr(list, 1UL));
    TEST_ASSERT_EQUAL_INT(3, *(const int *) cutil_List_get_ptr(list, 2UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_shiftElements_when_insertedAtMiddle(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int v1 = 1, v3 = 3, v2 = 2;
    cutil_List_append(list, &v1);
    cutil_List_append(list, &v3);

    /* Act */
    cutil_List_insert(list, 1UL, &v2);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(3UL, cutil_List_get_count(list));
    TEST_ASSERT_EQUAL_INT(1, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(2, *(const int *) cutil_List_get_ptr(list, 1UL));
    TEST_ASSERT_EQUAL_INT(3, *(const int *) cutil_List_get_ptr(list, 2UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_appendElement_when_insertedAtEnd(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int v1 = 1, v2 = 2, v3 = 3;
    cutil_List_append(list, &v1);
    cutil_List_append(list, &v2);

    /* Act */
    cutil_List_insert(list, 2UL, &v3);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(3UL, cutil_List_get_count(list));
    TEST_ASSERT_EQUAL_INT(1, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(2, *(const int *) cutil_List_get_ptr(list, 1UL));
    TEST_ASSERT_EQUAL_INT(3, *(const int *) cutil_List_get_ptr(list, 2UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_insertAllElements_when_insertMultCalled(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int v10 = 10, v30 = 30;
    cutil_List_append(list, &v10);
    cutil_List_append(list, &v30);
    const int ins[] = {20, 21};
    const size_t num_ins = CUTIL_GET_NATIVE_ARRAY_SIZE(ins);

    /* Act */
    cutil_List_insert_mult(list, 1UL, num_ins, ins);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(4UL, cutil_List_get_count(list));
    TEST_ASSERT_EQUAL_INT(10, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(20, *(const int *) cutil_List_get_ptr(list, 1UL));
    TEST_ASSERT_EQUAL_INT(21, *(const int *) cutil_List_get_ptr(list, 2UL));
    TEST_ASSERT_EQUAL_INT(30, *(const int *) cutil_List_get_ptr(list, 3UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_incrementCount_when_insertMultCalled(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int v = 0;
    cutil_List_append(list, &v);
    cutil_List_append(list, &v);
    const int ins[] = {1, 2, 3};
    const size_t num_ins = CUTIL_GET_NATIVE_ARRAY_SIZE(ins);

    /* Act */
    cutil_List_insert_mult(list, 1UL, num_ins, ins);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(5UL, cutil_List_get_count(list));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnFailure_when_insertPosOutOfBounds(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int v = 7;

    /* Act */
    const int rc = cutil_List_insert(list, 999UL, &v);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, rc);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_removeElement_when_removeCalledWithIndex(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {1, 2, 3};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    cutil_List_remove(list, 1UL);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(2UL, cutil_List_get_count(list));
    TEST_ASSERT_EQUAL_INT(1, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(3, *(const int *) cutil_List_get_ptr(list, 1UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_removeFirstElement_when_removeCalledAtZero(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {1, 2, 3};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    cutil_List_remove(list, 0UL);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(2UL, cutil_List_get_count(list));
    TEST_ASSERT_EQUAL_INT(2, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(3, *(const int *) cutil_List_get_ptr(list, 1UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_removeLastElement_when_removeCalledAtEnd(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {1, 2, 3};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    cutil_List_remove(list, 2UL);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(2UL, cutil_List_get_count(list));
    TEST_ASSERT_EQUAL_INT(1, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(2, *(const int *) cutil_List_get_ptr(list, 1UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_removeRange_when_removeMultCalled(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {1, 2, 3, 4, 5};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    cutil_List_remove_mult(list, 1UL, 3UL);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(2UL, cutil_List_get_count(list));
    TEST_ASSERT_EQUAL_INT(1, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(5, *(const int *) cutil_List_get_ptr(list, 1UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_removeRange_when_removeFromToCalled(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {0, 1, 2, 3, 4};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    cutil_List_remove_from_to(list, 1UL, 3UL);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(2UL, cutil_List_get_count(list));
    TEST_ASSERT_EQUAL_INT(0, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(4, *(const int *) cutil_List_get_ptr(list, 1UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnFailure_when_removeOutOfBounds(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();

    /* Act */
    const int rc = cutil_List_remove(list, 100UL);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, rc);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnFailure_when_removeMultExceedsLength(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {1, 2, 3};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    const int rc = cutil_List_remove_mult(list, 0UL, 100UL);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, rc);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnFailure_when_removeFromToBeginGtEnd(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {1, 2, 3, 4};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    const int rc = cutil_List_remove_from_to(list, 3UL, 1UL);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, rc);

    /* Cleanup */
    cutil_List_free(list);
}

static int
_cmp_int_desc(const void *a, const void *b)
{
    return -(*(const int *) a - *(const int *) b);
}

static void
_should_sortAscending_when_defaultComparatorUsed(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {3, 1, 2};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    cutil_List_sort(list);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(1, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(2, *(const int *) cutil_List_get_ptr(list, 1UL));
    TEST_ASSERT_EQUAL_INT(3, *(const int *) cutil_List_get_ptr(list, 2UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_sortDescending_when_customComparatorUsed(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {1, 3, 2};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }

    /* Act */
    cutil_List_sort_custom(list, &_cmp_int_desc);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(3, *(const int *) cutil_List_get_ptr(list, 0UL));
    TEST_ASSERT_EQUAL_INT(2, *(const int *) cutil_List_get_ptr(list, 1UL));
    TEST_ASSERT_EQUAL_INT(1, *(const int *) cutil_List_get_ptr(list, 2UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_notCrash_when_emptyListSorted(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();

    /* Act / Assert */
    cutil_List_sort(list);
    TEST_ASSERT_TRUE(true);

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_notCrash_when_singleElementSorted(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int v = 42;
    cutil_List_append(list, &v);

    /* Act */
    cutil_List_sort(list);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(42, *(const int *) cutil_List_get_ptr(list, 0UL));

    /* Cleanup */
    cutil_List_free(list);
}

static void
_should_returnCorrectElemType_when_variousTypesUsed(void)
{
    /* Arrange */
    const cutil_GenericType *const native_types[] = {
      CUTIL_GENERIC_TYPE_INT,  CUTIL_GENERIC_TYPE_DOUBLE,
      CUTIL_GENERIC_TYPE_CHAR, CUTIL_GENERIC_TYPE_LONG,
      CUTIL_GENERIC_TYPE_I32,
    };
    const size_t num_types = CUTIL_GET_NATIVE_ARRAY_SIZE(native_types);

    for (size_t i = 0; i < num_types; ++i) {
        /* Act */
        cutil_List *const list = cutil_ArrayList_alloc(native_types[i]);
        const cutil_GenericType *const type = cutil_List_get_elem_type(list);

        /* Assert */
        TEST_ASSERT_NOT_NULL(list);
        TEST_ASSERT_NOT_NULL(type);
        TEST_ASSERT_EQUAL_STRING(native_types[i]->name, type->name);

        /* Cleanup */
        cutil_List_free(list);
    }
}

static void
_should_returnNonNull_when_getConstIteratorCalled(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();

    /* Act */
    cutil_ConstIterator *const it = cutil_List_get_const_iterator(list);

    /* Assert */
    TEST_ASSERT_NOT_NULL(it);

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_List_free(list);
}

static void
_should_notAdvance_when_nextCalledOnEmptyList(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    cutil_ConstIterator *const it = cutil_List_get_const_iterator(list);

    /* Act */
    const cutil_Bool advanced = cutil_ConstIterator_next(it);

    /* Assert */
    TEST_ASSERT_FALSE(advanced);

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_List_free(list);
}

static void
_should_traverseAllElements_when_constIteratorUsed(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {10, 20, 30};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }
    cutil_ConstIterator *const it = cutil_List_get_const_iterator(list);

    /* Act / Assert */
    int idx = 0;
    while (cutil_ConstIterator_next(it)) {
        const int *const p = cutil_ConstIterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(p);
        TEST_ASSERT_EQUAL_INT(vals[idx], *p);
        ++idx;
    }
    TEST_ASSERT_EQUAL_INT((int) num_vals, idx);

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_List_free(list);
}

static void
_should_traverseAllElements_when_constIteratorRewound(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {10, 20, 30};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }
    cutil_ConstIterator *const it = cutil_List_get_const_iterator(list);

    while (cutil_ConstIterator_next(it)) {
    }

    /* Act */
    cutil_ConstIterator_rewind(it);
    int idx = 0;
    while (cutil_ConstIterator_next(it)) {
        const int *const p = cutil_ConstIterator_get_ptr(it);
        TEST_ASSERT_NOT_NULL(p);
        TEST_ASSERT_EQUAL_INT(vals[idx], *p);
        ++idx;
    }

    /* Assert */
    TEST_ASSERT_EQUAL_INT((int) num_vals, idx);

    /* Cleanup */
    cutil_ConstIterator_free(it);
    cutil_List_free(list);
}

static void
_should_returnNonNull_when_getIteratorCalled(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();

    /* Act */
    cutil_Iterator *const it = cutil_List_get_iterator(list);

    /* Assert */
    TEST_ASSERT_NOT_NULL(it);

    /* Cleanup */
    cutil_Iterator_free(it);
    cutil_List_free(list);
}

static void
_should_setCurrentElement_when_iteratorSetCalled(void)
{
    /* Arrange */
    cutil_List *const list = g_current_factory();
    const int vals[] = {1, 2, 3};
    const size_t num_vals = CUTIL_GET_NATIVE_ARRAY_SIZE(vals);
    for (size_t i = 0; i < num_vals; ++i) {
        cutil_List_append(list, &vals[i]);
    }
    cutil_Iterator *const it = cutil_List_get_iterator(list);
    cutil_Iterator_next(it);
    cutil_Iterator_next(it);
    const int new_val = 99;

    /* Act */
    cutil_Iterator_set(it, &new_val);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(99, *(const int *) cutil_List_get_ptr(list, 1UL));

    /* Cleanup */
    cutil_Iterator_free(it);
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

    /* --- Mock-based vtable dispatch tests --- */
    RUN_TEST(_should_returnTrue_when_twoListTypesAreEqual);
    RUN_TEST(_should_returnFalse_when_listTypesMismatch);
    RUN_TEST(_should_dispatchFree_when_listClearCalled);
    RUN_TEST(_should_dispatchReset_when_listResetCalled);
    RUN_TEST(_should_dispatchCopy_when_listCopyCalled);
    RUN_TEST(_should_dispatchDuplicate_when_listDuplicateCalled);
    RUN_TEST(_should_dispatch_when_setCalled);

    /* Iterator shim tests */
    RUN_TEST(_should_dispatchGetConstIterator_when_getConstIteratorCalled);
    RUN_TEST(_should_dispatchGetIterator_when_getIteratorCalled);
    RUN_TEST(_should_returnNull_when_getConstIteratorCalledOnNullList);
    RUN_TEST(_should_returnNull_when_getIteratorCalledOnNullList);

    /* --- ArrayList (INT) — full interface coverage --- */
    g_current_factory = &_arraylist_factory_int;

    RUN_TEST(_should_notCrash_when_listIsFreed);
    RUN_TEST(_should_clearAllElements_when_listIsReset);
    RUN_TEST(_should_preserveElements_when_listIsCopied);
    RUN_TEST(_should_returnIndependentCopy_when_listIsDuplicated);

    RUN_TEST(_should_returnZeroCount_when_listIsEmpty);
    RUN_TEST(_should_returnCorrectCount_when_elementsAppended);
    RUN_TEST(_should_returnElement_when_getCalledWithValidIndex);
    RUN_TEST(_should_returnFailure_when_getIndexOutOfBounds);
    RUN_TEST(_should_returnNonNull_when_getPtrCalledWithValidIndex);
    RUN_TEST(_should_returnMatchingValue_when_getPtrCalledWithValidIndex);
    RUN_TEST(_should_returnNull_when_getPtrIndexOutOfBounds);

    RUN_TEST(_should_locateElement_when_present);
    RUN_TEST(_should_returnNotFound_when_elementAbsent);
    RUN_TEST(_should_returnFirstOccurrence_when_duplicateElementPresent);
    RUN_TEST(_should_returnTrue_when_containsExistingElement);
    RUN_TEST(_should_returnFalse_when_containsAbsentElement);

    RUN_TEST(_should_returnSuccess_when_setValidIndex);
    RUN_TEST(_should_returnFailure_when_setOutOfBounds);
    RUN_TEST(_should_overwriteValue_when_setCalled);
    RUN_TEST(_should_swapElements_when_memswapElemCalled);
    RUN_TEST(_should_returnFailure_when_memswapElemOutOfBounds);

    RUN_TEST(_should_preserveOrder_when_elementsAppended);
    RUN_TEST(_should_shiftElements_when_insertedAtFront);
    RUN_TEST(_should_shiftElements_when_insertedAtMiddle);
    RUN_TEST(_should_appendElement_when_insertedAtEnd);
    RUN_TEST(_should_insertAllElements_when_insertMultCalled);
    RUN_TEST(_should_incrementCount_when_insertMultCalled);
    RUN_TEST(_should_returnFailure_when_insertPosOutOfBounds);

    RUN_TEST(_should_removeElement_when_removeCalledWithIndex);
    RUN_TEST(_should_removeFirstElement_when_removeCalledAtZero);
    RUN_TEST(_should_removeLastElement_when_removeCalledAtEnd);
    RUN_TEST(_should_removeRange_when_removeMultCalled);
    RUN_TEST(_should_removeRange_when_removeFromToCalled);
    RUN_TEST(_should_returnFailure_when_removeOutOfBounds);
    RUN_TEST(_should_returnFailure_when_removeMultExceedsLength);
    RUN_TEST(_should_returnFailure_when_removeFromToBeginGtEnd);

    RUN_TEST(_should_sortAscending_when_defaultComparatorUsed);
    RUN_TEST(_should_sortDescending_when_customComparatorUsed);
    RUN_TEST(_should_notCrash_when_emptyListSorted);
    RUN_TEST(_should_notCrash_when_singleElementSorted);

    RUN_TEST(_should_returnCorrectElemType_when_variousTypesUsed);

    RUN_TEST(_should_returnNonNull_when_getConstIteratorCalled);
    RUN_TEST(_should_notAdvance_when_nextCalledOnEmptyList);
    RUN_TEST(_should_traverseAllElements_when_constIteratorUsed);
    RUN_TEST(_should_traverseAllElements_when_constIteratorRewound);

    RUN_TEST(_should_returnNonNull_when_getIteratorCalled);
    RUN_TEST(_should_setCurrentElement_when_iteratorSetCalled);

    return UNITY_END();
}

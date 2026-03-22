#include "unity.h"

#include <cutil/data/generic/iterator.h>
#include <cutil/std/stdlib.h>
#include <cutil/util/macro.h>

/* --- Mock state and vtable infrastructure --------------------------------- */

typedef struct {
    int next_return;
    int get_return;
    int set_return;
    int remove_return;
    const void *get_ptr_return;
    int free_called;
    int next_called;
    int get_called;
    int get_ptr_called;
    int set_called;
    int remove_called;
    int rewind_called;
} MockIterState;

static void
_mock_free(void *data)
{
    MockIterState *const s = data;
    s->free_called = 1;
}

static cutil_Bool
_mock_next(void *data)
{
    MockIterState *const s = data;
    s->next_called = 1;
    return (cutil_Bool) s->next_return;
}

static int
_mock_get(const void *data, void *out)
{
    CUTIL_UNUSED(out);
    MockIterState *const s = CUTIL_CONST_CAST(data);
    s->get_called = 1;
    return s->get_return;
}

static const void *
_mock_get_ptr(const void *data)
{
    MockIterState *const s = CUTIL_CONST_CAST(data);
    s->get_ptr_called = 1;
    return s->get_ptr_return;
}

static int
_mock_set(void *data, const void *val)
{
    CUTIL_UNUSED(val);
    MockIterState *const s = data;
    s->set_called = 1;
    return s->set_return;
}

static int
_mock_remove(void *data)
{
    MockIterState *const s = data;
    s->remove_called = 1;
    return s->remove_return;
}

static void
_mock_rewind(void *data)
{
    MockIterState *const s = data;
    s->rewind_called = 1;
}

static const cutil_ConstIteratorType MOCK_CONST_ITER_TYPE = {
  .free = &_mock_free,
  .rewind = &_mock_rewind,
  .next = &_mock_next,
  .get = &_mock_get,
  .get_ptr = &_mock_get_ptr,
};

static const cutil_IteratorType MOCK_ITER_TYPE = {
  .free = &_mock_free,
  .rewind = &_mock_rewind,
  .next = &_mock_next,
  .get = &_mock_get,
  .get_ptr = &_mock_get_ptr,
  .set = &_mock_set,
  .remove = &_mock_remove,
};

/* Vtable with set=NULL and remove=NULL to test NULL-slot guards */
static const cutil_IteratorType MOCK_ITER_TYPE_NO_SET_REMOVE = {
  .free = &_mock_free,
  .next = &_mock_next,
  .get = &_mock_get,
  .get_ptr = &_mock_get_ptr,
  .set = NULL,
  .remove = NULL,
};

/* Vtable with rewind=NULL to test null-slot guard for rewind */
static const cutil_ConstIteratorType MOCK_CONST_ITER_TYPE_NO_REWIND = {
  .free = &_mock_free,
  .rewind = NULL,
  .next = &_mock_next,
  .get = &_mock_get,
  .get_ptr = &_mock_get_ptr,
};

/* Heap-allocates the iterator shell; the MockIterState lives on the stack.
   Callers are responsible for cleanup:
   - use free(it) in tests that do not exercise cutil_ConstIterator_free
   - use cutil_ConstIterator_free(it) only in the free-shim test */
static cutil_ConstIterator *
_make_const_iter(MockIterState *state, const cutil_ConstIteratorType *type)
{
    cutil_ConstIterator *const it = CUTIL_MALLOC_OBJECT(it);
    it->vtable = type;
    it->data = state;
    return it;
}

static cutil_Iterator *
_make_iter(MockIterState *state, const cutil_IteratorType *type)
{
    cutil_Iterator *const it = CUTIL_MALLOC_OBJECT(it);
    it->vtable = type;
    it->data = state;
    return it;
}

/* --- ConstIterator dispatch shim tests ------------------------------------ */

static void
_should_callNext_when_nextCalledOnConstIter(void)
{
    /* Arrange */
    MockIterState state = {0};
    state.next_return = 1;
    cutil_ConstIterator *const it
      = _make_const_iter(&state, &MOCK_CONST_ITER_TYPE);

    /* Act */
    const cutil_Bool result = cutil_ConstIterator_next(it);

    /* Assert */
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(1, state.next_called);

    /* Cleanup */
    free(it);
}

static void
_should_returnFalse_when_nextReturnsItExhausted(void)
{
    /* Arrange */
    MockIterState state = {0};
    state.next_return = 0;
    cutil_ConstIterator *const it
      = _make_const_iter(&state, &MOCK_CONST_ITER_TYPE);

    /* Act */
    const cutil_Bool result = cutil_ConstIterator_next(it);

    /* Assert */
    TEST_ASSERT_FALSE(result);

    /* Cleanup */
    free(it);
}

static void
_should_callGet_when_getCalledOnConstIter(void)
{
    /* Arrange */
    MockIterState state = {0};
    state.get_return = CUTIL_STATUS_SUCCESS;
    cutil_ConstIterator *const it
      = _make_const_iter(&state, &MOCK_CONST_ITER_TYPE);
    int buf = 0;

    /* Act */
    const int result = cutil_ConstIterator_get(it, &buf);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(1, state.get_called);

    /* Cleanup */
    free(it);
}

static void
_should_callGetPtr_when_getPtrCalledOnConstIter(void)
{
    /* Arrange */
    MockIterState state = {0};
    int sentinel = 0;
    state.get_ptr_return = &sentinel;
    cutil_ConstIterator *const it
      = _make_const_iter(&state, &MOCK_CONST_ITER_TYPE);

    /* Act */
    const void *result = cutil_ConstIterator_get_ptr(it);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(&sentinel, result);
    TEST_ASSERT_EQUAL_INT(1, state.get_ptr_called);

    /* Cleanup */
    free(it);
}

static void
_should_callFree_when_freeCalledOnConstIter(void)
{
    /* Arrange */
    MockIterState state = {0};
    cutil_ConstIterator *const it
      = _make_const_iter(&state, &MOCK_CONST_ITER_TYPE);

    /* Act */
    cutil_ConstIterator_free(it);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(1, state.free_called);
}

/* --- Iterator dispatch shim tests ----------------------------------------- */

static void
_should_callSet_when_setCalledOnIter(void)
{
    /* Arrange */
    MockIterState state = {0};
    state.set_return = CUTIL_STATUS_SUCCESS;
    cutil_Iterator *const it = _make_iter(&state, &MOCK_ITER_TYPE);
    int val = 99;

    /* Act */
    const int result = cutil_Iterator_set(it, &val);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(1, state.set_called);

    /* Cleanup */
    free(it);
}

static void
_should_callRemove_when_removeCalledOnIter(void)
{
    /* Arrange */
    MockIterState state = {0};
    state.remove_return = CUTIL_STATUS_SUCCESS;
    cutil_Iterator *const it = _make_iter(&state, &MOCK_ITER_TYPE);

    /* Act */
    const int result = cutil_Iterator_remove(it);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(1, state.remove_called);

    /* Cleanup */
    free(it);
}

static void
_should_returnFailure_when_setIsNull(void)
{
    /* Arrange */
    MockIterState state = {0};
    cutil_Iterator *const it
      = _make_iter(&state, &MOCK_ITER_TYPE_NO_SET_REMOVE);
    int val = 0;

    /* Act */
    const int result = cutil_Iterator_set(it, &val);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, result);
    TEST_ASSERT_EQUAL_INT(0, state.set_called);

    /* Cleanup */
    free(it);
}

static void
_should_returnFailure_when_removeIsNull(void)
{
    /* Arrange */
    MockIterState state = {0};
    cutil_Iterator *it = _make_iter(&state, &MOCK_ITER_TYPE_NO_SET_REMOVE);

    /* Act */
    const int result = cutil_Iterator_remove(it);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, result);
    TEST_ASSERT_EQUAL_INT(0, state.remove_called);

    /* Cleanup */
    free(it);
}

/* --- Null-pointer guard tests --------------------------------------------- */

static void
_should_returnFalse_when_iterIsNull(void)
{
    TEST_ASSERT_FALSE(cutil_ConstIterator_next(NULL));
}

static void
_should_returnFailure_when_iterIsNull_get(void)
{
    int buf = 0;
    TEST_ASSERT_EQUAL_INT(
      CUTIL_STATUS_FAILURE, cutil_ConstIterator_get(NULL, &buf)
    );
}

static void
_should_returnNull_when_iterIsNull(void)
{
    TEST_ASSERT_NULL(cutil_ConstIterator_get_ptr(NULL));
}

static void
_should_returnFailure_when_iterIsNull_set(void)
{
    int val = 0;
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, cutil_Iterator_set(NULL, &val));
}

static void
_should_returnFailure_when_iterIsNull_remove(void)
{
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, cutil_Iterator_remove(NULL));
}

/* --- Rewind dispatch shim tests ------------------------------------------- */

static void
_should_callRewind_when_rewindCalledOnConstIter(void)
{
    /* Arrange */
    MockIterState state = {0};
    cutil_ConstIterator *const it
      = _make_const_iter(&state, &MOCK_CONST_ITER_TYPE);

    /* Act */
    cutil_ConstIterator_rewind(it);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(1, state.rewind_called);

    /* Cleanup */
    free(it);
}

static void
_should_callRewind_when_rewindCalledOnIter(void)
{
    /* Arrange */
    MockIterState state = {0};
    cutil_Iterator *const it = _make_iter(&state, &MOCK_ITER_TYPE);

    /* Act */
    cutil_Iterator_rewind(it);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(1, state.rewind_called);

    /* Cleanup */
    free(it);
}

static void
_should_doNothing_when_rewindCalledOnNullConstIter(void)
{
    cutil_ConstIterator_rewind(NULL);
    TEST_ASSERT_TRUE(true);
}

static void
_should_doNothing_when_rewindCalledOnNullIter(void)
{
    cutil_Iterator_rewind(NULL);
    TEST_ASSERT_TRUE(true);
}

static void
_should_doNothing_when_constIterRewindSlotIsNull(void)
{
    /* Arrange */
    MockIterState state = {0};
    cutil_ConstIterator *const it
      = _make_const_iter(&state, &MOCK_CONST_ITER_TYPE_NO_REWIND);

    /* Act */
    cutil_ConstIterator_rewind(it);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, state.rewind_called);

    /* Cleanup */
    free(it);
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

    /* ConstIterator dispatch shim tests */
    RUN_TEST(_should_callNext_when_nextCalledOnConstIter);
    RUN_TEST(_should_returnFalse_when_nextReturnsItExhausted);
    RUN_TEST(_should_callGet_when_getCalledOnConstIter);
    RUN_TEST(_should_callGetPtr_when_getPtrCalledOnConstIter);
    RUN_TEST(_should_callFree_when_freeCalledOnConstIter);
    RUN_TEST(_should_callRewind_when_rewindCalledOnConstIter);

    /* Iterator dispatch shim tests */
    RUN_TEST(_should_callSet_when_setCalledOnIter);
    RUN_TEST(_should_callRemove_when_removeCalledOnIter);
    RUN_TEST(_should_returnFailure_when_setIsNull);
    RUN_TEST(_should_returnFailure_when_removeIsNull);
    RUN_TEST(_should_callRewind_when_rewindCalledOnIter);

    /* Null-pointer guard tests */
    RUN_TEST(_should_returnFalse_when_iterIsNull);
    RUN_TEST(_should_returnFailure_when_iterIsNull_get);
    RUN_TEST(_should_returnNull_when_iterIsNull);
    RUN_TEST(_should_returnFailure_when_iterIsNull_set);
    RUN_TEST(_should_returnFailure_when_iterIsNull_remove);
    RUN_TEST(_should_doNothing_when_rewindCalledOnNullConstIter);
    RUN_TEST(_should_doNothing_when_rewindCalledOnNullIter);
    RUN_TEST(_should_doNothing_when_constIterRewindSlotIsNull);

    return UNITY_END();
}

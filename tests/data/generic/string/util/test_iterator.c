#include "unity.h"
#include <cutil/data/generic/string/util/iterator.h>

#include <cutil/core/std/stdbool.h>
#include <cutil/core/std/stdio.h>
#include <cutil/core/std/stdlib.h>
#include <cutil/core/std/string.h>
#include <cutil/core/string/builder.h>
#include <cutil/core/util/macro.h>
#include <cutil/data/generic/iterator.h>

/* -------------------------------------------------------------------------
 * Lightweight mock iterator over a plain int array
 * ---------------------------------------------------------------------- */

#define REWOUND_SENTINEL ((size_t) -1)

typedef struct {
    const int *vals;
    size_t count;
    size_t idx;
} s_MockIntIter;

static void
sf_mock_free(void *data)
{
    free(data);
}

static void
sf_mock_rewind(void *data)
{
    s_MockIntIter *const it = data;
    it->idx = REWOUND_SENTINEL;
}

static cutil_Bool
sf_mock_next(void *data)
{
    s_MockIntIter *const it = data;
    if (it->idx != REWOUND_SENTINEL && it->idx >= it->count) {
        return CUTIL_FALSE;
    }
    ++it->idx;
    return CUTIL_BOOLIFY(it->idx < it->count);
}

static int
sf_mock_get(const void *data, void *out)
{
    const s_MockIntIter *const it = data;
    if (it->idx >= it->count) {
        return 1;
    }
    memcpy(out, &it->vals[it->idx], sizeof(int));
    return 0;
}

static const void *
sf_mock_get_ptr(const void *data)
{
    const s_MockIntIter *const it = data;
    if (it->idx >= it->count) {
        return NULL;
    }
    return &it->vals[it->idx];
}

static const cutil_ConstIteratorType MOCK_INT_VTABLE = {
  .name = "MockIntIter",
  .free = &sf_mock_free,
  .rewind = &sf_mock_rewind,
  .next = &sf_mock_next,
  .get = &sf_mock_get,
  .get_ptr = &sf_mock_get_ptr,
};

static cutil_ConstIterator *
sf_make_int_iter(const int *vals, size_t n)
{
    s_MockIntIter *const iter_data = CUTIL_MALLOC_OBJECT(iter_data);
    iter_data->vals = vals;
    iter_data->count = n;
    sf_mock_rewind(iter_data);

    cutil_ConstIterator *const it = CUTIL_MALLOC_OBJECT(it);
    it->vtable = &MOCK_INT_VTABLE;
    it->data = iter_data;
    return it;
}

/* -------------------------------------------------------------------------
 * Helpers
 * ---------------------------------------------------------------------- */

static const cutil_SequenceDelimiters DEFAULT_SEQ_DELIMS = {
  .start = "[",
  .start_len = 1UL,
  .end = "]",
  .end_len = 1UL,
  .delim = ",",
  .delim_len = 1UL
};

static size_t
sf_int_to_string_test_cb(
  const void *elem, char *buf, size_t buflen, const void *ctx
)
{
    CUTIL_UNUSED(ctx);
    const int v = *(const int *) elem;
    if (buf == NULL) {
        return (size_t) snprintf(NULL, 0, "%d", v);
    }
    return (size_t) snprintf(buf, buflen, "%d", v);
}

static size_t
sf_int_prefixed_to_string_cb(
  const void *elem, char *buf, size_t buflen, const void *ctx
)
{
    const char *const prefix = ctx;
    const int v = *(const int *) elem;
    if (buf == NULL) {
        return (size_t) snprintf(NULL, 0, "%s:%d", prefix, v);
    }
    return (size_t) snprintf(buf, buflen, "%s:%d", prefix, v);
}

/* -------------------------------------------------------------------------
 * cutil_ConstIterator_append_sequence_to_stringbuilder tests
 * ---------------------------------------------------------------------- */

static void
test_should_appendNothing_when_iteratorIsEmpty(void)
{
    /* Arrange */
    cutil_ConstIterator *const it = sf_make_int_iter(NULL, 0);
    cutil_SequenceLengths lens = {0, 0, 0};
    cutil_ConstIterator_get_lengths(it, &sf_int_to_string_test_cb, NULL, &lens);
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string("[");

    /* Act */
    cutil_ConstIterator_append_sequence_to_stringbuilder(
      it, sb, &lens, ",", &sf_int_to_string_test_cb, NULL
    );
    cutil_StringBuilder_append(sb, "]");

    /* Assert */
    char buf[16];
    cutil_StringBuilder_copy_string_to_buf(sb, sizeof buf, buf);
    TEST_ASSERT_EQUAL_STRING("[]", buf);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
    cutil_ConstIterator_free(it);
}

static void
test_should_appendSingleElement_when_iteratorHasOneElement(void)
{
    /* Arrange */
    const int vals[] = {42};
    cutil_ConstIterator *const it = sf_make_int_iter(vals, 1);
    cutil_SequenceLengths lens = {0, 0, 0};
    cutil_ConstIterator_get_lengths(it, &sf_int_to_string_test_cb, NULL, &lens);
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string("");

    /* Act */
    cutil_ConstIterator_append_sequence_to_stringbuilder(
      it, sb, &lens, ",", &sf_int_to_string_test_cb, NULL
    );

    /* Assert */
    char buf[16];
    cutil_StringBuilder_copy_string_to_buf(sb, sizeof buf, buf);
    TEST_ASSERT_EQUAL_STRING("42", buf);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
    cutil_ConstIterator_free(it);
}

static void
test_should_separateElementsWithDelimiter_when_iteratorHasMultipleElements(void)
{
    /* Arrange */
    const int vals[] = {1, 2, 3};
    cutil_ConstIterator *const it = sf_make_int_iter(vals, 3);
    cutil_SequenceLengths lens = {0, 0, 0};
    cutil_ConstIterator_get_lengths(it, &sf_int_to_string_test_cb, NULL, &lens);
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string("");

    /* Act */
    cutil_ConstIterator_append_sequence_to_stringbuilder(
      it, sb, &lens, ",", &sf_int_to_string_test_cb, NULL
    );

    /* Assert */
    char buf[16];
    cutil_StringBuilder_copy_string_to_buf(sb, sizeof buf, buf);
    TEST_ASSERT_EQUAL_STRING("1,2,3", buf);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
    cutil_ConstIterator_free(it);
}

static void
test_should_useNullCtx_when_callbackIgnoresCtx(void)
{
    /* Arrange */
    const int vals[] = {7, 8};
    cutil_ConstIterator *const it = sf_make_int_iter(vals, 2);
    cutil_SequenceLengths lens = {0, 0, 0};
    cutil_ConstIterator_get_lengths(it, &sf_int_to_string_test_cb, NULL, &lens);
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string("");

    /* Act */
    cutil_ConstIterator_append_sequence_to_stringbuilder(
      it, sb, &lens, ",", &sf_int_to_string_test_cb, NULL
    );

    /* Assert */
    char buf[16];
    cutil_StringBuilder_copy_string_to_buf(sb, sizeof buf, buf);
    TEST_ASSERT_EQUAL_STRING("7,8", buf);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
    cutil_ConstIterator_free(it);
}

static void
test_should_useNonNullCtx_when_callbackUsesCtx(void)
{
    /* Arrange */
    const int vals[] = {1, 2, 3};
    cutil_ConstIterator *const it = sf_make_int_iter(vals, 3);
    const char *const prefix = "X";
    cutil_SequenceLengths lens = {0, 0, 0};
    cutil_ConstIterator_get_lengths(
      it, &sf_int_prefixed_to_string_cb, prefix, &lens
    );
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string("");

    /* Act */
    cutil_ConstIterator_append_sequence_to_stringbuilder(
      it, sb, &lens, ",", &sf_int_prefixed_to_string_cb, prefix
    );

    /* Assert */
    char buf[32];
    cutil_StringBuilder_copy_string_to_buf(sb, sizeof buf, buf);
    TEST_ASSERT_EQUAL_STRING("X:1,X:2,X:3", buf);

    /* Cleanup */
    cutil_StringBuilder_free(sb);
    cutil_ConstIterator_free(it);
}

/* -------------------------------------------------------------------------
 * cutil_ConstIterator_get_lengths tests
 * ---------------------------------------------------------------------- */

static void
test_should_computeCountMaxAndSum_when_getLengthsCalledWithMultipleElements(
  void
)
{
    /* Arrange */
    const int vals[] = {1, 22, 333};
    cutil_ConstIterator *const it = sf_make_int_iter(vals, 3);
    cutil_SequenceLengths lens = {0, 0, 0};

    /* Act */
    cutil_ConstIterator_get_lengths(it, &sf_int_to_string_test_cb, NULL, &lens);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(3, lens.count);
    TEST_ASSERT_EQUAL_size_t(3, lens.max);
    TEST_ASSERT_EQUAL_size_t(6, lens.sum);

    /* Cleanup */
    cutil_ConstIterator_free(it);
}

static void
test_should_setZeroOutputs_when_getLengthsCalledOnEmptyIterator(void)
{
    /* Arrange */
    cutil_ConstIterator *const it = sf_make_int_iter(NULL, 0);
    cutil_SequenceLengths lens = {99, 99, 99};

    /* Act */
    cutil_ConstIterator_get_lengths(it, &sf_int_to_string_test_cb, NULL, &lens);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0, lens.count);
    TEST_ASSERT_EQUAL_size_t(0, lens.max);
    TEST_ASSERT_EQUAL_size_t(0, lens.sum);

    /* Cleanup */
    cutil_ConstIterator_free(it);
}

/* -------------------------------------------------------------------------
 * cutil_ConstIterator_to_string tests
 * ---------------------------------------------------------------------- */

static void
test_should_returnNullString_when_toStringCalledWithNullIterator(void)
{
    /* Arrange */
    char buf[8];

    /* Act */
    const size_t ret = cutil_ConstIterator_to_string(
      NULL, &DEFAULT_SEQ_DELIMS, &sf_int_to_string_test_cb, NULL, buf,
      sizeof buf
    );

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("NULL", buf);
    TEST_ASSERT_EQUAL_size_t(4, ret);
}

static void
test_should_renderWithBrackets_when_toStringCalledOnEmptyCollection(void)
{
    /* Arrange */
    cutil_ConstIterator *const it = sf_make_int_iter(NULL, 0);
    char buf[8];

    /* Act */
    const size_t ret = cutil_ConstIterator_to_string(
      it, &DEFAULT_SEQ_DELIMS, &sf_int_to_string_test_cb, NULL, buf, sizeof buf
    );

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("[]", buf);
    TEST_ASSERT_EQUAL_size_t(2, ret);

    /* Cleanup */
    cutil_ConstIterator_free(it);
}

static void
test_should_renderAllElements_when_toStringCalledOnNonEmptyCollection(void)
{
    /* Arrange */
    const int vals[] = {1, 2, 3};
    cutil_ConstIterator *const it = sf_make_int_iter(vals, 3);
    char buf[16];

    /* Act */
    cutil_ConstIterator_to_string(
      it, &DEFAULT_SEQ_DELIMS, &sf_int_to_string_test_cb, NULL, buf, sizeof buf
    );

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("[1,2,3]", buf);

    /* Cleanup */
    cutil_ConstIterator_free(it);
}

static void
test_should_returnRequiredLength_when_toStringCalledWithNullBuf(void)
{
    /* Arrange */
    const int vals[] = {42};
    cutil_ConstIterator *const it = sf_make_int_iter(vals, 1);

    /* Act */
    const size_t ret = cutil_ConstIterator_to_string(
      it, &DEFAULT_SEQ_DELIMS, &sf_int_to_string_test_cb, NULL, NULL, 0
    );

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(4, ret);

    /* Cleanup */
    cutil_ConstIterator_free(it);
}

static void
test_should_returnZero_when_toStringCalledWithTooSmallBuffer(void)
{
    /* Arrange */
    const int vals[] = {42};
    cutil_ConstIterator *const it = sf_make_int_iter(vals, 1);
    char buf[2];

    /* Act */
    const size_t ret = cutil_ConstIterator_to_string(
      it, &DEFAULT_SEQ_DELIMS, &sf_int_to_string_test_cb, NULL, buf, sizeof buf
    );

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(CUTIL_ERROR_SIZE, ret);

    /* Cleanup */
    cutil_ConstIterator_free(it);
}

/* -------------------------------------------------------------------------
 * Unity boilerplate
 * ---------------------------------------------------------------------- */

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

    RUN_TEST(test_should_appendNothing_when_iteratorIsEmpty);
    RUN_TEST(test_should_appendSingleElement_when_iteratorHasOneElement);
    RUN_TEST(
      test_should_separateElementsWithDelimiter_when_iteratorHasMultipleElements
    );
    RUN_TEST(test_should_useNullCtx_when_callbackIgnoresCtx);
    RUN_TEST(test_should_useNonNullCtx_when_callbackUsesCtx);
    RUN_TEST(
      test_should_computeCountMaxAndSum_when_getLengthsCalledWithMultipleElements
    );
    RUN_TEST(test_should_setZeroOutputs_when_getLengthsCalledOnEmptyIterator);
    RUN_TEST(test_should_returnNullString_when_toStringCalledWithNullIterator);
    RUN_TEST(
      test_should_renderWithBrackets_when_toStringCalledOnEmptyCollection
    );
    RUN_TEST(
      test_should_renderAllElements_when_toStringCalledOnNonEmptyCollection
    );
    RUN_TEST(test_should_returnRequiredLength_when_toStringCalledWithNullBuf);
    RUN_TEST(test_should_returnZero_when_toStringCalledWithTooSmallBuffer);

    return UNITY_END();
}

#include <cutil/string/util/iterator.h>

#include <cutil/io/log.h>
#include <cutil/std/stdbool.h>
#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

static const cutil_SequenceDelimiters CUTIL_SEQ_DELIMS_ARRAY_OBJECT = {
  .start = "[",
  .start_len = 1UL,
  .end = "]",
  .end_len = 1UL,
  .delim = ",",
  .delim_len = 1UL
};
const cutil_SequenceDelimiters *const CUTIL_SEQ_DELIMS_ARRAY
  = &CUTIL_SEQ_DELIMS_ARRAY_OBJECT;

static const cutil_SequenceDelimiters CUTIL_SEQ_DELIMS_SET_OBJECT = {
  .start = "{",
  .start_len = 1UL,
  .end = "}",
  .end_len = 1UL,
  .delim = ",",
  .delim_len = 1UL
};
const cutil_SequenceDelimiters *const CUTIL_SEQ_DELIMS_SET
  = &CUTIL_SEQ_DELIMS_SET_OBJECT;

static const cutil_SequenceDelimiters CUTIL_SEQ_DELIMS_MAP_OBJECT = {
  .start = "{",
  .start_len = 1UL,
  .end = "}",
  .end_len = 1UL,
  .delim = ",",
  .delim_len = 1UL
};
const cutil_SequenceDelimiters *const CUTIL_SEQ_DELIMS_MAP
  = &CUTIL_SEQ_DELIMS_MAP_OBJECT;

void
cutil_ConstIterator_get_lengths(
  cutil_ConstIterator *it,
  cutil_ElemToStringFunc *to_string_fn,
  const void *ctx,
  cutil_SequenceLengths *lens
)
{
    CUTIL_NULL_CHECK(it);
    CUTIL_NULL_CHECK(to_string_fn);
    CUTIL_NULL_CHECK(lens);
    cutil_ConstIterator_rewind(it);
    size_t local_count = 0UL;
    size_t local_max = 0UL;
    size_t local_sum = 0UL;
    while (cutil_ConstIterator_next(it)) {
        const void *const p = cutil_ConstIterator_get_ptr(it);
        const size_t len = to_string_fn(p, NULL, 0UL, ctx);
        ++local_count;
        if (len > local_max) {
            local_max = len;
        }
        local_sum += len;
    }
    lens->count = local_count;
    lens->max = local_max;
    lens->sum = local_sum;
}

void
cutil_ConstIterator_append_sequence_to_stringbuilder(
  cutil_ConstIterator *it,
  cutil_StringBuilder *sb,
  const cutil_SequenceLengths *lens,
  const char *delim,
  cutil_ElemToStringFunc *to_string_fn,
  const void *ctx
)
{
    CUTIL_NULL_CHECK(it);
    CUTIL_NULL_CHECK(sb);
    CUTIL_NULL_CHECK(to_string_fn);
    CUTIL_NULL_CHECK(lens);
    if (lens->max == 0UL) {
        return;
    }
    char *const auxbuf = CUTIL_MALLOC_MULT(auxbuf, lens->max + 1UL);
    cutil_ConstIterator_rewind(it);
    cutil_Bool first = CUTIL_TRUE;
    while (cutil_ConstIterator_next(it)) {
        if (!first) {
            cutil_StringBuilder_append(sb, delim);
        }
        first = CUTIL_FALSE;
        const void *const p = cutil_ConstIterator_get_ptr(it);
        CUTIL_UNUSED(to_string_fn(p, auxbuf, lens->max + 1UL, ctx));
        cutil_StringBuilder_append(sb, auxbuf);
    }
    free(auxbuf);
}

size_t
cutil_ConstIterator_to_string(
  cutil_ConstIterator *it,
  const cutil_SequenceDelimiters *delims,
  cutil_ElemToStringFunc *to_string_fn,
  const void *ctx,
  char *buf,
  size_t buflen
)
{
    CUTIL_NULL_CHECK(it);
    CUTIL_NULL_CHECK(delims);
    CUTIL_NULL_CHECK(delims->start);
    CUTIL_NULL_CHECK(delims->end);
    CUTIL_NULL_CHECK(delims->delim);
    CUTIL_NULL_CHECK(to_string_fn);
    if (it == NULL) {
        const int res = snprintf(buf, buflen, "NULL");
        return (res >= 0) ? (size_t) res : CUTIL_ERROR_SIZE;
    }

    const char *const start = delims->start;
    const char *const end = delims->end;
    const char *const delim = delims->delim;
    const size_t start_len = delims->start_len;
    const size_t end_len = delims->end_len;
    const size_t delim_len = delims->delim_len;

    cutil_SequenceLengths lens = {0UL, 0UL, 0UL};
    cutil_ConstIterator_get_lengths(it, to_string_fn, ctx, &lens);
    const size_t delim_total
      = (lens.count > 0UL) ? (lens.count - 1UL) * delim_len : 0UL;
    const size_t req = start_len + lens.sum + delim_total + end_len;
    if (buf == NULL) {
        return req;
    }
    if (req + 1UL > buflen) {
        cutil_log_warn(
          "Cannot serialize sequence: buffer too small"
          " (have %zu, need %zu)",
          buflen, req + 1UL
        );
        return CUTIL_ERROR_SIZE;
    }
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string(start);
    cutil_ConstIterator_append_sequence_to_stringbuilder(
      it, sb, &lens, delim, to_string_fn, ctx
    );
    cutil_StringBuilder_append(sb, end);
    const size_t result
      = cutil_StringBuilder_copy_string_to_buf(sb, buflen, buf);
    cutil_StringBuilder_free(sb);
    return result;
}

/** cutil/string/util/iterator.h
 *
 * Iterator serialization utilities.
 */

#ifndef CUTIL_STRING_UTIL_ITERATOR_H_INCLUDED
#define CUTIL_STRING_UTIL_ITERATOR_H_INCLUDED

#include <cutil/data/generic/iterator.h>
#include <cutil/std/stddef.h>
#include <cutil/string/builder.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Aggregated length information for a serialized sequence, populated by
 * cutil_ConstIterator_get_lengths.
 */
typedef struct {
    size_t count; /**< Number of elements. */
    size_t max;   /**< Maximum single-element string length. */
    size_t sum;   /**< Sum of all element string lengths. */
} cutil_SequenceLengths;

/**
 * Delimiter strings for sequence serialization.
 */
typedef struct {
    const char *start; /**< Opening delimiter string (e.g. "[" or "{"). */
    size_t start_len;  /**< Length of the opening delimiter string. */
    const char *end;   /**< Closing delimiter string (e.g. "]" or "}"). */
    size_t end_len;    /**< Length of the closing delimiter string. */
    const char *delim; /**< Delimiter string between elements (e.g. ","). */
    size_t delim_len;  /**< Length of the delimiter string. */
} cutil_SequenceDelimiters;

/**
 * Preset delimiters for array serialization.
 */
extern const cutil_SequenceDelimiters *const CUTIL_SEQ_DELIMS_ARRAY;

/**
 * Preset delimiters for set serialization.
 */
extern const cutil_SequenceDelimiters *const CUTIL_SEQ_DELIMS_SET;

/**
 * Preset delimiters for map serialization.
 */
extern const cutil_SequenceDelimiters *const CUTIL_SEQ_DELIMS_MAP;

/**
 * Per-element to_string callback with snprintf-style convention plus a
 * const opaque context pointer.
 *
 * @param[in] elem Pointer to the element to serialize.
 * @param[out] buf Destination buffer, or NULL to query the required size.
 * @param[in] buflen Size of the destination buffer, or 0 when querying.
 * @param[in] ctx Const opaque context (Array passes arr, Set passes type, Map
 *                passes map).
 *
 * @return Number of characters written (excluding NUL), or required size when
 *         buf is NULL.
 */
typedef size_t
cutil_ElemToStringFunc(
  const void *elem, char *buf, size_t buflen, const void *ctx
);

/**
 * Populates `lens` with the count, maximum single-element string length and
 * sum of all element string lengths by iterating over `it` once. Rewinds `it`
 * before iterating. Does NOT free `it`.
 *
 * @param[in, out] it Iterator to traverse (must not be NULL).
 * @param[in] to_string_fn Per-element serialization callback (must not be
 *                         NULL).
 * @param[in] ctx Const opaque context forwarded to `to_string_fn`.
 * @param[out] lens Output struct to populate (must not be NULL).
 */
void
cutil_ConstIterator_get_lengths(
  cutil_ConstIterator *it,
  cutil_ElemToStringFunc *to_string_fn,
  const void *ctx,
  cutil_SequenceLengths *lens
);

/**
 * Appends element representations to `sb`, separated by `delim`.
 *
 * Uses `lens`->max to allocate a single auxiliary buffer; formats each
 * element into it and appends -- with `delim` between elements -- to `sb`.
 * Rewinds `it` before iterating. The iterator is NOT freed by this function;
 * the caller owns it.
 *
 * @param[in, out] it Iterator to traverse (must not be NULL).
 * @param[in, out] sb StringBuilder to append to (must not be NULL).
 * @param[in] lens Length info from cutil_ConstIterator_get_lengths (lens->max
 *                 drives auxbuf size; must not be NULL).
 * @param[in] delim Delimiter string inserted between elements.
 * @param[in] to_string_fn Per-element serialization callback (must not be
 *                         NULL).
 * @param[in] ctx Const opaque context forwarded to `to_string_fn`.
 */
void
cutil_ConstIterator_append_sequence_to_stringbuilder(
  cutil_ConstIterator *it,
  cutil_StringBuilder *sb,
  const cutil_SequenceLengths *lens,
  const char *delim,
  cutil_ElemToStringFunc *to_string_fn,
  const void *ctx
);

/**
 * Full snprintf-style to_string for an iterator sequence bounded by `start` and
 * `end` strings and with `delim` inserted between elements.
 *
 * When `it` is NULL, serializes as "NULL" (container itself is NULL).
 * When `buf` is NULL, returns the required buffer size (excluding NUL).
 * When `buf` is too small, logs a warning and returns 0.
 * Otherwise writes at most `buflen` - 1 chars + NUL and returns chars written
 * (excluding NUL). The iterator is NOT freed by this function.
 *
 * @param[in, out] it Iterator, or NULL to serialize as "NULL".
 * @param[in] delims Delimiter strings for sequence serialization (must not be
 *                   NULL).
 * @param[in] to_string_fn Per-element serialization callback (must not be
 *                         NULL).
 * @param[in] ctx Const opaque context forwarded to `to_string_fn`.
 * @param[out] buf Destination buffer, or NULL to query required size.
 * @param[in] buflen Size of `buf` in bytes, or 0 when querying.
 *
 * @return Number of characters written (excluding NUL), or required size when
 *         buf is NULL.
 */
size_t
cutil_ConstIterator_to_string(
  cutil_ConstIterator *it,
  const cutil_SequenceDelimiters *delims,
  cutil_ElemToStringFunc *to_string_fn,
  const void *ctx,
  char *buf,
  size_t buflen
);

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_STRING_UTIL_ITERATOR_H_INCLUDED */

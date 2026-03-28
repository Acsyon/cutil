/** cutil/string/builder.h
 *
 * Header for string builder.
 */

#ifndef CUTIL_STRING_BUILDER_H_INCLUDED
#define CUTIL_STRING_BUILDER_H_INCLUDED

#include <stdarg.h>

#include <cutil/cutil.h>
#include <cutil/data/generic/type.h>
#include <cutil/status.h>
#include <cutil/std/stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * String builder.
 *
 * NUL-terminator invariant: str[length] == '\0' is maintained after every
 * operation. `length` counts stored characters and excludes the NUL
 * terminator; `capacity` >= `length` + 1.
 */
typedef struct {
    size_t capacity;
    char *str;
    size_t length;
    size_t bufsiz;
    char *buf;
} cutil_StringBuilder;

/**
 * Flags for resize process
 */
enum {
    CUTIL_RESIZE_FLAG_FORCE = (UINT32_C(1) << 0),
    CUTIL_RESIZE_FLAG_STRING = (UINT32_C(1) << 1),
    CUTIL_RESIZE_FLAG_BUFFER = (UINT32_C(1) << 2)
};

/**
 * Creates newly malloc'd cutil_StringBuilder object with default initial
 * capacity. The string is initialized to an empty NUL-terminated string.
 *
 * @return newly malloc'd cutil_StringBuilder object
 */
cutil_StringBuilder *
cutil_StringBuilder_create(void);

/**
 * Creates newly malloc'd cutil_StringBuilder object with initial capacity
 * `size`. The string is initialized to an empty NUL-terminated string.
 *
 * @param[in] size initial capacity (0 for default)
 *
 * @return newly malloc'd cutil_StringBuilder object
 */
cutil_StringBuilder *
cutil_StringBuilder_alloc(size_t size);

/**
 * Reads contents of `str` into newly malloc'd cutil_StringBuilder object.
 * The resulting builder contains a NUL-terminated copy of `str`.
 *
 * @param[in] str string to read data from
 *
 * @return newly malloc'd cutil_StringBuilder object with contents of `str`
 */
cutil_StringBuilder *
cutil_StringBuilder_from_string(const char *str);

/**
 * Copies contents of `sb` into newly malloc'd cutil_StringBuilder object.
 * The NUL-terminator invariant is preserved in the duplicate.
 *
 * @param[in] sb cutil_StringBuilder to copy data of
 *
 * @return newly malloc'd cutil_StringBuilder object with contents of `sb`
 */
cutil_StringBuilder *
cutil_StringBuilder_duplicate(const cutil_StringBuilder *sb);

/**
 * Frees memory pointed to by `sb`.
 *
 * @param[in] sb cutil_StringBuilder object to be freed
 */
void
cutil_StringBuilder_free(cutil_StringBuilder *sb);

/**
 * Frees contents of `sb`.
 *
 * @param[in] sb cutil_StringBuilder object to be cleared
 */
void
cutil_StringBuilder_clear(cutil_StringBuilder *sb);

/**
 * Clears contents of `sb` and resets sizes. The NUL-terminator invariant
 * is preserved: after this call sb->str[0] == '\0' and length == 0.
 *
 * @param[in] sb cutil_StringBuilder object to reset
 */
void
cutil_StringBuilder_reset(cutil_StringBuilder *sb);

/**
 * Copies contents of `src` into `dst`. The NUL-terminator invariant is
 * preserved in the destination.
 *
 * @param[in, out] dst cutil_StringBuilder to copy data into
 * @param[in] src cutil_StringBuilder to copy data of
 */
void
cutil_StringBuilder_copy(
  cutil_StringBuilder *dst, const cutil_StringBuilder *src
);

/**
 * Manually resizes (and potentially truncates) string and/or buffer inside `sb`
 * to new size `target` according to `flags`. When the string is truncated the
 * NUL-terminator is placed at the new `length` position.
 *
 * @param[in] sb cutil_StringBuilder to resize
 * @param[in] target target size of string and buffer
 * @param[in] flags flags for resize process
 */
void
cutil_StringBuilder_resize(
  cutil_StringBuilder *sb, size_t target, uint32_t flags
);

/**
 * Manually resizes string and buffer inside `sb` to exactly the current size.
 * This overrides the standard resize algorithm. The NUL-terminator invariant
 * is preserved.
 *
 * @param[in] sb cutil_StringBuilder to shrink to fit
 */
void
cutil_StringBuilder_shrink_to_fit(cutil_StringBuilder *sb);

/**
 * Returns the number of characters stored in `sb`, excluding the NUL
 * terminator. Equivalent to strlen(cutil_StringBuilder_get_string(sb)).
 *
 * @param[in] sb cutil_StringBuilder to get length of
 *
 * @return number of stored characters, excluding the NUL terminator, or
 * CUTIL_ERROR_SIZE on error
 */
inline size_t
cutil_StringBuilder_length(const cutil_StringBuilder *sb)
{
    CUTIL_NULL_CHECK(sb);
    return sb->length;
}

/**
 * Returns the NUL-terminated string stored in `sb`. The pointer is owned by
 * the builder and remains valid until the next mutating call.
 *
 * @param[in] sb cutil_StringBuilder to get string of
 *
 * @return NUL-terminated string owned by `sb`
 */
inline const char *
cutil_StringBuilder_get_string(const cutil_StringBuilder *sb)
{
    CUTIL_NULL_CHECK(sb);
    return sb->str;
}

/**
 * Returns a newly malloc'd NUL-terminated copy of the string in `sb`. The
 * caller is responsible for freeing the returned pointer.
 *
 * @param[in] sb cutil_StringBuilder to get string of
 *
 * @return newly malloc'd NUL-terminated copy of the string; caller must free
 */
char *
cutil_StringBuilder_duplicate_string(const cutil_StringBuilder *sb);

/**
 * Copies the NUL-terminated string in `sb` to buffer `buf` of size `buflen`.
 * If the buffer is too small (buflen < length + 1), no action is performed.
 * On success the buffer is NUL-terminated.
 *
 * @param[in] sb cutil_StringBuilder to get string of
 * @param[in] buflen size of buffer (must be >= length + 1 for success)
 * @param[in, out] buf buffer to write NUL-terminated string to
 *
 * @return length of copied string (without NUL character) or CUTIL_ERROR_SIZE
 * on error (e.g., if buffer is too small)
 */
size_t
cutil_StringBuilder_copy_string_to_buf(
  const cutil_StringBuilder *sb, size_t buflen, char *buf
);

/**
 * Copies string in `sb` to buffer pointed to by `p_buf` whose size is pointed
 * to by `p_buflen` and performs potentially necessary (re)allocations. If any
 * (re)allocation happens and `p_buflen` is not NULL, the new size of the buffer
 * (including the NUL character) is written to *`p_buflen`. *`p_buf` needs to be
 * freed.
 *
 * @param[in] sb cutil_StringBuilder to get string of
 * @param[in, out] p_buflen pointer to length of buffer to write new length to
 * (if it is not NULL)
 * @param[in, out] p_buf pointer to buffer to write string to
 *
 * @return length of copied string (without NUL character) or CUTIL_ERROR_SIZE
 * on error
 */
size_t
cutil_StringBuilder_copy_string(
  const cutil_StringBuilder *sb, size_t *p_buflen, char **p_buf
);

/**
 * Inserts va_list `args` according to printf-like format string `format` to
 * `sb` at position `pos` up to at most `maxlen` bytes.
 *
 * @param[in] sb cutil_StringBuilder to insert to
 * @param[in] pos position to insert string at
 * @param[in] maxlen maximum length that inserted string may have
 * @param[in] format format string for variadic arguments
 * @param[in] args va_list of variadic arguments
 *
 * @return number of bytes inserted or CUTIL_ERROR_SIZE on error
 */
size_t
cutil_StringBuilder_vninsertf(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t pos,
  size_t maxlen,
  const char *CUTIL_RESTRICT format,
  va_list args
);

/**
 * Inserts variadic arguments according to printf-like format string `format` to
 * `sb` at position `pos` up to at most `maxlen` bytes.
 *
 * @param[in] sb cutil_StringBuilder to insert to
 * @param[in] pos position to insert string at
 * @param[in] maxlen maximum length that inserted string may have
 * @param[in] format format string for variadic arguments
 *
 * @return number of bytes inserted or CUTIL_ERROR_SIZE on error
 */
size_t
cutil_StringBuilder_ninsertf(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t pos,
  size_t maxlen,
  const char *CUTIL_RESTRICT format,
  ...
);

/**
 * Inserts string `str` to `sb` at position `pos` up to at most `maxlen` bytes.
 *
 * @param[in] sb cutil_StringBuilder to insert to
 * @param[in] pos position to insert string at
 * @param[in] maxlen maximum length that inserted string may have
 * @param[in] str string to insert
 *
 * @return number of bytes inserted or CUTIL_ERROR_SIZE on error
 */
size_t
cutil_StringBuilder_ninsert(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t pos,
  size_t maxlen,
  const char *CUTIL_RESTRICT str
);

/**
 * Inserts va_list `args` according to printf-like format string `format` to
 * `sb` at position `pos` up.
 *
 * @param[in] sb cutil_StringBuilder to insert to
 * @param[in] pos position to insert string at
 * @param[in] format format string for variadic arguments
 * @param[in] args va_list of variadic arguments
 *
 * @return number of bytes inserted or CUTIL_ERROR_SIZE on error
 */
size_t
cutil_StringBuilder_vinsertf(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t pos,
  const char *CUTIL_RESTRICT format,
  va_list args
);

/**
 * Inserts variadic arguments according to printf-like format string `format` to
 * `sb` at position `pos` up.
 *
 * @param[in] sb cutil_StringBuilder to insert to
 * @param[in] pos position to insert string at
 * @param[in] format format string for variadic arguments
 *
 * @return number of bytes inserted or CUTIL_ERROR_SIZE on error
 */
size_t
cutil_StringBuilder_insertf(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t pos,
  const char *CUTIL_RESTRICT format,
  ...
);

/**
 * Inserts string `str` to `sb` at position `pos` up.
 *
 * @param[in] sb cutil_StringBuilder to insert to
 * @param[in] pos position to insert string at
 * @param[in] str string to insert
 *
 * @return number of bytes inserted or CUTIL_ERROR_SIZE on error
 */
size_t
cutil_StringBuilder_insert(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t pos,
  const char *CUTIL_RESTRICT str
);

/**
 * Appends va_list `args` according to printf-like format string `format` to
 * `sb` up to at most `maxlen` bytes.
 *
 * @param[in] sb cutil_StringBuilder to append to
 * @param[in] maxlen maximum length that appended string may have
 * @param[in] format format string for variadic arguments
 * @param[in] args va_list of variadic arguments
 *
 * @return number of bytes appended or CUTIL_ERROR_SIZE on error
 */
size_t
cutil_StringBuilder_vnappendf(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t maxlen,
  const char *CUTIL_RESTRICT format,
  va_list args
);

/**
 * Appends variadic arguments according to printf-like format string `format` to
 * `sb` up to at most `maxlen` bytes.
 *
 * @param[in] sb cutil_StringBuilder to append to
 * @param[in] maxlen maximum length that appended string may have
 * @param[in] format format string for variadic arguments
 *
 * @return number of bytes appended or CUTIL_ERROR_SIZE on error
 */
size_t
cutil_StringBuilder_nappendf(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t maxlen,
  const char *CUTIL_RESTRICT format,
  ...
);

/**
 * Appends string `str` to `sb` up to at most `maxlen` bytes.
 *
 * @param[in] sb cutil_StringBuilder to append to
 * @param[in] maxlen maximum length that appended string may have
 * @param[in] str string to append
 *
 * @return number of bytes appended or CUTIL_ERROR_SIZE on error
 */
size_t
cutil_StringBuilder_nappend(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t maxlen,
  const char *CUTIL_RESTRICT str
);

/**
 * Appends va_list `args` according to printf-like format string `format` to
 * `sb`.
 *
 * @param[in] sb cutil_StringBuilder to append to
 * @param[in] format format string for variadic arguments
 * @param[in] args va_list of variadic arguments
 *
 * @return number of bytes appended or CUTIL_ERROR_SIZE on error
 */
size_t
cutil_StringBuilder_vappendf(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  const char *CUTIL_RESTRICT format,
  va_list args
);

/**
 * Appends variadic arguments according to printf-like format string `format` to
 * `sb`.
 *
 * @param[in] sb cutil_StringBuilder to append to
 * @param[in] format format string for variadic arguments
 *
 * @return number of bytes appended or CUTIL_ERROR_SIZE on error
 */
size_t
cutil_StringBuilder_appendf(
  cutil_StringBuilder *CUTIL_RESTRICT sb, const char *CUTIL_RESTRICT format, ...
);

/**
 * Appends string `str` to `sb`.
 *
 * @param[in] sb cutil_StringBuilder to append to
 * @param[in] str string to append
 *
 * @return number of bytes appended or CUTIL_ERROR_SIZE on error
 */
size_t
cutil_StringBuilder_append(
  cutil_StringBuilder *CUTIL_RESTRICT sb, const char *CUTIL_RESTRICT str
);

/**
 * Deletes `num` chars from the string in `sb` starting at `pos` (inclusively).
 * The NUL-terminator invariant is preserved: sb->str[sb->length] == '\0'
 * after the call.
 *
 * @param[in] sb cutil_StringBuilder to delete from
 * @param[in] pos position to start deletion at
 * @param[in] num number of chars to delete
 */
void
cutil_StringBuilder_delete(cutil_StringBuilder *sb, size_t pos, size_t num);

/**
 * Deletes all chars between `begin` and `end` (inclusively) from the string in
 * `sb`. The NUL-terminator invariant is preserved.
 *
 * @param[in] sb cutil_StringBuilder to delete from
 * @param[in] begin position of the first char to delete
 * @param[in] end position of the last char to delete
 */
void
cutil_StringBuilder_delete_from_to(
  cutil_StringBuilder *sb, size_t begin, size_t end
);

/**
 * Returns whether Sets `lhs` and `rhs` contain exactly the same strings. Both
 * NULL or the same pointer compare equal. NULL and non-NULL compare unequal.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return Are `lhs` and `rhs` deeply equal?
 */
cutil_Bool
cutil_StringBuilder_deep_equals(
  const cutil_StringBuilder *lhs, const cutil_StringBuilder *rhs
);

/**
 * Three-way comparison of strings inside builders `lhs` and `rhs`. Calls strcmp
 * internally. NULL sorts before non-NULL; identical pointers compare equal.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return strcmp result
 */
int
cutil_StringBuilder_compare(
  const cutil_StringBuilder *lhs, const cutil_StringBuilder *rhs
);

/**
 * Computes the hash of the string inside `sb`. Returns CUTIL_HASH_C(0) for NULL
 * or an empty string.
 *
 * @param[in] sb cutil_StringBuilder to hash
 *
 * @return hash value
 */
cutil_hash_t
cutil_StringBuilder_hash(const cutil_StringBuilder *sb);

/**
 * Copies the string inside `sb` to `buf`.
 *
 * @param[in] sb cutil_StringBuilder to copy, or NULL
 * @param[out] buf destination buffer, or NULL to query required size
 * @param[in] buflen size of destination buffer in bytes, or 0 when querying
 *
 * @return number of characters written (excluding NUL), or required size when
 *         buf is NULL
 */
size_t
cutil_StringBuilder_to_string(
  const cutil_StringBuilder *sb, char *buf, size_t buflen
);

/**
 * Generic-type support functions
 */

/**
 * Generic (i.e, void-argument) version of 'cutil_StringBuilder_clear'.
 *
 * @param[in] set cutil_StringBuilder to clear
 */
inline void
cutil_StringBuilder_clear_generic(void *obj)
{
    cutil_StringBuilder *const set = (cutil_StringBuilder *) obj;
    cutil_StringBuilder_clear(set);
}

/**
 * Generic (i.e, void-argument) version of 'cutil_StringBuilder_copy'.
 *
 * @param[out] dst cutil_StringBuilder to copy to
 * @param[in] src cutil_StringBuilder to be copied
 */
inline void
cutil_StringBuilder_copy_generic(void *dst, const void *src)
{
    cutil_StringBuilder *const sb_dst = (cutil_StringBuilder *) dst;
    const cutil_StringBuilder *const sb_src = (const cutil_StringBuilder *) src;
    cutil_StringBuilder_copy(sb_dst, sb_src);
}

/**
 * Generic (i.e, void-argument) version of 'cutil_StringBuilder_deep_equals'.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return Are `lhs` and `rhs` deeply equal?
 */
cutil_Bool
cutil_StringBuilder_deep_equals_generic(const void *lhs, const void *rhs);

/**
 * Generic (i.e, void-argument) version of 'cutil_StringBuilder_compare'.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return negative if lhs < rhs, 0 if equal, positive if lhs > rhs
 */
int
cutil_StringBuilder_compare_generic(const void *lhs, const void *rhs);

/**
 * Generic (i.e, void-argument) version of 'cutil_StringBuilder_hash'.
 *
 * @param[in] sb string builder to hash
 *
 * @return hash value
 */
cutil_hash_t
cutil_StringBuilder_hash_generic(const void *sb);

/**
 * Generic (i.e, void-argument) version of 'cutil_StringBuilder_to_string'.
 *
 * @param[in] sb string builder to serialize, or NULL
 * @param[out] buf destination buffer, or NULL to query required size
 * @param[in] buflen size of destination buffer in bytes, or 0 when querying
 *
 * @return number of characters written (excluding NUL), or required size when
 *         buf is NULL
 */
size_t
cutil_StringBuilder_to_string_generic(const void *sb, char *buf, size_t buflen);

/**
 * Generic type descriptor for cutil_StringBuilder.
 */
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_STRING_BUILDER;

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_STRING_BUILDER_H_INCLUDED */

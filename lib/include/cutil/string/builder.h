/** cutil/string/builder.h
 *
 * Header for string builder.
 */

#ifndef CUTIL_STRING_BUILDER_H_INCLUDED
#define CUTIL_STRING_BUILDER_H_INCLUDED

#include <stdarg.h>

#include <cutil/cutil.h>
#include <cutil/std/stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * String builder.
 *
 * NUL-terminator invariant: @c str[length] == '\0' is maintained after every
 * operation. @c length counts stored characters and excludes the NUL
 * terminator; @c capacity >= length + 1.
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
    CUTIL_RESIZE_FLAG_FORCE = (1 << 0),
    CUTIL_RESIZE_FLAG_STRING = (1 << 1),
    CUTIL_RESIZE_FLAG_BUFFER = (1 << 2)
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
 * Clears contents of `sb` and resets sizes. The NUL-terminator invariant
 * is preserved: after this call @c sb->str[0] == '\0' and @c length == 0.
 *
 * @param[in] sb cutil_StringBuilder object to clear
 */
void
cutil_StringBuilder_clear(cutil_StringBuilder *sb);

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
 * NUL-terminator is placed at the new @c length position.
 *
 * @param[in] sb cutil_StringBuilder to resize
 * @param[in] target target size of string and buffer
 * @param[in] flags flags for resize process
 */
void
cutil_StringBuilder_resize(cutil_StringBuilder *sb, size_t target, int flags);

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
 * terminator. Equivalent to @c strlen(cutil_StringBuilder_get_string(sb)).
 *
 * @param[in] sb cutil_StringBuilder to get length of
 *
 * @return number of stored characters, excluding the NUL terminator
 */
inline size_t
cutil_StringBuilder_length(const cutil_StringBuilder *sb)
{
    return sb->length;
}

/**
 * Returns the NUL-terminated string stored in `sb`. The pointer is owned by
 * the builder and remains valid until the next mutating call.
 *
 * @param[in] sb cutil_StringBuilder to get string of
 *
 * @return NUL-terminated string owned by @c sb
 */
inline const char *
cutil_StringBuilder_get_string(const cutil_StringBuilder *sb)
{
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
 * @return length of copied string (without NUL character) or 0 if buffer is
 * too small
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
 * @return length of copied string (without NUL character)
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
 * @return number of bytes inserted
 */
int
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
 * @return number of bytes inserted
 */
int
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
 * @return number of bytes inserted
 */
int
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
 * @return number of bytes inserted
 */
int
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
 * @return number of bytes inserted
 */
int
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
 * @return number of bytes inserted
 */
int
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
 * @return number of bytes appended
 */
int
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
 * @return number of bytes appended
 */
int
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
 * @return number of bytes appended
 */
int
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
 * @return number of bytes appended
 */
int
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
 * @return number of bytes appended
 */
int
cutil_StringBuilder_appendf(
  cutil_StringBuilder *CUTIL_RESTRICT sb, const char *CUTIL_RESTRICT format, ...
);

/**
 * Appends string `str` to `sb`.
 *
 * @param[in] sb cutil_StringBuilder to append to
 * @param[in] str string to append
 *
 * @return number of bytes appended
 */
int
cutil_StringBuilder_append(
  cutil_StringBuilder *CUTIL_RESTRICT sb, const char *CUTIL_RESTRICT str
);

/**
 * Deletes `num` chars from the string in `sb` starting at `pos` (inclusively).
 * The NUL-terminator invariant is preserved: @c sb->str[sb->length] == '\0'
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

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_STRING_BUILDER_H_INCLUDED */

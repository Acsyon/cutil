/* cutil/stringbuilder.h
 *
 * Header for string builder
 *
 */

#ifndef CUTIL_STRINGBUILDER_H_INCLUDED
#define CUTIL_STRINGBUILDER_H_INCLUDED

#include <stdarg.h>
#include <stdlib.h>

#include <cutil/cutil.h>
#include <cutil/std/stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * String builder
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
 * capacity.
 *
 * @return newly malloc'd cutil_StringBuilder object
 */
cutil_StringBuilder *
cutil_StringBuilder_create(void);

/**
 * Creates newly malloc'd cutil_StringBuilder object with initial capacity
 * `size`.
 *
 * @param[in] size initial capacity (0 for default)
 *
 * @return newly malloc'd cutil_StringBuilder object
 */
cutil_StringBuilder *
cutil_StringBuilder_alloc(size_t size);

/**
 * Reads contents of `str` into newly malloc'd cutil_StringBuilder object.
 *
 * @param[in] str string to read data from
 *
 * @return newly malloc'd cutil_StringBuilder object with contents of `str`
 */
cutil_StringBuilder *
cutil_StringBuilder_from_string(const char *str);

/**
 * Reads contents of `in` into newly malloc'd cutil_StringBuilder object.
 *
 * @param[in] in FILE stream to read data from
 *
 * @return newly malloc'd cutil_StringBuilder object with contents of `in` (NULL
 * for error)
 */
cutil_StringBuilder *
cutil_StringBuilder_from_file(FILE *in);

/**
 * Copies contents of `sb` into newly malloc'd cutil_StringBuilder object.
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
 * Clears contents of `sb` and resets sizes.
 *
 * @param[in] sb cutil_StringBuilder object to clear
 */
void
cutil_StringBuilder_clear(cutil_StringBuilder *sb);

/**
 * Copies contents of `src` into `dst`.
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
 * to new size `target` according to `flags`.
 *
 * @param[in] sb cutil_StringBuilder to resize
 * @param[in] target target size of string and buffer
 * @param[in] flags flags for resize process
 */
void
cutil_StringBuilder_resize(cutil_StringBuilder *sb, size_t target, int flags);

/**
 * Manually resizes string and buffer inside `sb` to exatcly to current size.
 * This overrides standard resize algorithm.
 *
 * @param[in] sb cutil_StringBuilder to shrink to fit
 */
void
cutil_StringBuilder_shrink_to_fit(cutil_StringBuilder *sb);

/**
 * Returns length of string in `sb`.
 *
 * @param[in] sb cutil_StringBuilder to get length of
 *
 * @return length of string in `sb`
 */
inline size_t
cutil_StringBuilder_length(const cutil_StringBuilder *sb)
{
    return sb->length;
}

/**
 * Returns string in `sb`.
 *
 * @param[in] sb cutil_StringBuilder to get string of
 *
 * @return string in `sb`
 */
inline const char *
cutil_StringBuilder_get_string(const cutil_StringBuilder *sb)
{
    return sb->str;
}

/**
 * Returns newly malloc'd copy of string in `sb`. Has to be freed.
 *
 * @param[in] sb cutil_StringBuilder to get string of
 *
 * @return string in `sb`
 */
char *
cutil_StringBuilder_duplicate_string(const cutil_StringBuilder *sb);

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
 *
 * @param[in] sb cutil_StringBuilder to delete from
 * @param[in] pos position to start deletion at
 * @param[in] num number of chars to delete
 */
void
cutil_StringBuilder_delete(cutil_StringBuilder *sb, size_t pos, size_t num);

/**
 * Deletes all chars between `begin` and `end` (inclusively) from the string in
 * `sb` starting at `pos.
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

#endif /* CUTIL_STRINGBUILDER_H_INCLUDED */

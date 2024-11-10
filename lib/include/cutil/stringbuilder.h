/* cutil/stringbuilder.h
 *
 * Header for string builder
 *
 */

#ifndef CUTIL_STRINGBUILDER_H_INCLUDED
#define CUTIL_STRINGBUILDER_H_INCLUDED

#include <stdarg.h>
#include <stdlib.h>

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
 * Frees memory pointed to by `sb`.
 *
 * @param[in] sb cutil_StringBuilder object to be freed
 */
void
cutil_StringBuilder_free(cutil_StringBuilder *sb);

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
  cutil_StringBuilder *sb, size_t maxlen, const char *format, va_list args
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
  cutil_StringBuilder *sb, size_t maxlen, const char *format, ...
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
  cutil_StringBuilder *sb, const char *format, va_list args
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
cutil_StringBuilder_appendf(cutil_StringBuilder *sb, const char *format, ...);

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
cutil_StringBuilder_to_string(const cutil_StringBuilder *sb)
{
    return sb->str;
}

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_STRINGBUILDER_H_INCLUDED */

/* cutil/std/string.h
 *
 * Wrapper around standard "string.h" header
 *
 */

#ifndef CUTIL_STD_STRING_H_INCLUDED
#define CUTIL_STD_STRING_H_INCLUDED

#include <string.h>

#include <cutil/std/stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns length of string `str` excluding the terminating NUL character.
 * Counts at most `maxlen` characters.
 *
 * @param[in] str string to return size of
 * @param[in] num maximum number of bytes to count
 *
 * @return length of string `str` excluding the terminating NUL character
 */
size_t
cutil_strnlen(const char *str, size_t maxlen);

/**
 * Copies string `str` to newly malloc'd memory. Returns NULL if `str` is NULL.
 *
 * @param[in] str string to be copied
 *
 * @return pointer to newly malloc'd copy of `str`
 */
char *
cutil_strdup(const char *str);

/**
 * Copies at most `num` bytes plus one of string `str` to newly malloc'd memory.
 * Copied string is terminated with NUL character. Returns NULL if `str` is
 * NULL.
 *
 * @param[in] str string to be copied
 * @param[in] num maximum number of bytes to be copied
 *
 * @return pointer to newly malloc'd copy of `str`
 */
char *
cutil_strndup(const char *str, size_t num);

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_STD_STRING_H_INCLUDED */

/** cutil/core/std/string.h
 *
 * Wrapper around standard "string.h" header.
 */

#ifndef CUTIL_CORE_STD_STRING_H_INCLUDED
#define CUTIL_CORE_STD_STRING_H_INCLUDED

#include <string.h>

#include <cutil/core/std/stdlib.h>

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

/**
 * MACRO to duplicate an array of `num` elements of type `typeof(*(PTR))`
 * pointed to by `PTR`. Returns NULL if `PTR` is NULL.
 *
 * @param[in] PTR pointer to array to be duplicated
 * @param[in] NUM number of elements to be duplicated
 *
 * @return pointer to newly malloc'd copy of array pointed to by `PTR`
 */
#define CUTIL_MEMCPY_MULT(DST, SRC, NUM)                                       \
    memcpy((DST), (SRC), (size_t) (NUM) * sizeof *(DST))

/**
 * Copies `num` elements of type with size `size` pointed to by `ptr` to newly
 * malloc'd memory. Returns NULL if `ptr` is NULL.
 *
 * @param[in] ptr pointer to array to be duplicated
 * @param[in] size size of each element
 * @param[in] num number of elements to be duplicated
 *
 * @return pointer to newly malloc'd copy of array pointed to by `ptr`
 */
void *
cutil_memdup(const void *ptr, size_t size, size_t num);

/**
 * MACRO to duplicate an array of `num` elements of type `typeof(*(PTR))`
 * pointed to by `PTR`. Returns NULL if `PTR` is NULL.
 *
 * @param[in] PTR pointer to array to be duplicated
 * @param[in] NUM number of elements to be duplicated
 *
 * @return pointer to newly malloc'd copy of array pointed to by `PTR`
 */
#define CUTIL_MEMDUP_MULT(PTR, NUM)                                            \
    cutil_memdup((PTR), sizeof *(PTR), (size_t) (NUM))

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_CORE_STD_STRING_H_INCLUDED */

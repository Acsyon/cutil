/* cutil/std/stdlib.h
 *
 * Wrapper around standard "stdlib.h" header
 *
 */

#ifndef CUTIL_STD_STDLIB_H_INCLUDED
#define CUTIL_STD_STDLIB_H_INCLUDED

#include <stdlib.h>

#include <cutil/cutil.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Debug 'malloc': Checks if memory could be allocated, if not print error
 * message containing `size`, `file` and `line`.
 *
 * @param[in] size number of bytes of memory to be allocated
 * @param[in] file file name of calling function
 * @param[in] line line number of calling function
 *
 * @return pointer to allocated memory
 */
void *
cutil_dmalloc(size_t size, const char *file, int line);

/**
 * Debug 'calloc': Checks if memory could be allocated, if not print error
 * message containing `num`, `size`, `file` and `line`.
 *
 * @param[in] num number of elements to be allocated
 * @param[in] size size of each element
 * @param[in] file file name of calling function
 * @param[in] line line number of calling function
 *
 * @return pointer to allocated memory
 */
void *
cutil_dcalloc(size_t num, size_t size, const char *file, int line);

/**
 * Debug 'realloc': Checks if memory could be reallocated, if not print error
 * message containing `size`, `file` and `line`.
 *
 * @param[in] ptr pointer to memory that should be reallocated
 * @param[in] size number of bytes of memory to be allocated
 * @param[in] file file name of calling function
 * @param[in] line line number of calling function
 *
 * @return pointer to reallocated memory
 */
void *
cutil_drealloc(
  void *CUTIL_RESTRICT ptr,
  size_t size,
  const char *CUTIL_RESTRICT file,
  int line
);

#ifdef CUTIL_ENABLE_DEBUG_ALLOCS
    #define malloc(SIZE) cutil_dmalloc((SIZE), __FILE__, __LINE__)
    #define calloc(NUM, SIZE) cutil_dcalloc((NUM), (SIZE), __FILE__, __LINE__)
    #define realloc(PTR, SIZE) cutil_drealloc((PTR), (SIZE), __FILE__, __LINE__)
#endif

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_STD_STDLIB_H_INCLUDED */

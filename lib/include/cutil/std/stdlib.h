/** cutil/std/stdlib.h
 *
 * Wrapper around standard "stdlib.h" header.
 */

#ifndef CUTIL_STD_STDLIB_H_INCLUDED
#define CUTIL_STD_STDLIB_H_INCLUDED

#include <stdlib.h>

#include <cutil/cutil.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Wrapper around 'malloc': Returns NULL if `size` is 0.
 *
 * @param[in] size number of bytes of memory to be allocated
 *
 * @return pointer to allocated memory or NULL
 */
void *
cutil_malloc(size_t size);

/**
 * Wrapper around 'calloc': Returns NULL if `size` or `num` are 0. In the latter
 * case, also output a warning.
 *
 * @param[in] num number of elements to be allocated
 * @param[in] size size of each element
 *
 * @return pointer to allocated memory or NULL
 */
void *
cutil_calloc(size_t num, size_t size);

/**
 * Wrapper around 'realloc': Returns NULL if `size` is 0 and frees `ptr`.
 *
 * @param[in] ptr pointer to memory that should be reallocated
 * @param[in] size number of bytes of memory to be allocated
 *
 * @return pointer to reallocated memory
 */
void *
cutil_realloc(void *ptr, size_t size);

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

/**
 * MACRO for malloc'ing array PTR with NUM elements.
 *
 * @param[in] PTR array to be malloc'd
 * @param[in] NUM number of elements array shall have
 *
 * @return pointer to array
 */
#define CUTIL_MALLOC_MULT(PTR, NUM) malloc((NUM) * sizeof *(PTR))

/**
 * MACRO for calloc'ing array PTR with NUM elements.
 *
 * @param[in] PTR array to be calloc'd
 * @param[in] NUM number of elements array shall have
 *
 * @return pointer to array
 */
#define CUTIL_CALLOC_MULT(PTR, NUM) calloc((NUM), sizeof *(PTR))

/**
 * MACRO for realloc'ing array PTR with NUM elements.
 *
 * @param[in] PTR array to be realloc'd
 * @param[in] NUM new number of elements array shall have
 *
 * @return pointer to array
 */
#define CUTIL_REALLOC_MULT(PTR, NUM) realloc((PTR), (NUM) * sizeof *(PTR))

/**
 * MACRO for malloc'ing memory large enough to store memory of a single object
 * pointed to by PTR.
 *
 * @param[in] PTR pointer to be malloc'd
 *
 * @return pointer to newly malloc'd memory
 */
#define CUTIL_MALLOC_OBJECT(PTR) malloc(sizeof *(PTR))

/**
 * MACRO for calloc'ing memory large enough to store memory of a single object
 * pointed to by PTR.
 *
 * @param[in] PTR pointer to be calloc'd
 *
 * @return pointer to newly calloc'd memory
 */
#define CUTIL_CALLOC_OBJECT(PTR) calloc(1UL, sizeof *(PTR))

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_STD_STDLIB_H_INCLUDED */

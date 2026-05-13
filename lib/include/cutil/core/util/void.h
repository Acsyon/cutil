/** cutil/core/util/void.h
 *
 * Header for some utility functions for type-generic programming, like getting
 * elements of type-generic arrays and swapping memory blocks.
 */

#ifndef CUTIL_CORE_UTIL_VOID_H_INCLUDED
#define CUTIL_CORE_UTIL_VOID_H_INCLUDED

#include <cutil/core/debug/null.h>
#include <cutil/core/std/stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns a constant pointer to the `idx`-th element of the type-generic array
 * `data` whose elements have a size of `size` bytes.
 *
 * @param[in] size size of each element of `data` in bytes
 * @param[in] data pointer to type-generic array
 * @param[in] idx index to get element at
 *
 * @return constant pointer to the `idx`-th element of `data`
 */
inline const void *
cutil_void_array_get_elem_const(size_t size, const void *data, size_t idx)
{
    CUTIL_NULL_CHECK(data);
    return (const char *) data + idx * size;
}

/**
 * Returns a pointer to the `idx`-th element of the type-generic array `data`
 * whose elements have a size of `size` bytes.
 *
 * @param[in] size size of each element of `data` in bytes
 * @param[in] data pointer to type-generic array
 * @param[in] idx index to get element at
 *
 * @return pointer to the `idx`-th element of `data`
 */
inline void *
cutil_void_array_get_elem(size_t size, void *data, size_t idx)
{
    CUTIL_NULL_CHECK(data);
    return (char *) data + idx * size;
}

/**
 * Swaps `size` bytes of memory at `a` and `b`.
 *
 * @param[in, out] a pointer to the first memory block
 * @param[in, out] b pointer to the second memory block
 * @param[in] size number of bytes to swap
 */
inline void
cutil_void_memswap(void *a, void *b, size_t size)
{
    char tmp;
    char *const pa = (char *) a;
    char *const pb = (char *) b;
    for (size_t i = 0; i < size; ++i) {
        tmp = pa[i];
        pa[i] = pb[i];
        pb[i] = tmp;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_CORE_UTIL_VOID_H_INCLUDED */

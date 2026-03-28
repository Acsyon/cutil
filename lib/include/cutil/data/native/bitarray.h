/** cutil/native/bitarray.h
 *
 * Header for bit arrays.
 *
 */

#ifndef CUTIL_NATIVE_BITARRAY_H_INCLUDED
#define CUTIL_NATIVE_BITARRAY_H_INCLUDED

#include <limits.h>

#include <cutil/std/stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Array with bits as addressable units.
 */
typedef struct {
    size_t capacity;     /**< capacity of the array */
    unsigned char *data; /**< pointer to allocated memory */
} cutil_BitArray;

/**
 * Allocates memory for 'cutil_BitArray' with capacity `capacity`.
 *
 * @param[in] capacity lower bound for initial capacity
 *
 * @return newly malloc'd cutil_BitArray object
 */
cutil_BitArray *
cutil_BitArray_alloc(size_t num);

/**
 * Destructor for 'cutil_BitArray'.
 *
 * @param[in] arr cutil_BitArray object to be destroyed
 */
void
cutil_BitArray_free(cutil_BitArray *arr);

/**
 * Clears `arr`, i.e., frees memory of data and sets capacity to 0.
 *
 * @param[in] arr cutil_BitArray object to be cleared
 */
void
cutil_BitArray_clear(cutil_BitArray *arr);

/**
 * Copies contents from `src` to `dst`.
 *
 * @param[out] dst cutil_BitArray to copy to
 * @param[in] src cutil_BitArray to be copied
 */
void
cutil_BitArray_copy(cutil_BitArray *dst, const cutil_BitArray *src);

/**
 * Returns newly malloc'd copy of cutil_BitArray `arr`.
 *
 * @param[in] arr cutil_BitArray to be copied
 *
 * @return newly malloc'd copy of `arr`
 */
cutil_BitArray *
cutil_BitArray_duplicate(const cutil_BitArray *arr);

/**
 * Resize 'cutil_BitArray' to be able to hold `num` elements.
 *
 * @param[in] arr cutil_BitArray object to be resized
 * @param[in] num new number of elements in array
 */
void
cutil_BitArray_resize(cutil_BitArray *arr, size_t num);

/**
 * MACRO for size of single element of 'cutil_BitArray' in bits.
 */
#define cutil_BitArray_ELEM_SIZE_IN_BITS                                       \
    (CHAR_BIT * sizeof *((cutil_BitArray *) 0)->data)

/**
 * Returns capacity of `arr` in bits.
 *
 * @param[in] arr cutil_BitArray object to get capacity of
 *
 * @return capacity of `arr` in bits
 */
inline size_t
cutil_BitArray_get_capacity(const cutil_BitArray *arr)
{
    return arr->capacity * cutil_BitArray_ELEM_SIZE_IN_BITS;
}

/**
 * Returns bit of `arr` at index `idx`, i.e., a nonzero value if the bit is set.
 *
 * @param[in] arr cutil_BitArray object to get bit of
 * @param[in] idx index to get value at
 *
 * @return value of `arr` at index `idx`
 */
inline unsigned char
cutil_BitArray_get(const cutil_BitArray *arr, size_t idx)
{
    const div_t res = div(idx, cutil_BitArray_ELEM_SIZE_IN_BITS);
    return arr->data[res.quot] & ((unsigned char) 1 << res.rem);
}

/**
 * Sets bit of `arr` at index `idx`.
 *
 * @param[in] arr cutil_BitArray object to set bit of
 * @param[in] idx index to set bit at
 */
inline void
cutil_BitArray_set(cutil_BitArray *arr, size_t idx)
{
    const div_t res = div(idx, cutil_BitArray_ELEM_SIZE_IN_BITS);
    arr->data[res.quot] |= ((unsigned char) 1 << res.rem);
}

/**
 * Unsets bit of `arr` at index `idx`.
 *
 * @param[in] arr cutil_BitArray object to unset bit of
 * @param[in] idx index to unset bit at
 */
inline void
cutil_BitArray_unset(cutil_BitArray *arr, size_t idx)
{
    const div_t res = div(idx, cutil_BitArray_ELEM_SIZE_IN_BITS);
    arr->data[res.quot] &= ~((unsigned char) 1 << res.rem);
}

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_NATIVE_BITARRAY_H_INCLUDED */

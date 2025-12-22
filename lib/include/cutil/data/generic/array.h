/** cutil/generic/array.h
 *
 * Header for a type-generic array.
 *
 * Cannot directly be used as a type itself, i.e., there is no
 * `CUTIL_GENERIC_TYPE_ARRAY`, since `type` is not known upfront which prevents
 * the definition of a general `init` function. However, all other functions are
 * implemented so that one straightforwardly define an `init` function to define
 * the type struct. See the definitions for the native types below.
 */

#ifndef CUTIL_DATA_GENERIC_ARRAY_H_INCLUDED
#define CUTIL_DATA_GENERIC_ARRAY_H_INCLUDED

#include <cutil/data/generic/type.h>
#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Array with arbitrarily typed value of size `size`. Thin wrapper around
 * contiguous memory with type-generic convenience functions.
 */
typedef struct {
    const cutil_GenericType *type; /**< Type of each element */
    size_t capacity;               /**< capacity of the array */
    void *data;                    /**< pointer to allocated memory */
} cutil_Array;

/**
 * Allocates memory for 'cutil_Array' with capacity `num` of type `type`.
 *
 * @param[in] type type of each element
 * @param[in] num initial capacity, i.e., maximum number of elements
 *
 * @return newly malloc'd cutil_Array object
 */
cutil_Array *
cutil_Array_alloc(const cutil_GenericType *type, size_t num);

/**
 * Destructor for 'cutil_Array'.
 *
 * @param[in] arr cutil_Array object to be destroyed
 */
void
cutil_Array_free(cutil_Array *arr);

/**
 * Clears `arr`, i.e., frees memory of data and sets capacity to 0.
 *
 * @param[in] arr cutil_Array object to be cleared
 */
void
cutil_Array_clear(cutil_Array *arr);

/**
 * Copies contents from `src` to `dst`.
 *
 * @param[out] dst cutil_Array to copy to
 * @param[in] src cutil_Array to be copied
 */
void
cutil_Array_copy(cutil_Array *dst, const cutil_Array *src);

/**
 * Returns newly malloc'd copy of cutil_Array `arr`.
 *
 * @param[in] arr cutil_Array to be copied
 *
 * @return newly malloc'd copy of `arr`
 */
cutil_Array *
cutil_Array_duplicate(const cutil_Array *arr);

/**
 * Resize 'cutil_Array' to be able to hold `num` elements.
 *
 * @param[in] arr cutil_Array object to resize
 * @param[in] num new number of elements in array
 */
void
cutil_Array_resize(cutil_Array *arr, size_t num);

/**
 * Returns underlying type `arr`.
 *
 * @param[in] arr cutil_Array object to get type of
 *
 * @return underlying type `arr`
 */
inline const cutil_GenericType *
cutil_Array_get_type(const cutil_Array *arr)
{
    return arr->type;
}

/**
 * Returns size of elements of `arr` in bytes.
 *
 * @param[in] arr cutil_Array object to get size of elements of
 *
 * @return size of elements of `arr` in bytes
 */
inline size_t
cutil_Array_get_size(const cutil_Array *arr)
{
    return cutil_GenericType_get_size(arr->type);
}

/**
 * Returns capacity of `arr` in bytes.
 *
 * @param[in] arr cutil_Array object to get capacity of
 *
 * @return capacity of `arr` in bytes
 */
inline size_t
cutil_Array_get_capacity(const cutil_Array *arr)
{
    return arr->capacity;
}

/**
 * Get `num` values of `arr` at index `idx` and write it to `res`.
 *
 * @param[in] arr cutil_Array object to get values of
 * @param[in] idx index to get value at
 * @param[in] num number of elements to get
 * @param[out] res memory to write value to
 */
void
cutil_Array_get_mult(const cutil_Array *arr, size_t idx, size_t num, void *res);

/**
 * Get value of `arr` at index `idx` and write it to `res`.
 *
 * @param[in] arr cutil_Array object to get value of
 * @param[in] idx index to get value at
 * @param[out] res memory to write value to
 */
inline void
cutil_Array_get(const cutil_Array *arr, size_t idx, void *res)
{
    cutil_Array_get_mult(arr, idx, 1UL, res);
}

/**
 * Return pointer to element of `arr` at index `idx`.
 *
 * @param[in] arr cutil_Array object to return pointer of
 * @param[in] idx index to get pointer at
 *
 * @return pointer to element of `arr` at index `idx`
 */
inline const void *
cutil_Array_get_ptr(const cutil_Array *arr, size_t idx)
{
    const size_t size = cutil_Array_get_size(arr);
    return cutil_void_array_get_elem_const(size, arr->data, idx);
}

/**
 * Set `num` values of `arr` at index `idx` to value pointed to by `val`.
 *
 * @param[in] arr cutil_Array object to set values of
 * @param[in] idx index to set value at
 * @param[in] num number of elements to set
 * @param[in] res memory to write value from
 */
void
cutil_Array_set_mult(cutil_Array *arr, size_t idx, size_t num, const void *val);

/**
 * Set value of `arr` at index `idx` to value pointed to by `val`.
 *
 * @param[in] arr cutil_Array object to set value of
 * @param[in] idx index to set value at
 * @param[in] res memory to write value from
 */
inline void
cutil_Array_set(cutil_Array *arr, size_t idx, const void *val)
{
    cutil_Array_set_mult(arr, idx, 1UL, val);
}

/**
 * Returns whether `lhs` and `rhs` are element-wise deeply equal. Two arrays
 * are considered deeply equal if they share the same type, the same capacity,
 * and every corresponding pair of elements is deeply equal according to the
 * type's deep-equals function (or the fallback byte-wise comparator). Identical
 * pointers (including two NULLs) compare equal; one NULL and one non-NULL
 * compare unequal.
 *
 * @param[in] lhs left-hand side array
 * @param[in] rhs right-hand side array
 *
 * @return whether `lhs` and `rhs` are element-wise deeply equal
 */
cutil_Bool
cutil_Array_deep_equals(const cutil_Array *lhs, const cutil_Array *rhs);

/**
 * Three-way comparison of `lhs` and `rhs`. Arrays are compared first by type
 * descriptor (raw-byte order of the cutil_GenericType struct), then by
 * capacity, and finally element-by-element using the type's comparison function
 * (or the fallback byte-wise comparator). NULL pointers sort before non-NULL
 * ones; two NULL pointers compare equal.
 *
 * @param[in] lhs left-hand side array
 * @param[in] rhs right-hand side array
 *
 * @return negative if `lhs < rhs`, 0 if equal, positive if `lhs > rhs`
 */
int
cutil_Array_compare(const cutil_Array *lhs, const cutil_Array *rhs);

/**
 * Computes a hash value for `arr` by hashing each element with the type's hash
 * function (or the fallback byte-hash) and combining the results. Returns
 * `CUTIL_HASH_C(0)` for a NULL pointer or an empty array.
 *
 * @param[in] arr cutil_Array object to hash
 *
 * @return hash value for `arr`
 */
cutil_hash_t
cutil_Array_hash(const cutil_Array *arr);

/**
 * Serializes `arr` as `"[elem0,elem1,...,elemN-1]"` into `buf`, where each
 * element is formatted via the type's to_string function (or the fallback).
 * Follows the snprintf convention: pass `buf = NULL` with `buflen = 0` to
 * query the required buffer size; the return value always excludes the NUL
 * terminator. When `buf` is not NULL, at most `buflen - 1` characters are
 * written followed by a NUL terminator; returns 0 and logs a warning if the
 * buffer is too small. A NULL array pointer is serialized as `"NULL"`.
 *
 * @param[in] arr cutil_Array object to serialize, or NULL
 * @param[out] buf destination buffer, or NULL to query required size
 * @param[in] buflen size of destination buffer in bytes, or 0 when querying
 *
 * @return number of characters written (excluding NUL), or the number of
 *         characters that would be written (excluding NUL) when buf is NULL
 */
size_t
cutil_Array_to_string(const cutil_Array *arr, char *buf, size_t buflen);

/**
 * cutil_GenericType for Arrays of native types
 */

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_CHAR;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_SHORT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_INT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_LONG;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_LLONG;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_UCHAR;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_USHORT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_UINT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_ULONG;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_ULLONG;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_I8;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_I16;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_I32;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_I64;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_U8;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_U16;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_U32;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_U64;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_SIZET;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_FLOAT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_DOUBLE;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_LDOUBLE;

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_DATA_GENERIC_ARRAY_H_INCLUDED */

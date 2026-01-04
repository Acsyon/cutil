/** cutil/data/generic/type.h
 *
 * Header for a generic type that allows basic operations an memory management
 * of (potentially composite) objects.
 */

#ifndef CUTIL_DATA_GENERIC_TYPE_H_INCLUDED
#define CUTIL_DATA_GENERIC_TYPE_H_INCLUDED

#include <cutil/debug/null.h>
#include <cutil/io/log.h>
#include <cutil/std/stdbool.h>
#include <cutil/std/stdlib.h>
#include <cutil/util/compare.h>
#include <cutil/util/hash.h>

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
    char *const pa = a;
    char *const pb = b;
    for (size_t i = 0; i < size; ++i) {
        tmp = pa[i];
        pa[i] = pb[i];
        pb[i] = tmp;
    }
}

/**
 * Type that allows basic operations -- like (de)initialization, copying,
 * comparison and hashing -- as well as memory management of (composite)
 * objects.
 *
 * 'name' may not be null. 'size' may not be 0.
 *
 * Each of the function pointers may be NULL. In this case, the underlying
 * object is either not composite or the required function does not require
 * special actions. 'copy' is then replaced by 'memcpy', while 'init' and
 * 'clear' are no-ops. For 'hash', 'comp', 'deep_equals', and 'to_string',
 * fallback implementations are used.
 */
typedef struct {
    const char *const name;         /**< Name of the type */
    const size_t size;              /**< Size of each element in bytes */
    void (*const init)(void *obj);  /**< Initialization function */
    void (*const clear)(void *obj); /**< De-initialization function */
    void (*const copy)(void *dst, const void *src); /**< Copy function */
    /**< Deep-equals function */
    cutil_Bool (*const deep_equals)(const void *lhs, const void *rhs);
    cutil_CompFunc *const comp; /**< Comparison function */
    cutil_HashFunc *const hash; /**< Hash function */
    /**< Serialization function */
    size_t (*const to_string)(const void *data, char *buf, size_t buflen);
} cutil_GenericType;

/**
 * Returns name of `type`.
 *
 * @param[in] type cutil_GenericType object to get name of
 *
 * @return name of `type`
 */
inline const char *
cutil_GenericType_get_name(const cutil_GenericType *type)
{
    CUTIL_NULL_CHECK(type);
    return type->name;
}

/**
 * Returns size of `type` in bytes.
 *
 * @param[in] type cutil_GenericType object to get size of
 *
 * @return size of `type` in bytes
 */
inline size_t
cutil_GenericType_get_size(const cutil_GenericType *type)
{
    CUTIL_NULL_CHECK(type);
    return type->size;
}

/**
 * Returns whether `type` is valid, i.e., if the pointer is not NULL and the
 * members correspond to the specification.
 *
 * @param[in] type cutil_GenericType to check validity of
 *
 * @return is `type` valid?
 */
cutil_Bool
cutil_GenericType_is_valid(const cutil_GenericType *type);

/**
 * Returns whether `lhs` is equal to `rhs`.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return is `lhs` equal to `rhs`?
 */
cutil_Bool
cutil_GenericType_equals(
  const cutil_GenericType *lhs, const cutil_GenericType *rhs
);

/**
 * Applies 'init' function of `type` to the first `num` elements of `data`.
 * Corresponds to a no-op if 'init' is NULL or `num` is 0.
 *
 * @param[in] type cutil_GenericType to use 'init' function of
 * @param[in, out] data type-generic array to apply 'init' function to
 * @param[in] num number of elements to apply 'init' function to
 */
void
cutil_GenericType_apply_init_mult(
  const cutil_GenericType *type, void *data, size_t num
);

/**
 * Applies 'init' function of `type` to the first element of `data`.
 * Corresponds to a no-op if 'init' is NULL.
 *
 * @param[in] type cutil_GenericType to use 'init' function of
 * @param[in, out] data type-generic array to apply 'init' function to
 */
inline void
cutil_GenericType_apply_init(const cutil_GenericType *type, void *data)
{
    cutil_GenericType_apply_init_mult(type, data, 1UL);
}

/**
 * Applies 'clear' function of `type` to the first `num` elements of `data`.
 * Corresponds to a no-op if 'clear' is NULL or `num` is 0.
 *
 * @param[in] type cutil_GenericType to use 'clear' function of
 * @param[in, out] data type-generic array to apply 'clear' function to
 * @param[in] num number of elements to apply 'clear' function to
 */
void
cutil_GenericType_apply_clear_mult(
  const cutil_GenericType *type, void *data, size_t num
);

/**
 * Applies 'clear' function of `type` to the first element of `data`.
 * Corresponds to a no-op if 'clear' is NULL.
 *
 * @param[in] type cutil_GenericType to use 'clear' function of
 * @param[in, out] data type-generic array to apply 'clear' function to
 */
inline void
cutil_GenericType_apply_clear(const cutil_GenericType *type, void *data)
{
    cutil_GenericType_apply_clear_mult(type, data, 1UL);
}

/**
 * Applies 'copy' function of `type` to copy the first `num` elements from `src`
 * to `dst`. Uses 'memcpy' if 'copy' is NULL. Corresponds to a no-op if num is
 * 0.
 *
 * @param[in] type cutil_GenericType to use 'copy' function of
 * @param[in, out] dst type-generic array to copy to
 * @param[in] src type-generic array to copy from
 * @param[in] num number of elements to copy
 */
void
cutil_GenericType_apply_copy_mult(
  const cutil_GenericType *type, void *dst, const void *src, size_t num
);

/**
 * Applies 'copy' function of `type` to copy the first element from `src` to
 * `dst`. Uses 'memcpy' if 'copy' is NULL.
 *
 * @param[in] type cutil_GenericType to use 'copy' function of
 * @param[in, out] dst type-generic array to copy to
 * @param[in] src type-generic array to copy from
 */
inline void
cutil_GenericType_apply_copy(
  const cutil_GenericType *type, void *dst, const void *src
)
{
    cutil_GenericType_apply_copy_mult(type, dst, src, 1UL);
}

/**
 * Reallocs `data` from `old_num` to `new_num` and returns realloc'd memory.
 * Corresponds to a no-op if the sizes are equal. If elements are removed,
 * 'clear' is applied to them. Likewise, if elements are added, 'init' is
 * applied to them.
 *
 * @param[in] type cutil_GenericType to use 'clear' function of
 * @param[in] data type-generic array to realloc
 * @param[in] old_num number of elements of `data` before reallocation
 * @param[in] new_num number of elements of `data` after reallocation
 *
 * @return pointer to realloc'd memory
 */
void *
cutil_GenericType_apply_realloc(
  const cutil_GenericType *type, void *data, size_t old_num, size_t new_num
);

/**
 * Fallback deep-equals implementation. If `type->comp` is available it is
 * used (equality if comparison == 0); otherwise raw-byte comparison is used.
 *
 * @param[in] type cutil_GenericType describing the objects
 * @param[in] lhs left-hand side object pointer
 * @param[in] rhs right-hand side object pointer
 *
 * @return boolean indicating deep equality
 */
cutil_Bool
cutil_GenericType_fallback_deep_equals(
  const cutil_GenericType *type, const void *lhs, const void *rhs
);

/**
 * Applies the deep-equals function of `type` to `lhs` and `rhs`, or uses the
 * fallback deep-equals if no function is specified.
 *
 * @param[in] type cutil_GenericType describing the objects
 * @param[in] lhs left-hand side object pointer
 * @param[in] rhs right-hand side object pointer
 *
 * @return boolean indicating deep equality
 */
inline cutil_Bool
cutil_GenericType_apply_deep_equals(
  const cutil_GenericType *type, const void *lhs, const void *rhs
)
{
    CUTIL_NULL_CHECK(type);
    if (type->deep_equals == NULL) {
        cutil_log_debug("No deep equals function specified, use fallback");
        return cutil_GenericType_fallback_deep_equals(type, lhs, rhs);
    }
    return type->deep_equals(lhs, rhs);
}

/**
 * Fallback comparison function that compares the raw bytes of `lhs` and
 * `rhs` according to the `size` of `type`.
 *
 * @param[in] type cutil_GenericType describing the objects
 * @param[in] lhs left-hand side object pointer
 * @param[in] rhs right-hand side object pointer
 *
 * @return libc-style comparison result (<0, 0, >0)
 */
int
cutil_GenericType_fallback_comp(
  const cutil_GenericType *type, const void *lhs, const void *rhs
);

/**
 * Applies the comparison function of `type` to `lhs` and `rhs`, or uses the
 * fallback comparator if no function is specified.
 *
 * @param[in] type cutil_GenericType describing the objects
 * @param[in] lhs left-hand side object pointer
 * @param[in] rhs right-hand side object pointer
 *
 * @return libc-style comparison result (<0, 0, >0)
 */
inline int
cutil_GenericType_apply_compare(
  const cutil_GenericType *type, const void *lhs, const void *rhs
)
{
    CUTIL_NULL_CHECK(type);
    if (type->comp == NULL) {
        cutil_log_debug("No comparison function specified, use fallback");
        return cutil_GenericType_fallback_comp(type, lhs, rhs);
    }
    return type->comp(lhs, rhs);
}

/**
 * Fallback hash implementation for `type` that hashes the raw bytes of `data`.
 *
 * @param[in] type cutil_GenericType describing the object
 * @param[in] data pointer to object to hash
 *
 * @return hash value computed from the raw bytes of `data`
 */
cutil_hash_t
cutil_GenericType_fallback_hash(
  const cutil_GenericType *type, const void *data
);

/**
 * Applies the hash function of `type` to `data`, or uses the fallback hash if
 * no function is specified.
 *
 * @param[in] type cutil_GenericType describing the object
 * @param[in] data pointer to object to hash
 *
 * @return hash value for `data`
 */
inline cutil_hash_t
cutil_GenericType_apply_hash(const cutil_GenericType *type, const void *data)
{
    CUTIL_NULL_CHECK(type);
    if (type->hash == NULL) {
        cutil_log_debug("No hash function specified, use fallback");
        return cutil_GenericType_fallback_hash(type, data);
    }
    return type->hash(data);
}

/**
 * Fallback serialization implementation for `type` that outputs 'name#hash'.
 * Follows the snprintf pattern: when `buf` is NULL with `buflen` 0, returns
 * the number of bytes needed (including NUL terminator). Otherwise, writes up
 * to `buflen - 1` bytes + NUL terminator to `buf`.
 *
 * @param[in] type cutil_GenericType describing the object
 * @param[in] data pointer to object to serialize
 * @param[out] buf destination buffer, or NULL to query required size
 * @param[in] buflen size of destination buffer, or 0 when querying size
 *
 * @return number of bytes written (excluding NUL terminator), or bytes needed
 *         if truncated or buffer was NULL; does not include the NUL terminator
 *         in the returned count
 */
size_t
cutil_GenericType_fallback_to_string(
  const cutil_GenericType *type, const void *data, char *buf, size_t buflen
);

/**
 * Applies the to_string function of `type` to `data`, or uses the fallback
 * serialization if no function is specified. Follows the snprintf pattern:
 * when `buf` is NULL with `buflen` 0, returns the number of bytes needed
 * (including NUL terminator). Otherwise, writes up to `buflen - 1` bytes +
 * NUL terminator to `buf`.
 *
 * @param[in] type cutil_GenericType describing the object
 * @param[in] data pointer to object to serialize
 * @param[out] buf destination buffer, or NULL to query required size
 * @param[in] buflen size of destination buffer, or 0 when querying size
 *
 * @return number of bytes written (excluding NUL terminator), or bytes needed
 *         if truncated or buffer was NULL; does not include the NUL terminator
 *         in the returned count, or CUTIL_ERROR_SIZE on error
 */
inline size_t
cutil_GenericType_apply_to_string(
  const cutil_GenericType *type, const void *data, char *buf, size_t buflen
)
{
    CUTIL_NULL_CHECK(type);
    if (type->to_string == NULL) {
        cutil_log_debug("No serialization function specified, use fallback");
        return cutil_GenericType_fallback_to_string(type, data, buf, buflen);
    }
    return type->to_string(data, buf, buflen);
}

/**
 * cutil_GenericType for native types
 */

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_CHAR;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_SHORT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_INT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_LONG;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_LLONG;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_UCHAR;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_USHORT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_UINT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ULONG;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ULLONG;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_I8;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_I16;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_I32;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_I64;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_U8;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_U16;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_U32;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_U64;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_SIZET;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHT;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_FLOAT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_DOUBLE;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_LDOUBLE;

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_DATA_GENERIC_TYPE_H_INCLUDED */

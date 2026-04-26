/** cutil/data/generic/set.h
 *
 * Header for arbitrarily typed sets.
 */

#ifndef CUTIL_GENERIC_SET_H_INCLUDED
#define CUTIL_GENERIC_SET_H_INCLUDED

#include <cutil/data/generic/iterator.h>
#include <cutil/data/generic/type.h>
#include <cutil/debug/null.h>
#include <cutil/io/log.h>
#include <cutil/status.h>
#include <cutil/std/stdbool.h>
#include <cutil/std/stdlib.h>
#include <cutil/util/macro.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Type (vtable) for cutil sets.
 */
typedef struct {
    const char *const name;
    void (*const free)(void *data);
    void (*const reset)(void *data);
    void (*const copy)(void *dst, const void *src);
    void *(*const duplicate)(const void *data);
    size_t (*const get_count)(const void *data);
    cutil_Bool (*const contains)(const void *data, const void *elem);
    cutil_Status (*const add)(void *data, const void *elem);
    cutil_Status (*const remove)(void *data, const void *elem);
    const cutil_GenericType *(*const get_elem_type)(const void *data);
    cutil_ConstIterator *(*const get_const_iterator)(const void *data);
    cutil_Iterator *(*const get_iterator)(void *data);
} cutil_SetType;

/**
 * Returns whether `lhs` is equal to `rhs`.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return is `lhs` equal to `rhs`?
 */
cutil_Bool
cutil_SetType_equals(const cutil_SetType *lhs, const cutil_SetType *rhs);

/**
 * Abstract "class" for cutil sets.
 */
typedef struct {
    const cutil_SetType *vtable;
    void *data;
} cutil_Set;

/**
 * Convenience MACRO to check for NULLs in `SET` in debug mode.
 *
 * @param[in] SET cutil_Set to check for NULLs
 */
#define CUTIL_NULL_CHECKS_SET(SET)                                             \
    do {                                                                       \
        CUTIL_NULL_CHECK(SET);                                                 \
        CUTIL_NULL_CHECK(SET->vtable);                                         \
    } while (0)

/**
 * Returns vtable of `set`.
 *
 * @param[in] set cutil_Set object to get vtable of
 *
 * @return vtable of `set`
 */
inline const cutil_SetType *
cutil_Set_get_vtable(const cutil_Set *set)
{
    CUTIL_RETURN_NULL_IF_NULL(set);
    return set->vtable;
}

/**
 * Frees contents of `set`.
 * 
 * @param[in] set cutil_Set object to be cleared
 */
inline void
cutil_Set_clear(cutil_Set *set)
{
    CUTIL_RETURN_IF_NULL(set);
    CUTIL_NULL_CHECK_VTABLE(set->vtable, reset);
    CUTIL_RETURN_IF_NULL(set->vtable->reset);
    set->vtable->free(set->data);
    set->data = NULL;
}

/**
 * Destructor for 'cutil_Set'.
 *
 * @param[in] set cutil_Set object to be destroyed
 */
inline void
cutil_Set_free(cutil_Set *set)
{
    cutil_Set_clear(set);
    free(set);
}

/**
 * Removes all elements from `set`.
 *
 * @param[in] set cutil_Set to reset
 */
inline void
cutil_Set_reset(cutil_Set *set)
{
    CUTIL_NULL_CHECKS_SET(set);
    CUTIL_NULL_CHECK_VTABLE(set->vtable, reset);
    CUTIL_RETURN_IF_NULL(set->vtable->reset);
    set->vtable->reset(set->data);
}

/**
 * Copies contents from `src` to `dst`.
 *
 * @param[out] dst cutil_Set to copy to
 * @param[in] src cutil_Set to be copied
 */
inline void
cutil_Set_copy(cutil_Set *dst, const cutil_Set *src)
{
    CUTIL_NULL_CHECKS_SET(dst);
    CUTIL_NULL_CHECKS_SET(src);
    if (!cutil_SetType_equals(dst->vtable, src->vtable)) {
        cutil_log_warn(
          "Incompatible set types: '%s' vs. '%s'", dst->vtable->name,
          src->vtable->name
        );
        return;
    }
    CUTIL_NULL_CHECK_VTABLE(src->vtable, copy);
    CUTIL_RETURN_IF_NULL(src->vtable->copy);
    src->vtable->copy(dst->data, src->data);
}

/**
 * Returns newly malloc'd copy of cutil_Set `set`.
 *
 * @param[in] set cutil_Set to be copied
 *
 * @return newly malloc'd copy of `set`
 */
inline cutil_Set *
cutil_Set_duplicate(const cutil_Set *set)
{
    CUTIL_NULL_CHECKS_SET(set);
    CUTIL_NULL_CHECK_VTABLE(set->vtable, duplicate);
    CUTIL_RETURN_NULL_IF_NULL(set->vtable->duplicate);
    cutil_Set *const dup = (cutil_Set *) CUTIL_MALLOC_OBJECT(dup);
    dup->vtable = set->vtable;
    dup->data = set->vtable->duplicate(set->data);
    return dup;
}

/**
 * Returns number of elements in `set`.
 *
 * @param[in] set cutil_Set object to get number of elements of
 *
 * @return number of elements in `set`
 */
inline size_t
cutil_Set_get_count(const cutil_Set *set)
{
    CUTIL_NULL_CHECKS_SET(set);
    CUTIL_NULL_CHECK_VTABLE(set->vtable, get_count);
    CUTIL_RETURN_VAL_IF_NULL(set->vtable->get_count, 0UL);
    return set->vtable->get_count(set->data);
}

/**
 * Returns if `set` contains `elem`.
 *
 * @param[in] set cutil_Set to search in
 * @param[in] elem element to search for
 *
 * @return does `set` contain `elem`?
 */
inline cutil_Bool
cutil_Set_contains(const cutil_Set *set, const void *elem)
{
    CUTIL_NULL_CHECKS_SET(set);
    CUTIL_NULL_CHECK_VTABLE(set->vtable, contains);
    CUTIL_RETURN_VAL_IF_NULL(set->vtable->contains, false);
    return set->vtable->contains(set->data, elem);
}

/**
 * Adds `elem` to `set`.
 *
 * @param[in] set cutil_Set to add element to
 * @param[in] elem element to be added
 *
 * @return error code
 */
inline cutil_Status
cutil_Set_add(cutil_Set *set, const void *elem)
{
    CUTIL_NULL_CHECKS_SET(set);
    CUTIL_NULL_CHECK_VTABLE(set->vtable, add);
    CUTIL_RETURN_VAL_IF_NULL(set->vtable->add, CUTIL_STATUS_FAILURE);
    return set->vtable->add(set->data, elem);
}

/**
 * Removes `elem` from `set`.
 *
 * @param[in] set cutil_Set to remove element from
 * @param[in] elem element to be removed
 *
 * @return error code
 */
inline cutil_Status
cutil_Set_remove(cutil_Set *set, const void *elem)
{
    CUTIL_NULL_CHECKS_SET(set);
    CUTIL_NULL_CHECK_VTABLE(set->vtable, remove);
    CUTIL_RETURN_VAL_IF_NULL(set->vtable->remove, CUTIL_STATUS_FAILURE);
    return set->vtable->remove(set->data, elem);
}

/**
 * Returns element type of `set`.
 *
 * @param[in] set cutil_Set object to get element type of
 *
 * @return element type of `set`, or NULL if not available
 */
inline const cutil_GenericType *
cutil_Set_get_elem_type(const cutil_Set *set)
{
    CUTIL_NULL_CHECKS_SET(set);
    CUTIL_NULL_CHECK_VTABLE(set->vtable, get_elem_type);
    CUTIL_RETURN_NULL_IF_NULL(set->vtable->get_elem_type);
    return set->vtable->get_elem_type(set->data);
}

/**
 * Returns a newly heap-allocated read-only iterator over the elements of
 * `set`. The returned iterator must be free'd.
 *
 * @param[in] set cutil_Set to iterate over
 *
 * @return newly malloc'd cutil_ConstIterator, or NULL on failure
 */
inline cutil_ConstIterator *
cutil_Set_get_const_iterator(const cutil_Set *set)
{
    CUTIL_RETURN_NULL_IF_NULL(set);
    CUTIL_NULL_CHECK(set->vtable);
    CUTIL_NULL_CHECK_VTABLE(set->vtable, get_const_iterator);
    CUTIL_RETURN_NULL_IF_NULL(set->vtable->get_const_iterator);
    return set->vtable->get_const_iterator(set->data);
}

/**
 * Returns a newly heap-allocated read-write iterator over the elements of
 * `set`. Supports `remove`; `set` operation is NULL (replacing an element is
 * semantically meaningless). The returned iterator must be free'd.
 *
 * @param[in] set cutil_Set to iterate over
 *
 * @return newly malloc'd cutil_Iterator, or NULL on failure
 */
inline cutil_Iterator *
cutil_Set_get_iterator(cutil_Set *set)
{
    CUTIL_RETURN_NULL_IF_NULL(set);
    CUTIL_NULL_CHECK(set->vtable);
    CUTIL_NULL_CHECK_VTABLE(set->vtable, get_iterator);
    CUTIL_RETURN_NULL_IF_NULL(set->vtable->get_iterator);
    return set->vtable->get_iterator(set->data);
}

/**
 * Returns whether Sets `lhs` and `rhs` contain exactly the same elements, as
 * determined by `cutil_Set_contains`. Both NULL or the same pointer compare
 * equal. NULL and non-NULL compare unequal.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return Are `lhs` and `rhs` deeply equal?
 */
cutil_Bool
cutil_Set_deep_equals(const cutil_Set *lhs, const cutil_Set *rhs);

/**
 * Three-way comparison of Sets `lhs` and `rhs`. Ordered by: element type
 * (byte-wise), then count, then combined element hash as a tie-breaker. NULL
 * sorts before non-NULL; identical pointers compare equal.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return negative if lhs < rhs, 0 if equal, positive if lhs > rhs
 */
int
cutil_Set_compare(const cutil_Set *lhs, const cutil_Set *rhs);

/**
 * Computes an order-independent hash of `set` by XOR-folding the hash of
 * each element. Returns CUTIL_HASH_C(0) for NULL or an empty set.
 *
 * @param[in] set Set to hash
 *
 * @return hash value
 */
cutil_hash_t
cutil_Set_hash(const cutil_Set *set);

/**
 * Serializes `set` as "{elem0,elem1,...,elemN-1}" into `buf`. Follows the
 * snprintf convention: pass buf=NULL with buflen=0 to query the required buffer
 * size (return value excludes NUL). When buf is not NULL and buflen is
 * sufficient, writes at most buflen-1 chars + NUL and returns chars written
 * (excluding NUL). Returns 0 and logs a warning if buffer is too small. NULL is
 * serialized as "NULL".
 *
 * @param[in] set Set to serialize, or NULL
 * @param[out] buf destination buffer, or NULL to query required size
 * @param[in] buflen size of destination buffer in bytes, or 0 when querying
 *
 * @return number of characters written (excluding NUL), or required size when
 *         buf is NULL
 */
size_t
cutil_Set_to_string(const cutil_Set *set, char *buf, size_t buflen);

/**
 * Generic-type support functions
 */

/**
 * Generic (i.e, void-argument) version of 'cutil_Set_clear'.
 *
 * @param[in] set cutil_Set to clear
 */
inline void
cutil_Set_clear_generic(void *obj)
{
    cutil_Set *const set = (cutil_Set *) obj;
    cutil_Set_clear(set);
}

/**
 * Generic (i.e, void-argument) version of 'cutil_Set_copy'.
 *
 * @param[out] dst cutil_Set to copy to
 * @param[in] src cutil_Set to be copied
 */
inline void
cutil_Set_copy_generic(void *dst, const void *src)
{
    cutil_Set *const set_dst = (cutil_Set *) dst;
    const cutil_Set *const set_src = (const cutil_Set *) src;
    cutil_Set_copy(set_dst, set_src);
}

/**
 * Generic (i.e, void-argument) version of 'cutil_Set_deep_equals'.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return Are `lhs` and `rhs` deeply equal?
 */
cutil_Bool
cutil_Set_deep_equals_generic(const void *lhs, const void *rhs);

/**
 * Generic (i.e, void-argument) version of 'cutil_Set_compare'.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return negative if lhs < rhs, 0 if equal, positive if lhs > rhs
 */
int
cutil_Set_compare_generic(const void *lhs, const void *rhs);

/**
 * Generic (i.e, void-argument) version of 'cutil_Set_hash'.
 *
 * @param[in] set Set to hash
 *
 * @return hash value
 */
cutil_hash_t
cutil_Set_hash_generic(const void *set);

/**
 * Generic (i.e, void-argument) version of 'cutil_Set_to_string'.
 *
 * @param[in] set Set to serialize, or NULL
 * @param[out] buf destination buffer, or NULL to query required size
 * @param[in] buflen size of destination buffer in bytes, or 0 when querying
 *
 * @return number of characters written (excluding NUL), or required size when
 *         buf is NULL
 */
size_t
cutil_Set_to_string_generic(const void *set, char *buf, size_t buflen);

#undef CUTIL_NULL_CHECKS_SET

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_GENERIC_SET_H_INCLUDED */

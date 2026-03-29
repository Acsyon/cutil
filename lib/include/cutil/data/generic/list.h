/** cutil/data/generic/list.h
 *
 * Header for arbitrarily typed ordered lists.
 *
 */

#ifndef CUTIL_GENERIC_LIST_H_INCLUDED
#define CUTIL_GENERIC_LIST_H_INCLUDED

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

/** Sentinel value returned by `cutil_List_locate` when no element is found. */
#define CUTIL_LIST_NPOS ((size_t) -1)

/**
 * Type (vtable) for cutil lists.
 */
typedef struct {
    const char *const name;
    void (*const free)(void *data);
    void (*const reset)(void *data);
    void (*const copy)(void *dst, const void *src);
    void *(*const duplicate)(const void *data);
    size_t (*const get_count)(const void *data);
    int (*const get)(const void *data, size_t idx, void *out);
    const void *(*const get_ptr)(const void *data, size_t idx);
    size_t (*const locate)(const void *data, const void *elem);
    int (*const set)(void *data, size_t idx, const void *elem);
    int (*const append)(void *data, const void *elem);
    int (*const insert_mult)(
      void *data, size_t pos, size_t num, const void *elems
    );
    int (*const remove_mult)(void *data, size_t pos, size_t num);
    void (*const sort_custom)(void *data, cutil_CompFunc *comp);
    const cutil_GenericType *(*const get_elem_type)(const void *data);
    cutil_ConstIterator *(*const get_const_iterator)(const void *data);
    cutil_Iterator *(*const get_iterator)(void *data);
} cutil_ListType;

/**
 * Returns whether `lhs` is equal to `rhs`.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return is `lhs` equal to `rhs`?
 */
cutil_Bool
cutil_ListType_equals(const cutil_ListType *lhs, const cutil_ListType *rhs);

/**
 * Abstract "class" for cutil lists.
 */
typedef struct {
    const cutil_ListType *vtable;
    void *data;
} cutil_List;

/**
 * Convenience MACRO to check for NULLs in `LIST` in debug mode.
 *
 * @param[in] LIST cutil_List to check for NULLs
 */
#define CUTIL_NULL_CHECKS_LIST(LIST)                                           \
    do {                                                                       \
        CUTIL_NULL_CHECK(LIST);                                                \
        CUTIL_NULL_CHECK(LIST->vtable);                                        \
    } while (0)

/**
 * Returns vtable of `list`.
 *
 * @param[in] list cutil_List object to get vtable of
 *
 * @return vtable of `list`
 */
inline const cutil_ListType *
cutil_List_get_vtable(const cutil_List *list)
{
    CUTIL_RETURN_NULL_IF_NULL(list);
    return list->vtable;
}

/**
 * Frees contents of `list`.
 *
 * @param[in] list cutil_List object to be cleared
 */
inline void
cutil_List_clear(cutil_List *list)
{
    CUTIL_RETURN_IF_NULL(list);
    CUTIL_NULL_CHECK_VTABLE(list->vtable, reset);
    CUTIL_RETURN_IF_NULL(list->vtable->reset);
    list->vtable->free(list->data);
    list->data = NULL;
}

/**
 * Destructor for 'cutil_List'.
 *
 * @param[in] list cutil_List object to be destroyed
 */
inline void
cutil_List_free(cutil_List *list)
{
    cutil_List_clear(list);
    free(list);
}

/**
 * Removes all elements from `list`.
 *
 * @param[in] list cutil_List to reset
 */
inline void
cutil_List_reset(cutil_List *list)
{
    CUTIL_NULL_CHECKS_LIST(list);
    CUTIL_NULL_CHECK_VTABLE(list->vtable, reset);
    CUTIL_RETURN_IF_NULL(list->vtable->reset);
    list->vtable->reset(list->data);
}

/**
 * Copies contents from `src` to `dst`.
 *
 * @param[out] dst cutil_List to copy to
 * @param[in] src cutil_List to be copied
 */
inline void
cutil_List_copy(cutil_List *dst, const cutil_List *src)
{
    CUTIL_NULL_CHECKS_LIST(dst);
    CUTIL_NULL_CHECKS_LIST(src);
    if (!cutil_ListType_equals(dst->vtable, src->vtable)) {
        cutil_log_warn(
          "Incompatible list types: '%s' vs. '%s'", dst->vtable->name,
          src->vtable->name
        );
        return;
    }
    CUTIL_NULL_CHECK_VTABLE(src->vtable, copy);
    CUTIL_RETURN_IF_NULL(src->vtable->copy);
    src->vtable->copy(dst->data, src->data);
}

/**
 * Returns newly malloc'd copy of cutil_List `list`.
 *
 * @param[in] list cutil_List to be copied
 *
 * @return newly malloc'd copy of `list`
 */
inline cutil_List *
cutil_List_duplicate(const cutil_List *list)
{
    CUTIL_NULL_CHECKS_LIST(list);
    CUTIL_NULL_CHECK_VTABLE(list->vtable, duplicate);
    CUTIL_RETURN_NULL_IF_NULL(list->vtable->duplicate);
    cutil_List *const dup = CUTIL_MALLOC_OBJECT(dup);
    dup->vtable = list->vtable;
    dup->data = list->vtable->duplicate(list->data);
    return dup;
}

/**
 * Returns number of elements in `list`.
 *
 * @param[in] list cutil_List object to get number of elements of
 *
 * @return number of elements in `list`
 */
inline size_t
cutil_List_get_count(const cutil_List *list)
{
    CUTIL_NULL_CHECKS_LIST(list);
    CUTIL_NULL_CHECK_VTABLE(list->vtable, get_count);
    CUTIL_RETURN_VAL_IF_NULL(list->vtable->get_count, 0UL);
    return list->vtable->get_count(list->data);
}

/**
 * Copies element at `idx` into `out`.
 *
 * @param[in] list cutil_List to get element from
 * @param[in] idx index of element to get
 * @param[out] out destination buffer for element
 *
 * @return error code
 */
inline int
cutil_List_get(const cutil_List *list, size_t idx, void *out)
{
    CUTIL_NULL_CHECKS_LIST(list);
    CUTIL_NULL_CHECK_VTABLE(list->vtable, get);
    CUTIL_RETURN_VAL_IF_NULL(list->vtable->get, CUTIL_STATUS_FAILURE);
    return list->vtable->get(list->data, idx, out);
}

/**
 * Returns const pointer to element at `idx` in-place.
 *
 * @param[in] list cutil_List to get element from
 * @param[in] idx  index of element to get
 *
 * @return const pointer to element, or NULL on failure
 */
inline const void *
cutil_List_get_ptr(const cutil_List *list, size_t idx)
{
    CUTIL_NULL_CHECKS_LIST(list);
    CUTIL_NULL_CHECK_VTABLE(list->vtable, get_ptr);
    CUTIL_RETURN_NULL_IF_NULL(list->vtable->get_ptr);
    return list->vtable->get_ptr(list->data, idx);
}

/**
 * Returns the index of the first occurrence of `elem` in `list`, or
 * `CUTIL_LIST_NPOS` if not found.
 *
 * @param[in] list cutil_List to search in
 * @param[in] elem element to search for
 *
 * @return index of first occurrence, or CUTIL_LIST_NPOS if not found
 */
inline size_t
cutil_List_locate(const cutil_List *list, const void *elem)
{
    CUTIL_NULL_CHECKS_LIST(list);
    CUTIL_NULL_CHECK_VTABLE(list->vtable, locate);
    CUTIL_RETURN_VAL_IF_NULL(list->vtable->locate, CUTIL_LIST_NPOS);
    return list->vtable->locate(list->data, elem);
}

/**
 * Returns whether `list` contains `elem`.
 *
 * Implemented entirely at the interface level via `cutil_List_locate`.
 *
 * @param[in] list cutil_List to search in
 * @param[in] elem element to search for
 *
 * @return does `list` contain `elem`?
 */
inline cutil_Bool
cutil_List_contains(const cutil_List *list, const void *elem)
{
    return (cutil_List_locate(list, elem) != CUTIL_LIST_NPOS);
}

/**
 * Swaps element at `idx` of `list` with `elem`, writing the old element into
 * `elem`.
 *
 * @param[in] list cutil_List to swap element in
 * @param[in] idx index of element to swap
 * @param[in, out] elem element to swap with and buffer to write old element
 * into
 *
 * @return CUTIL_STATUS_SUCCESS on success, CUTIL_STATUS_FAILURE if `idx` is
 * out of bounds
 */
int
cutil_List_memswap_elem(cutil_List *list, size_t idx, void *elem);

/**
 * Sets element at `idx` of `list` to `elem`.
 *
 * @param[in] list cutil_List to set element in
 * @param[in] idx index of element to set
 * @param[in] elem element to set at index `idx`
 *
 * @return error code
 */
inline int
cutil_List_set(cutil_List *list, size_t idx, const void *elem)
{
    CUTIL_NULL_CHECKS_LIST(list);
    CUTIL_NULL_CHECK_VTABLE(list->vtable, set);
    CUTIL_RETURN_VAL_IF_NULL(list->vtable->set, CUTIL_STATUS_FAILURE);
    return list->vtable->set(list->data, idx, elem);
}

/**
 * Appends `elem` to the end of `list`.
 *
 * @param[in] list cutil_List to append element to
 * @param[in] elem element to be appended
 *
 * @return error code
 */
inline int
cutil_List_append(cutil_List *list, const void *elem)
{
    CUTIL_NULL_CHECKS_LIST(list);
    CUTIL_NULL_CHECK_VTABLE(list->vtable, append);
    CUTIL_RETURN_VAL_IF_NULL(list->vtable->append, CUTIL_STATUS_FAILURE);
    return list->vtable->append(list->data, elem);
}

/**
 * Inserts `num` elements from `elems` at position `pos` in `list`.
 *
 * @param[in] list  cutil_List to insert elements into
 * @param[in] pos   index at which to insert
 * @param[in] num     number of elements to insert
 * @param[in] elems pointer to array of elements to insert
 *
 * @return error code
 */
inline int
cutil_List_insert_mult(
  cutil_List *list, size_t pos, size_t num, const void *elems
)
{
    CUTIL_NULL_CHECKS_LIST(list);
    CUTIL_NULL_CHECK_VTABLE(list->vtable, insert_mult);
    CUTIL_RETURN_VAL_IF_NULL(list->vtable->insert_mult, CUTIL_STATUS_FAILURE);
    return list->vtable->insert_mult(list->data, pos, num, elems);
}

/**
 * Inserts a single element at position `pos` in `list`.
 *
 * Implemented entirely at the interface level via `cutil_List_insert_mult`.
 *
 * @param[in] list cutil_List to insert element into
 * @param[in] pos  index at which to insert
 * @param[in] elem element to insert
 *
 * @return error code
 */
inline int
cutil_List_insert(cutil_List *list, size_t pos, const void *elem)
{
    return cutil_List_insert_mult(list, pos, 1UL, elem);
}

/**
 * Removes `num` consecutive elements starting at position `pos` from `list`.
 *
 * @param[in] list cutil_List to remove elements from
 * @param[in] pos  index of first element to remove
 * @param[in] num    number of elements to remove
 *
 * @return error code
 */
inline int
cutil_List_remove_mult(cutil_List *list, size_t pos, size_t num)
{
    CUTIL_NULL_CHECKS_LIST(list);
    CUTIL_NULL_CHECK_VTABLE(list->vtable, remove_mult);
    CUTIL_RETURN_VAL_IF_NULL(list->vtable->remove_mult, CUTIL_STATUS_FAILURE);
    return list->vtable->remove_mult(list->data, pos, num);
}

/**
 * Removes the element at position `pos` from `list`.
 *
 * Implemented entirely at the interface level via `cutil_List_remove_mult`.
 *
 * @param[in] list cutil_List to remove element from
 * @param[in] pos  index of element to remove
 *
 * @return error code
 */
inline int
cutil_List_remove(cutil_List *list, size_t pos)
{
    return cutil_List_remove_mult(list, pos, 1UL);
}

/**
 * Removes all elements at indices [begin, end] inclusive from `list`.
 *
 * Implemented entirely at the interface level via `cutil_List_remove_mult`.
 *
 * @param[in] list  cutil_List to remove elements from
 * @param[in] begin index of first element to remove
 * @param[in] end   index of last element to remove (inclusive)
 *
 * @return error code
 */
inline int
cutil_List_remove_from_to(cutil_List *list, size_t begin, size_t end)
{
    if (begin > end) {
        return CUTIL_STATUS_FAILURE;
    }
    return cutil_List_remove_mult(list, begin, end - begin + 1UL);
}

/**
 * Sorts `list` using comparator `comp`.
 *
 * @param[in] list cutil_List to sort
 * @param[in] comp comparator function
 */
inline void
cutil_List_sort_custom(cutil_List *list, cutil_CompFunc *comp)
{
    CUTIL_NULL_CHECKS_LIST(list);
    CUTIL_NULL_CHECK_VTABLE(list->vtable, sort_custom);
    CUTIL_RETURN_IF_NULL(list->vtable->sort_custom);
    list->vtable->sort_custom(list->data, comp);
}

/**
 * Returns element type of `list`.
 *
 * @param[in] list cutil_List object to get element type of
 *
 * @return element type of `list`, or NULL if not available
 */
inline const cutil_GenericType *
cutil_List_get_elem_type(const cutil_List *list)
{
    CUTIL_NULL_CHECKS_LIST(list);
    CUTIL_NULL_CHECK_VTABLE(list->vtable, get_elem_type);
    CUTIL_RETURN_NULL_IF_NULL(list->vtable->get_elem_type);
    return list->vtable->get_elem_type(list->data);
}

/**
 * Sorts `list` using the element type's default comparator.
 *
 * Implemented entirely at the interface level via `cutil_List_sort_custom`.
 *
 * @param[in] list cutil_List to sort
 */
inline void
cutil_List_sort(cutil_List *list)
{
    const cutil_GenericType *const type = cutil_List_get_elem_type(list);
    CUTIL_RETURN_IF_NULL(type);
    cutil_List_sort_custom(list, type->comp);
}

/**
 * Returns a newly heap-allocated read-only iterator over the elements of
 * `list`. The returned iterator must be free'd.
 *
 * @param[in] list cutil_List to iterate over
 *
 * @return newly malloc'd cutil_ConstIterator, or NULL on failure
 */
inline cutil_ConstIterator *
cutil_List_get_const_iterator(const cutil_List *list)
{
    CUTIL_RETURN_NULL_IF_NULL(list);
    CUTIL_NULL_CHECK(list->vtable);
    CUTIL_NULL_CHECK_VTABLE(list->vtable, get_const_iterator);
    CUTIL_RETURN_NULL_IF_NULL(list->vtable->get_const_iterator);
    return list->vtable->get_const_iterator(list->data);
}

/**
 * Returns a newly heap-allocated read-write iterator over the elements of
 * `list`. The returned iterator must be free'd.
 *
 * @param[in] list cutil_List to iterate over
 *
 * @return newly malloc'd cutil_Iterator, or NULL on failure
 */
inline cutil_Iterator *
cutil_List_get_iterator(cutil_List *list)
{
    CUTIL_RETURN_NULL_IF_NULL(list);
    CUTIL_NULL_CHECK(list->vtable);
    CUTIL_NULL_CHECK_VTABLE(list->vtable, get_iterator);
    CUTIL_RETURN_NULL_IF_NULL(list->vtable->get_iterator);
    return list->vtable->get_iterator(list->data);
}

/**
 * Returns whether Lists `lhs` and `rhs` contain exactly the same elements in
 * the same order. Both NULL or the same pointer compare equal. NULL and
 * non-NULL compare unequal.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return Are `lhs` and `rhs` deeply equal?
 */
cutil_Bool
cutil_List_deep_equals(const cutil_List *lhs, const cutil_List *rhs);

/**
 * Three-way lexicographic comparison of Lists `lhs` and `rhs`. Ordered by:
 * element type (byte-wise), then element-by-element, then by count as a
 * tiebreaker. NULL sorts before non-NULL; identical pointers compare equal.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return negative if lhs < rhs, 0 if equal, positive if lhs > rhs
 */
int
cutil_List_compare(const cutil_List *lhs, const cutil_List *rhs);

/**
 * Computes an order-sensitive hash of `list` by combining element hashes.
 * Returns CUTIL_HASH_C(0) for NULL or an empty list.
 *
 * @param[in] list List to hash
 *
 * @return hash value
 */
cutil_hash_t
cutil_List_hash(const cutil_List *list);

/**
 * Serializes `list` as "[elem0,elem1,...,elemN-1]" into `buf`. Follows the
 * snprintf convention: pass buf=NULL with buflen=0 to query the required buffer
 * size (return value excludes NUL). When buf is not NULL and buflen is
 * sufficient, writes at most buflen-1 chars + NUL and returns chars written
 * (excluding NUL). Returns 0 and logs a warning if buffer is too small. NULL is
 * serialized as "NULL".
 *
 * @param[in] list List to serialize, or NULL
 * @param[out] buf destination buffer, or NULL to query required size
 * @param[in] buflen size of destination buffer in bytes, or 0 when querying
 *
 * @return number of characters written (excluding NUL), or required size when
 *         buf is NULL
 */
size_t
cutil_List_to_string(const cutil_List *list, char *buf, size_t buflen);

/**
 * Generic-type support functions
 */

/**
 * Generic (i.e, void-argument) version of 'cutil_List_clear'.
 *
 * @param[in] obj cutil_List to clear
 */
inline void
cutil_List_clear_generic(void *obj)
{
    cutil_List *const list = (cutil_List *) obj;
    cutil_List_clear(list);
}

/**
 * Generic (i.e, void-argument) version of 'cutil_List_copy'.
 *
 * @param[out] dst cutil_List to copy to
 * @param[in] src cutil_List to be copied
 */
inline void
cutil_List_copy_generic(void *dst, const void *src)
{
    cutil_List *const list_dst = (cutil_List *) dst;
    const cutil_List *const list_src = (const cutil_List *) src;
    cutil_List_copy(list_dst, list_src);
}

/**
 * Generic (i.e, void-argument) version of 'cutil_List_deep_equals'.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return Are `lhs` and `rhs` deeply equal?
 */
cutil_Bool
cutil_List_deep_equals_generic(const void *lhs, const void *rhs);

/**
 * Generic (i.e, void-argument) version of 'cutil_List_compare'.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return negative if lhs < rhs, 0 if equal, positive if lhs > rhs
 */
int
cutil_List_compare_generic(const void *lhs, const void *rhs);

/**
 * Generic (i.e, void-argument) version of 'cutil_List_hash'.
 *
 * @param[in] list List to hash
 *
 * @return hash value
 */
cutil_hash_t
cutil_List_hash_generic(const void *list);

/**
 * Generic (i.e, void-argument) version of 'cutil_List_to_string'.
 *
 * @param[in] list List to serialize, or NULL
 * @param[out] buf destination buffer, or NULL to query required size
 * @param[in] buflen size of destination buffer in bytes, or 0 when querying
 *
 * @return number of characters written (excluding NUL), or required size when
 *         buf is NULL
 */
size_t
cutil_List_to_string_generic(const void *list, char *buf, size_t buflen);

#undef CUTIL_NULL_CHECKS_LIST

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_GENERIC_LIST_H_INCLUDED */

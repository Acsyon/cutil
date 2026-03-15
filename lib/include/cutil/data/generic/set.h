/** cutil/data/generic/set.h
 *
 * Header for arbitrarily typed sets.
 *
 */

#ifndef CUTIL_GENERIC_SET_H_INCLUDED
#define CUTIL_GENERIC_SET_H_INCLUDED

#include <cutil/debug/null.h>
#include <cutil/io/log.h>
#include <cutil/status.h>
#include <cutil/std/stdbool.h>
#include <cutil/std/stdlib.h>
#include <cutil/data/generic/type.h>
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
    void (*const clear)(void *data);
    void (*const copy)(void *dst, const void *src);
    void *(*const duplicate)(const void *data);
    size_t (*const get_count)(const void *data);
    cutil_Bool (*const contains)(const void *data, const void *elem);
    int (*const add)(void *data, const void *elem);
    int (*const remove)(void *data, const void *elem);
    const cutil_GenericType *(*const get_elem_type)(const void *data);
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
 * Destructor for 'cutil_Set'.
 *
 * @param[in] set cutil_Set object to be destroyed
 */
inline void
cutil_Set_free(cutil_Set *set)
{
    CUTIL_RETURN_IF_NULL(set);
    CUTIL_NULL_CHECK(set->vtable->free);
    set->vtable->free(set->data);
    free(set);
}

/**
 * Removes all elements from `set`.
 *
 * @param[in] set cutil_Set to clear
 */
inline void
cutil_Set_clear(cutil_Set *set)
{
    CUTIL_NULL_CHECKS_SET(set);
    CUTIL_NULL_CHECK_VTABLE(set->vtable, clear);
    CUTIL_RETURN_IF_NULL(set->vtable->clear);
    set->vtable->clear(set->data);
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
    cutil_Set *const dup = CUTIL_MALLOC_OBJECT(dup);
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
inline int
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
inline int
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

#undef CUTIL_NULL_CHECKS_SET

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_GENERIC_SET_H_INCLUDED */

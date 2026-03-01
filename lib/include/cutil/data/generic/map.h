/** cutil/generic/map.h
 *
 * Header for arbitrarily typed maps.
 *
 */

#ifndef CUTIL_GENERIC_MAP_H_INCLUDED
#define CUTIL_GENERIC_MAP_H_INCLUDED

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
 * Type (vtable) for cutil maps.
 */
typedef struct {
    const char *const name;
    void (*const free)(void *data);
    void (*const clear)(void *data);
    void (*const copy)(void *dst, const void *src);
    void *(*const duplicate)(const void *data);
    size_t (*const get_count)(const void *data);
    int (*const remove)(void *data, const void *key);
    cutil_Bool (*const contains)(const void *data, const void *key);
    int (*const get)(const void *data, const void *key, void *val);
    const void *(*const get_ptr)(const void *data, const void *key);
    int (*const set)(void *data, const void *key, const void *val);
    const cutil_GenericType *(*const get_key_type)(const void *data);
    const cutil_GenericType *(*const get_val_type)(const void *data);
} cutil_MapType;

/**
 * Returns whether `lhs` is equal to `rhs`.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return is `lhs` equal to `rhs`?
 */
cutil_Bool
cutil_MapType_equals(const cutil_MapType *lhs, const cutil_MapType *rhs);

/**
 * Abstract "class" for cutil maps.
 */
typedef struct {
    const cutil_MapType *vtable;
    void *data;
} cutil_Map;

/**
 * Convenience MACRO to check for NULLs in `MAP` in debug mode.
 *
 * @param[in] MAP cutil_Map to check for NULLs
 */
#define CUTIL_NULL_CHECKS_MAP(MAP)                                             \
    do {                                                                       \
        CUTIL_NULL_CHECK(MAP);                                                 \
        CUTIL_NULL_CHECK(MAP->vtable);                                         \
    } while (0)

/**
 * Returns vtable of `map`.
 *
 * @param[in] map cutil_Map object to get vtable of
 *
 * @return vtable of `map`
 */
inline const cutil_MapType *
cutil_Map_get_vtable(const cutil_Map *map)
{
    CUTIL_RETURN_NULL_IF_NULL(map);
    return map->vtable;
}

/**
 * Destructor for 'cutil_Map'.
 *
 * @param[in] map cutil_Map object to be destroyed
 */
inline void
cutil_Map_free(cutil_Map *map)
{
    CUTIL_RETURN_IF_NULL(map);
    CUTIL_NULL_CHECK(map->vtable->free);
    map->vtable->free(map->data);
    free(map);
}

/**
 * Removes all entries from `map`.
 *
 * @param[in] map cutil_Map to clear
 */
inline void
cutil_Map_clear(cutil_Map *map)
{
    CUTIL_NULL_CHECKS_MAP(map);
    CUTIL_NULL_CHECK_VTABLE(map->vtable, clear);
    CUTIL_RETURN_IF_NULL(map->vtable->clear);
    map->vtable->clear(map->data);
}

/**
 * Copies contents from `src` to `dst`.
 *
 * @param[out] dst cutil_Map to copy to
 * @param[in] src cutil_Map to be copied
 */
inline void
cutil_Map_copy(cutil_Map *dst, const cutil_Map *src)
{
    CUTIL_NULL_CHECKS_MAP(dst);
    CUTIL_NULL_CHECKS_MAP(src);
    if (!cutil_MapType_equals(dst->vtable, src->vtable)) {
        cutil_log_warn(
          "Incompatible map types: '%s' vs. '%s'", dst->vtable->name,
          src->vtable->name
        );
        return;
    }
    CUTIL_NULL_CHECK_VTABLE(src->vtable, copy);
    CUTIL_RETURN_IF_NULL(src->vtable->copy);
    src->vtable->copy(dst->data, src->data);
}

/**
 * Returns newly malloc'd copy of cutil_Map `map`.
 *
 * @param[in] map cutil_Map to be copied
 *
 * @return newly malloc'd copy of `map`
 */
inline cutil_Map *
cutil_Map_duplicate(const cutil_Map *map)
{
    CUTIL_NULL_CHECKS_MAP(map);
    CUTIL_NULL_CHECK_VTABLE(map->vtable, duplicate);
    CUTIL_RETURN_NULL_IF_NULL(map->vtable->duplicate);
    cutil_Map *const dup = CUTIL_MALLOC_OBJECT(dup);
    dup->vtable = map->vtable;
    dup->data = map->vtable->duplicate(map->data);
    return dup;
}

/**
 * Returns number of entries in `map`.
 *
 * @param[in] map cutil_Map object to get number of entries of
 *
 * @return number of entries in `map`
 */
inline size_t
cutil_Map_get_count(const cutil_Map *map)
{
    CUTIL_NULL_CHECKS_MAP(map);
    CUTIL_NULL_CHECK_VTABLE(map->vtable, get_count);
    CUTIL_RETURN_VAL_IF_NULL(map->vtable->get_count, 0UL);
    return map->vtable->get_count(map->data);
}

/**
 * Removes entry with key `key` from `map`.
 *
 * @param[in] map cutil_Map to remove key from
 * @param[in] key key to be deleted
 *
 * @return error code
 */
inline int
cutil_Map_remove(cutil_Map *map, const void *key)
{
    CUTIL_NULL_CHECKS_MAP(map);
    CUTIL_NULL_CHECK_VTABLE(map->vtable, remove);
    CUTIL_RETURN_VAL_IF_NULL(map->vtable->remove, EXIT_FAILURE);
    return map->vtable->remove(map->data, key);
}

/**
 * Returns if `map` contains `key`.
 *
 * @param[in] map cutil_Map to search in
 * @param[in] key key to search for
 *
 * @return does `map` contain `key`?
 */
inline cutil_Bool
cutil_Map_contains(const cutil_Map *map, const void *key)
{
    CUTIL_NULL_CHECKS_MAP(map);
    CUTIL_NULL_CHECK_VTABLE(map->vtable, contains);
    CUTIL_RETURN_VAL_IF_NULL(map->vtable->contains, false);
    return map->vtable->contains(map->data, key);
}

/**
 * Write value corresponding to key `key` to `val`.
 *
 * @param[in] map cutil_Map to search in
 * @param[in] key key to search for
 * @param[out] val value of found key for
 *
 * @return error code
 */
inline int
cutil_Map_get(const cutil_Map *map, const void *key, void *val)
{
    CUTIL_NULL_CHECKS_MAP(map);
    CUTIL_NULL_CHECK_VTABLE(map->vtable, get);
    CUTIL_RETURN_VAL_IF_NULL(map->vtable->get, CUTIL_STATUS_FAILURE);
    return map->vtable->get(map->data, key, val);
}

/**
 * Return ptr to key corresponding to value `val`.
 *
 * @param[in] map cutil_Map to be worked with
 * @param[in] key key to be searched for
 *
 * @return ptr to key corresponding to value `val`, NULL if not present
 */
inline const void *
cutil_Map_get_ptr(const cutil_Map *map, const void *key)
{
    CUTIL_NULL_CHECKS_MAP(map);
    CUTIL_NULL_CHECK_VTABLE(map->vtable, get_ptr);
    CUTIL_RETURN_NULL_IF_NULL(map->vtable->get_ptr);
    return map->vtable->get_ptr(map->data, key);
}

/**
 * Insert key-value pair [`key`, `val`] into 'cutil_Map' and overwrite if
 * `key` is already present.
 *
 * @param[in] map cutil_Map to be worked with
 * @param[in] key key of value to be inserted
 * @param[in] val value to be inserted
 *
 * @return error code
 */
inline int
cutil_Map_set(cutil_Map *map, const void *key, const void *val)
{
    CUTIL_NULL_CHECKS_MAP(map);
    CUTIL_NULL_CHECK_VTABLE(map->vtable, set);
    CUTIL_RETURN_VAL_IF_NULL(map->vtable->set, CUTIL_STATUS_FAILURE);
    return map->vtable->set(map->data, key, val);
}

/**
 * Returns key type of `map`.
 *
 * @param[in] map cutil_Map object to get key type of
 *
 * @return key type of `map`, or NULL if not available
 */
inline const cutil_GenericType *
cutil_Map_get_key_type(const cutil_Map *map)
{
    CUTIL_NULL_CHECKS_MAP(map);
    CUTIL_NULL_CHECK_VTABLE(map->vtable, get_key_type);
    CUTIL_RETURN_NULL_IF_NULL(map->vtable->get_key_type);
    return map->vtable->get_key_type(map->data);
}

/**
 * Returns value type of `map`.
 *
 * @param[in] map cutil_Map object to get value type of
 *
 * @return value type of `map`, or NULL if not available
 */
inline const cutil_GenericType *
cutil_Map_get_val_type(const cutil_Map *map)
{
    CUTIL_NULL_CHECKS_MAP(map);
    CUTIL_NULL_CHECK_VTABLE(map->vtable, get_val_type);
    CUTIL_RETURN_NULL_IF_NULL(map->vtable->get_val_type);
    return map->vtable->get_val_type(map->data);
}

#undef CUTIL_NULL_CHECKS_MAP

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_GENERIC_MAP_H_INCLUDED */

/** cutil/generic/map.h
 *
 * Header for arbitrarily typed maps.
 *
 */

#ifndef CUTIL_GENERIC_MAP_H_INCLUDED
#define CUTIL_GENERIC_MAP_H_INCLUDED

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
 * Type (vtable) for cutil maps.
 */
typedef struct {
    const char *const name;
    void (*const free)(void *data);
    void (*const reset)(void *data);
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
    cutil_ConstIterator *(*const get_const_iterator)(const void *data);
    cutil_Iterator *(*const get_iterator)(void *data);
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
 * Frees contents of `map`.
 * 
 * @param[in] map cutil_Map object to be cleared
 */
inline void
cutil_Map_clear(cutil_Map *map)
{
    CUTIL_RETURN_IF_NULL(map);
    CUTIL_NULL_CHECK_VTABLE(map->vtable, reset);
    CUTIL_RETURN_IF_NULL(map->vtable->reset);
    map->vtable->free(map->data);
    map->data = NULL;
}

/**
 * Destructor for 'cutil_Map'.
 *
 * @param[in] map cutil_Map object to be destroyed
 */
inline void
cutil_Map_free(cutil_Map *map)
{
    cutil_Map_clear(map);
    free(map);
}

/**
 * Removes all entries from `map`.
 *
 * @param[in] map cutil_Map to reset
 */
inline void
cutil_Map_reset(cutil_Map *map)
{
    CUTIL_NULL_CHECKS_MAP(map);
    CUTIL_NULL_CHECK_VTABLE(map->vtable, reset);
    CUTIL_RETURN_IF_NULL(map->vtable->reset);
    map->vtable->reset(map->data);
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

/**
 * Returns a newly heap-allocated read-only iterator over the keys of `map`.
 * The returned iterator must be free'd.
 *
 * @param[in] map cutil_Map to iterate over
 *
 * @return newly malloc'd cutil_ConstIterator, or NULL on failure
 */
inline cutil_ConstIterator *
cutil_Map_get_const_iterator(const cutil_Map *map)
{
    CUTIL_RETURN_NULL_IF_NULL(map);
    CUTIL_NULL_CHECK(map->vtable);
    CUTIL_RETURN_NULL_IF_NULL(map->vtable->get_const_iterator);
    return map->vtable->get_const_iterator(map->data);
}

/**
 * Returns a newly heap-allocated read-write iterator over the keys of `map`.
 * The returned iterator must be free'd.
 *
 * @param[in] map cutil_Map to iterate over
 *
 * @return newly malloc'd cutil_Iterator, or NULL on failure
 */
inline cutil_Iterator *
cutil_Map_get_iterator(cutil_Map *map)
{
    CUTIL_RETURN_NULL_IF_NULL(map);
    CUTIL_NULL_CHECK(map->vtable);
    CUTIL_RETURN_NULL_IF_NULL(map->vtable->get_iterator);
    return map->vtable->get_iterator(map->data);
}

/**
 * Returns whether Maps `lhs` and `rhs` are deeply equal: same vtable, same
 * key and value types, same entry count, and every key in `lhs` is present in
 * `rhs` with a value that compares deeply equal via `val_type->deep_equals`.
 * Both NULL or the same pointer compare equal. NULL and non-NULL compare
 * unequal.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return Are `lhs` and `rhs` deeply equal?
 */
cutil_Bool
cutil_Map_deep_equals(const cutil_Map *lhs, const cutil_Map *rhs);

/**
 * Three-way comparison of Maps `lhs` and `rhs`. Ordered by: key type
 * (byte-wise), then value type (byte-wise), then count, then combined entry
 * hash as a tie-breaker. NULL sorts before non-NULL; identical pointers
 * compare equal.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return negative if lhs < rhs, 0 if equal, positive if lhs > rhs
 */
int
cutil_Map_compare(const cutil_Map *lhs, const cutil_Map *rhs);

/**
 * Computes a hash of `map` by XOR-folding `hash(key) ^ hash(value)` per
 * entry. Returns CUTIL_HASH_C(0) for NULL or an empty map.
 *
 * @param[in] map Map to hash
 *
 * @return hash value
 */
cutil_hash_t
cutil_Map_hash(const cutil_Map *map);

/**
 * Serializes `map` as "{key0:val0,key1:val1,...}" into `buf`. Follows the
 * snprintf convention: pass buf=NULL with buflen=0 to query the required
 * buffer size (return value excludes NUL). When buf is not NULL and buflen is
 * sufficient, writes at most buflen-1 chars + NUL and returns chars written
 * (excluding NUL). Returns 0 and logs a warning if buffer is too small. NULL
 * is serialized as "NULL".
 *
 * @param[in]  map    Map to serialize, or NULL
 * @param[out] buf    destination buffer, or NULL to query required size
 * @param[in]  buflen size of destination buffer in bytes, or 0 when querying
 *
 * @return number of characters written (excluding NUL), or required size when
 *         buf is NULL
 */
size_t
cutil_Map_to_string(const cutil_Map *map, char *buf, size_t buflen);

/**
 * Generic-type support functions
 */

/**
 * Generic (i.e, void-argument) version of 'cutil_Map_clear'.
 *
 * @param[in] obj cutil_Map to clear
 */
inline void
cutil_Map_clear_generic(void *obj)
{
    cutil_Map *const map = (cutil_Map *) obj;
    cutil_Map_clear(map);
}

/**
 * Generic (i.e, void-argument) version of 'cutil_Map_copy'.
 *
 * @param[out] dst cutil_Map to copy to
 * @param[in]  src cutil_Map to be copied
 */
inline void
cutil_Map_copy_generic(void *dst, const void *src)
{
    cutil_Map *const d = (cutil_Map *) dst;
    const cutil_Map *const s = (const cutil_Map *) src;
    cutil_Map_copy(d, s);
}

/**
 * Generic (i.e, void-argument) version of 'cutil_Map_deep_equals'.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return Are `lhs` and `rhs` deeply equal?
 */
cutil_Bool
cutil_Map_deep_equals_generic(const void *lhs, const void *rhs);

/**
 * Generic (i.e, void-argument) version of 'cutil_Map_compare'.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return negative if lhs < rhs, 0 if equal, positive if lhs > rhs
 */
int
cutil_Map_compare_generic(const void *lhs, const void *rhs);

/**
 * Generic (i.e, void-argument) version of 'cutil_Map_hash'.
 *
 * @param[in] map Map to hash
 *
 * @return hash value
 */
cutil_hash_t
cutil_Map_hash_generic(const void *map);

/**
 * Generic (i.e, void-argument) version of 'cutil_Map_to_string'.
 *
 * @param[in]  map    Map to serialize, or NULL
 * @param[out] buf    destination buffer, or NULL to query required size
 * @param[in]  buflen size of destination buffer in bytes, or 0 when querying
 *
 * @return number of characters written (excluding NUL), or required size when
 *         buf is NULL
 */
size_t
cutil_Map_to_string_generic(const void *map, char *buf, size_t buflen);

#undef CUTIL_NULL_CHECKS_MAP

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_GENERIC_MAP_H_INCLUDED */

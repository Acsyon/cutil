/** cutil/generic/map/hashmap.h
 *
 * Header for arbitrarily typed hash map.
 *
 */

#ifndef CUTIL_GENERIC_MAP_HASHMAP_H_INCLUDED
#define CUTIL_GENERIC_MAP_HASHMAP_H_INCLUDED

#include <cutil/data/generic/map.h>
#include <cutil/data/generic/type.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 'cutil_MapType' for a hash map.
 */
extern const cutil_MapType *const CUTIL_MAP_TYPE_HASHMAP;

/**
 * 'cutil_ConstIteratorType' for a hash map iterator (read-only).
 */
extern const cutil_ConstIteratorType *const CUTIL_CONST_ITERATOR_TYPE_HASHMAP;

/**
 * 'cutil_IteratorType' for a hash map iterator (read-write).
 */
extern const cutil_IteratorType *const CUTIL_ITERATOR_TYPE_HASHMAP;

/**
 * Constructor for 'cutil_Map' with key type and value type and a default
 * initial capacity.
 *
 * @param[in] key_type cutil_GenericType of keys
 * @param[in] val_type cutil_GenericType of vals
 *
 * @return newly malloc'd cutil_Map object
 */
cutil_Map *
cutil_HashMap_alloc(
  const cutil_GenericType *key_type, const cutil_GenericType *val_type
);

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_GENERIC_MAP_HASHMAP_H_INCLUDED */

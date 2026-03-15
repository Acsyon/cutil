/** cutil/data/generic/set/hashset.h
 *
 * Header for arbitrarily typed hash set.
 *
 */

#ifndef CUTIL_GENERIC_SET_HASHSET_H_INCLUDED
#define CUTIL_GENERIC_SET_HASHSET_H_INCLUDED

#include <cutil/data/generic/set.h>
#include <cutil/data/generic/type.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 'cutil_SetType' for a hash set.
 */
extern const cutil_SetType *const CUTIL_SET_TYPE_HASHSET;

/**
 * Constructor for 'cutil_Set' with element type and a default
 * initial capacity.
 *
 * @param[in] elem_type cutil_GenericType of elements
 *
 * @return newly malloc'd cutil_Set object
 */
cutil_Set *
cutil_HashSet_alloc(const cutil_GenericType *elem_type);

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_GENERIC_SET_HASHSET_H_INCLUDED */

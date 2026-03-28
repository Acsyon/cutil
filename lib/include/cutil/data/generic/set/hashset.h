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

/**
 * 'cutil_GenericType' instances for 'cutil_HashSet' with native element types.
 */

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_CHAR;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_SHORT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_INT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_LONG;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_LLONG;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_UCHAR;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_USHORT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_UINT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_ULONG;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_ULLONG;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_I8;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_I16;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_I32;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_I64;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_U8;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_U16;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_U32;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_U64;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_SIZET;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_HASHT;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_FLOAT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_DOUBLE;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_LDOUBLE;

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_GENERIC_SET_HASHSET_H_INCLUDED */

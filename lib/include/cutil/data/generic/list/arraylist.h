/** cutil/data/generic/list/arraylist.h
 *
 * Header for dynamically-allocated typed list backed by cutil_Array.
 *
 */

#ifndef CUTIL_GENERIC_LIST_ARRAYLIST_H_INCLUDED
#define CUTIL_GENERIC_LIST_ARRAYLIST_H_INCLUDED

#include <cutil/data/generic/list.h>
#include <cutil/data/generic/type.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 'cutil_ListType' for an array list.
 */
extern const cutil_ListType *const CUTIL_LIST_TYPE_ARRAYLIST;

/**
 * Constructor for 'cutil_List' backed by a dynamic array with the given
 * element type.
 *
 * @param[in] type cutil_GenericType of elements
 *
 * @return newly malloc'd cutil_List object, or NULL on invalid type
 */
cutil_List *
cutil_ArrayList_alloc(const cutil_GenericType *type);

/**
 * Returns allocated capacity (number of elements) of the backing array.
 *
 * @param[in] list cutil_List backed by an ArrayList
 *
 * @return allocated capacity in elements, or 0 if list is NULL
 */
size_t
cutil_ArrayList_get_capacity(const cutil_List *list);

/**
 * Resizes the backing array to exactly 'capacity' elements.
 * Elements within [0, min(capacity, length)) are preserved. If capacity <
 * length, excess elements are cleared and length is truncated.
 *
 * @param[in] list     cutil_List backed by an ArrayList
 * @param[in] capacity new capacity in elements
 *
 * @return error code
 */
int
cutil_ArrayList_resize(cutil_List *list, size_t capacity);

/**
 * Resizes the backing array to exactly the current logical length, releasing
 * any unused capacity.
 *
 * @param[in] list cutil_List backed by an ArrayList
 *
 * @return error code
 */
int
cutil_ArrayList_shrink_to_fit(cutil_List *list);

/**
 * 'cutil_GenericType' instances for 'cutil_ArrayList' with native element
 * types.
 */

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_CHAR;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_SHORT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_INT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_LONG;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_LLONG;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_UCHAR;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_USHORT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_UINT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_ULONG;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_ULLONG;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_I8;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_I16;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_I32;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_I64;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_U8;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_U16;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_U32;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_U64;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_SIZET;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_HASHT;

extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_FLOAT;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_DOUBLE;
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_LDOUBLE;

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_GENERIC_LIST_ARRAYLIST_H_INCLUDED */

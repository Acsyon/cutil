/** cutil/data/generic/string/type.h
 *
 * Typed string wrappers: cutil_String (owning) and cutil_StringView
 * (non-owning).
 */

#ifndef CUTIL_DATA_GENERIC_STRING_TYPE_H_INCLUDED
#define CUTIL_DATA_GENERIC_STRING_TYPE_H_INCLUDED

#include <cutil/data/generic/type.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Generic type descriptor for cutil_String.
 */
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_STRING;

/**
 * Generic type descriptor for cutil_StringView.
 */
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_STRING_VIEW;

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_DATA_GENERIC_STRING_TYPE_H_INCLUDED */

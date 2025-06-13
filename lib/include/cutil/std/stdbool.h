/* cutil/std/stdbool.h
 *
 * Wrapper around standard "stdbool.h" header
 *
 */

#ifndef CUTIL_STD_STDBOOL_H_INCLUDED
#define CUTIL_STD_STDBOOL_H_INCLUDED

#include <stdbool.h>

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Use unsigned 32-bit integers for 'bool' to be as platform- and
 * compiler-independent as possible.
 */
typedef uint32_t cutil_Bool;

/**
 * MACRO for converting `b` to boolean
 *
 * @param[in] b expression to be converted to bool
 *
 * @return boolification of `b`
 */
#define CUTIL_BOOLIFY(b) ((cutil_Bool) !!(b))

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_STD_STDBOOL_H_INCLUDED */

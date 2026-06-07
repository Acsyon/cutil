/** cutil/core/std/stdbool.h
 *
 * Wrapper around standard "stdbool.h" header.
 */

#ifndef CUTIL_CORE_STD_STDBOOL_H_INCLUDED
#define CUTIL_CORE_STD_STDBOOL_H_INCLUDED

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

/**
 * Completely unnecessary but pedantic MACRO for 'true'
 */
#define CUTIL_TRUE CUTIL_BOOLIFY(true)

/**
 * Completely unnecessary but pedantic MACRO for 'false'
 */
#define CUTIL_FALSE CUTIL_BOOLIFY(false)

/**
 * Converts a string to a bool value. Returns CUTIL_TRUE if `str` is "true"
 * (case-insensitive), CUTIL_FALSE otherwise.
 *
 * @param[in] str string to be converted to bool
 *
 * @return CUTIL_TRUE if `str` is "true" (case-insensitive), CUTIL_FALSE
 * otherwise
 */
cutil_Bool
cutil_atobool(const char *str);

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_CORE_STD_STDBOOL_H_INCLUDED */

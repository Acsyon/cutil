/* cutil/util/macro.h
 *
 * Header for miscellaneous utility macros.
 *
 */

#ifndef CUTIL_UTIL_MACRO_H_INCLUDED
#define CUTIL_UTIL_MACRO_H_INCLUDED

#include <cutil/std/stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * MACRO for calculating maximum of two values.
 *
 * @param[in] X value on left-hand side
 * @param[in] Y value on right-hand side
 *
 * @return maximum of X and Y
 */
#define CUTIL_MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

/**
 * MACRO for calculating minimum of two values.
 *
 * @param[in] X value on left-hand side
 * @param[in] Y value on right-hand side
 *
 * @return minimum of X and Y
 */
#define CUTIL_MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

/**
 * MACRO for clamp
 *
 * @param[in] VAL value to be clamped
 * @param[in] LO lower bound
 * @param[in] HI upper bound
 *
 * @return clamp of VAL, LO and HI
 */
#define CUTIL_CLAMP(VAL, LO, HI) CUTIL_MIN(CUTIL_MAX((VAL), (LO)), (HI))

/**
 * MACRO for calculating the sign of a number.
 *
 * @param[in] X value whose sign should be returned
 *
 * @return sign of X
 */
#define CUTIL_SGN(X) (((X) < 0) ? -1 : 1)

/**
 * MACRO for declaring unused variable in functions.
 *
 * @param[in] VAR unused variable
 */
#define CUTIL_UNUSED(VAR) ((void) (VAR))

/**
 * MACRO for returning RET from function if LVAL is RVAL.
 *
 * @param[in] LVAL value on left-hand side of comparison
 * @param[in] RVAL value on right-hand side of comparison
 * @param[in] RET returned value
 */
#define CUTIL_RETURN_VAL_IF_VAL(LVAL, RVAL, RET)                               \
    do {                                                                       \
        if ((LVAL) == (RVAL)) {                                                \
            return (RET);                                                      \
        }                                                                      \
    } while (0)

/**
 * MACRO for returning from function if LVAL is RVAL.
 *
 * @param[in] LVAL value on left-hand side of comparison
 * @param[in] RVAL value on right-hand side of comparison
 */
#define CUTIL_RETURN_IF_VAL(LVAL, RVAL)                                        \
    do {                                                                       \
        if ((LVAL) == (RVAL)) {                                                \
            return;                                                            \
        }                                                                      \
    } while (0)

/**
 * MACRO for returning NULL from function if PTR is NULL.
 *
 * @param[in] PTR pointer to be checked
 */
#define CUTIL_RETURN_NULL_IF_NULL(PTR)                                         \
    CUTIL_RETURN_VAL_IF_VAL((PTR), NULL, NULL)

/**
 * MACRO for returning from function if PTR is NULL.
 *
 * @param[in] PTR pointer to be checked
 */
#define CUTIL_RETURN_IF_NULL(PTR) CUTIL_RETURN_IF_VAL((PTR), NULL)

/**
 * MACRO for converting value of macro MCR to string literal
 *
 * @param[in] MCR macro whose value should be converted to string literal
 *
 * @return stringified value of MCR
 */
#define CUTIL_STRINGIFY_VAL(MCR) CUTIL_STRINGIFY_VAL_AUX(MCR)
#define CUTIL_STRINGIFY_VAL_AUX(MCR) #MCR

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_UTIL_MACRO_H_INCLUDED */

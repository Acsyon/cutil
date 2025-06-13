/* cutil/std/math.h
 *
 * Wrapper around standard "math.h" header
 *
 */

#ifndef CUTIL_STD_MATH_H_INCLUDED
#define CUTIL_STD_MATH_H_INCLUDED

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * GNU math.h constants
 */

/* e */
#ifndef M_E
    #define M_E 2.7182818284590452354
#endif

/* log_2 e */
#ifndef M_LOG2E
    #define M_LOG2E 1.4426950408889634074
#endif

/* log_10 e */
#ifndef M_LOG10E
    #define M_LOG10E 0.43429448190325182765
#endif

/* log_e 2 */
#ifndef M_LN2
    #define M_LN2 0.69314718055994530942
#endif

/* log_e 10 */
#ifndef M_LN10
    #define M_LN10 2.30258509299404568402
#endif

/* pi */
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

/* pi/2 */
#ifndef M_PI_2
    #define M_PI_2 1.57079632679489661923
#endif

/* pi/4 */
#ifndef M_PI_4
    #define M_PI_4 0.78539816339744830962
#endif

/* 1/pi */
#ifndef M_1_PI
    #define M_1_PI 0.31830988618379067154
#endif

/* 2/pi */
#ifndef M_2_PI
    #define M_2_PI 0.63661977236758134308
#endif

/* 2/sqrt(pi) */
#ifndef M_2_SQRTPI
    #define M_2_SQRTPI 1.12837916709551257390
#endif

/* sqrt(2) */
#ifndef M_SQRT2
    #define M_SQRT2 1.41421356237309504880
#endif

/* 1/sqrt(2) */
#ifndef M_SQRT1_2
    #define M_SQRT1_2 0.70710678118654752440
#endif

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_STD_MATH_H_INCLUDED */

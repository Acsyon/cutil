/** cutil/util/compare.h
 *
 * Header for libc-compatible comparison functions.
 */

#ifndef CUTIL_UTIL_COMPARE_H_INCLUDED
#define CUTIL_UTIL_COMPARE_H_INCLUDED

#include <cutil/std/inttypes.h>
#include <cutil/std/stddef.h>
#include <cutil/util/hash.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Typedef for a libc-compatible comparison function. Returns < 0 if lhs < rhs,
 * = 0 if lhs == rhs, > 0 if lhs > rhs.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @returns < 0 if lhs < rhs, = 0 if lhs == rhs, > 0 if lhs > rhs
 */
typedef int
cutil_CompFunc(const void *lhs, const void *rhs);

/**
 * MACRO FOR a for a libc-compatible comparison operation. Returns < 0 if lhs <
 * rhs, = 0 if lhs == rhs, > 0 if lhs > rhs.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @returns < 0 if lhs < rhs, = 0 if lhs == rhs, > 0 if lhs > rhs
 */
#define CUTIL_DEFAULT_COMPARISON_KERNEL(LHS, RHS)                              \
    (((LHS) > (RHS)) - ((LHS) < (RHS)))

/**
 * Compares (first) `num` bytes of memory pointed to by `lhs` and `rhs`
 * byte-wise.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 * @param[in] num number of bytes to compare
 *
 * @returns comparison of first differing byte according to 'cutil_CompFunc'
 */
inline int
cutil_compare_bytes(const void *lhs, const void *rhs, size_t num)
{
    if (lhs == rhs) {
        return 0;
    }
    if (lhs == NULL || rhs == NULL) {
        return CUTIL_DEFAULT_COMPARISON_KERNEL(lhs, rhs);
    }
    const unsigned char *const a = lhs;
    const unsigned char *const b = rhs;
    for (size_t i = 0; i < num; ++i) {
        const unsigned char ai = a[i];
        const unsigned char bi = b[i];
        if (ai != bi) {
            return CUTIL_DEFAULT_COMPARISON_KERNEL(ai, bi);
        }
    }
    return 0;
}

/**
 * Compares `size_lhs` and `size_rhs` and then bytes of memory pointed to by
 * `lhs` and `rhs` byte-wise.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 * @param[in] size_lhs size of memory pointed to by `lhs` in bytes
 * @param[in] size_rhs size of memory pointed to by `rhs` in bytes
 *
 * @returns comparison of differing sizes or byte according to 'cutil_CompFunc'
 */
inline int
cutil_compare_sizes(
  const void *lhs, const void *rhs, size_t size_lhs, size_t size_rhs
)
{
    if (size_lhs != size_rhs) {
        return CUTIL_DEFAULT_COMPARISON_KERNEL(size_lhs, size_rhs);
    }
    return cutil_compare_bytes(lhs, rhs, size_lhs);
}

/**
 * MACRO to compare arbitrary variables `LHS` and `RHS` by calling
 * 'cutil_compare_sizes' with the appropriate sizes.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @returns result of 'cutil_compare_sizes'
 */
#define CUTIL_COMPARE(LHS, RHS)                                                \
    cutil_compare_sizes(&(LHS), &(RHS), sizeof(LHS), sizeof(RHS))

/**
 * MACRO for the definition of libc-compatible comparison functions.
 *
 * @param[in] TYPE type of the comparison function
 * @param[in] ID identification string to discriminate functions
 */
#define CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(TYPE, ID)                    \
                                                                               \
    inline int cutil_compare_##ID(const void *lhs, const void *rhs)            \
    {                                                                          \
        const TYPE *const a = lhs;                                             \
        const TYPE *const b = rhs;                                             \
        return CUTIL_DEFAULT_COMPARISON_KERNEL(*a, *b);                        \
    }

CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(char, char)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(short int, short)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(int, int)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(long int, long)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(long long int, llong)

CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(unsigned char, uchar)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(unsigned short int, ushort)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(unsigned int, uint)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(unsigned long int, ulong)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(unsigned long long int, ullong)

CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(int8_t, i8)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(int16_t, i16)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(int32_t, i32)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(int64_t, i64)

CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(uint8_t, u8)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(uint16_t, u16)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(uint32_t, u32)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(uint64_t, u64)

CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(size_t, size_t)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(cutil_hash_t, hash_t)

CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(float, float)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(double, double)
CUTIL_TEMPLATE_DEFINE_COMPARISON_FUNCTION(long double, ldouble)

#undef CUTIL_DEFINE_COMPARISON_FUNCTION_TEMPLATE
#undef CUTIL_DEFAULT_COMPARISON_KERNEL

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_UTIL_COMPARE_H_INCLUDED */

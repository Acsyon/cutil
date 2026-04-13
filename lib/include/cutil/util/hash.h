/** cutil/util/hash.h
 *
 * Header for hashing stuff.
 */

#ifndef CUTIL_UTIL_HASH_H_INCLUDED
#define CUTIL_UTIL_HASH_H_INCLUDED

#include <cutil/std/inttypes.h>
#include <cutil/std/stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Typedefs for hash values
 */
typedef uint64_t cutil_hash_t;

/**
 * MACRO that expands to a cutil_hash_t having the value `VAL`
 *
 * @param[in] VAL value to expand to a cutil_hash_t
 *
 * @return cutil_hash_t of `VAL`
 */
#define CUTIL_HASH_C(VAL) UINT64_C(VAL)

/**
 * Typedef for hash functions
 */
typedef cutil_hash_t
cutil_HashFunc(const void *ptr);

/**
 * Returns hash value for byte pattern of length `len` pointed to by `ptr` with
 * seed `seed`.
 *
 * @param[in] ptr pointer to bit pattern
 * @param[in] len length of bit pattern
 * @param[in] seed seed of hash
 *
 * @return hash of bit pattern
 *
 * @note Stolen from libstdc++
 */
cutil_hash_t
cutil_hash_bytes(const void *ptr, size_t len, cutil_hash_t seed);

/**
 * Combines `seed` in-place with `hash`.
 *
 * @param[in, out] seed to be combined with hash
 * @param[in] hash to be combined with seed
 *
 * @note Stolen from GLM
 */
inline void
cutil_hash_combine_inplace(cutil_hash_t *seed, cutil_hash_t hash)
{
    hash += CUTIL_HASH_C(0x9e3779b9) + (*seed << 6) + (*seed >> 2);
    *seed ^= hash;
}

/**
 * Combine `seed` with `hash`.
 *
 * @param[in] seed to be combined with hash
 * @param[in] hash to be combined with seed
 *
 * @return combined hash
 *
 * @note Stolen from GLM
 */
inline cutil_hash_t
cutil_hash_combine(cutil_hash_t seed, cutil_hash_t hash)
{
    cutil_hash_t res = seed;
    cutil_hash_combine_inplace(&res, hash);
    return res;
}

/**
 * Trivial hash function definitions for INTEGER types TYPE. Each function has
 * the suffix ID.
 */
#define CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(TYPE, ID)                   \
    /**                                                                        \
     * Return hash value for INTEGER pointed to by `ptr`.                      \
     *                                                                         \
     * @param[in] ptr pointer to INTEGER to be hashed                          \
     *                                                                         \
     * @return hash of value pointed to by `ptr`                               \
     *                                                                         \
     * @note Stolen from libstdc++                                             \
     */                                                                        \
    inline cutil_hash_t cutil_hashfunc_##ID(const void *ptr)                   \
    {                                                                          \
        const TYPE val = *(const TYPE *) ptr;                                  \
        return (cutil_hash_t) val;                                             \
    }                                                                          \
                                                                               \
    /**                                                                        \
     * Return hash value for INTEGER `val`.                                    \
     *                                                                         \
     * @param[in] val INTEGER to be hashed                                     \
     *                                                                         \
     * @return hash of value `val`                                             \
     *                                                                         \
     * @note Stolen from libstdc++                                             \
     */                                                                        \
    inline cutil_hash_t cutil_hash_##ID(TYPE val)                              \
    {                                                                          \
        return cutil_hashfunc_##ID(&val);                                      \
    }

CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(char, char)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(short int, short)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(int, int)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(long int, long)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(long long int, llong)

CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(unsigned char, uchar)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(unsigned short int, ushort)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(unsigned int, uint)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(unsigned long int, ulong)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(unsigned long long int, ullong)

CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(int8_t, i8)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(int16_t, i16)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(int32_t, i32)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(int64_t, i64)

CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(uint8_t, u8)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(uint16_t, u16)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(uint32_t, u32)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(uint64_t, u64)

CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(size_t, size_t)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER(cutil_hash_t, hash_t)

#undef CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_INTEGER

/**
 * Trivial hash function definitions for FLOATING types TYPE. Each function has
 * the suffix ID.
 */
#define CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_FLOATING(TYPE, ID)                  \
    /**                                                                        \
     * Return hash value for FLOATING pointed to by `ptr`.                     \
     *                                                                         \
     * @param[in] ptr pointer to FLOATING to be hashed                         \
     *                                                                         \
     * @return hash of value pointed to by `ptr`                               \
     *                                                                         \
     * @note Stolen from libstdc++                                             \
     */                                                                        \
    inline cutil_hash_t cutil_hashfunc_##ID(const void *ptr)                   \
    {                                                                          \
        const TYPE val = *(const TYPE *) ptr;                                  \
        if (!val) {                                                            \
            return (cutil_hash_t) 0;                                           \
        }                                                                      \
        return cutil_hash_bytes(ptr, sizeof(TYPE), CUTIL_HASH_C(0xc70f6907));  \
    }                                                                          \
                                                                               \
    /**                                                                        \
     * Return hash value for FLOATING `val`.                                   \
     *                                                                         \
     * @param[in] val FLOATING to be hashed                                    \
     *                                                                         \
     * @return hash of value `val`                                             \
     *                                                                         \
     * @note Stolen from libstdc++                                             \
     */                                                                        \
    inline cutil_hash_t cutil_hash_##ID(TYPE val)                              \
    {                                                                          \
        return cutil_hashfunc_##ID(&val);                                      \
    }

CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_FLOATING(float, float)
CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_FLOATING(double, double)

/**
 * Return hash value for long double pointed to by `ptr`.
 *
 * @param[in] ptr pointer to long double to be hashed
 *
 * @return hash of value pointed to by `ptr`
 */
inline cutil_hash_t
cutil_hashfunc_ldouble(const void *ptr)
{
    const long double val = *(const long double *) ptr;
    if (!val) {
        return (cutil_hash_t) 0;
    }

    /*
     * Some ABIs store long double in a padded object representation.
     * Hashing the raw bytes can consume indeterminate padding and trigger
     * Memcheck false positives/taint. Canonicalize through double first.
     */
    const double dbl = (double) val;
    return cutil_hash_bytes(&dbl, sizeof dbl, CUTIL_HASH_C(0xc70f6907));
}

/**
 * Return hash value for long double `val`.
 *
 * @param[in] val long double to be hashed
 *
 * @return hash of value `val`
 */
inline cutil_hash_t
cutil_hash_ldouble(long double val)
{
    return cutil_hashfunc_ldouble(&val);
}

#undef CUTIL_TEMPLATE_DEFINE_TRIVIAL_HASH_FLOATING

/**
 * Return hash value for string pointed to by `ptr`.
 *
 * @param[in] ptr pointer to string to be hashed
 *
 * @return hash of value pointed to by `ptr`
 */
cutil_hash_t
cutil_hashfunc_str(const void *ptr);

/**
 * Return hash value for string pointed to by `ptr`.
 *
 * @param[in] ptr pointer to string to be hashed
 *
 * @return hash of value `str`
 */
inline cutil_hash_t
cutil_hash_str(const char *str)
{
    return cutil_hashfunc_str(str);
}

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_UTIL_HASH_H_INCLUDED */

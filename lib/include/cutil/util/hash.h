/** cutil/util/hash.h
 *
 * Header for hashing stuff.
 *
 */

#ifndef CUTIL_UTIL_HASH_H_INCLUDED
#define CUTIL_UTIL_HASH_H_INCLUDED

#include <cutil/std/inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Typedef for hash functions
 */
typedef uint64_t
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
uint64_t
cutil_hash_bytes(const void *ptr, uint64_t len, uint64_t seed);

/**
 * Combine `seed` with hash`.
 *
 * @param[in] seed to be combined with hash
 * @param[in] hash to be combined with seed
 *
 * @return combined hash
 *
 * @note Stolen from GLM
 */
inline uint64_t
cutil_hash_combine(uint64_t seed, uint64_t hash)
{
    hash += UINT64_C(0x9e3779b9) + (seed << 6) + (seed >> 2);
    return (seed ^ hash);
}

/**
 * Trivial hash function definitions for INTEGER types TYPE. Each function has
 * the suffix ID.
 */
#define DEFINE_TRIVIAL_HASH_INTEGER(TYPE, ID)                                  \
    /**                                                                        \
     * Return hash value for INTEGER pointed to by `ptr`.                      \
     *                                                                         \
     * @param[in] ptr pointer to INTEGER to be hashed                          \
     *                                                                         \
     * @return hash of value pointed to by `ptr`                               \
     *                                                                         \
     * @note Stolen from libstdc++                                             \
     */                                                                        \
    inline uint64_t cutil_hashfunc_##ID(const void *ptr)                       \
    {                                                                          \
        const TYPE val = *(const TYPE *) ptr;                                  \
        return (uint64_t) val;                                                 \
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
    inline uint64_t cutil_hash_##ID(TYPE val)                                  \
    {                                                                          \
        return cutil_hashfunc_##ID(&val);                                      \
    }

DEFINE_TRIVIAL_HASH_INTEGER(char, char)
DEFINE_TRIVIAL_HASH_INTEGER(short, short)
DEFINE_TRIVIAL_HASH_INTEGER(int, int)
DEFINE_TRIVIAL_HASH_INTEGER(long, long)
DEFINE_TRIVIAL_HASH_INTEGER(long long, llong)

DEFINE_TRIVIAL_HASH_INTEGER(unsigned char, uchar)
DEFINE_TRIVIAL_HASH_INTEGER(unsigned short, ushort)
DEFINE_TRIVIAL_HASH_INTEGER(unsigned int, uint)
DEFINE_TRIVIAL_HASH_INTEGER(unsigned long, ulong)
DEFINE_TRIVIAL_HASH_INTEGER(unsigned long long, ullong)

DEFINE_TRIVIAL_HASH_INTEGER(int8_t, i8)
DEFINE_TRIVIAL_HASH_INTEGER(int16_t, i16)
DEFINE_TRIVIAL_HASH_INTEGER(int32_t, i32)
DEFINE_TRIVIAL_HASH_INTEGER(int64_t, i64)

DEFINE_TRIVIAL_HASH_INTEGER(uint8_t, u8)
DEFINE_TRIVIAL_HASH_INTEGER(uint16_t, u16)
DEFINE_TRIVIAL_HASH_INTEGER(uint32_t, u32)
DEFINE_TRIVIAL_HASH_INTEGER(uint64_t, u64)

#undef DEFINE_TRIVIAL_HASH_INTEGER

/**
 * Trivial hash function definitions for FLOATING types TYPE. Each function has
 * the suffix ID.
 */
#define DEFINE_TRIVIAL_HASH_FLOATING(TYPE, ID)                                 \
    /**                                                                        \
     * Return hash value for FLOATING pointed to by `ptr`.                     \
     *                                                                         \
     * @param[in] ptr pointer to FLOATING to be hashed                         \
     *                                                                         \
     * @return hash of value pointed to by `ptr`                               \
     *                                                                         \
     * @note Stolen from libstdc++                                             \
     */                                                                        \
    inline uint64_t cutil_hashfunc_##ID(const void *ptr)                       \
    {                                                                          \
        const TYPE val = *(const TYPE *) ptr;                                  \
        if (!val) {                                                            \
            return (uint64_t) 0;                                               \
        }                                                                      \
        return cutil_hash_bytes(ptr, sizeof(TYPE), UINT64_C(0xc70f6907));      \
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
    inline uint64_t cutil_hash_##ID(TYPE val)                                  \
    {                                                                          \
        return cutil_hashfunc_##ID(&val);                                      \
    }

DEFINE_TRIVIAL_HASH_FLOATING(float, float)
DEFINE_TRIVIAL_HASH_FLOATING(double, double)
DEFINE_TRIVIAL_HASH_FLOATING(long double, ldouble)

#undef DEFINE_TRIVIAL_HASH_FLOATING

/**
 * Return hash value for string pointed to by `ptr`.
 *
 * @param[in] ptr pointer to string to be hashed
 *
 * @return hash of value pointed to by `ptr`
 */
uint64_t
cutil_hashfunc_str(const void *ptr);

/**
 * Return hash value for string pointed to by `ptr`.
 *
 * @param[in] ptr pointer to string to be hashed
 *
 * @return hash of value `str`
 */
inline uint64_t
cutil_hash_str(const char *str)
{
    return cutil_hashfunc_str(str);
}

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_UTIL_HASH_H_INCLUDED */

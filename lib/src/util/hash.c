#include <cutil/util/hash.h>

#include <cutil/std/string.h>

/**
 * Stolen from libstdc++
 */
static inline cutil_hash_t
_shift_mix(cutil_hash_t v)
{
    return v ^ (v >> 47);
}

static inline cutil_hash_t
_unaligned_load(const char *p)
{
    cutil_hash_t result;
    memcpy(&result, p, sizeof result);
    return result;
}

static inline cutil_hash_t
_load_bytes(const char *p, int n)
{
    cutil_hash_t result = 0;
    --n;
    do {
        result = (result << 8) + (unsigned char) p[n];
    } while (--n >= 0);
    return result;
}

cutil_hash_t
cutil_hash_bytes(const void *ptr, size_t len, cutil_hash_t seed)
{
    static const cutil_hash_t mul
      = (((cutil_hash_t) 0xc6a4a793UL) << 32UL) + (cutil_hash_t) 0x5bd1e995UL;
    const char *const buf = (const char *) ptr;

    const cutil_hash_t len_aligned = len & ~(cutil_hash_t) 0x7;
    const char *const end = buf + len_aligned;
    cutil_hash_t hash = seed ^ (len * mul);
    for (const char *p = buf; p != end; p += 8) {
        const cutil_hash_t data = _shift_mix(_unaligned_load(p) * mul) * mul;
        hash ^= data;
        hash *= mul;
    }
    if ((len & 0x7) != 0) {
        const cutil_hash_t data = _load_bytes(end, len & 0x7);
        hash ^= data;
        hash *= mul;
    }
    hash = _shift_mix(hash) * mul;
    hash = _shift_mix(hash);
    return hash;
}

extern inline void
cutil_hash_combine_inplace(cutil_hash_t *seed, cutil_hash_t hash);

extern inline cutil_hash_t
cutil_hash_combine(cutil_hash_t seed, cutil_hash_t hash);

/**
 * Trivial hash function declarations
 */
#define CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(TYPE, ID)                          \
    extern inline cutil_hash_t cutil_hash_##ID(TYPE val);                      \
    extern inline cutil_hash_t cutil_hashfunc_##ID(const void *ptr);

CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(char, char)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(short, short)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(int, int)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(long, long)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(long long, llong)

CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(unsigned char, uchar)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(unsigned short, ushort)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(unsigned int, uint)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(unsigned long, ulong)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(unsigned long long, ullong)

CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(int8_t, i8)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(int16_t, i16)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(int32_t, i32)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(int64_t, i64)

CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(uint8_t, u8)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(uint16_t, u16)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(uint32_t, u32)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(uint64_t, u64)

CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(size_t, size_t)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(cutil_hash_t, hash_t)

CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(float, float)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(double, double)
CUTIL_TEMPLATE_DECLARE_TRIVIAL_HASH(long double, ldouble)

/**
 * DJB33 hash algorithm
 */
cutil_hash_t
cutil_hashfunc_str(const void *ptr)
{
    const char *str = ptr;
    uint32_t hash = 5381;
    char c;
    while ((c = *str++)) {
        hash += (hash << 5);
        hash ^= c;
    }
    return hash;
}

extern inline cutil_hash_t
cutil_hash_str(const char *str);

#include <cutil/util/hash.h>

#include <cutil/std/string.h>

/**
 * Stolen from libstdc++
 */
static inline uint64_t
_shift_mix(uint64_t v)
{
    return v ^ (v >> 47);
}

static inline uint64_t
_unaligned_load(const char *p)
{
    uint64_t result;
    memcpy(&result, p, sizeof result);
    return result;
}

static inline uint64_t
_load_bytes(const char *p, int n)
{
    uint64_t result = 0;
    --n;
    do {
        result = (result << 8) + (unsigned char) p[n];
    } while (--n >= 0);
    return result;
}

uint64_t
cutil_hash_bytes(const void *ptr, uint64_t len, uint64_t seed)
{
    static const uint64_t mul
      = (((uint64_t) 0xc6a4a793UL) << 32UL) + (uint64_t) 0x5bd1e995UL;
    const char *const buf = (const char *) ptr;

    const uint64_t len_aligned = len & ~(uint64_t) 0x7;
    const char *const end = buf + len_aligned;
    uint64_t hash = seed ^ (len * mul);
    for (const char *p = buf; p != end; p += 8) {
        const uint64_t data = _shift_mix(_unaligned_load(p) * mul) * mul;
        hash ^= data;
        hash *= mul;
    }
    if ((len & 0x7) != 0) {
        const uint64_t data = _load_bytes(end, len & 0x7);
        hash ^= data;
        hash *= mul;
    }
    hash = _shift_mix(hash) * mul;
    hash = _shift_mix(hash);
    return hash;
}

extern inline uint64_t
cutil_hash_combine(uint64_t seed, uint64_t hash);

/**
 * Trivial hash function declarations
 */
#define DECLARE_TRIVIAL_HASH(TYPE, ID)                                         \
    extern inline uint64_t cutil_hash_##ID(TYPE val);                          \
    extern inline uint64_t cutil_hashfunc_##ID(const void *ptr);

DECLARE_TRIVIAL_HASH(char, char)
DECLARE_TRIVIAL_HASH(short, short)
DECLARE_TRIVIAL_HASH(int, int)
DECLARE_TRIVIAL_HASH(long, long)
DECLARE_TRIVIAL_HASH(long long, llong)

DECLARE_TRIVIAL_HASH(unsigned char, uchar)
DECLARE_TRIVIAL_HASH(unsigned short, ushort)
DECLARE_TRIVIAL_HASH(unsigned int, uint)
DECLARE_TRIVIAL_HASH(unsigned long, ulong)
DECLARE_TRIVIAL_HASH(unsigned long long, ullong)

DECLARE_TRIVIAL_HASH(int8_t, i8)
DECLARE_TRIVIAL_HASH(int16_t, i16)
DECLARE_TRIVIAL_HASH(int32_t, i32)
DECLARE_TRIVIAL_HASH(int64_t, i64)

DECLARE_TRIVIAL_HASH(uint8_t, u8)
DECLARE_TRIVIAL_HASH(uint16_t, u16)
DECLARE_TRIVIAL_HASH(uint32_t, u32)
DECLARE_TRIVIAL_HASH(uint64_t, u64)

DECLARE_TRIVIAL_HASH(float, float)
DECLARE_TRIVIAL_HASH(double, double)
DECLARE_TRIVIAL_HASH(long double, ldouble)

/**
 * DJB33 hash algorithm
 */
uint64_t
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

extern inline uint64_t
cutil_hash_str(const char *str);

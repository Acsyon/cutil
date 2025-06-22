#include "unity.h"

#include <cutil/util/hash.h>

#include <cutil/std/stdlib.h>

/**
 * Helper macros for testing hash functions
 */
#define TEST_HASH_EQUAL(TYPE, ID, VALUE, EXPECTED)                             \
    do {                                                                       \
        /* Arrange */                                                          \
        const TYPE val = VALUE;                                                \
        /* Act */                                                              \
        const uint64_t hash1 = cutil_hash_##ID(val);                           \
        const uint64_t hash2 = cutil_hashfunc_##ID(&val);                      \
        /* Assert */                                                           \
        TEST_ASSERT_EQUAL_UINT64(EXPECTED, hash1);                             \
        TEST_ASSERT_EQUAL_UINT64(EXPECTED, hash2);                             \
    } while (0)

#define TEST_HASH_NOT_EQUAL(TYPE, ID, VALUE, EXPECTED)                         \
    do {                                                                       \
        /* Arrange */                                                          \
        const TYPE val = VALUE;                                                \
        /* Act */                                                              \
        const uint64_t hash1 = cutil_hash_##ID(val);                           \
        const uint64_t hash2 = cutil_hashfunc_##ID(&val);                      \
        /* Assert */                                                           \
        TEST_ASSERT_NOT_EQUAL_UINT64(EXPECTED, hash1);                         \
        TEST_ASSERT_NOT_EQUAL_UINT64(EXPECTED, hash2);                         \
    } while (0)

static void
_should_returnValue_when_hashIntegers(void)
{
    TEST_HASH_EQUAL(char, char, 'A', 65);
    TEST_HASH_EQUAL(short, short, 12345, 12345);
    TEST_HASH_EQUAL(int, int, -42, (uint64_t) -42);
    TEST_HASH_EQUAL(long, long, 987654321L, 987654321L);
    TEST_HASH_EQUAL(long long, llong, 123456789012345LL, 123456789012345LL);

    TEST_HASH_EQUAL(unsigned char, uchar, 'B', 66);
    TEST_HASH_EQUAL(unsigned short, ushort, 54321, 54321);
    TEST_HASH_EQUAL(unsigned int, uint, 42, 42);
    TEST_HASH_EQUAL(unsigned long, ulong, 9876543210UL, 9876543210UL);
    TEST_HASH_EQUAL(
      unsigned long long, ullong, 123456789012345ULL, 123456789012345ULL
    );

    TEST_HASH_EQUAL(int8_t, i8, -128, (uint64_t) -128);
    TEST_HASH_EQUAL(int16_t, i16, -32768, (uint64_t) -32768);
    TEST_HASH_EQUAL(int32_t, i32, -2147483648, (uint64_t) -2147483648);
    TEST_HASH_EQUAL(
      int64_t, i64, -9223372036854775807LL, (uint64_t) -9223372036854775807LL
    );

    TEST_HASH_EQUAL(uint8_t, u8, 255, 255);
    TEST_HASH_EQUAL(uint16_t, u16, 65535, 65535);
    TEST_HASH_EQUAL(uint32_t, u32, 4294967295, 4294967295);
    TEST_HASH_EQUAL(
      uint64_t, u64, 18446744073709551615ULL, 18446744073709551615ULL
    );
}

static void
_should_returnZeroOrSomething_when_hashFloats(void)
{
    TEST_HASH_EQUAL(float, float, 0.0F, 0);
    TEST_HASH_NOT_EQUAL(float, float, 3.14159265f, 0);

    TEST_HASH_EQUAL(double, double, 0.0, 0);
    TEST_HASH_NOT_EQUAL(double, double, 2.718281828459045, 0);

    TEST_HASH_EQUAL(long double, ldouble, 0.0L, 0);
    TEST_HASH_NOT_EQUAL(long double, ldouble, 3.162277660168, 0);
}

static void
_should_combineHashesCorrectly_when_varyInput(void)
{
    /* Arrange */
    const uint64_t seed = 12345;
    const uint64_t hash = 67890;

    /* Act */
    const uint64_t combined1 = cutil_hash_combine(seed, hash);
    const uint64_t combined2 = cutil_hash_combine(hash, seed);
    const uint64_t combined3 = cutil_hash_combine(seed + 1, hash);
    const uint64_t combined4 = cutil_hash_combine(seed, hash + 1);

    /* Assert */
    TEST_ASSERT_NOT_EQUAL_UINT64(combined1, combined2);
    TEST_ASSERT_NOT_EQUAL_UINT64(combined1, combined3);
    TEST_ASSERT_NOT_EQUAL_UINT64(combined1, combined4);

    TEST_ASSERT_NOT_EQUAL_INT(1, abs((int) (combined1 - combined2)));
    TEST_ASSERT_NOT_EQUAL_INT(1, abs((int) (combined1 - combined3)));
    TEST_ASSERT_NOT_EQUAL_INT(1, abs((int) (combined1 - combined4)));
}

static void
_should_hashBytesCorrectly_when_provideString(void)
{
    /* Arrange */
    const char data1[] = "test data";
    const size_t len1 = sizeof data1 - 1;
    const char data2[] = "different";
    const size_t len2 = sizeof data2 - 1;
    const uint64_t seed = 42;

    /* Act */
    const uint64_t hash1 = cutil_hash_bytes(data1, len1, seed);
    const uint64_t hash2 = cutil_hash_bytes(data2, len2, seed);
    const uint64_t hash3 = cutil_hash_bytes(data1, len1, seed + 1);

    /* Assert */
    TEST_ASSERT_NOT_EQUAL_UINT64(hash1, hash2);
    TEST_ASSERT_NOT_EQUAL_UINT64(hash1, hash3);

    TEST_ASSERT_NOT_EQUAL_INT(1, abs((int) (hash1 - hash2)));
    TEST_ASSERT_NOT_EQUAL_INT(1, abs((int) (hash1 - hash3)));
}

static void
_should_hashStringCorrectly_when_provideEmptyAndNonEmpty(void)
{
    /* Arrange */
    const char *const str = "hello world";
    const uint64_t empty_hash = 5381; /* DJB33 initial value */

    /* Act */
    const uint64_t hash1 = cutil_hash_str(str);
    const uint64_t hash2 = cutil_hashfunc_str(str);
    const uint64_t hash3 = cutil_hash_str("hello world!");
    const uint64_t hash4 = cutil_hash_str("");

    /* Assert */
    TEST_ASSERT_EQUAL_UINT64(hash1, hash2);
    TEST_ASSERT_NOT_EQUAL_UINT64(hash1, hash3);
    TEST_ASSERT_EQUAL_UINT64(hash4, empty_hash);
}

void
setUp(void)
{}

void
tearDown(void)
{}

int
main(void)
{
    UNITY_BEGIN();

    RUN_TEST(_should_returnValue_when_hashIntegers);
    RUN_TEST(_should_returnZeroOrSomething_when_hashFloats);
    RUN_TEST(_should_combineHashesCorrectly_when_varyInput);
    RUN_TEST(_should_hashBytesCorrectly_when_provideString);
    RUN_TEST(_should_hashStringCorrectly_when_provideEmptyAndNonEmpty);

    return UNITY_END();
}

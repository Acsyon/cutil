#include "unity.h"
#include <cutil/util/compare.h>

#include <cutil/std/inttypes.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

static void
_string_comparison_bytes(const char *const data[2])
{
    const size_t len0 = strlen(data[0]);
    const size_t len1 = strlen(data[1]);
    const size_t size = CUTIL_MIN(len0, len1);
    TEST_ASSERT_EQUAL_INT(
      strcmp(data[0], data[1]) == 0,
      cutil_compare_bytes(data[0], data[1], size) == 0
    );
}

static void
_should_compareBytesCorrectly_when_compareDataCompletely(void)
{
    /* Arrange */
    const char *const DATA0 = "Test Data";
    const char *const DATA[][2] = {
      {"Hello World", "Hello World"},
      {"Alpha", "alpha"},
      {"Hello", "HelLo"},
      {"World", "WorlD"},
    };
    const size_t NUM_DATA = CUTIL_GET_NATIVE_ARRAY_SIZE(DATA);

    const uint8_t DATA1[] = {0x01, 0x02, 0x03, 0xFF, 0x00};
    const uint8_t DATA2[] = {0x01, 0x02, 0x03, 0xFE, 0x00};

    const struct Point {
        int x;
        int y;
    } P1 = {10, 20}, P2 = {10, 25};

    /* Act */
    /* Assert */
    _string_comparison_bytes((const char *[]) {DATA0, DATA0});
    for (size_t i = 0; i < NUM_DATA; ++i) {
        _string_comparison_bytes(DATA[i]);
    }
    TEST_ASSERT_EQUAL_INT(0, cutil_compare_bytes(DATA1, DATA1, sizeof DATA1));
    TEST_ASSERT_EQUAL_INT(1, cutil_compare_bytes(DATA1, DATA2, sizeof DATA1));
    TEST_ASSERT_EQUAL_INT(0, cutil_compare_bytes(&P1, &P1, sizeof P1));
    TEST_ASSERT_EQUAL_INT(-1, cutil_compare_bytes(&P1, &P2, sizeof P1));
}

static void
_should_compareBytesCorrectly_when_compareStringsPartially(void)
{
    /* Arrange */
    const char *const DATA[][2] = {
      {"Hello World", "Hello Universe"},
      {"Any", "Different"},
    };

    /* Act */
    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, cutil_compare_bytes(DATA[0][0], DATA[0][1], 5UL));
    TEST_ASSERT_EQUAL_INT(0, cutil_compare_bytes(DATA[1][0], DATA[1][1], 0UL));
}

static void
_should_compareBytesCorrectly_when_compareNullPointers(void)
{
    const char DATA[] = "Test";
    const size_t len = sizeof DATA;

    /* Both pointers NULL */
    TEST_ASSERT_EQUAL_INT(0, cutil_compare_bytes(NULL, NULL, len));

    /* One pointer NULL, one valid */
    TEST_ASSERT_EQUAL_INT(-1, cutil_compare_bytes(NULL, DATA, len));
    TEST_ASSERT_EQUAL_INT(1, cutil_compare_bytes(DATA, NULL, len));
}

static void
_should_compareSizesCorrectly_when_compareDifferentSizes(void)
{
    /* Arrange */
    const char DATA1[] = "Data One";
    const char DATA2[] = "Data Two";

    /* Act */
    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, cutil_compare_sizes(DATA1, DATA2, 4, 4));
    TEST_ASSERT_EQUAL_INT(-1, cutil_compare_sizes(DATA1, DATA2, 8, 8));
    TEST_ASSERT_EQUAL_INT(1, cutil_compare_sizes(DATA2, DATA1, 8, 8));
}

static void
_should_compareSizesCorrectly_when_compareZeroSizes(void)
{
    /* Arrange */
    const char DATA1[] = "Any";
    const char DATA2[] = "Data";

    /* Act */
    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, cutil_compare_sizes(DATA1, DATA2, 0, 0));
    TEST_ASSERT_EQUAL_INT(-1, cutil_compare_sizes(DATA1, DATA2, 0, 5));
    TEST_ASSERT_EQUAL_INT(1, cutil_compare_sizes(DATA1, DATA2, 5, 0));
}

static void
_should_compareSizesCorrectly_when_compareNullPointers(void)
{
    /* Arrange */
    const char DATA[] = "Test";

    /* Act */
    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, cutil_compare_sizes(NULL, NULL, 0, 0));
    TEST_ASSERT_EQUAL_INT(0, cutil_compare_sizes(NULL, NULL, 5, 5));

    TEST_ASSERT_EQUAL_INT(-1, cutil_compare_sizes(NULL, DATA, 0, 0));
    TEST_ASSERT_EQUAL_INT(1, cutil_compare_sizes(DATA, NULL, 0, 0));

    TEST_ASSERT_EQUAL_INT(-1, cutil_compare_sizes(NULL, DATA, 3, 3));
    TEST_ASSERT_EQUAL_INT(1, cutil_compare_sizes(DATA, NULL, 5, 5));

    TEST_ASSERT_EQUAL_INT(-1, cutil_compare_sizes(NULL, DATA, 3, 5));
    TEST_ASSERT_EQUAL_INT(1, cutil_compare_sizes(DATA, NULL, 5, 3));
}

#define COMPARISON_FUNCTION(TYPE, ID, AUXTYPE)                                 \
    static void _cmp_fnc_##ID(size_t i_lhs, size_t i_rhs, const AUXTYPE *arr)  \
    {                                                                          \
        /* Arrange */                                                          \
        const TYPE a = (TYPE) arr[i_lhs];                                      \
        const TYPE b = (TYPE) arr[i_rhs];                                      \
                                                                               \
        /* Act */                                                              \
        const int val = cutil_compare_##ID(&a, &b);                            \
                                                                               \
        /* Assert */                                                           \
        if (i_lhs < i_rhs) {                                                   \
            TEST_ASSERT_LESS_THAN_INT(0, val);                                 \
        } else if (i_lhs > i_rhs) {                                            \
            TEST_ASSERT_GREATER_THAN_INT(0, val);                              \
        } else {                                                               \
            TEST_ASSERT_EQUAL(0, val);                                         \
        }                                                                      \
    }

COMPARISON_FUNCTION(char, char, char)
COMPARISON_FUNCTION(short int, short, char)
COMPARISON_FUNCTION(int, int, char)
COMPARISON_FUNCTION(long int, long, char)
COMPARISON_FUNCTION(long long int, llong, char)

COMPARISON_FUNCTION(unsigned char, uchar, unsigned char)
COMPARISON_FUNCTION(unsigned short int, ushort, unsigned char)
COMPARISON_FUNCTION(unsigned int, uint, unsigned char)
COMPARISON_FUNCTION(unsigned long int, ulong, unsigned char)
COMPARISON_FUNCTION(unsigned long long int, ullong, unsigned char)

COMPARISON_FUNCTION(int8_t, i8, char)
COMPARISON_FUNCTION(int16_t, i16, char)
COMPARISON_FUNCTION(int32_t, i32, char)
COMPARISON_FUNCTION(int64_t, i64, char)

COMPARISON_FUNCTION(uint8_t, u8, unsigned char)
COMPARISON_FUNCTION(uint16_t, u16, unsigned char)
COMPARISON_FUNCTION(uint32_t, u32, unsigned char)
COMPARISON_FUNCTION(uint64_t, u64, unsigned char)

COMPARISON_FUNCTION(size_t, size_t, unsigned char)
COMPARISON_FUNCTION(cutil_hash_t, hash_t, unsigned char)

COMPARISON_FUNCTION(float, float, float)
COMPARISON_FUNCTION(double, double, float)
COMPARISON_FUNCTION(long double, ldouble, float)

const char SIGNED_INPUT[] = {
  -100, -50, -10, -5, 0, 6, 12, 60, 120,
};
const unsigned char UNSIGNED_INPUT[] = {
  0, 5, 6, 10, 12, 50, 60, 100, 120,
};
const float FLOAT_INPUT[] = {
  -100.1, -50.2, -10.3, -5.4, 0.0, 6.5, 12.6, 60.7, 120.8,
};

static void
_should_compareCorrectly_when_useSignedInput(void)
{
    const size_t COUNT = CUTIL_GET_NATIVE_ARRAY_SIZE(SIGNED_INPUT);
    for (size_t i_lhs = 0; i_lhs < COUNT; ++i_lhs) {
        for (size_t i_rhs = 0; i_rhs < COUNT; ++i_rhs) {
            _cmp_fnc_char(i_lhs, i_rhs, SIGNED_INPUT);
            _cmp_fnc_short(i_lhs, i_rhs, SIGNED_INPUT);
            _cmp_fnc_int(i_lhs, i_rhs, SIGNED_INPUT);
            _cmp_fnc_long(i_lhs, i_rhs, SIGNED_INPUT);
            _cmp_fnc_llong(i_lhs, i_rhs, SIGNED_INPUT);
            _cmp_fnc_i8(i_lhs, i_rhs, SIGNED_INPUT);
            _cmp_fnc_i16(i_lhs, i_rhs, SIGNED_INPUT);
            _cmp_fnc_i32(i_lhs, i_rhs, SIGNED_INPUT);
            _cmp_fnc_i64(i_lhs, i_rhs, SIGNED_INPUT);
        }
    }
}

static void
_should_compareCorrectly_when_useUnsignedInput(void)
{
    const size_t COUNT = CUTIL_GET_NATIVE_ARRAY_SIZE(UNSIGNED_INPUT);
    for (size_t i_lhs = 0; i_lhs < COUNT; ++i_lhs) {
        for (size_t i_rhs = 0; i_rhs < COUNT; ++i_rhs) {
            _cmp_fnc_uchar(i_lhs, i_rhs, UNSIGNED_INPUT);
            _cmp_fnc_ushort(i_lhs, i_rhs, UNSIGNED_INPUT);
            _cmp_fnc_uint(i_lhs, i_rhs, UNSIGNED_INPUT);
            _cmp_fnc_ulong(i_lhs, i_rhs, UNSIGNED_INPUT);
            _cmp_fnc_ullong(i_lhs, i_rhs, UNSIGNED_INPUT);
            _cmp_fnc_u8(i_lhs, i_rhs, UNSIGNED_INPUT);
            _cmp_fnc_u16(i_lhs, i_rhs, UNSIGNED_INPUT);
            _cmp_fnc_u32(i_lhs, i_rhs, UNSIGNED_INPUT);
            _cmp_fnc_u64(i_lhs, i_rhs, UNSIGNED_INPUT);
            _cmp_fnc_size_t(i_lhs, i_rhs, UNSIGNED_INPUT);
            _cmp_fnc_hash_t(i_lhs, i_rhs, UNSIGNED_INPUT);
        }
    }
}

static void
_should_compareCorrectly_when_useFloatInput(void)
{
    const size_t COUNT = CUTIL_GET_NATIVE_ARRAY_SIZE(FLOAT_INPUT);
    for (size_t i_lhs = 0; i_lhs < COUNT; ++i_lhs) {
        for (size_t i_rhs = 0; i_rhs < COUNT; ++i_rhs) {
            _cmp_fnc_float(i_lhs, i_rhs, FLOAT_INPUT);
            _cmp_fnc_double(i_lhs, i_rhs, FLOAT_INPUT);
            _cmp_fnc_ldouble(i_lhs, i_rhs, FLOAT_INPUT);
        }
    }
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

    RUN_TEST(_should_compareBytesCorrectly_when_compareDataCompletely);
    RUN_TEST(_should_compareBytesCorrectly_when_compareStringsPartially);
    RUN_TEST(_should_compareBytesCorrectly_when_compareNullPointers);
    RUN_TEST(_should_compareSizesCorrectly_when_compareDifferentSizes);
    RUN_TEST(_should_compareSizesCorrectly_when_compareZeroSizes);
    RUN_TEST(_should_compareSizesCorrectly_when_compareNullPointers);
    RUN_TEST(_should_compareCorrectly_when_useSignedInput);
    RUN_TEST(_should_compareCorrectly_when_useUnsignedInput);
    RUN_TEST(_should_compareCorrectly_when_useFloatInput);

    return UNITY_END();
}

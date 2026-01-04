#include "unity.h"

#include <cutil/data/generic/type.h>

#include <cutil/std/stdio.h>
#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

static void
_noop_one(void *obj)
{
    CUTIL_UNUSED(obj);
}

static size_t _init_ctr = 0;

static void
_init_fnc_count(void *obj)
{
    CUTIL_UNUSED(obj);
    ++_init_ctr;
}

static size_t _clear_ctr = 0;

static void
_clear_fnc_count(void *obj)
{
    CUTIL_UNUSED(obj);
    ++_clear_ctr;
}

static size_t _copy_ctr = 0;

static void
_copy_fnc_count(void *dst, const void *src)
{
    CUTIL_UNUSED(dst);
    CUTIL_UNUSED(src);
    ++_copy_ctr;
}

static size_t _hash_ctr = 0;

static uint64_t
_hash_fnc_count(const void *ptr)
{
    CUTIL_UNUSED(ptr);
    ++_hash_ctr;
    return UINT64_C(0xCAFEBABE);
}

static size_t _comp_ctr = 0;

static int
_comp_fnc_count(const void *lhs, const void *rhs)
{
    CUTIL_UNUSED(lhs);
    CUTIL_UNUSED(rhs);
    ++_comp_ctr;
    return 42;
}

static size_t _deepeq_ctr = 0;

static cutil_Bool
_deepeq_fnc_count(const void *lhs, const void *rhs)
{
    CUTIL_UNUSED(lhs);
    CUTIL_UNUSED(rhs);
    ++_deepeq_ctr;
    return true;
}

static size_t _tostring_ctr = 0;

static size_t
_tostring_fnc_count(const void *data, char *buf, size_t bufsize)
{
    CUTIL_UNUSED(data);
    ++_tostring_ctr;
    const char *const msg = "OK";
    const size_t len = 2; /* strlen("OK") */
    if (buf == NULL || bufsize == 0) {
        return len;
    }
    const size_t to_write = (bufsize - 1 < len) ? bufsize - 1 : len;
    memcpy(buf, msg, to_write);
    buf[to_write] = '\0';
    return len;
}

static void
_should_returnCorrectElement_when_useVoidGetter(void)
{
    /* Arrange */
    size_t ELEMS[] = {1, 2, 4, 8, 16, 32, 64, 128};
    const size_t NUM_ELEMS = CUTIL_GET_NATIVE_ARRAY_SIZE(ELEMS);

    for (size_t i = 0; i < NUM_ELEMS; ++i) {
        /* Act */
        const void *const p_const
          = cutil_void_array_get_elem_const(sizeof *ELEMS, ELEMS, i);
        void *const p = cutil_void_array_get_elem(sizeof *ELEMS, ELEMS, i);

        /* Assert */
        TEST_ASSERT_EQUAL_PTR(&ELEMS[i], p_const);
        TEST_ASSERT_EQUAL_PTR(&ELEMS[i], p);
    }
}

static void
_should_returnSize_when_haveDifferentSizes(void)
{
    /* Arrange */
    const size_t SIZES[] = {0, 1, 2, 4, 8, 16, 32, 64, 128};
    const size_t NUM_SIZES = CUTIL_GET_NATIVE_ARRAY_SIZE(SIZES);

    for (size_t i_size = 0; i_size < NUM_SIZES; ++i_size) {
        const size_t size = SIZES[i_size];
        const cutil_GenericType type = {.size = size};

        /* Act */
        const size_t result = cutil_GenericType_get_size(&type);

        /* Assert */
        TEST_ASSERT_EQUAL_size_t(size, result);
    }
}

static void
_should_returnInvalid_when_typeIsNull(void)
{
    /* Act */
    cutil_Bool valid = cutil_GenericType_is_valid(NULL);

    /* Assert */
    TEST_ASSERT_FALSE(valid);
}

static void
_should_returnInvalid_when_sizeIsZero(void)
{
    /* Arrange */
    const cutil_GenericType type = {
      .name = "test",
      .size = 0,
    };

    /* Act */
    const cutil_Bool valid = cutil_GenericType_is_valid(&type);

    /* Assert */
    TEST_ASSERT_FALSE(valid);
}

static void
_should_returnValid_when_sizeIsNonzero(void)
{
    /* Arrange */
    const size_t SIZES[] = {1, 2, 4, 8, 16, 32, 64, 128};
    const size_t NUM_SIZES = CUTIL_GET_NATIVE_ARRAY_SIZE(SIZES);

    for (size_t i = 0; i < NUM_SIZES; ++i) {
        const cutil_GenericType type = {.name = "test", .size = SIZES[i]};

        /* Act */
        const cutil_Bool valid = cutil_GenericType_is_valid(&type);

        /* Assert */
        TEST_ASSERT_TRUE(valid);
    }
}

static void
_should_compareEqual_when_typesAreIdentical(void)
{
    /* Arrange */
    const cutil_GenericType type1 = {.size = 4, .init = &_noop_one};
    const cutil_GenericType type2 = {.size = 4, .init = &_noop_one};

    /* Act */
    const cutil_Bool equals = cutil_GenericType_equals(&type1, &type2);

    /* Assert */
    TEST_ASSERT_TRUE(equals);
}

static void
_should_compareNotEqual_when_typesAreDifferent(void)
{
    /* Arrange */
    const cutil_GenericType type1 = {.size = 4};
    const cutil_GenericType type2 = {.size = 8};

    /* Act */
    const cutil_Bool equals = cutil_GenericType_equals(&type1, &type2);

    /* Assert */
    TEST_ASSERT_FALSE(equals);
}

static void
_should_callInitAppropriateCount_when_applyInit(void)
{
    /* Arrange */
    int ARRAY[64] = {0};
    const cutil_GenericType type
      = {.size = sizeof *ARRAY, .init = &_init_fnc_count};
    _init_ctr = 0;

    /* Act */
    const size_t num = 32;
    cutil_GenericType_apply_init_mult(&type, ARRAY, num);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(num, _init_ctr);
}

static void
_should_callClearAppropriateCount_when_applyClear(void)
{
    /* Arrange */
    int ARRAY[64] = {0};
    const cutil_GenericType type
      = {.size = sizeof *ARRAY, .clear = &_clear_fnc_count};
    _clear_ctr = 0;
    const size_t num = 24;

    /* Act */
    cutil_GenericType_apply_clear_mult(&type, ARRAY, num);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(num, _clear_ctr);
}

static void
_should_callCopyAppropriateCount_when_applyCopy(void)
{
    /* Arrange */
    int SRC[64] = {0};
    int DST[64] = {0};
    const cutil_GenericType type
      = {.size = sizeof *SRC, .copy = &_copy_fnc_count};
    _copy_ctr = 0;
    const size_t num = 16;

    /* Act */
    cutil_GenericType_apply_copy_mult(&type, DST, SRC, num);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(num, _copy_ctr);
}

static void
_should_notCallFuncs_when_functionsAreNull(void)
{
    /* Arrange */
    int SRC[64] = {0};
    int DST[64] = {0};
    for (size_t i = 0; i < CUTIL_GET_NATIVE_ARRAY_SIZE(SRC); ++i) {
        SRC[i] = (int) i;
        DST[i] = (int) i;
    }
    const cutil_GenericType type = {.size = sizeof *SRC};

    /* Act */
    cutil_GenericType_apply_init_mult(&type, DST, 10);
    cutil_GenericType_apply_clear_mult(&type, DST, 10);
    cutil_GenericType_apply_copy_mult(&type, DST, SRC, 10);

    /* Assert - arrays should be unchanged */
    TEST_ASSERT_EQUAL_INT_ARRAY(SRC, DST, CUTIL_GET_NATIVE_ARRAY_SIZE(SRC));
}

static void
_should_callInitOnNewElements_when_reallocGrows(void)
{
    /* Arrange */
    const size_t old_num = 10;
    const size_t new_num = 20;
    int *data = CUTIL_MALLOC_MULT(data, old_num);
    const cutil_GenericType type
      = {.size = sizeof *data, .init = &_init_fnc_count};
    _init_ctr = 0;

    /* Act */
    data = cutil_GenericType_apply_realloc(&type, data, old_num, new_num);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(new_num - old_num, _init_ctr);
    free(data);
}

static void
_should_callClearOnRemovedElements_when_reallocShrinks(void)
{
    /* Arrange */
    const size_t old_num = 20;
    const size_t new_num = 10;
    int *data = CUTIL_MALLOC_MULT(data, old_num);
    const cutil_GenericType type
      = {.size = sizeof(int), .clear = &_clear_fnc_count};
    _clear_ctr = 0;

    /* Act */
    data = cutil_GenericType_apply_realloc(&type, data, old_num, new_num);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(old_num - new_num, _clear_ctr);
    free(data);
}

static void
_should_useFallbackHash_when_functionIsNull(void)
{
    /* Arrange */
    const int val = 42;
    const cutil_GenericType type = {.size = sizeof val};

    /* Act */
    const uint64_t result = cutil_GenericType_apply_hash(&type, &val);
    const uint64_t expected = cutil_GenericType_fallback_hash(&type, &val);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT64(expected, result);
}

static void
_should_useCustomHash_when_functionIsProvided(void)
{
    /* Arrange */
    const int val = 42;
    const cutil_GenericType type = {
      .size = sizeof val,
      .hash = &_hash_fnc_count,
    };
    _hash_ctr = 0;

    /* Act */
    const uint64_t result = cutil_GenericType_apply_hash(&type, &val);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT64(UINT64_C(0xCAFEBABE), result);
    TEST_ASSERT_EQUAL_size_t(1, _hash_ctr);
}

static void
_should_useFallbackCompare_when_functionIsNull(void)
{
    /* Arrange */
    const int a = 5, b = 5;
    const cutil_GenericType type = {.size = sizeof a};

    /* Act */
    const int result = cutil_GenericType_apply_compare(&type, &a, &b);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, result);
}

static void
_should_useCustomCompare_when_functionIsProvided(void)
{
    /* Arrange */
    const int a = 5, b = 10;
    const cutil_GenericType type = {
      .size = sizeof a,
      .comp = &_comp_fnc_count,
    };
    _comp_ctr = 0;

    /* Act */
    const int result = cutil_GenericType_apply_compare(&type, &a, &b);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(42, result);
    TEST_ASSERT_EQUAL_size_t(1, _comp_ctr);
}

static void
_should_useFallbackDeepEquals_when_functionIsNull(void)
{
    /* Arrange */
    const int a = 7, b = 7;
    const cutil_GenericType type = {.size = sizeof a};

    /* Act */
    const cutil_Bool result
      = cutil_GenericType_apply_deep_equals(&type, &a, &b);

    /* Assert */
    TEST_ASSERT_TRUE(result);
}

static void
_should_useCustomDeepEquals_when_functionIsProvided(void)
{
    /* Arrange */
    const int a = 7, b = 9;
    const cutil_GenericType type
      = {.size = sizeof a, .deep_equals = &_deepeq_fnc_count};
    _deepeq_ctr = 0;

    /* Act */
    const cutil_Bool result
      = cutil_GenericType_apply_deep_equals(&type, &a, &b);

    /* Assert */
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_size_t(1, _deepeq_ctr);
}

static void
_should_useFallbackToString_when_functionIsNull(void)
{
    /* Arrange */
    const int val = 42;
    const cutil_GenericType type
      = {.name = "name", .size = sizeof val, .hash = &cutil_hashfunc_int};
    char buf[256];

    /* Act */
    const size_t len
      = cutil_GenericType_apply_to_string(&type, &val, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("name#0x000000000000002A", buf);
    TEST_ASSERT_EQUAL_size_t(23, len);
}

static void
_should_useCustomToString_when_functionIsProvided(void)
{
    /* Arrange */
    const int val = 123;
    const cutil_GenericType type
      = {.size = sizeof val, .to_string = &_tostring_fnc_count};
    _tostring_ctr = 0;
    char buf[256];

    /* Act */
    const size_t len
      = cutil_GenericType_apply_to_string(&type, &val, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("OK", buf);
    TEST_ASSERT_EQUAL_size_t(2, len);
    TEST_ASSERT_EQUAL_size_t(1, _tostring_ctr);
}

static void
_should_querySize_when_bufferIsNull(void)
{
    /* Arrange */
    const int val = 42;
    const cutil_GenericType type
      = {.name = "name", .size = sizeof val, .hash = &cutil_hashfunc_int};
    const char ASSERT_STR[] = "name#0x000000000000002A";
    static const size_t assert_len = sizeof ASSERT_STR - 1;
    char buf[assert_len + 1]; /* assert_len + 1 for NUL terminator */

    /* Act */
    const size_t needed_len
      = cutil_GenericType_apply_to_string(&type, &val, NULL, 0);
    const size_t actual_len
      = cutil_GenericType_apply_to_string(&type, &val, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(assert_len, needed_len);
    TEST_ASSERT_EQUAL_STRING(ASSERT_STR, buf);
    TEST_ASSERT_EQUAL_size_t(assert_len, actual_len);
}

static void
_should_returnTrue_when_nativeTypeDeepEqualsEqualValues(void)
{
    /* Signed integer native types */
    {
        const char a = 'A', b = 'A';
        TEST_ASSERT_TRUE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_CHAR, &a, &b)
        );
    }
    {
        const int a = 42, b = 42;
        TEST_ASSERT_TRUE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_INT, &a, &b)
        );
    }
    {
        const long long a = -1LL, b = -1LL;
        TEST_ASSERT_TRUE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_LLONG, &a, &b)
        );
    }

    /* Unsigned integer native types */
    {
        const unsigned int a = 100U, b = 100U;
        TEST_ASSERT_TRUE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_UINT, &a, &b)
        );
    }
    {
        const unsigned long long a = 0ULL, b = 0ULL;
        TEST_ASSERT_TRUE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_ULLONG, &a, &b)
        );
    }

    /* Fixed-width integer native types */
    {
        const int32_t a = -999, b = -999;
        TEST_ASSERT_TRUE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_I32, &a, &b)
        );
    }
    {
        const uint64_t a = UINT64_C(0xDEADBEEF), b = UINT64_C(0xDEADBEEF);
        TEST_ASSERT_TRUE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_U64, &a, &b)
        );
    }

    /* size_t */
    {
        const size_t a = 256, b = 256;
        TEST_ASSERT_TRUE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_SIZET, &a, &b)
        );
    }

    /* Floating-point native types */
    {
        const float a = 1.5f, b = 1.5f;
        TEST_ASSERT_TRUE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_FLOAT, &a, &b)
        );
    }
    {
        const double a = 3.14, b = 3.14;
        TEST_ASSERT_TRUE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_DOUBLE, &a, &b)
        );
    }
    {
        const long double a = 2.718L, b = 2.718L;
        TEST_ASSERT_TRUE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_LDOUBLE, &a, &b)
        );
    }
}

static void
_should_returnFalse_when_nativeTypeDeepEqualsUnequalValues(void)
{
    /* Signed integer native types */
    {
        const char a = 'A', b = 'Z';
        TEST_ASSERT_FALSE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_CHAR, &a, &b)
        );
    }
    {
        const int a = 42, b = -42;
        TEST_ASSERT_FALSE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_INT, &a, &b)
        );
    }
    {
        const long long a = -1LL, b = 0LL;
        TEST_ASSERT_FALSE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_LLONG, &a, &b)
        );
    }

    /* Unsigned integer native types */
    {
        const unsigned int a = 100U, b = 200U;
        TEST_ASSERT_FALSE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_UINT, &a, &b)
        );
    }
    {
        const unsigned long long a = 0ULL, b = 1ULL;
        TEST_ASSERT_FALSE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_ULLONG, &a, &b)
        );
    }

    /* Fixed-width integer native types */
    {
        const int32_t a = -999, b = 999;
        TEST_ASSERT_FALSE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_I32, &a, &b)
        );
    }
    {
        const uint64_t a = UINT64_C(0xDEADBEEF), b = UINT64_C(0xCAFEBABE);
        TEST_ASSERT_FALSE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_U64, &a, &b)
        );
    }

    /* size_t */
    {
        const size_t a = 256, b = 512;
        TEST_ASSERT_FALSE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_SIZET, &a, &b)
        );
    }

    /* Floating-point native types */
    {
        const float a = 1.5f, b = 2.5f;
        TEST_ASSERT_FALSE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_FLOAT, &a, &b)
        );
    }
    {
        const double a = 3.14, b = 2.71;
        TEST_ASSERT_FALSE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_DOUBLE, &a, &b)
        );
    }
    {
        const long double a = 2.718L, b = 3.141L;
        TEST_ASSERT_FALSE(
          cutil_GenericType_apply_deep_equals(CUTIL_GENERIC_TYPE_LDOUBLE, &a, &b)
        );
    }
}

static void
_should_returnCorrectString_when_nativeTypeToStringIntegers(void)
{
    char buf[64];
    char expected[64];
    size_t len;
    size_t exp_len;

    /* char */
    {
        const char val = 42;
        exp_len = (size_t) snprintf(expected, sizeof expected, "%hhi", val);
        len = cutil_GenericType_apply_to_string(
          CUTIL_GENERIC_TYPE_CHAR, &val, buf, sizeof buf
        );
        TEST_ASSERT_EQUAL_STRING(expected, buf);
        TEST_ASSERT_EQUAL_size_t(exp_len, len);
    }

    /* int */
    {
        const int val = -123;
        exp_len = (size_t) snprintf(expected, sizeof expected, "%i", val);
        len = cutil_GenericType_apply_to_string(
          CUTIL_GENERIC_TYPE_INT, &val, buf, sizeof buf
        );
        TEST_ASSERT_EQUAL_STRING(expected, buf);
        TEST_ASSERT_EQUAL_size_t(exp_len, len);
    }

    /* long */
    {
        const long val = 100000L;
        exp_len = (size_t) snprintf(expected, sizeof expected, "%li", val);
        len = cutil_GenericType_apply_to_string(
          CUTIL_GENERIC_TYPE_LONG, &val, buf, sizeof buf
        );
        TEST_ASSERT_EQUAL_STRING(expected, buf);
        TEST_ASSERT_EQUAL_size_t(exp_len, len);
    }

    /* unsigned int */
    {
        const unsigned int val = 255U;
        exp_len = (size_t) snprintf(expected, sizeof expected, "%u", val);
        len = cutil_GenericType_apply_to_string(
          CUTIL_GENERIC_TYPE_UINT, &val, buf, sizeof buf
        );
        TEST_ASSERT_EQUAL_STRING(expected, buf);
        TEST_ASSERT_EQUAL_size_t(exp_len, len);
    }

    /* int64_t */
    {
        const int64_t val = INT64_C(-9876543210);
        exp_len = (size_t) snprintf(
          expected, sizeof expected, "%" PRId64, val
        );
        len = cutil_GenericType_apply_to_string(
          CUTIL_GENERIC_TYPE_I64, &val, buf, sizeof buf
        );
        TEST_ASSERT_EQUAL_STRING(expected, buf);
        TEST_ASSERT_EQUAL_size_t(exp_len, len);
    }

    /* uint32_t */
    {
        const uint32_t val = UINT32_C(0xDEAD);
        exp_len = (size_t) snprintf(
          expected, sizeof expected, "%" PRIu32, val
        );
        len = cutil_GenericType_apply_to_string(
          CUTIL_GENERIC_TYPE_U32, &val, buf, sizeof buf
        );
        TEST_ASSERT_EQUAL_STRING(expected, buf);
        TEST_ASSERT_EQUAL_size_t(exp_len, len);
    }

    /* size_t */
    {
        const size_t val = 12345;
        exp_len = (size_t) snprintf(expected, sizeof expected, "%zu", val);
        len = cutil_GenericType_apply_to_string(
          CUTIL_GENERIC_TYPE_SIZET, &val, buf, sizeof buf
        );
        TEST_ASSERT_EQUAL_STRING(expected, buf);
        TEST_ASSERT_EQUAL_size_t(exp_len, len);
    }
}

static void
_should_returnCorrectString_when_nativeTypeToStringFloats(void)
{
    char buf[64];
    char expected[64];
    size_t len;
    size_t exp_len;

    /* float */
    {
        const float val = 1.5f;
        exp_len = (size_t) snprintf(expected, sizeof expected, "%f", val);
        len = cutil_GenericType_apply_to_string(
          CUTIL_GENERIC_TYPE_FLOAT, &val, buf, sizeof buf
        );
        TEST_ASSERT_EQUAL_STRING(expected, buf);
        TEST_ASSERT_EQUAL_size_t(exp_len, len);
    }

    /* double */
    {
        const double val = 3.14;
        exp_len = (size_t) snprintf(expected, sizeof expected, "%f", val);
        len = cutil_GenericType_apply_to_string(
          CUTIL_GENERIC_TYPE_DOUBLE, &val, buf, sizeof buf
        );
        TEST_ASSERT_EQUAL_STRING(expected, buf);
        TEST_ASSERT_EQUAL_size_t(exp_len, len);
    }

    /* long double */
    {
        const long double val = 2.718L;
        exp_len = (size_t) snprintf(expected, sizeof expected, "%Lf", val);
        len = cutil_GenericType_apply_to_string(
          CUTIL_GENERIC_TYPE_LDOUBLE, &val, buf, sizeof buf
        );
        TEST_ASSERT_EQUAL_STRING(expected, buf);
        TEST_ASSERT_EQUAL_size_t(exp_len, len);
    }
}

static void
_should_haveValidType_when_useNativeTypes(void)
{
#define MAKE_TYPE_INFO(TYPE, ID_UPPER, ID_LOWER)                               \
    {                                                                          \
        .type = CUTIL_GENERIC_TYPE_##ID_UPPER, .name = "cutil_" #ID_LOWER,     \
        .size = sizeof(TYPE)                                                   \
    }

    /* Arrange */
    const struct _type_info {
        const cutil_GenericType *const type;
        const char *const name;
        const size_t size;
    } INFOS[] = {
      MAKE_TYPE_INFO(char, CHAR, char),
      MAKE_TYPE_INFO(short, SHORT, short),
      MAKE_TYPE_INFO(int, INT, int),
      MAKE_TYPE_INFO(long, LONG, long),
      MAKE_TYPE_INFO(long long, LLONG, llong),
      MAKE_TYPE_INFO(unsigned char, UCHAR, uchar),
      MAKE_TYPE_INFO(unsigned short, USHORT, ushort),
      MAKE_TYPE_INFO(unsigned int, UINT, uint),
      MAKE_TYPE_INFO(unsigned long, ULONG, ulong),
      MAKE_TYPE_INFO(unsigned long long, ULLONG, ullong),
      MAKE_TYPE_INFO(int8_t, I8, i8),
      MAKE_TYPE_INFO(int16_t, I16, i16),
      MAKE_TYPE_INFO(int32_t, I32, i32),
      MAKE_TYPE_INFO(int64_t, I64, i64),
      MAKE_TYPE_INFO(uint8_t, U8, u8),
      MAKE_TYPE_INFO(uint16_t, U16, u16),
      MAKE_TYPE_INFO(uint32_t, U32, u32),
      MAKE_TYPE_INFO(uint64_t, U64, u64),
      MAKE_TYPE_INFO(size_t, SIZET, size_t),
      MAKE_TYPE_INFO(cutil_hash_t, HASHT, hash_t),
      MAKE_TYPE_INFO(float, FLOAT, float),
      MAKE_TYPE_INFO(double, DOUBLE, double),
      MAKE_TYPE_INFO(long double, LDOUBLE, ldouble),
    };
    const size_t NUM_INFOS = CUTIL_GET_NATIVE_ARRAY_SIZE(INFOS);

    for (size_t i = 0; i < NUM_INFOS; ++i) {
        const struct _type_info *const info = &INFOS[i];
        const cutil_GenericType *const type = info->type;
        const char *const name = info->name;
        const size_t size = info->size;

        /* Act */
        const cutil_Bool valid = cutil_GenericType_is_valid(type);
        const char *const nm = cutil_GenericType_get_name(type);
        const size_t sz = cutil_GenericType_get_size(type);

        /* Assert */
        TEST_ASSERT_TRUE(valid);
        TEST_ASSERT_EQUAL_STRING(name, nm);
        TEST_ASSERT_EQUAL_size_t(size, sz);
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

    RUN_TEST(_should_returnCorrectElement_when_useVoidGetter);
    RUN_TEST(_should_returnSize_when_haveDifferentSizes);
    RUN_TEST(_should_returnInvalid_when_typeIsNull);
    RUN_TEST(_should_returnInvalid_when_sizeIsZero);
    RUN_TEST(_should_returnValid_when_sizeIsNonzero);
    RUN_TEST(_should_compareEqual_when_typesAreIdentical);
    RUN_TEST(_should_compareNotEqual_when_typesAreDifferent);
    RUN_TEST(_should_callInitAppropriateCount_when_applyInit);
    RUN_TEST(_should_callClearAppropriateCount_when_applyClear);
    RUN_TEST(_should_callCopyAppropriateCount_when_applyCopy);
    RUN_TEST(_should_notCallFuncs_when_functionsAreNull);
    RUN_TEST(_should_callInitOnNewElements_when_reallocGrows);
    RUN_TEST(_should_callClearOnRemovedElements_when_reallocShrinks);
    RUN_TEST(_should_useFallbackHash_when_functionIsNull);
    RUN_TEST(_should_useCustomHash_when_functionIsProvided);
    RUN_TEST(_should_useFallbackCompare_when_functionIsNull);
    RUN_TEST(_should_useCustomCompare_when_functionIsProvided);
    RUN_TEST(_should_useFallbackDeepEquals_when_functionIsNull);
    RUN_TEST(_should_useCustomDeepEquals_when_functionIsProvided);
    RUN_TEST(_should_useFallbackToString_when_functionIsNull);
    RUN_TEST(_should_useCustomToString_when_functionIsProvided);
    RUN_TEST(_should_querySize_when_bufferIsNull);
    RUN_TEST(_should_haveValidType_when_useNativeTypes);
    RUN_TEST(_should_returnTrue_when_nativeTypeDeepEqualsEqualValues);
    RUN_TEST(_should_returnFalse_when_nativeTypeDeepEqualsUnequalValues);
    RUN_TEST(_should_returnCorrectString_when_nativeTypeToStringIntegers);
    RUN_TEST(_should_returnCorrectString_when_nativeTypeToStringFloats);

    return UNITY_END();
}

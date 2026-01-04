
#include <cutil/data/generic/type.h>

#include <cutil/std/stdio.h>
#include <cutil/std/string.h>
#include <cutil/util/compare.h>
#include <cutil/util/macro.h>

extern inline const void *
cutil_void_array_get_elem_const(size_t size, const void *data, size_t idx);

extern inline void *
cutil_void_array_get_elem(size_t size, void *data, size_t idx);

extern inline const char *
cutil_GenericType_get_name(const cutil_GenericType *type);

extern inline size_t
cutil_GenericType_get_size(const cutil_GenericType *type);

cutil_Bool
cutil_GenericType_is_valid(const cutil_GenericType *type)
{
    if (type == NULL) {
        return false;
    }
    if (type->name == NULL) {
        return false;
    }
    if (type->size == 0) {
        return false;
    }
    return true;
}

cutil_Bool
cutil_GenericType_equals(
  const cutil_GenericType *lhs, const cutil_GenericType *rhs
)
{
    return (cutil_compare_bytes(lhs, rhs, sizeof(cutil_GenericType)) == 0);
}

static inline void
_cutil_GenericType_apply_callback(
  const cutil_GenericType *type,
  void *data,
  size_t num,
  void (*const callback)(void *obj)
)
{
    CUTIL_NULL_CHECK(type);
    if (num == 0UL || callback == NULL) {
        return;
    }
    const size_t size = type->size;
    for (size_t idx = 0; idx < num; ++idx) {
        void *const p = cutil_void_array_get_elem(size, data, idx);
        callback(p);
    }
}

void
cutil_GenericType_apply_init(
  const cutil_GenericType *type, void *data, size_t num
)
{
    _cutil_GenericType_apply_callback(type, data, num, type->init);
}

void
cutil_GenericType_apply_clear(
  const cutil_GenericType *type, void *data, size_t num
)
{
    _cutil_GenericType_apply_callback(type, data, num, type->clear);
}

void
cutil_GenericType_apply_copy(
  const cutil_GenericType *type, void *dst, const void *src, size_t num
)
{
    CUTIL_NULL_CHECK(type);
    CUTIL_NULL_CHECK(dst);
    CUTIL_NULL_CHECK(src);
    if (num == 0UL) {
        return;
    }
    void (*const copy)(void *dst, const void *src) = type->copy;
    const size_t size = type->size;
    if (copy == NULL) {
        memcpy(dst, src, num * size);
        return;
    }
    for (size_t idx = 0; idx < num; ++idx) {
        void *const p_dst = cutil_void_array_get_elem(size, dst, idx);
        const void *const p_src
          = cutil_void_array_get_elem_const(size, src, idx);
        copy(p_dst, p_src);
    }
}

void *
cutil_GenericType_apply_realloc(
  const cutil_GenericType *type, void *data, size_t old_num, size_t new_num
)
{
    CUTIL_NULL_CHECK(type);
    CUTIL_NULL_CHECK(data);
    if (old_num == new_num) {
        return data;
    }
    const size_t size = type->size;
    if (old_num > new_num) {
        const size_t diff = old_num - new_num;
        void *const p = cutil_void_array_get_elem(size, data, new_num);
        cutil_GenericType_apply_clear(type, p, diff);
    }
    data = cutil_realloc(data, size * new_num);
    if (data != NULL && new_num > old_num) {
        const size_t diff = new_num - old_num;
        void *const p = cutil_void_array_get_elem(size, data, old_num);
        cutil_GenericType_apply_init(type, p, diff);
    }
    return data;
}

cutil_Bool
cutil_GenericType_fallback_deep_equals(
  const cutil_GenericType *type, const void *lhs, const void *rhs
)
{
    CUTIL_NULL_CHECK(type);
    if (type->comp != NULL) {
        return (type->comp(lhs, rhs) == 0);
    }
    return (cutil_compare_bytes(lhs, rhs, type->size) == 0);
}

extern inline cutil_Bool
cutil_GenericType_apply_deep_equals(
  const cutil_GenericType *type, const void *lhs, const void *rhs
);

int
cutil_GenericType_fallback_comp(
  const cutil_GenericType *type, const void *lhs, const void *rhs
)
{
    CUTIL_NULL_CHECK(type);
    return cutil_compare_bytes(lhs, rhs, type->size);
}

extern inline int
cutil_GenericType_apply_compare(
  const cutil_GenericType *type, const void *lhs, const void *rhs
);

cutil_hash_t
cutil_GenericType_fallback_hash(const cutil_GenericType *type, const void *data)
{
    CUTIL_NULL_CHECK(type);
    return cutil_hash_bytes(data, type->size, UINT64_C(0xc70f6907));
}

extern inline cutil_hash_t
cutil_GenericType_apply_hash(const cutil_GenericType *type, const void *data);

size_t
cutil_GenericType_fallback_to_string(
  const cutil_GenericType *type, const void *data, char *buf, size_t buflen
)
{
    CUTIL_NULL_CHECK(type);
    const cutil_hash_t hash = cutil_GenericType_apply_hash(type, data);
    const int res = snprintf(buf, buflen, "%s#0x%016" PRIX64, type->name, hash);
    return (size_t) CUTIL_MAX(0, res);
}

extern inline size_t
cutil_GenericType_apply_to_string(
  const cutil_GenericType *type, const void *data, char *buf, size_t buflen
);

#define CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(TYPE, ID_LOWER, FMT)          \
    static inline size_t _cutil_to_string_##ID_LOWER(                          \
      const void *data, char *buf, size_t buflen                               \
    )                                                                          \
    {                                                                          \
        const TYPE x = *(const TYPE *) data;                                   \
        const int res = snprintf(buf, buflen, "%" FMT, x);                     \
        return (size_t) CUTIL_MAX(0, res);                                     \
    }

CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(char, char, "hhi")
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(short, short, "hi")
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(int, int, "i")
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(long, long, "li")
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(long long, llong, "lli")

CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(unsigned char, uchar, "hhu")
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(unsigned short, ushort, "hu")
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(unsigned int, uint, "u")
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(unsigned long, ulong, "lu")
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(unsigned long long, ullong, "llu")

CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(int8_t, i8, PRId8)
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(int16_t, i16, PRId16)
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(int32_t, i32, PRId32)
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(int64_t, i64, PRId64)

CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(uint8_t, u8, PRIu8)
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(uint16_t, u16, PRIu16)
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(uint32_t, u32, PRIu32)
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(uint64_t, u64, PRIu64)

CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(size_t, size_t, "zu")
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(cutil_hash_t, hash_t, PRIu64)

CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(float, float, "f")
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(double, double, "f")
CUTIL_TEMPLATE_DEFINE_TO_STRING_FUNCTION(long double, ldouble, "Lf")

#define CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(                    \
  TYPE, ID_UPPER, ID_LOWER                                                     \
)                                                                              \
    static inline cutil_Bool _cutil_deep_equals_##ID_LOWER(                    \
      const void *lhs, const void *rhs                                         \
    )                                                                          \
    {                                                                          \
        return *(const TYPE *) lhs == *(const TYPE *) rhs;                     \
    }                                                                          \
                                                                               \
    static const cutil_GenericType CUTIL_GENERIC_TYPE_##ID_UPPER##_INSTANCE    \
      = {                                                                      \
        .name = "cutil_" #ID_LOWER,                                            \
        .size = sizeof(TYPE),                                                  \
        .deep_equals = &_cutil_deep_equals_##ID_LOWER,                         \
        .comp = &cutil_compare_##ID_LOWER,                                     \
        .hash = &cutil_hashfunc_##ID_LOWER,                                    \
        .to_string = &_cutil_to_string_##ID_LOWER                              \
    }

CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(char, CHAR, char);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(short, SHORT, short);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(int, INT, int);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(long, LONG, long);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(long long, LLONG, llong);

CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(unsigned char, UCHAR, uchar);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(
  unsigned short, USHORT, ushort
);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(unsigned int, UINT, uint);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(unsigned long, ULONG, ulong);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(
  unsigned long long, ULLONG, ullong
);

CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(int8_t, I8, i8);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(int16_t, I16, i16);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(int32_t, I32, i32);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(int64_t, I64, i64);

CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(uint8_t, U8, u8);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(uint16_t, U16, u16);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(uint32_t, U32, u32);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(uint64_t, U64, u64);

CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(size_t, SIZET, size_t);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(cutil_hash_t, HASHT, hash_t);

CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(float, FLOAT, float);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(double, DOUBLE, double);
CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(
  long double, LDOUBLE, ldouble
);

const cutil_GenericType *const CUTIL_GENERIC_TYPE_CHAR
  = &CUTIL_GENERIC_TYPE_CHAR_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_SHORT
  = &CUTIL_GENERIC_TYPE_SHORT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_INT
  = &CUTIL_GENERIC_TYPE_INT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_LONG
  = &CUTIL_GENERIC_TYPE_LONG_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_LLONG
  = &CUTIL_GENERIC_TYPE_LLONG_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_UCHAR
  = &CUTIL_GENERIC_TYPE_UCHAR_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_USHORT
  = &CUTIL_GENERIC_TYPE_USHORT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_UINT
  = &CUTIL_GENERIC_TYPE_UINT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ULONG
  = &CUTIL_GENERIC_TYPE_ULONG_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ULLONG
  = &CUTIL_GENERIC_TYPE_ULLONG_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_I8
  = &CUTIL_GENERIC_TYPE_I8_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_I16
  = &CUTIL_GENERIC_TYPE_I16_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_I32
  = &CUTIL_GENERIC_TYPE_I32_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_I64
  = &CUTIL_GENERIC_TYPE_I64_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_U8
  = &CUTIL_GENERIC_TYPE_U8_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_U16
  = &CUTIL_GENERIC_TYPE_U16_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_U32
  = &CUTIL_GENERIC_TYPE_U32_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_U64
  = &CUTIL_GENERIC_TYPE_U64_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_SIZET
  = &CUTIL_GENERIC_TYPE_SIZET_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHT
  = &CUTIL_GENERIC_TYPE_HASHT_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_FLOAT
  = &CUTIL_GENERIC_TYPE_FLOAT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_DOUBLE
  = &CUTIL_GENERIC_TYPE_DOUBLE_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_LDOUBLE
  = &CUTIL_GENERIC_TYPE_LDOUBLE_INSTANCE;

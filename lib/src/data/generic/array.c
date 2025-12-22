#include <cutil/data/generic/array.h>

#include <cutil/io/log.h>
#include <cutil/string/builder.h>
#include <cutil/util/macro.h>

static void
_cutil_Array_init_type(cutil_Array *arr, const cutil_GenericType *type)
{
    arr->type = type;
    arr->capacity = 0UL;
    arr->data = NULL;
}

static cutil_Array *
_cutil_Array_calloc(const cutil_GenericType *type)
{
    cutil_Array *const arr = CUTIL_CALLOC_OBJECT(arr);

    _cutil_Array_init_type(arr, type);

    return arr;
}

cutil_Array *
cutil_Array_alloc(const cutil_GenericType *type, size_t num)
{
    if (!cutil_GenericType_is_valid(type)) {
        cutil_log_warn("Invalid type for array");
        return NULL;
    }

    cutil_Array *const arr = _cutil_Array_calloc(type);
    cutil_Array_resize(arr, num);
    return arr;
}

void
cutil_Array_free(cutil_Array *arr)
{
    CUTIL_RETURN_IF_NULL(arr);

    cutil_Array_clear(arr);

    free(arr);
}

void
cutil_Array_clear(cutil_Array *arr)
{
    cutil_Array_resize(arr, 0);
}

void
cutil_Array_copy(cutil_Array *dst, const cutil_Array *src)
{
    if (!cutil_GenericType_equals(dst->type, src->type)) {
        cutil_log_warn("Trying to copy arrays with unequal types");
        return;
    }
#ifdef CUTIL_DEBUG
    if (dst->capacity != src->capacity) {
        cutil_log_debug("Copying unequal arrays");
    }
#endif /* CUTIL_DEBUG */
    const size_t capacity = CUTIL_MIN(dst->capacity, src->capacity);
    cutil_GenericType_apply_copy(src->type, dst->data, src->data, capacity);
}

cutil_Array *
cutil_Array_duplicate(const cutil_Array *arr)
{
    cutil_Array *const dup = cutil_Array_alloc(arr->type, arr->capacity);
    cutil_Array_copy(dup, arr);
    return dup;
}

void
cutil_Array_resize(cutil_Array *arr, size_t num)
{
    arr->data = cutil_GenericType_apply_realloc(
      arr->type, arr->data, arr->capacity, num
    );
    arr->capacity = num;
}

extern inline const cutil_GenericType *
cutil_Array_get_type(const cutil_Array *arr);

extern inline size_t
cutil_Array_get_size(const cutil_Array *arr);

extern inline size_t
cutil_Array_get_capacity(const cutil_Array *arr);

void
cutil_Array_get_mult(const cutil_Array *arr, size_t idx, size_t num, void *res)
{
#ifdef CUTIL_DEBUG
    if (idx >= arr->capacity) {
        cutil_log_warn("Trying to get array index out of bounds");
        return;
    }
    if (idx + num - 1UL >= arr->capacity) {
        cutil_log_warn("Trying to get too many array elements");
        return;
    }
#endif /* CUTIL_DEBUG */
    const void *const src = cutil_Array_get_ptr(arr, idx);
    cutil_GenericType_apply_copy(arr->type, res, src, num);
}

extern inline void
cutil_Array_get(const cutil_Array *arr, size_t idx, void *res);

extern inline const void *
cutil_Array_get_ptr(const cutil_Array *arr, size_t idx);

void
cutil_Array_set_mult(cutil_Array *arr, size_t idx, size_t num, const void *val)
{
#ifdef CUTIL_DEBUG
    if (idx >= arr->capacity) {
        cutil_log_warn("Trying to set array index out of bounds");
        return;
    }
    if (idx + num - 1UL >= arr->capacity) {
        cutil_log_warn("Trying to set too many array elements");
        return;
    }
#endif /* CUTIL_DEBUG */
    const cutil_GenericType *const type = arr->type;
    void *const dst = cutil_void_array_get_elem(type->size, arr->data, idx);
    cutil_GenericType_apply_copy(arr->type, dst, val, num);
}

extern inline void
cutil_Array_set(cutil_Array *arr, size_t idx, const void *val);

cutil_Bool
cutil_Array_deep_equals(const cutil_Array *lhs, const cutil_Array *rhs)
{
    if (lhs == rhs) {
        return true;
    }
    if (lhs == NULL || rhs == NULL) {
        return false;
    }
    if (!cutil_GenericType_equals(lhs->type, rhs->type)) {
        return false;
    }
    if (lhs->capacity != rhs->capacity) {
        return false;
    }
    for (size_t idx = 0; idx < lhs->capacity; ++idx) {
        const void *const p_lhs = cutil_Array_get_ptr(lhs, idx);
        const void *const p_rhs = cutil_Array_get_ptr(rhs, idx);
        if (!cutil_GenericType_apply_deep_equals(lhs->type, p_lhs, p_rhs)) {
            return false;
        }
    }
    return true;
}

int
cutil_Array_compare(const cutil_Array *lhs, const cutil_Array *rhs)
{
    if (lhs == rhs) {
        return 0;
    }
    if (lhs == NULL || rhs == NULL) {
        return (rhs == NULL) - (lhs == NULL);
    }
    int cmp = cutil_compare_bytes(lhs->type, rhs->type, sizeof *lhs->type);
    if (cmp != 0) {
        return cmp;
    }
    cmp = cutil_compare_size_t(&lhs->capacity, &rhs->capacity);
    if (cmp != 0) {
        return cmp;
    }
    for (size_t idx = 0; idx < lhs->capacity; ++idx) {
        const void *const p_lhs = cutil_Array_get_ptr(lhs, idx);
        const void *const p_rhs = cutil_Array_get_ptr(rhs, idx);
        cmp = cutil_GenericType_apply_compare(lhs->type, p_lhs, p_rhs);
        if (cmp != 0) {
            return cmp;
        }
    }
    return 0;
}

cutil_hash_t
cutil_Array_hash(const cutil_Array *arr)
{
    if (arr == NULL || arr->capacity == 0UL) {
        return CUTIL_HASH_C(0);
    }
    const void *p = cutil_Array_get_ptr(arr, 0UL);
    cutil_hash_t res = cutil_GenericType_apply_hash(arr->type, p);
    for (size_t idx = 1; idx < arr->capacity; ++idx) {
        p = cutil_Array_get_ptr(arr, idx);
        const cutil_hash_t hash = cutil_GenericType_apply_hash(arr->type, p);
        cutil_hash_combine_inplace(&res, hash);
    }
    return res;
}

static inline void
_append_array_elem_to_stringbuilder(
  const cutil_Array *arr,
  size_t idx,
  cutil_StringBuilder *sb,
  char *buf,
  size_t buflen
)
{
    const void *const p = cutil_Array_get_ptr(arr, idx);
    (void) cutil_GenericType_apply_to_string(arr->type, p, buf, buflen);
    cutil_StringBuilder_append(sb, buf);
}

size_t
cutil_Array_to_string(const cutil_Array *arr, char *buf, size_t buflen)
{
    if (arr == NULL) {
        const int res = snprintf(buf, buflen, "NULL");
        return (size_t) CUTIL_MAX(0, res);
    }
    /* Query the serialized length of each element via the NULL/0 convention. */
    size_t max = 0UL;
    size_t sum_elem_len = 0UL;
    for (size_t idx = 0; idx < arr->capacity; ++idx) {
        const void *const p = cutil_Array_get_ptr(arr, idx);
        const size_t len
          = cutil_GenericType_apply_to_string(arr->type, p, NULL, 0UL);
        sum_elem_len += len;
        if (len > max) {
            max = len;
        }
    }
    /* '[' + elements + ',' separators + ']', all excluding NUL. */
    if (arr->capacity > 0UL) {
        sum_elem_len += arr->capacity - 1UL;
    }
    size_t res = 2UL + sum_elem_len;
    if (buf == NULL) {
        return res;
    }
    if (res + 1UL > buflen) {
        cutil_log_warn("Cannot serialize cutil_Array: buffer too small");
        return 0UL;
    }
    cutil_StringBuilder *const sb = cutil_StringBuilder_from_string("[");
    if (arr->capacity > 0UL) {
        const size_t auxlen = max + 1UL;
        char *const auxbuf = CUTIL_MALLOC_MULT(auxbuf, auxlen);
        _append_array_elem_to_stringbuilder(arr, 0UL, sb, auxbuf, auxlen);
        for (size_t idx = 1; idx < arr->capacity; ++idx) {
            cutil_StringBuilder_append(sb, ",");
            _append_array_elem_to_stringbuilder(arr, idx, sb, auxbuf, auxlen);
        }
        free(auxbuf);
    }
    cutil_StringBuilder_append(sb, "]");
    res = cutil_StringBuilder_copy_string_to_buf(sb, buflen, buf);
    cutil_StringBuilder_free(sb);
    return res;
}

static inline void
_cutil_Array_clear(void *obj)
{
    cutil_Array *const arr = obj;
    cutil_Array_clear(arr);
}

static inline void
_cutil_Array_copy(void *dst, const void *src)
{
    cutil_Array *const arr_dst = dst;
    const cutil_Array *const arr_src = src;
    cutil_Array_copy(arr_dst, arr_src);
}

static inline cutil_Bool
_cutil_Array_deep_equals(const void *lhs, const void *rhs)
{
    const cutil_Array *const arr_lhs = lhs;
    const cutil_Array *const arr_rhs = rhs;
    return cutil_Array_deep_equals(arr_lhs, arr_rhs);
}

static inline int
_cutil_Array_comp(const void *lhs, const void *rhs)
{
    const cutil_Array *const arr_lhs = lhs;
    const cutil_Array *const arr_rhs = rhs;
    return cutil_Array_compare(arr_lhs, arr_rhs);
}

static inline cutil_hash_t
_cutil_Array_hash(const void *ptr)
{
    const cutil_Array *const arr = ptr;
    return cutil_Array_hash(arr);
}

static size_t
_cutil_Array_to_string(const void *obj, char *buf, size_t buflen)
{
    const cutil_Array *const arr = obj;
    return cutil_Array_to_string(arr, buf, buflen);
}

#define CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(                    \
  TYPE, ID_UPPER, ID_LOWER                                                     \
)                                                                              \
    static void _cutil_Array_init_##ID_LOWER(void *obj)                        \
    {                                                                          \
        cutil_Array *const arr = obj;                                          \
        _cutil_Array_init_type(arr, CUTIL_GENERIC_TYPE_##ID_UPPER);            \
    }                                                                          \
                                                                               \
    static const cutil_GenericType                                             \
      CUTIL_GENERIC_TYPE_ARRAY_##ID_UPPER##_INSTANCE = {                       \
        .name = "cutil_Array<" #ID_LOWER ">",                                  \
        .size = sizeof(cutil_Array),                                           \
        .init = &_cutil_Array_init_##ID_LOWER,                                 \
        .clear = &_cutil_Array_clear,                                          \
        .copy = &_cutil_Array_copy,                                            \
        .deep_equals = &_cutil_Array_deep_equals,                              \
        .comp = &_cutil_Array_comp,                                            \
        .hash = &_cutil_Array_hash,                                            \
        .to_string = &_cutil_Array_to_string                                   \
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

const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_CHAR
  = &CUTIL_GENERIC_TYPE_ARRAY_CHAR_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_SHORT
  = &CUTIL_GENERIC_TYPE_ARRAY_SHORT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_INT
  = &CUTIL_GENERIC_TYPE_ARRAY_INT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_LONG
  = &CUTIL_GENERIC_TYPE_ARRAY_LONG_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_LLONG
  = &CUTIL_GENERIC_TYPE_ARRAY_LLONG_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_UCHAR
  = &CUTIL_GENERIC_TYPE_ARRAY_UCHAR_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_USHORT
  = &CUTIL_GENERIC_TYPE_ARRAY_USHORT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_UINT
  = &CUTIL_GENERIC_TYPE_ARRAY_UINT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_ULONG
  = &CUTIL_GENERIC_TYPE_ARRAY_ULONG_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_ULLONG
  = &CUTIL_GENERIC_TYPE_ARRAY_ULLONG_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_I8
  = &CUTIL_GENERIC_TYPE_ARRAY_I8_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_I16
  = &CUTIL_GENERIC_TYPE_ARRAY_I16_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_I32
  = &CUTIL_GENERIC_TYPE_ARRAY_I32_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_I64
  = &CUTIL_GENERIC_TYPE_ARRAY_I64_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_U8
  = &CUTIL_GENERIC_TYPE_ARRAY_U8_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_U16
  = &CUTIL_GENERIC_TYPE_ARRAY_U16_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_U32
  = &CUTIL_GENERIC_TYPE_ARRAY_U32_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_U64
  = &CUTIL_GENERIC_TYPE_ARRAY_U64_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_SIZET
  = &CUTIL_GENERIC_TYPE_ARRAY_SIZET_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_HASHT
  = &CUTIL_GENERIC_TYPE_ARRAY_HASHT_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_FLOAT
  = &CUTIL_GENERIC_TYPE_ARRAY_FLOAT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_DOUBLE
  = &CUTIL_GENERIC_TYPE_ARRAY_DOUBLE_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAY_LDOUBLE
  = &CUTIL_GENERIC_TYPE_ARRAY_LDOUBLE_INSTANCE;

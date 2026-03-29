#include <cutil/data/generic/list/arraylist.h>

#include <cutil/data/generic/array.h>
#include <cutil/data/generic/iterator.h>
#include <cutil/io/log.h>
#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

#define ARRAYLIST_GROWTH_THRESHOLD ((size_t) (1024UL * 1024UL))
#define ITER_REWOUND_SENTINEL ((size_t) -1)

#ifndef NDEBUG
    /**
     * MACRO for checking if a cutil_List is an ArrayList. If not, function and
     * type names are logged.
     */
    #define CUTIL_ARRAYLIST_TYPE_CHECK(LIST)                                   \
        do {                                                                   \
            if (LIST->vtable != CUTIL_LIST_TYPE_ARRAYLIST) {                   \
                cutil_log_warn(                                                \
                  "%s: expected list of type %s, got %s", __func__,            \
                  CUTIL_LIST_TYPE_ARRAYLIST->name, LIST->vtable->name          \
                );                                                             \
            }                                                                  \
        } while (0)
#else
    #define CUTIL_ARRAYLIST_TYPE_CHECK(LIST) ((void) (LIST))
#endif /* NDEBUG */

typedef struct {
    cutil_Array *arr; /**< Backing store; arr->capacity >= length always */
    size_t length;    /**< Logical element count (indices 0..length-1 valid) */
} _cutil_ArrayListData;

static size_t
_arraylist_next_capacity(size_t current, size_t needed)
{
    size_t cap = (current == 0UL) ? 1UL : current;
    while (cap < needed) {
        if (cap < ARRAYLIST_GROWTH_THRESHOLD) {
            cap *= 2UL;
        } else {
            cap += ARRAYLIST_GROWTH_THRESHOLD;
        }
    }
    return cap;
}

static cutil_Status
_arraylist_ensure_capacity(_cutil_ArrayListData *ald, size_t needed)
{
    const size_t old_cap = ald->arr->capacity;
    if (needed <= old_cap) {
        return CUTIL_STATUS_SUCCESS;
    }
    const size_t new_cap = _arraylist_next_capacity(old_cap, needed);
    cutil_log_debug("ArrayList: capacity grow %zu to %zu", old_cap, new_cap);
    cutil_Array_resize(ald->arr, new_cap);
    return CUTIL_STATUS_SUCCESS;
}

static _cutil_ArrayListData *
_cutil_ArrayList_create_data(const cutil_GenericType *type)
{
    _cutil_ArrayListData *const ald = CUTIL_MALLOC_OBJECT(ald);
    ald->arr = cutil_Array_alloc(type, 0UL);
    ald->length = 0UL;
    return ald;
}

static void
_cutil_ArrayList_free(void *data)
{
    CUTIL_RETURN_IF_NULL(data);

    _cutil_ArrayListData *const ald = data;
    cutil_Array_free(ald->arr);

    free(ald);
}

static void
_cutil_ArrayList_reset(void *data)
{
    CUTIL_NULL_CHECK(data);
    _cutil_ArrayListData *const ald = data;
    const cutil_Array *const arr = ald->arr;
    const cutil_GenericType *const type = arr->type;
    cutil_GenericType_apply_clear_mult(type, arr->data, ald->length);
    ald->length = 0UL;
}

static void
_cutil_ArrayList_copy(void *dst, const void *src)
{
    CUTIL_NULL_CHECK(dst);
    CUTIL_NULL_CHECK(src);

    _cutil_ArrayListData *const dst_ald = dst;
    const _cutil_ArrayListData *const src_ald = src;

    _cutil_ArrayList_reset(dst_ald);
    _arraylist_ensure_capacity(dst_ald, src_ald->length);
    if (src_ald->length > 0UL) {
        cutil_Array_set_mult(
          dst_ald->arr, 0UL, src_ald->length, src_ald->arr->data
        );
    }
    dst_ald->length = src_ald->length;
}

static void *
_cutil_ArrayList_duplicate(const void *data)
{
    CUTIL_NULL_CHECK(data);

    const _cutil_ArrayListData *const src_ald = data;
    _cutil_ArrayListData *const dup
      = _cutil_ArrayList_create_data(src_ald->arr->type);
    _cutil_ArrayList_copy(dup, src_ald);
    return dup;
}

static size_t
_cutil_ArrayList_get_count(const void *data)
{
    CUTIL_NULL_CHECK(data);

    const _cutil_ArrayListData *const ald = data;
    return ald->length;
}

static cutil_Status
_cutil_ArrayList_get(const void *data, size_t idx, void *out)
{
    CUTIL_NULL_CHECK(data);
    CUTIL_NULL_CHECK(out);

    const _cutil_ArrayListData *const ald = data;
    const size_t length = ald->length;
    if (idx >= length) {
        cutil_log_warn(
          "ArrayList get: index %zu out of bounds (length=%zu)", idx, length
        );
        return CUTIL_STATUS_FAILURE;
    }
    cutil_Array_get(ald->arr, idx, out);
    return CUTIL_STATUS_SUCCESS;
}

static const void *
_cutil_ArrayList_get_ptr(const void *data, size_t idx)
{
    CUTIL_NULL_CHECK(data);

    const _cutil_ArrayListData *const ald = data;
    if (idx >= ald->length) {
        return NULL;
    }
    return cutil_Array_get_ptr(ald->arr, idx);
}

static size_t
_cutil_ArrayList_locate(const void *data, const void *elem)
{
    CUTIL_NULL_CHECK(data);
    CUTIL_NULL_CHECK(elem);

    const _cutil_ArrayListData *const ald = data;
    const cutil_GenericType *const type = ald->arr->type;
    for (size_t i = 0UL; i < ald->length; ++i) {
        const void *const p = cutil_Array_get_ptr(ald->arr, i);
        if (cutil_GenericType_apply_deep_equals(type, p, elem)) {
            return i;
        }
    }
    return CUTIL_LIST_NPOS;
}

static cutil_Status
_cutil_ArrayList_insert_mult(
  void *data, size_t pos, size_t num, const void *elems
)
{
    CUTIL_NULL_CHECK(data);
    CUTIL_NULL_CHECK(elems);

    _cutil_ArrayListData *const ald = data;
    if (pos > ald->length) {
        cutil_log_warn(
          "ArrayList insert: pos %zu > length %zu", pos, ald->length
        );
        return CUTIL_STATUS_FAILURE;
    }
    CUTIL_RETURN_VAL_IF_VAL(num, 0UL, CUTIL_STATUS_SUCCESS);
    const cutil_GenericType *const type = ald->arr->type;
    _arraylist_ensure_capacity(ald, ald->length + num);
    if (pos < ald->length) {
        void *const dst
          = cutil_void_array_get_elem(type->size, ald->arr->data, pos + num);
        const void *const src
          = cutil_void_array_get_elem_const(type->size, ald->arr->data, pos);
        memmove(dst, src, type->size * (ald->length - pos));
    }
    cutil_Array_set_mult(ald->arr, pos, num, elems);
    ald->length += num;
    return CUTIL_STATUS_SUCCESS;
}

static cutil_Status
_cutil_ArrayList_set(void *data, size_t idx, const void *elem)
{
    CUTIL_NULL_CHECK(data);
    CUTIL_NULL_CHECK(elem);

    const _cutil_ArrayListData *const ald = data;
    const size_t length = ald->length;
    if (idx >= length) {
        cutil_log_warn(
          "ArrayList set: index %zu out of bounds (length=%zu)", idx, length
        );
        return CUTIL_STATUS_FAILURE;
    }
    cutil_Array_set(ald->arr, idx, elem);
    return CUTIL_STATUS_SUCCESS;
}

static cutil_Status
_cutil_ArrayList_append(void *data, const void *elem)
{
    CUTIL_NULL_CHECK(data);

    _cutil_ArrayListData *const ald = data;
    return _cutil_ArrayList_insert_mult(data, ald->length, 1UL, elem);
}

static cutil_Status
_cutil_ArrayList_remove_mult(void *data, size_t pos, size_t num)
{
    CUTIL_NULL_CHECK(data);

    _cutil_ArrayListData *const ald = data;
    CUTIL_RETURN_VAL_IF_VAL(num, 0UL, CUTIL_STATUS_SUCCESS);
    if (pos + num > ald->length || pos > ald->length) {
        cutil_log_warn(
          "ArrayList remove: range [%zu, %zu) exceeds length %zu", pos,
          pos + num, ald->length
        );
        return CUTIL_STATUS_FAILURE;
    }
    const cutil_GenericType *const type = ald->arr->type;
    /* 1. Clear removed elements */
    cutil_GenericType_apply_clear_mult(
      type, cutil_void_array_get_elem(type->size, ald->arr->data, pos), num
    );
    /* 2. Shift tail left */
    if (pos + num < ald->length) {
        void *const dst
          = cutil_void_array_get_elem(type->size, ald->arr->data, pos);
        const void *const src = cutil_void_array_get_elem_const(
          type->size, ald->arr->data, pos + num
        );
        memmove(dst, src, type->size * (ald->length - pos - num));
    }
    /* 3. Zero vacated tail slots */
    memset(
      cutil_void_array_get_elem(type->size, ald->arr->data, ald->length - num),
      0, num * type->size
    );
    ald->length -= num;
    return CUTIL_STATUS_SUCCESS;
}

static void
_cutil_ArrayList_sort_custom(void *data, cutil_CompFunc *comp)
{
    CUTIL_NULL_CHECK(data);
    CUTIL_NULL_CHECK(comp);

    _cutil_ArrayListData *const ald = data;
    CUTIL_RETURN_IF_VAL(ald->length, 0UL);
    qsort(ald->arr->data, ald->length, ald->arr->type->size, comp);
}

static const cutil_GenericType *
_cutil_ArrayList_get_elem_type(const void *data)
{
    CUTIL_RETURN_NULL_IF_NULL(data);
    const _cutil_ArrayListData *const ald = data;
    return cutil_Array_get_type(ald->arr);
}

typedef struct {
    const _cutil_ArrayListData *ald; /**< const view */
    size_t idx;
} _cutil_ArrayListConstIter;

static void
_cutil_ArrayListConstIter_free(void *data)
{
    free(data);
}

static void
_cutil_ArrayListConstIter_rewind(void *data)
{
    CUTIL_NULL_CHECK(data);

    _cutil_ArrayListConstIter *const iter = data;
    iter->idx = ITER_REWOUND_SENTINEL;
}

static cutil_Bool
_cutil_ArrayListConstIter_next(void *data)
{
    CUTIL_NULL_CHECK(data);

    _cutil_ArrayListConstIter *const iter = data;
    const _cutil_ArrayListData *const ald = iter->ald;
    if (iter->idx != ITER_REWOUND_SENTINEL && iter->idx >= ald->length) {
        return CUTIL_FALSE;
    }
    ++iter->idx;
    return CUTIL_BOOLIFY(iter->idx < ald->length);
}

static const void *
_cutil_ArrayListConstIter_get_ptr(const void *data)
{
    CUTIL_NULL_CHECK(data);

    const _cutil_ArrayListConstIter *const iter = data;
    const _cutil_ArrayListData *const ald = iter->ald;
    if (iter->idx >= ald->length) {
        return NULL;
    }
    return cutil_Array_get_ptr(ald->arr, iter->idx);
}

static cutil_Status
_cutil_ArrayListConstIter_get(const void *data, void *out)
{
    CUTIL_NULL_CHECK(data);
    CUTIL_NULL_CHECK(out);

    const _cutil_ArrayListConstIter *const iter = data;
    const _cutil_ArrayListData *const ald = iter->ald;
    if (iter->idx >= ald->length) {
        return CUTIL_STATUS_FAILURE;
    }
    cutil_Array_get(ald->arr, iter->idx, out);
    return CUTIL_STATUS_SUCCESS;
}

static const cutil_ConstIteratorType
  CUTIL_CONST_ITERATOR_TYPE_ARRAYLIST_INSTANCE = {
    .name = "cutil_ConstIterator<cutil_ArrayList>",
    .free = &_cutil_ArrayListConstIter_free,
    .rewind = &_cutil_ArrayListConstIter_rewind,
    .next = &_cutil_ArrayListConstIter_next,
    .get = &_cutil_ArrayListConstIter_get,
    .get_ptr = &_cutil_ArrayListConstIter_get_ptr,
};
const cutil_ConstIteratorType *const CUTIL_CONST_ITERATOR_TYPE_ARRAYLIST
  = &CUTIL_CONST_ITERATOR_TYPE_ARRAYLIST_INSTANCE;

typedef struct {
    _cutil_ArrayListData *ald; /**< mutable view */
    size_t idx;
} _cutil_ArrayListIter;

static cutil_Status
_cutil_ArrayListIter_set(void *data, const void *val)
{
    CUTIL_NULL_CHECK(data);
    CUTIL_NULL_CHECK(val);

    _cutil_ArrayListIter *const iter = data;
    _cutil_ArrayListData *const ald = iter->ald;
    if (iter->idx >= ald->length) {
        return CUTIL_STATUS_FAILURE;
    }
    cutil_Array_set(ald->arr, iter->idx, val);
    return CUTIL_STATUS_SUCCESS;
}

static cutil_Status
_cutil_ArrayListIter_remove(void *data)
{
    CUTIL_NULL_CHECK(data);

    _cutil_ArrayListIter *const iter = data;
    _cutil_ArrayListData *const ald = iter->ald;
    if (iter->idx >= ald->length) {
        return CUTIL_STATUS_FAILURE;
    }
    const cutil_Status rc = _cutil_ArrayList_remove_mult(ald, iter->idx, 1UL);
    if (rc == CUTIL_STATUS_SUCCESS) {
        iter->idx = (iter->idx > 0UL) ? iter->idx - 1UL : ITER_REWOUND_SENTINEL;
    }
    return rc;
}

static const cutil_IteratorType CUTIL_ITERATOR_TYPE_ARRAYLIST_INSTANCE = {
  .name = "cutil_Iterator<cutil_ArrayList>",
  .free = &_cutil_ArrayListConstIter_free,
  .rewind = &_cutil_ArrayListConstIter_rewind,
  .next = &_cutil_ArrayListConstIter_next,
  .get = &_cutil_ArrayListConstIter_get,
  .get_ptr = &_cutil_ArrayListConstIter_get_ptr,
  .set = &_cutil_ArrayListIter_set,
  .remove = &_cutil_ArrayListIter_remove,
};
const cutil_IteratorType *const CUTIL_ITERATOR_TYPE_ARRAYLIST
  = &CUTIL_ITERATOR_TYPE_ARRAYLIST_INSTANCE;

static cutil_ConstIterator *
_cutil_ArrayList_get_const_iterator(const void *data)
{
    CUTIL_RETURN_NULL_IF_NULL(data);

    _cutil_ArrayListConstIter *const iter = CUTIL_MALLOC_OBJECT(iter);
    iter->ald = data;
    _cutil_ArrayListConstIter_rewind(iter);

    cutil_ConstIterator *const it = CUTIL_MALLOC_OBJECT(it);
    it->vtable = CUTIL_CONST_ITERATOR_TYPE_ARRAYLIST;
    it->data = iter;

    cutil_log_debug("ArrayList: created const iterator");
    return it;
}

static cutil_Iterator *
_cutil_ArrayList_get_iterator(void *data)
{
    CUTIL_RETURN_NULL_IF_NULL(data);

    _cutil_ArrayListIter *const iter = CUTIL_MALLOC_OBJECT(iter);
    iter->ald = data;
    _cutil_ArrayListConstIter_rewind(iter);

    cutil_Iterator *const it = CUTIL_MALLOC_OBJECT(it);
    it->vtable = CUTIL_ITERATOR_TYPE_ARRAYLIST;
    it->data = iter;

    cutil_log_debug("ArrayList: created iterator");
    return it;
}

static const cutil_ListType CUTIL_LIST_TYPE_ARRAYLIST_OBJECT = {
  .name = "cutil_ArrayList",
  .free = &_cutil_ArrayList_free,
  .reset = &_cutil_ArrayList_reset,
  .copy = &_cutil_ArrayList_copy,
  .duplicate = &_cutil_ArrayList_duplicate,
  .get_count = &_cutil_ArrayList_get_count,
  .get = &_cutil_ArrayList_get,
  .get_ptr = &_cutil_ArrayList_get_ptr,
  .locate = &_cutil_ArrayList_locate,
  .set = &_cutil_ArrayList_set,
  .append = &_cutil_ArrayList_append,
  .insert_mult = &_cutil_ArrayList_insert_mult,
  .remove_mult = &_cutil_ArrayList_remove_mult,
  .sort_custom = &_cutil_ArrayList_sort_custom,
  .get_elem_type = &_cutil_ArrayList_get_elem_type,
  .get_const_iterator = &_cutil_ArrayList_get_const_iterator,
  .get_iterator = &_cutil_ArrayList_get_iterator,
};
const cutil_ListType *const CUTIL_LIST_TYPE_ARRAYLIST
  = &CUTIL_LIST_TYPE_ARRAYLIST_OBJECT;

cutil_List *
cutil_ArrayList_alloc(const cutil_GenericType *type)
{
    if (!cutil_GenericType_is_valid(type)) {
        cutil_log_warn("cutil_ArrayList_alloc: invalid type");
        return NULL;
    }
    cutil_List *const list = CUTIL_MALLOC_OBJECT(list);
    list->vtable = CUTIL_LIST_TYPE_ARRAYLIST;
    list->data = _cutil_ArrayList_create_data(type);
    return list;
}

size_t
cutil_ArrayList_get_capacity(const cutil_List *list)
{
    CUTIL_NULL_CHECK(list);
    CUTIL_ARRAYLIST_TYPE_CHECK(list);

    const _cutil_ArrayListData *const ald = list->data;
    return ald->arr->capacity;
}

cutil_Status
cutil_ArrayList_resize(cutil_List *list, size_t capacity)
{
    CUTIL_NULL_CHECK(list);
    CUTIL_ARRAYLIST_TYPE_CHECK(list);

    _cutil_ArrayListData *const ald = list->data;
    /* Truncate if shrinking below current logical length */
    if (capacity < ald->length) {
        const cutil_GenericType *const type = ald->arr->type;
        cutil_GenericType_apply_clear_mult(
          type, cutil_void_array_get_elem(type->size, ald->arr->data, capacity),
          ald->length - capacity
        );
        ald->length = capacity;
    }
    cutil_Array_resize(ald->arr, capacity);
    return CUTIL_STATUS_SUCCESS;
}

cutil_Status
cutil_ArrayList_shrink_to_fit(cutil_List *list)
{
    CUTIL_NULL_CHECK(list);
    CUTIL_ARRAYLIST_TYPE_CHECK(list);

    _cutil_ArrayListData *const ald = list->data;
    cutil_Array_resize(ald->arr, ald->length);
    return CUTIL_STATUS_SUCCESS;
}

#define CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(                    \
  TYPE, ID_UPPER, ID_LOWER                                                     \
)                                                                              \
    static void _cutil_ArrayList_init_##ID_LOWER(void *obj)                    \
    {                                                                          \
        cutil_List *const list = obj;                                          \
        list->vtable = CUTIL_LIST_TYPE_ARRAYLIST;                              \
        list->data                                                             \
          = _cutil_ArrayList_create_data(CUTIL_GENERIC_TYPE_##ID_UPPER);       \
    }                                                                          \
                                                                               \
    static const cutil_GenericType                                             \
      CUTIL_GENERIC_TYPE_ARRAYLIST_##ID_UPPER##_INSTANCE = {                   \
        .name = "cutil_ArrayList<" #ID_LOWER ">",                              \
        .size = sizeof(cutil_List),                                            \
        .init = &_cutil_ArrayList_init_##ID_LOWER,                             \
        .clear = &cutil_List_clear_generic,                                    \
        .copy = &cutil_List_copy_generic,                                      \
        .deep_equals = &cutil_List_deep_equals_generic,                        \
        .comp = &cutil_List_compare_generic,                                   \
        .hash = &cutil_List_hash_generic,                                      \
        .to_string = &cutil_List_to_string_generic                             \
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

const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_CHAR
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_CHAR_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_SHORT
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_SHORT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_INT
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_INT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_LONG
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_LONG_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_LLONG
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_LLONG_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_UCHAR
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_UCHAR_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_USHORT
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_USHORT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_UINT
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_UINT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_ULONG
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_ULONG_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_ULLONG
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_ULLONG_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_I8
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_I8_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_I16
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_I16_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_I32
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_I32_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_I64
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_I64_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_U8
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_U8_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_U16
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_U16_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_U32
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_U32_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_U64
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_U64_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_SIZET
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_SIZET_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_HASHT
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_HASHT_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_FLOAT
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_FLOAT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_DOUBLE
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_DOUBLE_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_ARRAYLIST_LDOUBLE
  = &CUTIL_GENERIC_TYPE_ARRAYLIST_LDOUBLE_INSTANCE;

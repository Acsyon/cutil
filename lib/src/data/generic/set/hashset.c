#include <cutil/data/generic/set/hashset.h>

#include <cutil/data/generic/map/hashmap.h>
#include <cutil/data/generic/type.h>
#include <cutil/io/log.h>
#include <cutil/std/stdlib.h>

cutil_Set *
cutil_HashSet_alloc(const cutil_GenericType *elem_type)
{
    if (!cutil_GenericType_is_valid(elem_type)) {
        return NULL;
    }
    cutil_Set *const set = CUTIL_MALLOC_OBJECT(set);
    set->vtable = CUTIL_SET_TYPE_HASHSET;
    set->data = cutil_HashMap_alloc(elem_type, CUTIL_GENERIC_TYPE_UNIT);
    return set;
}

static void
_cutil_HashSet_free(void *data)
{
    cutil_Map *const map = data;
    cutil_Map_free(map);
}

static void
_cutil_HashSet_reset(void *data)
{
    cutil_Map *const map = data;
    cutil_Map_reset(map);
}

static void
_cutil_HashSet_copy(void *dst, const void *src)
{
    cutil_Map *const dst_map = dst;
    const cutil_Map *const src_map = src;
    cutil_Map_copy(dst_map, src_map);
}

static void *
_cutil_HashSet_duplicate(const void *data)
{
    const cutil_Map *const map = data;
    return cutil_Map_duplicate(map);
}

static size_t
_cutil_HashSet_get_count(const void *data)
{
    const cutil_Map *const map = data;
    return cutil_Map_get_count(map);
}

static cutil_Bool
_cutil_HashSet_contains(const void *data, const void *elem)
{
    const cutil_Map *const map = data;
    return cutil_Map_contains(map, elem);
}

static cutil_Status
_cutil_HashSet_add(void *data, const void *elem)
{
    cutil_Map *const map = data;
    if (cutil_Map_contains(map, elem)) {
        cutil_log_warn("HashSet add: element already in set, skipping");
        return CUTIL_STATUS_SUCCESS;
    }
    return cutil_Map_set(map, elem, &CUTIL_UNIT_VALUE);
}

static cutil_Status
_cutil_HashSet_remove(void *data, const void *elem)
{
    cutil_Map *const map = data;
    return cutil_Map_remove(map, elem);
}

static const cutil_GenericType *
_cutil_HashSet_get_elem_type(const void *data)
{
    const cutil_Map *const map = data;
    return cutil_Map_get_key_type(map);
}

static cutil_ConstIterator *
_cutil_HashSet_get_const_iterator(const void *data)
{
    const cutil_Map *const map = data;
    return cutil_Map_get_const_iterator(map);
}

static cutil_Iterator *
_cutil_HashSet_get_iterator(void *data)
{
    cutil_Map *const map = data;
    return cutil_Map_get_iterator(map);
}

static const cutil_SetType CUTIL_SET_TYPE_HASHSET_OBJECT = {
  .name = "cutil_HashSet",
  .free = &_cutil_HashSet_free,
  .reset = &_cutil_HashSet_reset,
  .copy = &_cutil_HashSet_copy,
  .duplicate = &_cutil_HashSet_duplicate,
  .get_count = &_cutil_HashSet_get_count,
  .contains = &_cutil_HashSet_contains,
  .add = &_cutil_HashSet_add,
  .remove = &_cutil_HashSet_remove,
  .get_elem_type = &_cutil_HashSet_get_elem_type,
  .get_const_iterator = &_cutil_HashSet_get_const_iterator,
  .get_iterator = &_cutil_HashSet_get_iterator,
};

const cutil_SetType *const CUTIL_SET_TYPE_HASHSET
  = &CUTIL_SET_TYPE_HASHSET_OBJECT;

#define CUTIL_TEMPLATE_DEFINE_GENERIC_NATIVE_TYPE_INSTANCE(                    \
  TYPE, ID_UPPER, ID_LOWER                                                     \
)                                                                              \
    static void _cutil_HashSet_init_##ID_LOWER(void *obj)                      \
    {                                                                          \
        cutil_Set *const set = obj;                                            \
        set->vtable = CUTIL_SET_TYPE_HASHSET;                                  \
        set->data = cutil_HashMap_alloc(                                       \
          CUTIL_GENERIC_TYPE_##ID_UPPER, CUTIL_GENERIC_TYPE_UNIT               \
        );                                                                     \
    }                                                                          \
                                                                               \
    static const cutil_GenericType                                             \
      CUTIL_GENERIC_TYPE_HASHSET_##ID_UPPER##_INSTANCE = {                     \
        .name = "cutil_HashSet<" #ID_LOWER ">",                                \
        .size = sizeof(cutil_Set),                                             \
        .init = &_cutil_HashSet_init_##ID_LOWER,                               \
        .clear = &cutil_Set_clear_generic,                                     \
        .copy = &cutil_Set_copy_generic,                                       \
        .deep_equals = &cutil_Set_deep_equals_generic,                         \
        .comp = &cutil_Set_compare_generic,                                    \
        .hash = &cutil_Set_hash_generic,                                       \
        .to_string = &cutil_Set_to_string_generic                              \
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

const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_CHAR
  = &CUTIL_GENERIC_TYPE_HASHSET_CHAR_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_SHORT
  = &CUTIL_GENERIC_TYPE_HASHSET_SHORT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_INT
  = &CUTIL_GENERIC_TYPE_HASHSET_INT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_LONG
  = &CUTIL_GENERIC_TYPE_HASHSET_LONG_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_LLONG
  = &CUTIL_GENERIC_TYPE_HASHSET_LLONG_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_UCHAR
  = &CUTIL_GENERIC_TYPE_HASHSET_UCHAR_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_USHORT
  = &CUTIL_GENERIC_TYPE_HASHSET_USHORT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_UINT
  = &CUTIL_GENERIC_TYPE_HASHSET_UINT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_ULONG
  = &CUTIL_GENERIC_TYPE_HASHSET_ULONG_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_ULLONG
  = &CUTIL_GENERIC_TYPE_HASHSET_ULLONG_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_I8
  = &CUTIL_GENERIC_TYPE_HASHSET_I8_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_I16
  = &CUTIL_GENERIC_TYPE_HASHSET_I16_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_I32
  = &CUTIL_GENERIC_TYPE_HASHSET_I32_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_I64
  = &CUTIL_GENERIC_TYPE_HASHSET_I64_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_U8
  = &CUTIL_GENERIC_TYPE_HASHSET_U8_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_U16
  = &CUTIL_GENERIC_TYPE_HASHSET_U16_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_U32
  = &CUTIL_GENERIC_TYPE_HASHSET_U32_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_U64
  = &CUTIL_GENERIC_TYPE_HASHSET_U64_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_SIZET
  = &CUTIL_GENERIC_TYPE_HASHSET_SIZET_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_HASHT
  = &CUTIL_GENERIC_TYPE_HASHSET_HASHT_INSTANCE;

const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_FLOAT
  = &CUTIL_GENERIC_TYPE_HASHSET_FLOAT_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_DOUBLE
  = &CUTIL_GENERIC_TYPE_HASHSET_DOUBLE_INSTANCE;
const cutil_GenericType *const CUTIL_GENERIC_TYPE_HASHSET_LDOUBLE
  = &CUTIL_GENERIC_TYPE_HASHSET_LDOUBLE_INSTANCE;

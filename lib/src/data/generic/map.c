#include <cutil/data/generic/map.h>

#include <cutil/io/log.h>
#include <cutil/std/stdlib.h>
#include <cutil/string/builder.h>
#include <cutil/string/util/iterator.h>
#include <cutil/util/compare.h>
#include <cutil/util/macro.h>

cutil_Bool
cutil_MapType_equals(const cutil_MapType *lhs, const cutil_MapType *rhs)
{
    return (cutil_compare_bytes(lhs, rhs, sizeof(cutil_MapType)) == 0);
}

extern inline const cutil_MapType *
cutil_Map_get_vtable(const cutil_Map *map);

extern inline void
cutil_Map_clear(cutil_Map *map);

extern inline void
cutil_Map_free(cutil_Map *map);

extern inline void
cutil_Map_reset(cutil_Map *map);

extern inline void
cutil_Map_copy(cutil_Map *dst, const cutil_Map *src);

extern inline cutil_Map *
cutil_Map_duplicate(const cutil_Map *map);

extern inline size_t
cutil_Map_get_count(const cutil_Map *map);

extern inline int
cutil_Map_remove(cutil_Map *map, const void *key);

extern inline cutil_Bool
cutil_Map_contains(const cutil_Map *map, const void *key);

extern inline int
cutil_Map_get(const cutil_Map *map, const void *key, void *val);

extern inline const void *
cutil_Map_get_ptr(const cutil_Map *map, const void *key);

extern inline int
cutil_Map_set(cutil_Map *map, const void *key, const void *val);

extern inline const cutil_GenericType *
cutil_Map_get_key_type(const cutil_Map *map);

extern inline const cutil_GenericType *
cutil_Map_get_val_type(const cutil_Map *map);

extern inline cutil_ConstIterator *
cutil_Map_get_const_iterator(const cutil_Map *map);

extern inline cutil_Iterator *
cutil_Map_get_iterator(cutil_Map *map);

cutil_Bool
cutil_Map_deep_equals(const cutil_Map *lhs, const cutil_Map *rhs)
{
    return cutil_Map_deep_equals_generic(lhs, rhs);
}

int
cutil_Map_compare(const cutil_Map *lhs, const cutil_Map *rhs)
{
    return cutil_Map_compare_generic(lhs, rhs);
}

cutil_hash_t
cutil_Map_hash(const cutil_Map *map)
{
    return cutil_Map_hash_generic(map);
}

size_t
cutil_Map_to_string(const cutil_Map *map, char *buf, size_t buflen)
{
    return cutil_Map_to_string_generic(map, buf, buflen);
}

extern inline void
cutil_Map_clear_generic(void *obj);

extern inline void
cutil_Map_copy_generic(void *dst, const void *src);

cutil_Bool
cutil_Map_deep_equals_generic(const void *vlhs, const void *vrhs)
{
    const cutil_Map *const lhs = vlhs;
    const cutil_Map *const rhs = vrhs;
    if (lhs == rhs) {
        return true;
    }
    if (lhs == NULL || rhs == NULL) {
        return false;
    }
    if (!cutil_MapType_equals(lhs->vtable, rhs->vtable)) {
        return false;
    }
    const cutil_GenericType *const lhs_key_type = cutil_Map_get_key_type(lhs);
    const cutil_GenericType *const rhs_key_type = cutil_Map_get_key_type(rhs);
    if (!cutil_GenericType_equals(lhs_key_type, rhs_key_type)) {
        return false;
    }
    const cutil_GenericType *const lhs_val_type = cutil_Map_get_val_type(lhs);
    const cutil_GenericType *const rhs_val_type = cutil_Map_get_val_type(rhs);
    if (!cutil_GenericType_equals(lhs_val_type, rhs_val_type)) {
        return false;
    }
    if (cutil_Map_get_count(lhs) != cutil_Map_get_count(rhs)) {
        return false;
    }
    cutil_Bool result = true;
    cutil_ConstIterator *const it = cutil_Map_get_const_iterator(lhs);
    while (cutil_ConstIterator_next(it)) {
        const void *const key = cutil_ConstIterator_get_ptr(it);
        if (!cutil_Map_contains(rhs, key)) {
            result = false;
            break;
        }
        const void *const lhs_val = cutil_Map_get_ptr(lhs, key);
        const void *const rhs_val = cutil_Map_get_ptr(rhs, key);
        if (!cutil_GenericType_apply_deep_equals(
              lhs_val_type, lhs_val, rhs_val
            ))
        {
            result = false;
            break;
        }
    }
    cutil_ConstIterator_free(it);
    return result;
}

int
cutil_Map_compare_generic(const void *vlhs, const void *vrhs)
{
    const cutil_Map *const lhs = vlhs;
    const cutil_Map *const rhs = vrhs;
    if (lhs == rhs) {
        return 0;
    }
    if (lhs == NULL || rhs == NULL) {
        return (rhs == NULL) - (lhs == NULL);
    }
    const cutil_GenericType *const lhs_key_type = cutil_Map_get_key_type(lhs);
    const cutil_GenericType *const rhs_key_type = cutil_Map_get_key_type(rhs);
    int cmp
      = cutil_compare_bytes(lhs_key_type, rhs_key_type, sizeof *lhs_key_type);
    if (cmp != 0) {
        return cmp;
    }
    const cutil_GenericType *const lhs_val_type = cutil_Map_get_val_type(lhs);
    const cutil_GenericType *const rhs_val_type = cutil_Map_get_val_type(rhs);
    cmp = cutil_compare_bytes(lhs_val_type, rhs_val_type, sizeof *lhs_val_type);
    if (cmp != 0) {
        return cmp;
    }
    const size_t lhs_count = cutil_Map_get_count(lhs);
    const size_t rhs_count = cutil_Map_get_count(rhs);
    cmp = cutil_compare_size_t(&lhs_count, &rhs_count);
    if (cmp != 0) {
        return cmp;
    }
    const cutil_hash_t lhs_hash = cutil_Map_hash(lhs);
    const cutil_hash_t rhs_hash = cutil_Map_hash(rhs);
    return cutil_compare_hash_t(&lhs_hash, &rhs_hash);
}

cutil_hash_t
cutil_Map_hash_generic(const void *vmap)
{
    const cutil_Map *const map = vmap;
    if (map == NULL || cutil_Map_get_count(map) == 0UL) {
        return CUTIL_HASH_C(0);
    }
    const cutil_GenericType *const key_type = cutil_Map_get_key_type(map);
    const cutil_GenericType *const val_type = cutil_Map_get_val_type(map);
    cutil_ConstIterator *const it = cutil_Map_get_const_iterator(map);
    cutil_hash_t res = CUTIL_HASH_C(0);
    while (cutil_ConstIterator_next(it)) {
        const void *const key = cutil_ConstIterator_get_ptr(it);
        const void *const val = cutil_Map_get_ptr(map, key);
        const cutil_hash_t hk = cutil_GenericType_apply_hash(key_type, key);
        const cutil_hash_t hv = cutil_GenericType_apply_hash(val_type, val);
        res ^= (hk ^ hv);
    }
    cutil_ConstIterator_free(it);
    return res;
}

static size_t
_map_entry_to_string(const void *key, char *buf, size_t buflen, const void *ctx)
{
    static const char SEP = ':';
    static const size_t SEP_LEN = 1UL;
    CUTIL_UNUSED(buflen);

    const cutil_Map *const map = ctx;
    const cutil_GenericType *const key_type = cutil_Map_get_key_type(map);
    const cutil_GenericType *const val_type = cutil_Map_get_val_type(map);
    const void *const val = cutil_Map_get_ptr(map, key);

    const size_t key_len
      = cutil_GenericType_apply_to_string(key_type, key, NULL, 0UL);
    const size_t val_len
      = cutil_GenericType_apply_to_string(val_type, val, NULL, 0UL);
    const size_t total = key_len + 1UL + val_len;

    if (buf == NULL) {
        return total;
    }

    cutil_GenericType_apply_to_string(key_type, key, buf, key_len + 1UL);
    buf[key_len] = SEP;
    cutil_GenericType_apply_to_string(
      val_type, val, buf + key_len + SEP_LEN, val_len + 1UL
    );
    return total;
}

size_t
cutil_Map_to_string_generic(const void *vmap, char *buf, size_t buflen)
{
    const cutil_Map *const map = vmap;
    cutil_ConstIterator *const it
      = (map != NULL) ? cutil_Map_get_const_iterator(map) : NULL;
    const size_t result = cutil_ConstIterator_to_string(
      it, CUTIL_SEQ_DELIMS_MAP, &_map_entry_to_string, map, buf, buflen
    );
    cutil_ConstIterator_free(it);
    return result;
}

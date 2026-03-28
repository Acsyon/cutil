#include <cutil/data/generic/set.h>

#include <cutil/string/builder.h>
#include <cutil/string/util/iterator.h>
#include <cutil/util/compare.h>

cutil_Bool
cutil_SetType_equals(const cutil_SetType *lhs, const cutil_SetType *rhs)
{
    return (cutil_compare_bytes(lhs, rhs, sizeof(cutil_SetType)) == 0);
}

extern inline const cutil_SetType *
cutil_Set_get_vtable(const cutil_Set *set);

extern inline void
cutil_Set_clear(cutil_Set *set);

extern inline void
cutil_Set_free(cutil_Set *set);

extern inline void
cutil_Set_reset(cutil_Set *set);

extern inline void
cutil_Set_copy(cutil_Set *dst, const cutil_Set *src);

extern inline cutil_Set *
cutil_Set_duplicate(const cutil_Set *set);

extern inline size_t
cutil_Set_get_count(const cutil_Set *set);

extern inline cutil_Bool
cutil_Set_contains(const cutil_Set *set, const void *elem);

extern inline int
cutil_Set_add(cutil_Set *set, const void *elem);

extern inline int
cutil_Set_remove(cutil_Set *set, const void *elem);

extern inline const cutil_GenericType *
cutil_Set_get_elem_type(const cutil_Set *set);

extern inline cutil_ConstIterator *
cutil_Set_get_const_iterator(const cutil_Set *set);

extern inline cutil_Iterator *
cutil_Set_get_iterator(cutil_Set *set);

cutil_Bool
cutil_Set_deep_equals(const cutil_Set *lhs, const cutil_Set *rhs)
{
    return cutil_Set_deep_equals_generic(lhs, rhs);
}

int
cutil_Set_compare(const cutil_Set *lhs, const cutil_Set *rhs)
{
    return cutil_Set_compare_generic(lhs, rhs);
}

cutil_hash_t
cutil_Set_hash(const cutil_Set *set)
{
    return cutil_Set_hash_generic(set);
}

size_t
cutil_Set_to_string(const cutil_Set *set, char *buf, size_t buflen)
{
    return cutil_Set_to_string_generic(set, buf, buflen);
}

extern inline void
cutil_Set_clear_generic(void *obj);

extern inline void
cutil_Set_copy_generic(void *dst, const void *src);

cutil_Bool
cutil_Set_deep_equals_generic(const void *vlhs, const void *vrhs)
{
    const cutil_Set *const lhs = vlhs;
    const cutil_Set *const rhs = vrhs;
    if (lhs == rhs) {
        return true;
    }
    if (lhs == NULL || rhs == NULL) {
        return false;
    }
    if (!cutil_SetType_equals(lhs->vtable, rhs->vtable)) {
        return false;
    }
    const cutil_GenericType *const lhs_type = cutil_Set_get_elem_type(lhs);
    const cutil_GenericType *const rhs_type = cutil_Set_get_elem_type(rhs);
    if (!cutil_GenericType_equals(lhs_type, rhs_type)) {
        return false;
    }
    if (cutil_Set_get_count(lhs) != cutil_Set_get_count(rhs)) {
        return false;
    }
    cutil_Bool result = true;
    cutil_ConstIterator *const it = cutil_Set_get_const_iterator(lhs);
    while (cutil_ConstIterator_next(it)) {
        const void *const p = cutil_ConstIterator_get_ptr(it);
        if (!cutil_Set_contains(rhs, p)) {
            result = false;
            break;
        }
    }
    cutil_ConstIterator_free(it);
    return result;
}

int
cutil_Set_compare_generic(const void *vlhs, const void *vrhs)
{
    const cutil_Set *const lhs = vlhs;
    const cutil_Set *const rhs = vrhs;
    if (lhs == rhs) {
        return 0;
    }
    if (lhs == NULL || rhs == NULL) {
        return (rhs == NULL) - (lhs == NULL);
    }
    const cutil_GenericType *const lhs_type = cutil_Set_get_elem_type(lhs);
    const cutil_GenericType *const rhs_type = cutil_Set_get_elem_type(rhs);
    int cmp = cutil_compare_bytes(lhs_type, rhs_type, sizeof *lhs_type);
    if (cmp != 0) {
        return cmp;
    }
    const size_t lhs_count = cutil_Set_get_count(lhs);
    const size_t rhs_count = cutil_Set_get_count(rhs);
    cmp = cutil_compare_size_t(&lhs_count, &rhs_count);
    if (cmp != 0) {
        return cmp;
    }
    const cutil_hash_t lhs_hash = cutil_Set_hash(lhs);
    const cutil_hash_t rhs_hash = cutil_Set_hash(rhs);
    return cutil_compare_hash_t(&lhs_hash, &rhs_hash);
}

cutil_hash_t
cutil_Set_hash_generic(const void *vset)
{
    const cutil_Set *const set = vset;
    if (set == NULL || cutil_Set_get_count(set) == 0UL) {
        return CUTIL_HASH_C(0);
    }
    const cutil_GenericType *const type = cutil_Set_get_elem_type(set);
    cutil_ConstIterator *const it = cutil_Set_get_const_iterator(set);
    cutil_hash_t res = CUTIL_HASH_C(0);
    while (cutil_ConstIterator_next(it)) {
        const void *const p = cutil_ConstIterator_get_ptr(it);
        const cutil_hash_t h = cutil_GenericType_apply_hash(type, p);
        res ^= h;
    }
    cutil_ConstIterator_free(it);
    return res;
}

static size_t
_set_elem_to_string(const void *elem, char *buf, size_t buflen, const void *ctx)
{
    const cutil_GenericType *const type = ctx;
    return cutil_GenericType_apply_to_string(type, elem, buf, buflen);
}

size_t
cutil_Set_to_string_generic(const void *vset, char *buf, size_t buflen)
{
    const cutil_Set *const set = vset;
    const cutil_GenericType *const type
      = (set != NULL) ? cutil_Set_get_elem_type(set) : NULL;
    cutil_ConstIterator *const it
      = (set != NULL) ? cutil_Set_get_const_iterator(set) : NULL;
    const size_t result = cutil_ConstIterator_to_string(
      it, CUTIL_SEQ_DELIMS_SET, &_set_elem_to_string, type, buf, buflen
    );
    cutil_ConstIterator_free(it);
    return result;
}

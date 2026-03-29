#include <cutil/data/generic/list.h>

#include <cutil/string/builder.h>
#include <cutil/string/util/iterator.h>
#include <cutil/util/compare.h>

cutil_Bool
cutil_ListType_equals(const cutil_ListType *lhs, const cutil_ListType *rhs)
{
    return (cutil_compare_bytes(lhs, rhs, sizeof(cutil_ListType)) == 0);
}

extern inline const cutil_ListType *
cutil_List_get_vtable(const cutil_List *list);

extern inline void
cutil_List_clear(cutil_List *list);

extern inline void
cutil_List_free(cutil_List *list);

extern inline void
cutil_List_reset(cutil_List *list);

extern inline void
cutil_List_copy(cutil_List *dst, const cutil_List *src);

extern inline cutil_List *
cutil_List_duplicate(const cutil_List *list);

extern inline size_t
cutil_List_get_count(const cutil_List *list);

extern inline int
cutil_List_get(const cutil_List *list, size_t idx, void *out);

extern inline const void *
cutil_List_get_ptr(const cutil_List *list, size_t idx);

extern inline size_t
cutil_List_locate(const cutil_List *list, const void *elem);

extern inline cutil_Bool
cutil_List_contains(const cutil_List *list, const void *elem);

int
cutil_List_memswap_elem(cutil_List *list, size_t idx, void *elem)
{
    const size_t size = cutil_List_get_elem_type(list)->size;
    /* I am slightly disgusted, but we should be fine since the list
     * implementations are never backed by actual read-only memory... */
    void *const p = (void *) cutil_List_get_ptr(list, idx);
    CUTIL_RETURN_VAL_IF_NULL(p, CUTIL_STATUS_FAILURE);
    cutil_void_memswap(p, elem, size);
    return CUTIL_STATUS_SUCCESS;
}

extern inline int
cutil_List_set(cutil_List *list, size_t idx, const void *elem);

extern inline int
cutil_List_append(cutil_List *list, const void *elem);

extern inline int
cutil_List_insert_mult(
  cutil_List *list, size_t pos, size_t num, const void *elems
);

extern inline int
cutil_List_insert(cutil_List *list, size_t pos, const void *elem);

extern inline int
cutil_List_remove_mult(cutil_List *list, size_t pos, size_t num);

extern inline int
cutil_List_remove(cutil_List *list, size_t pos);

extern inline int
cutil_List_remove_from_to(cutil_List *list, size_t begin, size_t end);

extern inline void
cutil_List_sort_custom(cutil_List *list, cutil_CompFunc *comp);

extern inline void
cutil_List_sort(cutil_List *list);

extern inline const cutil_GenericType *
cutil_List_get_elem_type(const cutil_List *list);

extern inline cutil_ConstIterator *
cutil_List_get_const_iterator(const cutil_List *list);

extern inline cutil_Iterator *
cutil_List_get_iterator(cutil_List *list);

extern inline void
cutil_List_clear_generic(void *obj);

extern inline void
cutil_List_copy_generic(void *dst, const void *src);

cutil_Bool
cutil_List_deep_equals(const cutil_List *lhs, const cutil_List *rhs)
{
    return cutil_List_deep_equals_generic(lhs, rhs);
}

int
cutil_List_compare(const cutil_List *lhs, const cutil_List *rhs)
{
    return cutil_List_compare_generic(lhs, rhs);
}

cutil_hash_t
cutil_List_hash(const cutil_List *list)
{
    return cutil_List_hash_generic(list);
}

size_t
cutil_List_to_string(const cutil_List *list, char *buf, size_t buflen)
{
    return cutil_List_to_string_generic(list, buf, buflen);
}

cutil_Bool
cutil_List_deep_equals_generic(const void *vlhs, const void *vrhs)
{
    const cutil_List *const lhs = vlhs;
    const cutil_List *const rhs = vrhs;
    if (lhs == rhs) {
        return CUTIL_TRUE;
    }
    if (lhs == NULL || rhs == NULL) {
        return CUTIL_FALSE;
    }
    if (!cutil_ListType_equals(lhs->vtable, rhs->vtable)) {
        return CUTIL_FALSE;
    }
    const cutil_GenericType *const lhs_type = cutil_List_get_elem_type(lhs);
    const cutil_GenericType *const rhs_type = cutil_List_get_elem_type(rhs);
    if (!cutil_GenericType_equals(lhs_type, rhs_type)) {
        return CUTIL_FALSE;
    }
    const size_t num = cutil_List_get_count(lhs);
    if (num != cutil_List_get_count(rhs)) {
        return CUTIL_FALSE;
    }
    for (size_t i = 0; i < num; ++i) {
        const void *const p_lhs = cutil_List_get_ptr(lhs, i);
        const void *const p_rhs = cutil_List_get_ptr(rhs, i);
        if (!cutil_GenericType_apply_deep_equals(lhs_type, p_lhs, p_rhs)) {
            return CUTIL_FALSE;
        }
    }
    return CUTIL_TRUE;
}

int
cutil_List_compare_generic(const void *vlhs, const void *vrhs)
{
    const cutil_List *const lhs = vlhs;
    const cutil_List *const rhs = vrhs;
    if (lhs == rhs) {
        return 0;
    }
    if (lhs == NULL || rhs == NULL) {
        return (rhs == NULL) - (lhs == NULL);
    }
    const cutil_GenericType *const lhs_type = cutil_List_get_elem_type(lhs);
    const cutil_GenericType *const rhs_type = cutil_List_get_elem_type(rhs);
    int cmp = cutil_compare_bytes(lhs_type, rhs_type, sizeof *lhs_type);
    if (cmp != 0) {
        return cmp;
    }
    const size_t lhs_num = cutil_List_get_count(lhs);
    const size_t rhs_num = cutil_List_get_count(rhs);
    if (lhs_num != rhs_num) {
        return cutil_compare_size_t(&lhs_num, &rhs_num);
    }
    for (size_t i = 0; i < lhs_num; ++i) {
        const void *const p_lhs = cutil_List_get_ptr(lhs, i);
        const void *const p_rhs = cutil_List_get_ptr(rhs, i);
        cmp = cutil_GenericType_apply_compare(lhs_type, p_lhs, p_rhs);
        if (cmp != 0) {
            return cmp;
        }
    }
    return 0;
}

cutil_hash_t
cutil_List_hash_generic(const void *vlist)
{
    const cutil_List *const list = vlist;
    if (list == NULL || cutil_List_get_count(list) == 0UL) {
        return CUTIL_HASH_C(0);
    }
    const cutil_GenericType *const type = cutil_List_get_elem_type(list);
    const size_t num = cutil_List_get_count(list);
    const void *const p0 = cutil_List_get_ptr(list, 0UL);
    cutil_hash_t h = cutil_GenericType_apply_hash(type, p0);
    for (size_t i = 1UL; i < num; ++i) {
        const void *const p = cutil_List_get_ptr(list, i);
        cutil_hash_combine_inplace(&h, cutil_GenericType_apply_hash(type, p));
    }
    return h;
}

static size_t
_list_elem_to_string(
  const void *elem, char *buf, size_t buflen, const void *ctx
)
{
    const cutil_GenericType *const type = ctx;
    return cutil_GenericType_apply_to_string(type, elem, buf, buflen);
}

size_t
cutil_List_to_string_generic(const void *vlist, char *buf, size_t buflen)
{
    const cutil_List *const list = vlist;
    cutil_ConstIterator *const it = cutil_List_get_const_iterator(list);
    const cutil_GenericType *const type = cutil_List_get_elem_type(list);
    const size_t res = cutil_ConstIterator_to_string(
      it, CUTIL_SEQ_DELIMS_ARRAY, &_list_elem_to_string, type, buf, buflen
    );
    cutil_ConstIterator_free(it);
    return res;
}

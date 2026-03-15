#include <cutil/data/generic/set.h>

#include <cutil/util/compare.h>

cutil_Bool
cutil_SetType_equals(
  const cutil_SetType *lhs, const cutil_SetType *rhs
)
{
    return (cutil_compare_bytes(lhs, rhs, sizeof(cutil_SetType)) == 0);
}

extern inline const cutil_SetType *
cutil_Set_get_vtable(const cutil_Set *set);

extern inline void
cutil_Set_free(cutil_Set *set);

extern inline void
cutil_Set_clear(cutil_Set *set);

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

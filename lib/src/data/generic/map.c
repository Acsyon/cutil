#include <cutil/data/generic/map.h>

#include <cutil/util/compare.h>

cutil_Bool
cutil_MapType_equals(
  const cutil_MapType *lhs, const cutil_MapType *rhs
)
{
    return (cutil_compare_bytes(lhs, rhs, sizeof(cutil_MapType)) == 0);
}

extern inline const cutil_MapType *
cutil_Map_get_vtable(const cutil_Map *map);

extern inline void
cutil_Map_free(cutil_Map *map);

extern inline void
cutil_Map_clear(cutil_Map *map);

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

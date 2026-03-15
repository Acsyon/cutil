#include <cutil/data/generic/set/hashset.h>

#include <cutil/data/generic/map/hashmap.h>
#include <cutil/data/generic/type.h>
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
_cutil_HashSet_clear(void *data)
{
    cutil_Map *const map = data;
    cutil_Map_clear(map);
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

static int
_cutil_HashSet_add(void *data, const void *elem)
{
    cutil_Map *const map = data;
    return cutil_Map_set(map, elem, &CUTIL_UNIT_VALUE);
}

static int
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

static const cutil_SetType CUTIL_SET_TYPE_HASHSET_OBJECT = {
  .name = "cutil_HashSet",
  .free = &_cutil_HashSet_free,
  .clear = &_cutil_HashSet_clear,
  .copy = &_cutil_HashSet_copy,
  .duplicate = &_cutil_HashSet_duplicate,
  .get_count = &_cutil_HashSet_get_count,
  .contains = &_cutil_HashSet_contains,
  .add = &_cutil_HashSet_add,
  .remove = &_cutil_HashSet_remove,
  .get_elem_type = &_cutil_HashSet_get_elem_type,
};

const cutil_SetType *const CUTIL_SET_TYPE_HASHSET
  = &CUTIL_SET_TYPE_HASHSET_OBJECT;

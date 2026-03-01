#include <cutil/data/generic/map/hashmap.h>

#include <cutil/data/generic/array.h>
#include <cutil/data/native/bitarray.h>
#include <cutil/status.h>
#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

#define CUTIL_MAP_INITIAL_CAPACITY 8
#define CUTIL_MAP_RESIZE_CAPACITY (2.0 / 3.0)
#define CUTIL_MAP_EXPAND_FACTOR ((size_t) 2)
#define CUTIL_MAP_ERROR_INDEX ((size_t) -1)
#define CUTIL_MAP_PERTURB_SHIFT 5

typedef struct _cutil_HashMap {
    const cutil_GenericType *key_type;
    const cutil_GenericType *val_type;
    size_t capacity;
    size_t num_entries;
    size_t num_tombstones;
    cutil_Array *keys;
    cutil_Array *vals;
    cutil_hash_t *hashes;
    cutil_BitArray *is_set;
    cutil_BitArray *tombstones;
} cutil_HashMap;

static void
_cutil_HashMap_alloc_arrays(cutil_HashMap *hashmap)
{
    hashmap->keys = cutil_Array_alloc(hashmap->key_type, hashmap->capacity);
    hashmap->vals = cutil_Array_alloc(hashmap->val_type, hashmap->capacity);
    hashmap->hashes = CUTIL_MALLOC_MULT(hashmap->hashes, hashmap->capacity);
    hashmap->is_set = cutil_BitArray_alloc(hashmap->capacity);
    hashmap->tombstones = cutil_BitArray_alloc(hashmap->capacity);
}

static void
_cutil_HashMap_free_arrays(cutil_HashMap *hashmap)
{
    cutil_Array_free(hashmap->keys);
    cutil_Array_free(hashmap->vals);
    free(hashmap->hashes);
    cutil_BitArray_free(hashmap->is_set);
    cutil_BitArray_free(hashmap->tombstones);
}

static size_t
_cutil_HashMap_locate_key(const cutil_HashMap *hashmap, const void *key)
{
    const cutil_GenericType *const key_type = hashmap->key_type;

    const size_t hash = cutil_GenericType_apply_hash(key_type, key);
    const size_t mask = hashmap->capacity - 1;

    size_t index = hash & mask;
    size_t perturb = hash;
    size_t i = 0;
    cutil_Bool is_set;

    while ((is_set = cutil_BitArray_get(hashmap->is_set, index))
           || cutil_BitArray_get(hashmap->tombstones, index))
    {
        if (is_set) {
            const void *const p = cutil_Array_get_ptr(hashmap->keys, index);
            if (cutil_GenericType_apply_compare(key_type, key, p) == 0) {
                return index;
            }
        }
        perturb >>= CUTIL_MAP_PERTURB_SHIFT;
        i = (i << 2) + i + perturb + 1;
        index = i & mask;
    }

    return CUTIL_MAP_ERROR_INDEX;
}

static int
_cutil_HashMap_set_entry_unchecked(
  cutil_HashMap *hashmap, size_t hash, const void *key, const void *val
);

static int
_cutil_HashMap_expand(cutil_HashMap *hashmap)
{
    const size_t new_capacity = hashmap->capacity * CUTIL_MAP_EXPAND_FACTOR;
    if (new_capacity < hashmap->capacity
        || new_capacity >= CUTIL_MAP_ERROR_INDEX)
    {
        return CUTIL_STATUS_FAILURE;
    }

    cutil_HashMap tmp_hashmap = *hashmap;

    hashmap->capacity = new_capacity;
    _cutil_HashMap_alloc_arrays(hashmap);

    for (size_t index = 0; index < tmp_hashmap.capacity; ++index) {
        if (cutil_BitArray_get(tmp_hashmap.is_set, index)) {
            const size_t hash = tmp_hashmap.hashes[index];
            const void *const key
              = cutil_Array_get_ptr(tmp_hashmap.keys, index);
            const void *const val
              = cutil_Array_get_ptr(tmp_hashmap.vals, index);
            _cutil_HashMap_set_entry_unchecked(hashmap, hash, key, val);
        }
    }

    _cutil_HashMap_free_arrays(&tmp_hashmap);

    return CUTIL_STATUS_SUCCESS;
}

static int
_cutil_HashMap_set_entry_aux(
  cutil_HashMap *hashmap,
  size_t hash,
  const void *key,
  const void *val,
  cutil_Bool check
)
{
    const cutil_GenericType *const key_type = hashmap->key_type;

    hash = cutil_GenericType_apply_hash(key_type, key);

    if (check) {
        const size_t num_total = hashmap->num_entries + hashmap->num_tombstones;
        if (num_total >= hashmap->capacity * CUTIL_MAP_RESIZE_CAPACITY) {
            const int status = _cutil_HashMap_expand(hashmap);
            if (status != CUTIL_STATUS_SUCCESS) {
                return status;
            }
        }
    }

    const size_t mask = hashmap->capacity - 1;

    size_t index = hash & mask;
    size_t perturb = hash;
    size_t i = 0;

    while (cutil_BitArray_get(hashmap->is_set, index)) {
        if (check && hashmap->hashes[index] == hash) {
            const void *const p = cutil_Array_get_ptr(hashmap->keys, index);
            if (cutil_GenericType_apply_compare(key_type, key, p) == 0) {
                cutil_Array_set(hashmap->vals, index, val);
                return CUTIL_STATUS_SUCCESS;
            }
        }
        perturb >>= CUTIL_MAP_PERTURB_SHIFT;
        i = (i << 2) + i + perturb + 1;
        index = i & mask;
    }

    ++hashmap->num_entries;
    cutil_Array_set(hashmap->keys, index, key);
    cutil_Array_set(hashmap->vals, index, val);
    hashmap->hashes[index] = (cutil_hash_t) hash;
    cutil_BitArray_set(hashmap->is_set, index);
    if (cutil_BitArray_get(hashmap->tombstones, index)) {
        --hashmap->num_tombstones;
        cutil_BitArray_unset(hashmap->tombstones, index);
    }

    return CUTIL_STATUS_SUCCESS;
}

static inline int
_cutil_HashMap_set_entry_unchecked(
  cutil_HashMap *hashmap, size_t hash, const void *key, const void *val
)
{
    return _cutil_HashMap_set_entry_aux(hashmap, hash, key, val, false);
}

static inline int
_cutil_HashMap_set_entry(
  cutil_HashMap *hashmap, const void *key, const void *val
)
{
    return _cutil_HashMap_set_entry_aux(hashmap, 0, key, val, true);
}

cutil_Map *
cutil_HashMap_alloc(
  const cutil_GenericType *key_type, const cutil_GenericType *val_type
)
{
    if (!cutil_GenericType_is_valid(key_type)) {
        cutil_log_warn("Key type is not valid");
        return NULL;
    }
    if (!cutil_GenericType_is_valid(val_type)) {
        cutil_log_warn("Value type is not valid");
        return NULL;
    }

    cutil_Map *const map = CUTIL_MALLOC_OBJECT(map);

    map->vtable = CUTIL_MAP_TYPE_HASHMAP;
    cutil_HashMap *const hashmap = map->data = CUTIL_MALLOC_OBJECT(hashmap);

    hashmap->key_type = key_type;
    hashmap->val_type = val_type;

    hashmap->capacity = CUTIL_MAP_INITIAL_CAPACITY;
    hashmap->num_entries = 0;
    hashmap->num_tombstones = 0;

    _cutil_HashMap_alloc_arrays(hashmap);

    return map;
}

static void
_cutil_HashMap_free(void *data)
{
    cutil_HashMap *const hashmap = data;
    CUTIL_RETURN_IF_NULL(hashmap);

    _cutil_HashMap_free_arrays(hashmap);

    free(hashmap);
}

static void
_cutil_HashMap_clear(void *data)
{
    cutil_HashMap *const hashmap = data;
    _cutil_HashMap_free_arrays(hashmap);

    hashmap->capacity = CUTIL_MAP_INITIAL_CAPACITY;
    hashmap->num_entries = 0;
    hashmap->num_tombstones = 0;

    _cutil_HashMap_alloc_arrays(hashmap);
}

static size_t
_cutil_HashMap_get_count(const void *data)
{
    const cutil_HashMap *const hashmap = data;
    return hashmap->num_entries;
}

static int
_cutil_HashMap_remove(void *data, const void *key)
{
    cutil_HashMap *const hashmap = data;
    const size_t idx = _cutil_HashMap_locate_key(hashmap, key);
    if (idx == CUTIL_MAP_ERROR_INDEX) {
        return CUTIL_STATUS_FAILURE;
    }
    --hashmap->num_entries;
    cutil_BitArray_unset(hashmap->is_set, idx);
    ++hashmap->num_tombstones;
    cutil_BitArray_set(hashmap->tombstones, idx);
    return CUTIL_STATUS_SUCCESS;
}

static cutil_Bool
_cutil_HashMap_contains(const void *data, const void *key)
{
    const cutil_HashMap *const hashmap = data;
    const size_t idx = _cutil_HashMap_locate_key(hashmap, key);
    return CUTIL_BOOLIFY(idx != CUTIL_MAP_ERROR_INDEX);
}

static int
_cutil_HashMap_get(const void *data, const void *key, void *val)
{
    const cutil_HashMap *const hashmap = data;
    const size_t idx = _cutil_HashMap_locate_key(hashmap, key);
    if (idx == CUTIL_MAP_ERROR_INDEX) {
        return CUTIL_STATUS_FAILURE;
    }
    cutil_Array_get(hashmap->vals, idx, val);
    return CUTIL_STATUS_SUCCESS;
}

static const void *
_cutil_HashMap_get_ptr(const void *data, const void *key)
{
    const cutil_HashMap *const hashmap = data;
    const size_t idx = _cutil_HashMap_locate_key(hashmap, key);
    if (idx == CUTIL_MAP_ERROR_INDEX) {
        return NULL;
    }
    return cutil_Array_get_ptr(hashmap->vals, idx);
}

static int
_cutil_HashMap_set(void *data, const void *key, const void *val)
{
    cutil_HashMap *const hashmap = data;
    const size_t idx = _cutil_HashMap_locate_key(hashmap, key);
    if (idx == CUTIL_MAP_ERROR_INDEX) {
        return _cutil_HashMap_set_entry(hashmap, key, val);
    }
    cutil_Array_set(hashmap->vals, idx, val);
    return CUTIL_STATUS_SUCCESS;
}

static void
_cutil_HashMap_copy(void *dst, const void *src)
{
    cutil_HashMap *const dst_hashmap = dst;
    const cutil_HashMap *const src_hashmap = src;

    _cutil_HashMap_clear(dst_hashmap);

    for (size_t i = 0; i < src_hashmap->capacity; ++i) {
        if (cutil_BitArray_get(src_hashmap->is_set, i)
            && !cutil_BitArray_get(src_hashmap->tombstones, i))
        {
            const void *const key = cutil_Array_get_ptr(src_hashmap->keys, i);
            const void *const val = cutil_Array_get_ptr(src_hashmap->vals, i);
            _cutil_HashMap_set_entry(dst_hashmap, key, val);
        }
    }
}

static void *
_cutil_HashMap_duplicate(const void *data)
{
    const cutil_HashMap *const src = data;

    cutil_Map *const new_map
      = cutil_HashMap_alloc(src->key_type, src->val_type);
    if (new_map == NULL) {
        return NULL;
    }

    cutil_HashMap *const dst_raw = new_map->data;
    _cutil_HashMap_copy(dst_raw, src);

    free(new_map);
    return dst_raw;
}

static const cutil_GenericType *
_cutil_HashMap_get_key_type(const void *data)
{
    const cutil_HashMap *const hashmap = data;
    return hashmap->key_type;
}

static const cutil_GenericType *
_cutil_HashMap_get_val_type(const void *data)
{
    const cutil_HashMap *const hashmap = data;
    return hashmap->val_type;
}

static const cutil_MapType CUTIL_MAP_TYPE_HASHMAP_OBJECT = {
  .name = "cutil_HashMap",
  .free = &_cutil_HashMap_free,
  .clear = &_cutil_HashMap_clear,
  .copy = &_cutil_HashMap_copy,
  .duplicate = &_cutil_HashMap_duplicate,
  .get_count = &_cutil_HashMap_get_count,
  .remove = &_cutil_HashMap_remove,
  .contains = &_cutil_HashMap_contains,
  .get = &_cutil_HashMap_get,
  .get_ptr = &_cutil_HashMap_get_ptr,
  .set = &_cutil_HashMap_set,
  .get_key_type = &_cutil_HashMap_get_key_type,
  .get_val_type = &_cutil_HashMap_get_val_type,
};

const cutil_MapType *const CUTIL_MAP_TYPE_HASHMAP
  = &CUTIL_MAP_TYPE_HASHMAP_OBJECT;

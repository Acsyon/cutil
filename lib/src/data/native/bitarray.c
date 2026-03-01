#include <cutil/data/native/bitarray.h>

#include <cutil/io/log.h>
#include <cutil/std/math.h>
#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

static inline size_t
_cutil_BitArray_convert_bits_to_capacity(size_t num)
{
    return (size_t) ceil(num * 1.0 / cutil_BitArray_ELEM_SIZE_IN_BITS);
}

cutil_BitArray *
cutil_BitArray_alloc(size_t num)
{
    cutil_BitArray *const arr = CUTIL_MALLOC_OBJECT(arr);

    const size_t capacity = _cutil_BitArray_convert_bits_to_capacity(num);
    arr->capacity = capacity;
    arr->data
      = (capacity > 0UL) ? CUTIL_CALLOC_MULT(arr->data, capacity) : NULL;

    return arr;
}

void
cutil_BitArray_free(cutil_BitArray *arr)
{
    CUTIL_RETURN_IF_NULL(arr);

    cutil_BitArray_clear(arr);

    free(arr);
}

void
cutil_BitArray_clear(cutil_BitArray *arr)
{
    free(arr->data);
    arr->data = NULL;
    arr->capacity = 0UL;
}

void
cutil_BitArray_copy(cutil_BitArray *dst, const cutil_BitArray *src)
{
#ifdef CUTIL_DEBUG
    if (dst->capacity != src->capacity) {
        cutil_log_debug("Copying unequal bit arrays");
    }
#endif /* CUTIL_DEBUG */
    const size_t capacity = CUTIL_MIN(dst->capacity, src->capacity);
    CUTIL_RETURN_IF_VAL(capacity, 0UL);
    memcpy(dst->data, src->data, capacity * sizeof *src->data);
}

cutil_BitArray *
cutil_BitArray_duplicate(const cutil_BitArray *arr)
{
    const size_t capacity = cutil_BitArray_get_capacity(arr);
    cutil_BitArray *const dup = cutil_BitArray_alloc(capacity);

    cutil_BitArray_copy(dup, arr);

    return dup;
}

void
cutil_BitArray_resize(cutil_BitArray *arr, size_t num)
{
    CUTIL_RETURN_IF_VAL(arr->capacity, num);

    if (num == 0UL) {
        cutil_BitArray_clear(arr);
        return;
    }
    const size_t old_capacity = arr->capacity;
    const size_t capacity = _cutil_BitArray_convert_bits_to_capacity(num);
    if (old_capacity == capacity) {
        return;
    }
    arr->capacity = capacity;
    if (arr->data == NULL) {
        arr->data = CUTIL_CALLOC_MULT(arr->data, capacity);
    } else {
        arr->data = CUTIL_REALLOC_MULT(arr->data, capacity);
        if (old_capacity < capacity) {
            memset(&arr->data[old_capacity], 0, capacity - old_capacity);
        }
    }
}

extern inline size_t
cutil_BitArray_get_capacity(const cutil_BitArray *arr);

extern inline unsigned char
cutil_BitArray_get(const cutil_BitArray *arr, size_t idx);

extern inline void
cutil_BitArray_set(cutil_BitArray *arr, size_t idx);

extern inline void
cutil_BitArray_unset(cutil_BitArray *arr, size_t idx);

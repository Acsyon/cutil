#include <cutil/core/util/void.h>

extern inline const void *
cutil_void_array_get_elem_const(size_t size, const void *data, size_t idx);

extern inline void *
cutil_void_array_get_elem(size_t size, void *data, size_t idx);

extern inline void
cutil_void_memswap(void *a, void *b, size_t size);

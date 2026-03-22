#include <cutil/data/generic/iterator.h>

extern inline void
cutil_ConstIterator_free(cutil_ConstIterator *it);

extern inline void
cutil_ConstIterator_rewind(cutil_ConstIterator *it);

extern inline cutil_Bool
cutil_ConstIterator_next(cutil_ConstIterator *it);

extern inline cutil_Status
cutil_ConstIterator_get(const cutil_ConstIterator *it, void *out);

extern inline const void *
cutil_ConstIterator_get_ptr(const cutil_ConstIterator *it);

extern inline void
cutil_Iterator_free(cutil_Iterator *it);

extern inline void
cutil_Iterator_rewind(cutil_Iterator *it);

extern inline cutil_Bool
cutil_Iterator_next(cutil_Iterator *it);

extern inline cutil_Status
cutil_Iterator_get(const cutil_Iterator *it, void *out);

extern inline const void *
cutil_Iterator_get_ptr(const cutil_Iterator *it);

extern inline cutil_Status
cutil_Iterator_set(cutil_Iterator *it, const void *val);

extern inline cutil_Status
cutil_Iterator_remove(cutil_Iterator *it);

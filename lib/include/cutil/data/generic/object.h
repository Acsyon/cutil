/** cutil/data/generic/object.h
 *
 * Lightweight value wrapper pairing a cutil_GenericType descriptor with a
 * heap-allocated data pointer. Owns exactly one type-erased value.
 */

#ifndef CUTIL_DATA_GENERIC_OBJECT_H_INCLUDED
#define CUTIL_DATA_GENERIC_OBJECT_H_INCLUDED

#include <cutil/data/generic/type.h>
#include <cutil/std/stdbool.h>
#include <cutil/util/hash.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Generic object wrapper. Owns a single value described by a GenericType.
 */
typedef struct {
    const cutil_GenericType *type; /**< Type descriptor for the wrapped value */
    void *data;                    /**< Pointer to heap-allocated value data */
} cutil_GenericObject;

/**
 * Allocates an empty GenericObject (type=NULL, data=NULL).
 *
 * @return newly allocated empty GenericObject
 */
cutil_GenericObject *
cutil_GenericObject_alloc(void);

/**
 * Allocates a GenericObject and deep-copies `data` via the type descriptor.
 * If `data` is NULL, the value is initialized via the type's init callback.
 *
 * @param[in] type type descriptor (must be valid)
 * @param[in] data pointer to source data, or NULL for default init
 *
 * @return newly allocated GenericObject, or NULL if type is invalid
 */
cutil_GenericObject *
cutil_GenericObject_create(const cutil_GenericType *type, const void *data);

/**
 * Clears internal data and frees the GenericObject struct.
 *
 * @param[in] obj GenericObject to free, or NULL (no-op)
 */
void
cutil_GenericObject_free(cutil_GenericObject *obj);

/**
 * Releases the internal data buffer and resets type and data to NULL.
 *
 * @param[in] obj GenericObject to clear, or NULL (no-op)
 */
void
cutil_GenericObject_clear(cutil_GenericObject *obj);

/**
 * Deep-copies src into dst. Clears dst first. If src is empty, dst becomes
 * empty.
 *
 * @param[in,out] dst destination GenericObject
 * @param[in] src source GenericObject
 */
void
cutil_GenericObject_copy(
  cutil_GenericObject *dst, const cutil_GenericObject *src
);

/**
 * Returns the type descriptor of the wrapped value.
 *
 * @param[in] obj GenericObject to query
 *
 * @return type descriptor, or NULL if empty
 */
const cutil_GenericType *
cutil_GenericObject_get_type(const cutil_GenericObject *obj);

/**
 * Returns a read-only pointer to the wrapped data.
 *
 * @param[in] obj GenericObject to query
 *
 * @return pointer to data, or NULL if empty
 */
const void *
cutil_GenericObject_get_data(const cutil_GenericObject *obj);

/**
 * Generic (void-argument) version of cutil_GenericObject_clear.
 *
 * @param[in] obj GenericObject to clear
 */
inline void
cutil_GenericObject_clear_generic(void *obj)
{
    cutil_GenericObject *const go = (cutil_GenericObject *) obj;
    cutil_GenericObject_clear(go);
}

/**
 * Generic (void-argument) version of cutil_GenericObject_copy.
 *
 * @param[in, out] dst GenericObject to copy into
 * @param[in] src GenericObject to copy from
 */
inline void
cutil_GenericObject_copy_generic(void *dst, const void *src)
{
    cutil_GenericObject *const dst_obj = (cutil_GenericObject *) dst;
    const cutil_GenericObject *const src_obj
      = (const cutil_GenericObject *) src;
    cutil_GenericObject_copy(dst_obj, src_obj);
}

/**
 * Generic (void-argument) deep equality comparison.
 *
 * @param[in] lhs left-hand side
 * @param[in] rhs right-hand side
 *
 * @return true if both wrap the same type and equal data
 */
cutil_Bool
cutil_GenericObject_deep_equals_generic(const void *lhs, const void *rhs);

/**
 * Generic (void-argument) three-way comparison. Orders by type name first,
 * then by inner value.
 *
 * @param[in] lhs left-hand side
 * @param[in] rhs right-hand side
 *
 * @return strcmp-style result
 */
int
cutil_GenericObject_compare_generic(const void *lhs, const void *rhs);

/**
 * Generic (void-argument) hash function.
 *
 * @param[in] obj GenericObject to hash
 *
 * @return hash value
 */
cutil_hash_t
cutil_GenericObject_hash_generic(const void *obj);

/**
 * Generic (void-argument) serialization.
 *
 * @param[in] obj GenericObject to serialize
 * @param[out] buf destination buffer
 * @param[in] buflen buffer size in bytes
 *
 * @return number of characters written (excl. NUL)
 */
size_t
cutil_GenericObject_to_string_generic(
  const void *obj, char *buf, size_t buflen
);

/**
 * Generic type descriptor for cutil_GenericObject.
 */
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_GENERIC_OBJECT;

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_DATA_GENERIC_OBJECT_H_INCLUDED */

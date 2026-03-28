/** cutil/string/type.h
 *
 * Typed string wrappers: cutil_String (owning) and cutil_StringView
 * (non-owning)
 *
 */

#ifndef CUTIL_STRING_TYPE_H_INCLUDED
#define CUTIL_STRING_TYPE_H_INCLUDED

#include <cutil/data/generic/type.h>
#include <cutil/std/stdbool.h>
#include <cutil/std/stddef.h>
#include <cutil/util/hash.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Owning string wrapper. Holds a heap-allocated copy of the source string.
 */
typedef struct {
    char *str;     /**< Pointer to the heap-allocated string */
    size_t length; /**< Length of the string, excluding the NUL terminator */
} cutil_String;

/**
 * Creates a cutil_String by duplicating `str`. If `str` is NULL, the returned
 * string has str == NULL and length == 0.
 *
 * @param[in] str NUL-terminated string to duplicate, or NULL
 *
 * @return owning cutil_String
 */
cutil_String *
cutil_String_from_string(const char *str);

/**
 * Creates a cutil_String by duplicating at most `maxlen` bytes from `str`.
 * If `str` is NULL, the returned string has str == NULL and length == 0.
 *
 * @param[in] str NUL-terminated string to duplicate, or NULL
 * @param[in] maxlen maximum number of bytes to duplicate
 *
 * @return owning cutil_String
 */
cutil_String *
cutil_String_from_nstring(const char *str, size_t maxlen);

/**
 * Frees memory pointed to by `s`.
 *
 * @param[in] s cutil_String object to be freed
 */
void
cutil_String_free(cutil_String *s);

/**
 * Frees the held string and resets to NULL/0.
 *
 * @param[in] s cutil_String to clear
 */
void
cutil_String_clear(cutil_String *s);

/**
 * Copies `src` into `dst`, freeing any existing string in `dst`.
 *
 * @param[in, out] dst cutil_String to copy into
 * @param[in] src cutil_String to copy from
 */
void
cutil_String_copy(cutil_String *dst, const cutil_String *src);

/**
 * Returns whether `lhs` and `rhs` contain the same string content.
 *
 * @param[in] lhs left-hand side
 * @param[in] rhs right-hand side
 *
 * @return are `lhs` and `rhs` deeply equal?
 */
cutil_Bool
cutil_String_deep_equals(const cutil_String *lhs, const cutil_String *rhs);

/**
 * Three-way comparison. NULL sorts before non-NULL.
 *
 * @param[in] lhs left-hand side
 * @param[in] rhs right-hand side
 *
 * @return strcmp-style result
 */
int
cutil_String_compare(const cutil_String *lhs, const cutil_String *rhs);

/**
 * Computes hash. Returns CUTIL_HASH_C(0) for NULL or empty string.
 *
 * @param[in] s cutil_String to hash
 *
 * @return hash value
 */
cutil_hash_t
cutil_String_hash(const cutil_String *s);

/**
 * Serializes the string into `buf`. Writes "NULL" if `s` is NULL or its
 * internal pointer is NULL.
 *
 * @param[in] s cutil_String to serialize, or NULL
 * @param[out] buf destination buffer
 * @param[in] buflen buffer size in bytes
 *
 * @return number of characters written (excl. NUL), or 0 if buffer too small
 */
size_t
cutil_String_to_string(const cutil_String *s, char *buf, size_t buflen);

/**
 * Generic (void-argument) version of cutil_String_clear.
 *
 * @param[in] obj cutil_String to clear
 */
inline void
cutil_String_clear_generic(void *obj)
{
    cutil_String *const str = (cutil_String *) obj;
    cutil_String_clear(str);
}

/**
 * Generic (void-argument) version of cutil_String_copy.
 *
 * @param[in] dst cutil_String to copy into
 * @param[in] src cutil_String to copy from
 */
inline void
cutil_String_copy_generic(void *dst, const void *src)
{
    cutil_String *const dst_str = (cutil_String *) dst;
    const cutil_String *const src_str = (const cutil_String *) src;
    cutil_String_copy(dst_str, src_str);
}

/**
 * Generic (void-argument) version of cutil_String_deep_equals.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return Are `lhs` and `rhs` deeply equal?
 */
cutil_Bool
cutil_String_deep_equals_generic(const void *lhs, const void *rhs);

/**
 * Generic (void-argument) version of cutil_String_compare.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return strcmp-style result
 */
int
cutil_String_compare_generic(const void *lhs, const void *rhs);

/**
 * Generic (void-argument) version of cutil_String_hash.
 *
 * @param[in] s cutil_String to hash
 *
 * @return hash value
 */
cutil_hash_t
cutil_String_hash_generic(const void *s);

/**
 * Generic (void-argument) version of cutil_String_to_string.
 *
 * @param[in] s cutil_String to serialize, or NULL
 * @param[out] buf destination buffer
 * @param[in] buflen buffer size in bytes
 *
 * @return number of characters written (excl. NUL), or 0 if buffer too small
 */
size_t
cutil_String_to_string_generic(const void *s, char *buf, size_t buflen);

/**
 * Generic type descriptor for cutil_String.
 */
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_STRING;

/**
 * Non-owning string view. Holds a pointer into an externally owned string.
 */
typedef struct {
    const char *str; /**< Pointer to the externally owned string */
    size_t length;   /**< Length of the string, excluding the NUL terminator */
} cutil_StringView;

/**
 * Creates a cutil_StringView referencing `str`. If `str` is NULL, the returned
 * view has str == NULL and length == 0.
 *
 * @param[in] str NUL-terminated string to reference, or NULL
 *
 * @return non-owning cutil_StringView
 */
cutil_StringView *
cutil_StringView_from_string(const char *str);

/**
 * Creates a cutil_StringView referencing `str` with at most `maxlen`
 * visible characters.
 *
 * @param[in] str NUL-terminated string to reference, or NULL
 * @param[in] maxlen maximum number of visible characters
 *
 * @return non-owning cutil_StringView
 */
cutil_StringView *
cutil_StringView_from_nstring(const char *str, size_t maxlen);

/**
 * Frees memory pointed to by `sv`.
 *
 * @param[in] sv cutil_StringView object to be freed
 */
void
cutil_StringView_free(cutil_StringView *sv);

/**
 * Resets the view to NULL/0. Does not free memory (non-owning).
 *
 * @param[in] sv cutil_StringView to clear
 */
void
cutil_StringView_clear(cutil_StringView *sv);

/**
 * Shallow-copies `src` into `dst`.
 *
 * @param[in, out] dst cutil_StringView to copy into
 * @param[in] src cutil_StringView to copy from
 */
void
cutil_StringView_copy(cutil_StringView *dst, const cutil_StringView *src);

/**
 * Returns whether `lhs` and `rhs` reference equal string content.
 *
 * @param[in] lhs left-hand side
 * @param[in] rhs right-hand side
 *
 * @return are `lhs` and `rhs` deeply equal?
 */
cutil_Bool
cutil_StringView_deep_equals(
  const cutil_StringView *lhs, const cutil_StringView *rhs
);

/**
 * Three-way comparison. NULL sorts before non-NULL.
 *
 * @param[in] lhs left-hand side
 * @param[in] rhs right-hand side
 *
 * @return strcmp-style result
 */
int
cutil_StringView_compare(
  const cutil_StringView *lhs, const cutil_StringView *rhs
);

/**
 * Computes hash. Returns CUTIL_HASH_C(0) for NULL or empty string.
 *
 * @param[in] sv cutil_StringView to hash
 *
 * @return hash value
 */
cutil_hash_t
cutil_StringView_hash(const cutil_StringView *sv);

/**
 * Serializes the view into `buf` using `%.*s` to respect the view length.
 * Writes "NULL" if the internal pointer is NULL.
 *
 * @param[in] sv cutil_StringView to serialize, or NULL
 * @param[out] buf destination buffer
 * @param[in] buflen buffer size in bytes
 *
 * @return number of characters written (excl. NUL), or 0 if buffer too small
 */
size_t
cutil_StringView_to_string(
  const cutil_StringView *sv, char *buf, size_t buflen
);

/**
 * Generic (void-argument) version of cutil_StringView_clear.
 *
 * @param[in] obj cutil_StringView to clear
 */
inline void
cutil_StringView_clear_generic(void *obj)
{
    cutil_StringView *const sv = (cutil_StringView *) obj;
    cutil_StringView_clear(sv);
}

/**
 * Generic (void-argument) version of cutil_StringView_copy.
 *
 * @param[in] dst cutil_StringView to copy into
 * @param[in] src cutil_StringView to copy from
 */
inline void
cutil_StringView_copy_generic(void *dst, const void *src)
{
    cutil_StringView *const sv_dst = (cutil_StringView *) dst;
    const cutil_StringView *const sv_src = (const cutil_StringView *) src;
    cutil_StringView_copy(sv_dst, sv_src);
}

/**
 * Generic (void-argument) version of cutil_StringView_deep_equals.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return Are `lhs` and `rhs` deeply equal?
 */
cutil_Bool
cutil_StringView_deep_equals_generic(const void *lhs, const void *rhs);

/**
 * Generic (void-argument) version of cutil_StringView_compare.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 *
 * @return strcmp-style result
 */
int
cutil_StringView_compare_generic(const void *lhs, const void *rhs);

/**
 * Generic (void-argument) version of cutil_StringView_hash.
 *
 * @param[in] sv cutil_StringView to hash
 *
 * @return hash value
 */
cutil_hash_t
cutil_StringView_hash_generic(const void *sv);

/**
 * Generic (void-argument) version of cutil_StringView_to_string.
 *
 * @param[in] sv cutil_StringView to serialize, or NULL
 * @param[out] buf destination buffer
 * @param[in] buflen buffer size in bytes
 *
 * @return number of characters written (excl. NUL), or 0 if buffer too small
 */
size_t
cutil_StringView_to_string_generic(const void *sv, char *buf, size_t buflen);

/**
 * Generic type descriptor for cutil_StringView.
 */
extern const cutil_GenericType *const CUTIL_GENERIC_TYPE_STRING_VIEW;

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_STRING_TYPE_H_INCLUDED */

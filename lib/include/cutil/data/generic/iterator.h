/** cutil/data/generic/iterator.h
 *
 * Header for generic iterators.
 *
 */

#ifndef CUTIL_DATA_GENERIC_ITERATOR_H_INCLUDED
#define CUTIL_DATA_GENERIC_ITERATOR_H_INCLUDED

#include <cutil/debug/null.h>
#include <cutil/status.h>
#include <cutil/std/stdbool.h>
#include <cutil/std/stdlib.h>
#include <cutil/util/macro.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Type (vtable) for read-only cutil iterators.
 */
typedef struct {
    const char *const name;
    void (*const free)(void *data);
    void (*const rewind)(void *data);
    cutil_Bool (*const next)(void *data);
    int (*const get)(const void *data, void *out);
    const void *(*const get_ptr)(const void *data);
} cutil_ConstIteratorType;

/**
 * Abstract "class" for read-only cutil iterators.
 */
typedef struct {
    const cutil_ConstIteratorType *vtable;
    void *data;
} cutil_ConstIterator;

/**
 * Convenience MACRO to check for NULLs in `IT` in debug mode.
 *
 * @param[in] IT cutil_ConstIterator to check for NULLs
 */
#define CUTIL_NULL_CHECKS_CONST_ITERATOR(IT)                                   \
    do {                                                                       \
        CUTIL_NULL_CHECK(IT);                                                  \
        CUTIL_NULL_CHECK(IT->vtable);                                          \
    } while (0)

/**
 * Destructor for 'cutil_ConstIterator'.
 *
 * @param[in] it cutil_ConstIterator object to be destroyed
 */
inline void
cutil_ConstIterator_free(cutil_ConstIterator *it)
{
    CUTIL_RETURN_IF_NULL(it);
    CUTIL_NULL_CHECK(it->vtable->free);
    it->vtable->free(it->data);
    free(it);
}

/**
 * Rewinds `it` to the position before the first element, allowing
 * full re-traversal. Idempotent: calling rewind on a fresh or already-
 * rewound iterator is a no-op with no side effects.
 *
 * Invoking rewind on a NULL iterator or on an iterator whose vtable
 * rewind slot is NULL is a safe no-op.
 *
 * @param[in] it cutil_ConstIterator to rewind
 */
inline void
cutil_ConstIterator_rewind(cutil_ConstIterator *it)
{
    CUTIL_RETURN_IF_NULL(it);
    CUTIL_NULL_CHECKS_CONST_ITERATOR(it);
    CUTIL_RETURN_IF_NULL(it->vtable->rewind);
    it->vtable->rewind(it->data);
}

/**
 * Advances `it` to the next element.
 *
 * @param[in] it cutil_ConstIterator to advance
 *
 * @return CUTIL_TRUE if a next element exists, CUTIL_FALSE when
 *         exhausted. Once exhausted, further calls return CUTIL_FALSE
 *         without modifying iterator state.
 */
inline cutil_Bool
cutil_ConstIterator_next(cutil_ConstIterator *it)
{
    CUTIL_RETURN_VAL_IF_NULL(it, false);
    CUTIL_NULL_CHECKS_CONST_ITERATOR(it);
    CUTIL_RETURN_VAL_IF_NULL(it->vtable->next, false);
    return it->vtable->next(it->data);
}

/**
 * Copies the current element of `it` into `out`.
 *
 * @param[in] it cutil_ConstIterator to get element from
 * @param[out] out buffer to copy element into
 *
 * @return error code
 */
inline int
cutil_ConstIterator_get(const cutil_ConstIterator *it, void *out)
{
    CUTIL_RETURN_VAL_IF_NULL(it, CUTIL_STATUS_FAILURE);
    CUTIL_NULL_CHECKS_CONST_ITERATOR(it);
    CUTIL_RETURN_VAL_IF_NULL(it->vtable->get, CUTIL_STATUS_FAILURE);
    return it->vtable->get(it->data, out);
}

/**
 * Returns a read-only pointer to the current element of `it`.
 *
 * @param[in] it cutil_ConstIterator to get element pointer from
 *
 * @return read-only pointer to current element, or NULL if unavailable
 */
inline const void *
cutil_ConstIterator_get_ptr(const cutil_ConstIterator *it)
{
    CUTIL_RETURN_NULL_IF_NULL(it);
    CUTIL_NULL_CHECKS_CONST_ITERATOR(it);
    CUTIL_RETURN_NULL_IF_NULL(it->vtable->get_ptr);
    return it->vtable->get_ptr(it->data);
}

/**
 * Type (vtable) for read-write cutil iterators.
 */
typedef struct {
    const char *const name;
    void (*const free)(void *data);
    void (*const rewind)(void *data);
    cutil_Bool (*const next)(void *data);
    int (*const get)(const void *data, void *out);
    const void *(*const get_ptr)(const void *data);
    int (*const set)(void *data, const void *val);
    int (*const remove)(void *data);
} cutil_IteratorType;

/**
 * Abstract "class" for read-write cutil iterators.
 */
typedef struct {
    const cutil_IteratorType *vtable;
    void *data;
} cutil_Iterator;

/**
 * Convenience MACRO to check for NULLs in `IT` in debug mode.
 *
 * @param[in] IT cutil_Iterator to check for NULLs
 */
#define CUTIL_NULL_CHECKS_ITERATOR(IT)                                         \
    do {                                                                       \
        CUTIL_NULL_CHECK(IT);                                                  \
        CUTIL_NULL_CHECK(IT->vtable);                                          \
    } while (0)

/**
 * Destructor for 'cutil_Iterator'.
 *
 * @param[in] it cutil_Iterator object to be destroyed
 */
inline void
cutil_Iterator_free(cutil_Iterator *it)
{
    CUTIL_RETURN_IF_NULL(it);
    CUTIL_NULL_CHECK(it->vtable->free);
    it->vtable->free(it->data);
    free(it);
}

/**
 * Rewinds `it` to the position before the first element. See
 * cutil_ConstIterator_rewind for full semantics.
 *
 * @param[in] it cutil_Iterator to rewind
 */
inline void
cutil_Iterator_rewind(cutil_Iterator *it)
{
    CUTIL_RETURN_IF_NULL(it);
    CUTIL_NULL_CHECKS_ITERATOR(it);
    CUTIL_RETURN_IF_NULL(it->vtable->rewind);
    it->vtable->rewind(it->data);
}

/**
 * Advances `it` to the next element.
 *
 * @param[in] it cutil_Iterator to advance
 *
 * @return CUTIL_TRUE if a next element exists, CUTIL_FALSE when
 *         exhausted. Once exhausted, further calls return CUTIL_FALSE
 *         without modifying iterator state.
 */
inline cutil_Bool
cutil_Iterator_next(cutil_Iterator *it)
{
    CUTIL_RETURN_VAL_IF_NULL(it, false);
    CUTIL_NULL_CHECKS_ITERATOR(it);
    CUTIL_RETURN_VAL_IF_NULL(it->vtable->next, false);
    return it->vtable->next(it->data);
}

/**
 * Copies the current element of `it` into `out`.
 *
 * @param[in] it cutil_Iterator to get element from
 * @param[out] out buffer to copy element into
 *
 * @return error code
 */
inline int
cutil_Iterator_get(const cutil_Iterator *it, void *out)
{
    CUTIL_RETURN_VAL_IF_NULL(it, CUTIL_STATUS_FAILURE);
    CUTIL_NULL_CHECKS_ITERATOR(it);
    CUTIL_RETURN_VAL_IF_NULL(it->vtable->get, CUTIL_STATUS_FAILURE);
    return it->vtable->get(it->data, out);
}

/**
 * Returns a read-only pointer to the current element of `it`.
 *
 * @param[in] it cutil_Iterator to get element pointer from
 *
 * @return read-only pointer to current element, or NULL if unavailable
 */
inline const void *
cutil_Iterator_get_ptr(const cutil_Iterator *it)
{
    CUTIL_RETURN_NULL_IF_NULL(it);
    CUTIL_NULL_CHECKS_ITERATOR(it);
    CUTIL_RETURN_NULL_IF_NULL(it->vtable->get_ptr);
    return it->vtable->get_ptr(it->data);
}

/**
 * Overwrites the current element of `it` with `val`.
 *
 * @param[in] it cutil_Iterator to modify
 * @param[in] val value to write to current element
 *
 * @return error code
 */
inline int
cutil_Iterator_set(cutil_Iterator *it, const void *val)
{
    CUTIL_RETURN_VAL_IF_NULL(it, CUTIL_STATUS_FAILURE);
    CUTIL_NULL_CHECKS_ITERATOR(it);
    CUTIL_RETURN_VAL_IF_NULL(it->vtable->set, CUTIL_STATUS_FAILURE);
    return it->vtable->set(it->data, val);
}

/**
 * Removes the current element of `it` from the backing container.
 *
 * @param[in] it cutil_Iterator to remove element from
 *
 * @return error code
 */
inline int
cutil_Iterator_remove(cutil_Iterator *it)
{
    CUTIL_RETURN_VAL_IF_NULL(it, CUTIL_STATUS_FAILURE);
    CUTIL_NULL_CHECKS_ITERATOR(it);
    CUTIL_RETURN_VAL_IF_NULL(it->vtable->remove, CUTIL_STATUS_FAILURE);
    return it->vtable->remove(it->data);
}

#undef CUTIL_NULL_CHECKS_CONST_ITERATOR
#undef CUTIL_NULL_CHECKS_ITERATOR

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_DATA_GENERIC_ITERATOR_H_INCLUDED */

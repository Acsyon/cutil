/** cutil/debug/null.h
 *
 * Header for debug handling of null pointers.
 */

#ifndef CUTIL_DEBUG_NULL_H_INCLUDED
#define CUTIL_DEBUG_NULL_H_INCLUDED

#include <cutil/std/stdlib.h>

#ifdef CUTIL_ENABLE_NULL_CHECKS
    #include <cutil/io/log.h>

    /**
     * MACRO for checking if a pointer argument is NULL but shouldn't be. If so,
     * variable name, file name and line are logged.
     *
     * @param[in] VAR variable to be checked
     */
    #define CUTIL_NULL_CHECK(VAR)                                              \
        do {                                                                   \
            if ((VAR) == NULL) {                                               \
                cutil_log_error(                                               \
                  "'%s' cannot be NULL (%s:%i)", CUTIL_STRINGIFY_VAL(VAR),     \
                  __FILE__, __LINE__                                           \
                );                                                             \
            }                                                                  \
        } while (0)

    /**
     * MACRO for checking if a function pointerin a vtable is NULL. If so, the
     * function and type names are logged.
     *
     * @param[in] VTABLE vtable to be checked
     * @param[in] FNC function (pointer) name to be checked
     */
    #define CUTIL_NULL_CHECK_VTABLE(VTABLE, FNC)                               \
        do {                                                                   \
            if ((VTABLE->FNC) == NULL) {                                       \
                cutil_log_warn(                                                \
                  "Function '%s' not supported for '%s'",                      \
                  CUTIL_STRINGIFY_VAL(FNC), (VTABLE->name)                     \
                );                                                             \
            }                                                                  \
        } while (0)
#else
    #define CUTIL_NULL_CHECK(VAR) (void) (VAR)
    #define CUTIL_NULL_CHECK_VTABLE(VTABLE, FNC) (void) (VTABLE->FNC)
#endif

#endif /* CUTIL_DEBUG_NULL_H_INCLUDED */

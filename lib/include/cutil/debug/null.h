/* cutil/debug/null.h
 *
 * Header for debug handling of null pointers
 *
 */

#ifndef CUTIL_DEBUG_NULL_H_INCLUDED
#define CUTIL_DEBUG_NULL_H_INCLUDED

#include <cutil/std/stdlib.h>

/**
 * MACRO for checking if a pointer argument is NULL but shouldn't be. If so,
 * variable name, file name and line are logged.
 *
 * @param[in] VAR variable to be checked
 */
#ifdef CUTIL_ENABLE_NULL_CHECKS
    #include <cutil/io/log.h>

    #define CUTIL_NULL_CHECK(VAR)                                              \
        do {                                                                   \
            if ((VAR) == NULL) {                                               \
                cutil_log_error(                                               \
                  "'%s' cannot be NULL (%s:%i)", __FILE__, __LINE__            \
                );                                                             \
            }                                                                  \
        } while (0)
#else
    #define CUTIL_NULL_CHECK(VAR) (void) (VAR)
#endif

#endif /* CUTIL_DEBUG_NULL_H_INCLUDED */

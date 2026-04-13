/** cutil/status.h
 *
 * Header for return-value status codes.
 */

#ifndef CUTIL_STATUS_H_INCLUDED
#define CUTIL_STATUS_H_INCLUDED

/**
 * Convenience MACRO for functions that return a size_t value. Should be used
 * when a function needs to return a size_t value but also needs to be able to
 * indicate an error.
 */
#define CUTIL_ERROR_SIZE ((size_t) -1)

/**
 * Convenience MACRO for functions that return a size_t index. Should be used
 * when a function needs to return a size_t index but also needs to be able to
 * indicate an error.
 */
#define CUTIL_ERROR_INDEX ((size_t) -1)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumerator for potential cutil return-value status codes
 */
typedef enum {
    CUTIL_STATUS_FAILURE = -1, /**< Generic failure */
    CUTIL_STATUS_SUCCESS = 0,  /**< Success */
} cutil_Status;

/**
 * Returns a string describing `status`.
 *
 * @param[in] status cutil_Status value to get string of
 *
 * @return string describing `status`
 */
const char *
cutil_strerror(cutil_Status status);

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_STATUS_H_INCLUDED */

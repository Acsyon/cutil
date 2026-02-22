/* cutil/status.h
 *
 * Header for return-value status codes
 *
 */

#ifndef CUTIL_STATUS_H_INCLUDED
#define CUTIL_STATUS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumerator for potential cutil return-value status codes
 */
enum {
    CUTIL_STATUS_FAILURE = -1, /**< Generic failure */
    CUTIL_STATUS_SUCCESS = 0,  /**< Success */
};

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_STATUS_H_INCLUDED */

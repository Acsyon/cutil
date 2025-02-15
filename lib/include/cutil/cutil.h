/* cutil/cutil.h
 *
 * Header for general stuff like wrappers around C language features (e.g., the
 * 'restrict' keyword)
 *
 */

#ifndef CUTIL_CUTIL_H_INCLUDED
#define CUTIL_CUTIL_H_INCLUDED

#ifndef __cplusplus
    #if __STDC_VERSION__ >= 199901L
        #define CUTIL_RESTRICT restrict
    #else
        #define CUTIL_RESTRICT
    #endif
#else
    #define CUTIL_RESTRICT
#endif

#endif /* CUTIL_CUTIL_H_INCLUDED */

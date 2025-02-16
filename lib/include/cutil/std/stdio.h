/* cutil/std/stdio.h
 *
 * Wrapper around standard "stdio.h" header
 *
 */

#ifndef CUTIL_STD_STDIO_H_INCLUDED
#define CUTIL_STD_STDIO_H_INCLUDED

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Safely close `in` (ignores NULL and standard streams).
 *
 * @param[in] in FILE stream to be closed
 *
 * @return return value of fclose (success indicator)
 */
int
cutil_sfclose(FILE *in);

/**
 * Returns the size in bytes of FILE stream `in`.
 * 
 * @param[in] in FILE stream to return the size of
 *
 * @return the size in bytes of FILE stream `in`
 */
long
cutil_get_filesize(FILE *in);

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_STD_STDIO_H_INCLUDED */

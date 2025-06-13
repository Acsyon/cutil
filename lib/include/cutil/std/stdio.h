/* cutil/std/stdio.h
 *
 * Wrapper around standard "stdio.h" header
 *
 */

#ifndef CUTIL_STD_STDIO_H_INCLUDED
#define CUTIL_STD_STDIO_H_INCLUDED

#include <stdio.h>

#include <cutil/cutil.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Debug 'fopen': Checks if file could be opened, if not print error message
 * containing `filename`, `modes`, `file` and `line`.
 *
 * @param[in] fname file to be opened
 * @param[in] modes file opening modes (like fopen)
 * @param[in] file file name of calling function
 * @param[in] line line number of calling function
 *
 * @return stream to opened file
 */
FILE *
cutil_dfopen(
  const char *CUTIL_RESTRICT fname,
  const char *CUTIL_RESTRICT modes,
  const char *CUTIL_RESTRICT file,
  int line
);

#ifdef CUTIL_ENABLE_DEBUG_FOPEN
    #define fopen(FNAME, MODES)                                                \
        cutil_dfopen((FNAME), (MODES), __FILE__, __LINE__)
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

#ifdef CUTIL_ENABLE_SAFE_FCLOSE
    #define fclose(STREAM) cutil_sfclose((STREAM))
#endif

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

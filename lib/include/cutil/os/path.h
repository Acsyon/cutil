/** cutil/os/path.h
 *
 * Header for system path stuff.
 */

#ifndef CUTIL_OS_PATH_H_INCLUDED
#define CUTIL_OS_PATH_H_INCLUDED

#include <cutil/status.h>
#include <cutil/std/stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Path delimiter for separating components in paths.
 */
extern const char CUTIL_PATH_DELIMITER;

/**
 * Default permissions for created paths (octal representation).
 */
extern const unsigned int CUTIL_PATH_DEFAULT_MODE;

/**
 * Creates `path` and parents (if `recursive` is true) with permissions `mode`.
 *
 * @param[in] path string of path to be created
 * @param[in] mode octal representation of permissions of path to be created
 * @param[in] recursive whether to create parent directories recursively
 *
 * @return CUTIL_STATUS_SUCCESS on success, CUTIL_STATUS_FAILURE on failure
 */
cutil_Status
cutil_mkdir(const char *path, unsigned int mode, cutil_Bool recursive);

/**
 * Creates `path` and parents (if necessary) with permissions `mode` (similar to
 * 'mkdir -p').
 *
 * @param[in] path string of path to be created
 * @param[in] mode octal representation of permissions of path to be created
 *
 * @return CUTIL_STATUS_SUCCESS on success, CUTIL_STATUS_FAILURE on failure
 */
inline cutil_Status
cutil_mkdirp(const char *path, unsigned int mode)
{
    return cutil_mkdir(path, mode, CUTIL_TRUE);
}

/**
 * Removes file or directory at `path`. Fails if directory is non-empty when
 * `recursive` is CUTIL_FALSE. When `recursive` is CUTIL_TRUE, removes all
 * contents recursively before removing `path` itself.
 *
 * @param[in] path      string of file / path to be removed
 * @param[in] recursive whether to remove contents recursively
 *
 * @return CUTIL_STATUS_SUCCESS on success, CUTIL_STATUS_FAILURE on failure
 */
cutil_Status
cutil_rm(const char *path, cutil_Bool recursive);

/**
 * Removes all contents recursively before removing `path` itself.
 *
 * @param[in] path string of file / path to be removed
 *
 * @return CUTIL_STATUS_SUCCESS on success, CUTIL_STATUS_FAILURE on failure
 */
inline cutil_Status
cutil_rmr(const char *path)
{
    return cutil_rm(path, CUTIL_TRUE);
}

/**
 * Returns CUTIL_TRUE if `path` exists and is a directory.
 *
 * @param[in] path string of path to check
 *
 * @return CUTIL_TRUE if `path` is a directory, CUTIL_FALSE otherwise
 */
cutil_Bool
cutil_isdir(const char *path);

/**
 * Returns CUTIL_TRUE if `path` exists and is a regular file.
 *
 * @param[in] path string of path to check
 *
 * @return CUTIL_TRUE if `path` is a regular file, CUTIL_FALSE otherwise
 */
cutil_Bool
cutil_isfile(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_OS_PATH_H_INCLUDED */

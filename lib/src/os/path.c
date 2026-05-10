#include <cutil/os/path.h>

#include <errno.h>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cutil/io/log.h>
#include <cutil/std/stdbool.h>
#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

#define ERROR_STAT_MODE 0
#define PATH_BUFFER_SIZE 4096

const char CUTIL_PATH_DELIMITER = '/';
const unsigned int CUTIL_PATH_DEFAULT_MODE = 0755;

/* 'mkdir' with errno checking */
static inline cutil_Status
_cutil_smkdir(const char *path, unsigned int mode, struct stat *pst)
{
    if (stat(path, pst) == -1) {
        if (mkdir(path, mode) && errno != EEXIST) {
            cutil_log_error("Could not create path '%s'", path);
            return CUTIL_STATUS_FAILURE;
        }
    }
    return CUTIL_STATUS_SUCCESS;
}

cutil_Status
cutil_mkdir(const char *path, unsigned int mode, cutil_Bool recursive)
{
    CUTIL_RETURN_VAL_IF_NULL(path, CUTIL_STATUS_FAILURE);

    struct stat st = {0};

    /* Return if path already exists. */
    if (stat(path, &st) == 0) {
        return CUTIL_STATUS_SUCCESS;
    }

    /* If not and if recursive is specified, create directories in between from
     * bottom up */
    if (recursive) {
        char *const cpy = cutil_strdup(path);
        const size_t len = strlen(path);
        if (cpy[len - 1] == CUTIL_PATH_DELIMITER) {
            cpy[len - 1] = '\0';
        }
        for (char *p = cpy + 1; *p != '\0'; ++p) {
            if (*p == CUTIL_PATH_DELIMITER) {
                *p = '\0';
                if (_cutil_smkdir(cpy, mode, &st) == CUTIL_STATUS_FAILURE) {
                    free(cpy);
                    return CUTIL_STATUS_FAILURE;
                }
                *p = CUTIL_PATH_DELIMITER;
            }
        }
        free(cpy);
    }

    /* Finally, try if full path can be created */
    return _cutil_smkdir(path, mode, &st);
}

extern inline cutil_Status
cutil_mkdirp(const char *path, unsigned int mode);

static cutil_Bool
_cutil_keep_dir(const struct dirent *entry)
{
    return strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0;
}

cutil_Status
cutil_rm(const char *path, cutil_Bool recursive)
{
    CUTIL_RETURN_VAL_IF_NULL(path, CUTIL_STATUS_FAILURE);

    if (cutil_isfile(path)) {
        if (unlink(path) != 0) {
            cutil_log_error("Could not remove file '%s'", path);
            return CUTIL_STATUS_FAILURE;
        }
        return CUTIL_STATUS_SUCCESS;
    }

    if (!recursive) {
        if (rmdir(path) != 0) {
            cutil_log_error("Could not remove directory '%s'", path);
            return CUTIL_STATUS_FAILURE;
        }
        return CUTIL_STATUS_SUCCESS;
    }

    /* Recursive removal */
    DIR *const dir = opendir(path);
    if (!dir) {
        cutil_log_error("Could not open directory '%s'", path);
        return CUTIL_STATUS_FAILURE;
    }

    struct dirent *entry;
    cutil_Status result = CUTIL_STATUS_SUCCESS;
    while ((entry = readdir(dir)) != NULL) {
        if (_cutil_keep_dir(entry)) {
            continue;
        }
        char child[PATH_BUFFER_SIZE] = {0};
        snprintf(
          child, sizeof child, "%s%c%s", path, CUTIL_PATH_DELIMITER,
          entry->d_name
        );

        if (cutil_rmr(child) != CUTIL_STATUS_SUCCESS) {
            result = CUTIL_STATUS_FAILURE;
        }
    }
    closedir(dir);

    if (result == CUTIL_STATUS_SUCCESS) {
        if (rmdir(path) != 0) {
            cutil_log_error("Could not remove directory '%s'", path);
            result = CUTIL_STATUS_FAILURE;
        }
    }

    return result;
}

extern inline cutil_Status
cutil_rmr(const char *path);

static struct stat
_cutil_stat(const char *path)
{
    struct stat st;
    if (stat(path, &st) == -1) {
        cutil_log_error("Could not stat '%s'", path);
        st.st_mode = ERROR_STAT_MODE;
    }
    return st;
}

cutil_Bool
cutil_isdir(const char *path)
{
    struct stat st = _cutil_stat(path);
    if (st.st_mode == ERROR_STAT_MODE) {
        return CUTIL_FALSE;
    }
    return S_ISDIR(st.st_mode) ? CUTIL_TRUE : CUTIL_FALSE;
}

cutil_Bool
cutil_isfile(const char *path)
{
    struct stat st = _cutil_stat(path);
    if (st.st_mode == ERROR_STAT_MODE) {
        return CUTIL_FALSE;
    }
    return S_ISREG(st.st_mode) ? CUTIL_TRUE : CUTIL_FALSE;
}

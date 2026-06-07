#include <cutil/core/std/string.h>

#include <ctype.h>

#include <cutil/core/debug/null.h>
#include <cutil/core/io/log.h>
#include <cutil/core/util/macro.h>

size_t
cutil_strnlen(const char *str, size_t maxlen)
{
    CUTIL_NULL_CHECK(str);
    size_t len = 0;
    while (str[len] != '\0' && maxlen > len) {
        ++len;
    }
    return len;
}

char *
cutil_strdup(const char *str)
{
    CUTIL_RETURN_VAL_IF_VAL(str, NULL, NULL);

    const size_t len = strlen(str) + 1;
    char *cpy = malloc(len);
    if (cpy == NULL) {
        cutil_log_error("strdup: malloc(%zu) failed", len);
        return NULL;
    }
    cpy = memcpy(cpy, str, len);

    return cpy;
}

char *
cutil_strndup(const char *str, size_t maxlen)
{
    CUTIL_RETURN_VAL_IF_VAL(str, NULL, NULL);

    const size_t len = cutil_strnlen(str, maxlen) + 1;
    char *cpy = malloc(len);
    if (cpy == NULL) {
        cutil_log_error("strndup: malloc(%zu) failed", len);
        return NULL;
    }
    cpy = memcpy(cpy, str, len);
    cpy[len - 1] = '\0';

    return cpy;
}

void *
cutil_memdup(const void *ptr, size_t size, size_t num)
{
    CUTIL_RETURN_VAL_IF_VAL(ptr, NULL, NULL);

    char *cpy = malloc(size * num);
    if (cpy == NULL) {
        cutil_log_error("memdup: malloc(%zu) failed", size * num);
        return NULL;
    }
    cpy = memcpy(cpy, ptr, size * num);
    return cpy;
}

int
cutil_strcicmp(const char *lhs, const char *rhs)
{
    CUTIL_NULL_CHECK(lhs);
    CUTIL_NULL_CHECK(rhs);
    int res = 0;
    for (;; ++lhs, ++rhs) {
        res = tolower((unsigned char) *lhs) - tolower((unsigned char) *rhs);
        if (res != 0 || !*lhs || !*rhs) {
            break;
        }
    }
    return res;
}

int
cutil_strncicmp(const char *lhs, const char *rhs, size_t n)
{
    CUTIL_NULL_CHECK(lhs);
    CUTIL_NULL_CHECK(rhs);
    int res = 0;
    for (size_t i = 0; i < n; ++i, ++lhs, ++rhs) {
        res = tolower((unsigned char) *lhs) - tolower((unsigned char) *rhs);
        if (res != 0 || !*lhs || !*rhs) {
            break;
        }
    }
    return res;
}

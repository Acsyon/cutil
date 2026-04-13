#include <cutil/std/string.h>

#include <cutil/debug/null.h>
#include <cutil/io/log.h>
#include <cutil/util/macro.h>

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

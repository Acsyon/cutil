#include <cutil/std/string.h>

#include <cutil/util/macro.h>

size_t
cutil_strnlen(const char *str, size_t maxlen)
{
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
    cpy = memcpy(cpy, str, len);

    return cpy;
}

char *
cutil_strndup(const char *str, size_t maxlen)
{
    CUTIL_RETURN_VAL_IF_VAL(str, NULL, NULL);

    const size_t len = cutil_strnlen(str, maxlen) + 1;
    char *cpy = malloc(len);
    cpy = memcpy(cpy, str, len);
    cpy[len - 1] = '\0';

    return cpy;
}

#include <cutil/core/std/stdbool.h>

#include <cutil/core/std/string.h>

cutil_Bool
cutil_atobool(const char *str)
{
    return CUTIL_BOOLIFY(cutil_strncicmp(str, "true", sizeof "true") == 0);
}

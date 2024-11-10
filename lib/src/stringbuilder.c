#include <cutil/stringbuilder.h>

#include <stdio.h>
#include <string.h>

#define STRING_DEFAULT_SIZE 64
#define STRING_THRESHOLD_SIZE 1024
#define BUFFER_THRESHOLD_SIZE 1024
#define EXPANSION_FACTOR 2.0

/**
 * Adjusts (and potentially reallocs) string in `sb` to `newsize`.
 * 
 * @param[in] sb cutil_StringBuilder to adjust string of
 * @param[in] newsize new size of string
 */
static void
_cutil_StringBuilder_adjust_str(cutil_StringBuilder *sb, size_t newsize)
{
    if (newsize <= sb->capacity) {
        return;
    }
    if (newsize < STRING_THRESHOLD_SIZE) {
        sb->capacity *= EXPANSION_FACTOR;
    } else {
        sb->capacity += STRING_THRESHOLD_SIZE;
    }
    sb->str = realloc(sb->str, sb->capacity * sizeof *sb->str);
}

/**
 * Adjusts (and potentially reallocs) buffer in `sb` to `newsize`.
 * 
 * @param[in] sb cutil_StringBuilder to adjust buffer of
 * @param[in] newsize new size of buffer
 */
static void
_cutil_StringBuilder_adjust_buf(cutil_StringBuilder *sb, size_t newsize)
{
    if (newsize <= sb->bufsiz) {
        return;
    }
    if (newsize < BUFFER_THRESHOLD_SIZE) {
        sb->bufsiz *= EXPANSION_FACTOR;
    } else {
        sb->bufsiz += BUFFER_THRESHOLD_SIZE;
    }
    sb->buf = realloc(sb->buf, sb->bufsiz * sizeof *sb->buf);
}

cutil_StringBuilder *
cutil_StringBuilder_alloc(size_t size)
{
    cutil_StringBuilder *const sb = malloc(sizeof *sb);

    if (size == 0) {
        size = STRING_DEFAULT_SIZE;
    }

    sb->capacity = size;
    sb->str = malloc(sb->capacity * sizeof *sb->str);
    sb->length = 0;
    sb->bufsiz = (size > STRING_DEFAULT_SIZE) ? size : BUFFER_THRESHOLD_SIZE;
    sb->buf = malloc(sb->bufsiz * sizeof *sb->buf);

    return sb;
}

void
cutil_StringBuilder_free(cutil_StringBuilder *sb)
{
    if (sb == NULL) {
        return;
    }

    free(sb->str);
    free(sb->buf);

    free(sb);
}

int
cutil_StringBuilder_vnappendf(
  cutil_StringBuilder *sb, size_t maxlen, const char *format, va_list args
)
{
    _cutil_StringBuilder_adjust_buf(sb, maxlen);
    const int res = vsnprintf(sb->buf, maxlen, format, args);
    if (res < 0) {
        return res;
    }
    sb->length += res;
    _cutil_StringBuilder_adjust_str(sb, sb->length);
    strcat(sb->str, sb->buf);
    return res;
}

int
cutil_StringBuilder_nappendf(
  cutil_StringBuilder *sb, size_t maxlen, const char *format, ...
)
{
    va_list args;
    va_start(args, format);
    const int res = cutil_StringBuilder_vnappendf(sb, maxlen, format, args);
    va_end(args);
    return res;
}

int
cutil_StringBuilder_vappendf(
  cutil_StringBuilder *sb, const char *format, va_list args
)
{
    const size_t maxlen = vsnprintf(NULL, 0, format, args) + 1;
    return cutil_StringBuilder_vnappendf(sb, maxlen, format, args);
}

int
cutil_StringBuilder_appendf(cutil_StringBuilder *sb, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    const int res = cutil_StringBuilder_vappendf(sb, format, args);
    va_end(args);
    return res;
}

extern inline size_t
cutil_StringBuilder_length(const cutil_StringBuilder *sb);

extern inline const char *
cutil_StringBuilder_to_string(const cutil_StringBuilder *sb);

#include <cutil/stringbuilder.h>

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define STRING_DEFAULT_SIZE 64
#define STRING_THRESHOLD_SIZE 1024
#define BUFFER_THRESHOLD_SIZE 1024
#define EXPANSION_FACTOR 2.0

static inline size_t
_norm_exp(size_t target, double factor)
{
    if (target < STRING_DEFAULT_SIZE) {
        return STRING_DEFAULT_SIZE;
    }
    return (size_t) exp(ceil(log(1.0 * target) / log(factor)));
}

static inline size_t
_norm_lin(size_t target, size_t threshold)
{
    return (target / threshold + 1) * threshold;
}

static size_t
_normalize_size(size_t target, size_t threshold)
{
    if (target < threshold) {
        return _norm_exp(target, 1.0 * EXPANSION_FACTOR);
    }
    if (target > threshold) {
        return _norm_lin(target, threshold);
    }
    return threshold;
}

/**
 * Adjusts (and potentially reallocs) char array pointed to by `p_carr` to
 * `target`. A pointer to the (previous) size is passed in `p_size` so that it
 * can be adjusted accordingly if a reallocation happens. A resize can be forced
 * with the force flag, causing the new size to be exactly `target`. If the
 * resize is not forced, reallocation depends on the `threshold` below of which
 * the expansion happens exponentially (by EXPANSION_FACTOR) while linearly (in
 * multiples of the threshold) above.
 *
 * @param[in, out] p_carr pointer to char array to enlarge
 * @param[in, out] p_size pointer to size of char array
 * @param[in] threshold threshold between linear and exponential growth
 * @param[in] target target size of string
 * @param[in] force should the resize be forced?
 */
static void
_adjust_carr(
  char **p_carr, size_t *p_size, size_t threshold, size_t target, bool force
)
{
    if (force) {
        *p_size = target;
    } else {
        *p_size = _normalize_size(target, threshold);
    }
    *p_carr = realloc(*p_carr, *p_size * sizeof **p_carr);
}

static void
_enlarge_carr(char **p_carr, size_t *p_size, size_t threshold, size_t target)
{
    if (target <= *p_size) {
        return;
    }
    _adjust_carr(p_carr, p_size, threshold, target, false);
}

cutil_StringBuilder *
cutil_StringBuilder_alloc(size_t size)
{
    cutil_StringBuilder *const sb = malloc(sizeof *sb);

    size = _normalize_size(size, STRING_THRESHOLD_SIZE);
    sb->capacity = size;
    sb->str = calloc(sb->capacity, sizeof *sb->str);
    sb->length = 0;
    sb->bufsiz = size;
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

void
cutil_StringBuilder_clear(cutil_StringBuilder *sb)
{
    const size_t flags = CUTIL_RESIZE_FLAG_STRING | CUTIL_RESIZE_FLAG_BUFFER;
    cutil_StringBuilder_resize(sb, 0, flags);
}

void
cutil_StringBuilder_resize(cutil_StringBuilder *sb, size_t target, int flags)
{
    const bool force = flags & CUTIL_RESIZE_FLAG_FORCE;
    if (flags & CUTIL_RESIZE_FLAG_STRING) {
        _adjust_carr(
          &sb->str, &sb->capacity, STRING_THRESHOLD_SIZE, target, force
        );
        if (sb->length >= target) {
            sb->length = target;
            sb->str[sb->length] = '\0';
        }
    }
    if (flags & CUTIL_RESIZE_FLAG_BUFFER) {
        _adjust_carr(
          &sb->buf, &sb->bufsiz, BUFFER_THRESHOLD_SIZE, target, force
        );
    }
}

void
cutil_StringBuilder_shrink_to_fit(cutil_StringBuilder *sb)
{
    const int str_flags = CUTIL_RESIZE_FLAG_STRING | CUTIL_RESIZE_FLAG_FORCE;
    cutil_StringBuilder_resize(sb, sb->length + 1, str_flags);
    const int buf_flags = CUTIL_RESIZE_FLAG_BUFFER;
    cutil_StringBuilder_resize(sb, 0, buf_flags);
}

extern inline size_t
cutil_StringBuilder_length(const cutil_StringBuilder *sb);

extern inline const char *
cutil_StringBuilder_get_string(const cutil_StringBuilder *sb);

char *
cutil_StringBuilder_duplicate_string(const cutil_StringBuilder *sb)
{
    const size_t len = sb->length + 1;
    char *const cpy = malloc(len * sizeof *cpy);
    strcpy(cpy, sb->str);
    return cpy;
}

int
cutil_StringBuilder_vninsertf(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t pos,
  size_t maxlen,
  const char *CUTIL_RESTRICT format,
  va_list args
)
{
    if (pos > sb->length) {
        pos = sb->length;
    }
    const size_t remainder = sb->length - pos;
    _enlarge_carr(
      &sb->buf, &sb->bufsiz, BUFFER_THRESHOLD_SIZE, maxlen + remainder
    );
    const int res = vsnprintf(sb->buf, maxlen, format, args);
    if (res < 0) {
        return res;
    }
    if (remainder > 0) {
        strncpy(&sb->buf[res], &sb->str[pos], remainder + 1);
    }
    sb->length += res;
    _enlarge_carr(
      &sb->str, &sb->capacity, STRING_THRESHOLD_SIZE, sb->length + 1
    );
    strncpy(&sb->str[pos], sb->buf, res + remainder + 1);
    return res;
}

int
cutil_StringBuilder_ninsertf(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t pos,
  size_t maxlen,
  const char *CUTIL_RESTRICT format,
  ...
)
{
    va_list args;
    va_start(args, format);
    const int res
      = cutil_StringBuilder_vninsertf(sb, pos, maxlen, format, args);
    va_end(args);
    return res;
}

int
cutil_StringBuilder_ninsert(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t pos,
  size_t maxlen,
  const char *CUTIL_RESTRICT str
)
{
    return cutil_StringBuilder_ninsertf(sb, pos, maxlen, "%s", str);
}

int
cutil_StringBuilder_vinsertf(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t pos,
  const char *CUTIL_RESTRICT format,
  va_list args
)
{
    va_list args_cpy;
    va_copy(args_cpy, args);
    const int res = vsnprintf(NULL, 0, format, args_cpy);
    if (res < 0) {
        return res;
    }
    const size_t maxlen = res + 1;
    return cutil_StringBuilder_vninsertf(sb, pos, maxlen, format, args);
}

int
cutil_StringBuilder_insertf(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t pos,
  const char *CUTIL_RESTRICT format,
  ...
)
{
    va_list args;
    va_start(args, format);
    const int res = cutil_StringBuilder_vinsertf(sb, pos, format, args);
    va_end(args);
    return res;
}

int
cutil_StringBuilder_insert(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t pos,
  const char *CUTIL_RESTRICT str
)
{
    return cutil_StringBuilder_insertf(sb, pos, "%s", str);
}

int
cutil_StringBuilder_vnappendf(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t maxlen,
  const char *CUTIL_RESTRICT format,
  va_list args
)
{
    return cutil_StringBuilder_vninsertf(sb, sb->length, maxlen, format, args);
}

int
cutil_StringBuilder_nappendf(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t maxlen,
  const char *CUTIL_RESTRICT format,
  ...
)
{
    va_list args;
    va_start(args, format);
    const int res = cutil_StringBuilder_vnappendf(sb, maxlen, format, args);
    va_end(args);
    return res;
}

int
cutil_StringBuilder_nappend(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  size_t maxlen,
  const char *CUTIL_RESTRICT str
)
{
    return cutil_StringBuilder_nappendf(sb, maxlen, "%s", str);
}

int
cutil_StringBuilder_vappendf(
  cutil_StringBuilder *CUTIL_RESTRICT sb,
  const char *CUTIL_RESTRICT format,
  va_list args
)
{
    return cutil_StringBuilder_vinsertf(sb, sb->length, format, args);
}

int
cutil_StringBuilder_appendf(
  cutil_StringBuilder *CUTIL_RESTRICT sb, const char *CUTIL_RESTRICT format, ...
)
{
    va_list args;
    va_start(args, format);
    const int res = cutil_StringBuilder_vappendf(sb, format, args);
    va_end(args);
    return res;
}

int
cutil_StringBuilder_append(
  cutil_StringBuilder *CUTIL_RESTRICT sb, const char *CUTIL_RESTRICT str
)
{
    return cutil_StringBuilder_appendf(sb, "%s", str);
}

void
cutil_StringBuilder_delete(cutil_StringBuilder *sb, size_t pos, size_t num)
{
    if (num == 0) {
        return;
    }
    if (pos > sb->length) {
        return;
    }
    if (pos + num > sb->length) {
        num = sb->length - pos;
    }
    memmove(&sb->str[pos], &sb->str[pos + num], num);
    sb->length -= num;
}

void
cutil_StringBuilder_delete_from_to(
  cutil_StringBuilder *sb, size_t begin, size_t end
)
{
    if (end > sb->length) {
        end = sb->length;
    }
    if (end < begin) {
        return;
    }
    cutil_StringBuilder_delete(sb, begin, end - begin + 1);
}

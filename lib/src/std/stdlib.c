#include <cutil/std/stdlib.h>

#include <cutil/debug/null.h>
#include <cutil/io/log.h>

void *
cutil_malloc(size_t size)
{
    if (size == 0UL) {
        return NULL;
    }
    return malloc(size);
}

void *
cutil_calloc(size_t num, size_t size)
{
    if (num == 0UL) {
        return NULL;
    }
    if (size == 0UL) {
        cutil_log_warn("size of calloc is 0");
        return NULL;
    }
    return calloc(num, size);
}

void *
cutil_realloc(void *ptr, size_t size)
{
    if (size == 0UL) {
        free(ptr);
        return NULL;
    }
    return realloc(ptr, size);
}

/**
 * Delete potential debug redefinitions.
 */

#ifdef malloc
    #undef malloc
#endif

#ifdef calloc
    #undef calloc
#endif

#ifdef realloc
    #undef realloc
#endif

void *
cutil_dmalloc(size_t size, const char *file, int line)
{
    void *const ptr = malloc(size);
    if (ptr == NULL) {
        cutil_log_error("malloc(%zu) failed (%s:%i)", size, file, line);
    }
    return ptr;
}

void *
cutil_dcalloc(size_t num, size_t size, const char *file, int line)
{
    void *const ptr = calloc(num, size);
    if (ptr == NULL) {
        cutil_log_error(
          "calloc(%zu, %zu) failed (%s:%i)", num, size, file, line
        );
    }
    return ptr;
}

void *
cutil_drealloc(
  void *CUTIL_RESTRICT ptr,
  size_t size,
  const char *CUTIL_RESTRICT file,
  int line
)
{
    ptr = realloc(ptr, size);
    if (ptr == NULL) {
        cutil_log_error("realloc(ptr, %zu) failed (%s:%i)", size, file, line);
    }
    return ptr;
}

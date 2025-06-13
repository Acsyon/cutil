#include <cutil/std/stdio.h>

#include <cutil/debug/null.h>
#include <cutil/io/log.h>

/**
 * Delete potential debug redefinitions.
 */

#ifdef fopen
    #undef fopen
#endif

#ifdef fclose
    #undef fclose
#endif

FILE *
cutil_dfopen(
  const char *CUTIL_RESTRICT fname,
  const char *CUTIL_RESTRICT modes,
  const char *CUTIL_RESTRICT file,
  int line
)
{
    CUTIL_NULL_CHECK(fname);
    CUTIL_NULL_CHECK(modes);
    CUTIL_NULL_CHECK(file);

    FILE *const out = fopen(fname, modes);
    if (out == NULL) {
        cutil_log_error(
          "fopen(%s, %s) failed! %s:%i", fname, modes, file, line
        );
    }
    return out;
}

int
cutil_sfclose(FILE *in)
{
    if (in == NULL || in == stdin || in == stdout || in == stderr) {
        return 0;
    }
    return fclose(in);
}

long
cutil_get_filesize(FILE *in)
{
    CUTIL_NULL_CHECK(in);

    fpos_t init_pos;
    fgetpos(in, &init_pos);
    rewind(in);
    fseek(in, 0L, SEEK_END);
    const long fsize = ftell(in) + 1;
    fsetpos(in, &init_pos);

    return fsize;
}

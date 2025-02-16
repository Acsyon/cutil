#include <cutil/std/stdio.h>

#include <cutil/cutil.h>

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
    fpos_t init_pos;
    fgetpos(in, &init_pos);
    rewind(in);
    fseek(in, 0L, SEEK_END);
    const long fsize = ftell(in) + 1;
    fsetpos(in, &init_pos);

    return fsize;
}

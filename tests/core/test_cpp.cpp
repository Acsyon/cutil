#include <cutil/core/debug/null.h>

#include <cutil/core/io/log.h>

#include <cutil/core/posix/getopt.h>

#include <cutil/core/std/inttypes.h>
#include <cutil/core/std/math.h>
#include <cutil/core/std/stdbool.h>
#include <cutil/core/std/stddef.h>
#include <cutil/core/std/stdio.h>
#include <cutil/core/std/stdlib.h>
#include <cutil/core/std/string.h>

#include <cutil/data/generic/string/util/iterator.h>

#include <cutil/core/string/builder.h>
#include <cutil/core/string/type.h>

#include <cutil/core/util/compare.h>
#include <cutil/core/util/hash.h>
#include <cutil/core/util/macro.h>

#include <cutil/core/cutil.h>
#include <cutil/core/status.h>

/**
 * Empty test that includes all cutil headers and just tests whether the C++
 * compilation succeeds.
 */
int
main()
{
    return 0;
}

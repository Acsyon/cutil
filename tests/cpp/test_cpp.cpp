#include <cutil/data/generic/list/arraylist.h>
#include <cutil/data/generic/map/hashmap.h>
#include <cutil/data/generic/set/hashset.h>

#include <cutil/data/generic/array.h>
#include <cutil/data/generic/iterator.h>
#include <cutil/data/generic/list.h>
#include <cutil/data/generic/map.h>
#include <cutil/data/generic/object.h>
#include <cutil/data/generic/set.h>
#include <cutil/data/generic/type.h>
#include <cutil/data/native/bitarray.h>

#include <cutil/debug/null.h>

#include <cutil/io/log.h>

#include <cutil/posix/getopt.h>

#include <cutil/std/inttypes.h>
#include <cutil/std/math.h>
#include <cutil/std/stdbool.h>
#include <cutil/std/stddef.h>
#include <cutil/std/stdio.h>
#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>

#include <cutil/string/util/iterator.h>

#include <cutil/string/builder.h>
#include <cutil/string/type.h>

#include <cutil/util/compare.h>
#include <cutil/util/hash.h>
#include <cutil/util/macro.h>

#include <cutil/cutil.h>
#include <cutil/status.h>

/**
 * Empty test that includes all cutil headers and just tests whether the C++
 * compilation succeeds.
 */
int
main()
{
    return 0;
}

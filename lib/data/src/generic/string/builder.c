#include <cutil/data/generic/string/builder.h>

#include <cutil/core/string/builder.h>

#include "../../../../core/src/string/priv.h"

static const cutil_GenericType CUTIL_GENERIC_TYPE_STRING_BUILDER_INSTANCE = {
  .name = "cutil_StringBuilder",
  .size = sizeof(cutil_StringBuilder),
  .init = &priv_cutil_StringBuilder_init_default,
  .clear = &cutil_StringBuilder_clear_generic,
  .copy = &cutil_StringBuilder_copy_generic,
  .deep_equals = &cutil_StringBuilder_deep_equals_generic,
  .comp = &cutil_StringBuilder_compare_generic,
  .hash = &cutil_StringBuilder_hash_generic,
  .to_string = &cutil_StringBuilder_to_string_generic
};
const cutil_GenericType *const CUTIL_GENERIC_TYPE_STRING_BUILDER
  = &CUTIL_GENERIC_TYPE_STRING_BUILDER_INSTANCE;

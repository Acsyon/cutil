#include <cutil/data/generic/string/type.h>

#include <cutil/core/string/type.h>

#include "../../../../core/src/string/priv.h"

static const cutil_GenericType CUTIL_GENERIC_TYPE_STRING_INSTANCE = {
  .name = "cutil_String",
  .size = sizeof(cutil_String),
  .init = &priv_cutil_String_init_default,
  .clear = &cutil_String_clear_generic,
  .copy = &cutil_String_copy_generic,
  .deep_equals = &cutil_String_deep_equals_generic,
  .comp = &cutil_String_compare_generic,
  .hash = &cutil_String_hash_generic,
  .to_string = &cutil_String_to_string_generic
};
const cutil_GenericType *const CUTIL_GENERIC_TYPE_STRING
  = &CUTIL_GENERIC_TYPE_STRING_INSTANCE;

static const cutil_GenericType CUTIL_GENERIC_TYPE_STRING_VIEW_INSTANCE = {
  .name = "cutil_StringView",
  .size = sizeof(cutil_StringView),
  .init = &priv_cutil_StringView_init_default,
  .clear = &cutil_StringView_clear_generic,
  .copy = &cutil_StringView_copy_generic,
  .deep_equals = &cutil_StringView_deep_equals_generic,
  .comp = &cutil_StringView_compare_generic,
  .hash = &cutil_StringView_hash_generic,
  .to_string = &cutil_StringView_to_string_generic
};
const cutil_GenericType *const CUTIL_GENERIC_TYPE_STRING_VIEW
  = &CUTIL_GENERIC_TYPE_STRING_VIEW_INSTANCE;

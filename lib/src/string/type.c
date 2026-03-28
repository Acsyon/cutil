#include <cutil/string/type.h>

#include <cutil/io/log.h>
#include <cutil/std/stdio.h>
#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

cutil_String *
cutil_String_from_string(const char *str)
{
    cutil_String *const s = CUTIL_CALLOC_OBJECT(s);

    if (str != NULL) {
        s->str = cutil_strdup(str);
        s->length = strlen(str);
    }

    return s;
}

cutil_String *
cutil_String_from_nstring(const char *str, size_t maxlen)
{
    cutil_String *const s = CUTIL_CALLOC_OBJECT(s);

    if (str != NULL) {
        s->str = cutil_strndup(str, maxlen);
        s->length = strlen(s->str);
    }

    return s;
}

void
cutil_String_free(cutil_String *s)
{
    CUTIL_RETURN_IF_NULL(s);

    cutil_String_clear(s);

    free(s);
}

void
cutil_String_clear(cutil_String *s)
{
    CUTIL_RETURN_IF_NULL(s);
    free(s->str);
    s->str = NULL;
    s->length = 0;
}

void
cutil_String_copy(cutil_String *dst, const cutil_String *src)
{
    CUTIL_NULL_CHECK(dst);
    CUTIL_NULL_CHECK(src);

    free(dst->str);
    dst->str = cutil_strdup(src->str);
    dst->length = src->length;
}

cutil_Bool
cutil_String_deep_equals(const cutil_String *lhs, const cutil_String *rhs)
{
    return cutil_String_deep_equals_generic(lhs, rhs);
}

int
cutil_String_compare(const cutil_String *lhs, const cutil_String *rhs)
{
    return cutil_String_compare_generic(lhs, rhs);
}

cutil_hash_t
cutil_String_hash(const cutil_String *s)
{
    return cutil_String_hash_generic(s);
}

size_t
cutil_String_to_string(const cutil_String *s, char *buf, size_t buflen)
{
    return cutil_String_to_string_generic(s, buf, buflen);
}

extern inline void
cutil_String_clear_generic(void *obj);

extern inline void
cutil_String_copy_generic(void *dst, const void *src);

cutil_Bool
cutil_String_deep_equals_generic(const void *vlhs, const void *vrhs)
{
    const cutil_String *const lhs = vlhs;
    const cutil_String *const rhs = vrhs;
    if (lhs == rhs) {
        return true;
    }
    if (lhs == NULL || rhs == NULL) {
        return false;
    }
    if (lhs->str == rhs->str) {
        return true;
    }
    if (lhs->str == NULL || rhs->str == NULL) {
        return false;
    }
    return (strcmp(lhs->str, rhs->str) == 0);
}

int
cutil_String_compare_generic(const void *vlhs, const void *vrhs)
{
    const cutil_String *const lhs = vlhs;
    const cutil_String *const rhs = vrhs;
    if (lhs == rhs) {
        return 0;
    }
    if (lhs == NULL || rhs == NULL) {
        return (rhs == NULL) - (lhs == NULL);
    }
    if (lhs->str == rhs->str) {
        return 0;
    }
    if (lhs->str == NULL || rhs->str == NULL) {
        return (rhs->str == NULL) - (lhs->str == NULL);
    }
    return strcmp(lhs->str, rhs->str);
}

cutil_hash_t
cutil_String_hash_generic(const void *vs)
{
    const cutil_String *const s = vs;
    if (s == NULL || s->length == 0) {
        return CUTIL_HASH_C(0);
    }
    return cutil_hash_str(s->str);
}

size_t
cutil_String_to_string_generic(const void *vs, char *buf, size_t buflen)
{
    const cutil_String *const s = vs;
    if (s == NULL || s->str == NULL) {
        const int res = snprintf(buf, buflen, "NULL");
        return (size_t) CUTIL_MAX(0, res);
    }
    if (s->length + 1UL > buflen) {
        cutil_log_warn("Cannot serialize cutil_String: buffer too small");
        return 0UL;
    }
    return snprintf(buf, buflen, "%s", s->str);
}

static void
_cutil_String_init_default(void *vs)
{
    cutil_String *const s = vs;
    memset(s, 0, sizeof *s);
}

static const cutil_GenericType CUTIL_GENERIC_TYPE_STRING_INSTANCE = {
  .name = "cutil_String",
  .size = sizeof(cutil_String),
  .init = &_cutil_String_init_default,
  .clear = &cutil_String_clear_generic,
  .copy = &cutil_String_copy_generic,
  .deep_equals = &cutil_String_deep_equals_generic,
  .comp = &cutil_String_compare_generic,
  .hash = &cutil_String_hash_generic,
  .to_string = &cutil_String_to_string_generic
};
const cutil_GenericType *const CUTIL_GENERIC_TYPE_STRING
  = &CUTIL_GENERIC_TYPE_STRING_INSTANCE;

cutil_StringView *
cutil_StringView_from_string(const char *str)
{
    cutil_StringView *const sv = CUTIL_CALLOC_OBJECT(sv);

    if (str != NULL) {
        sv->str = str;
        sv->length = strlen(str);
    }

    return sv;
}

cutil_StringView *
cutil_StringView_from_nstring(const char *str, size_t maxlen)
{
    cutil_StringView *const sv = CUTIL_CALLOC_OBJECT(sv);

    if (str != NULL) {
        sv->str = str;
        sv->length = cutil_strnlen(str, maxlen);
    }

    return sv;
}

void
cutil_StringView_free(cutil_StringView *sv)
{
    CUTIL_RETURN_IF_NULL(sv);

    cutil_StringView_clear(sv);

    free(sv);
}

void
cutil_StringView_clear(cutil_StringView *sv)
{
    CUTIL_RETURN_IF_NULL(sv);
    sv->str = NULL;
    sv->length = 0;
}

void
cutil_StringView_copy(cutil_StringView *dst, const cutil_StringView *src)
{
    CUTIL_NULL_CHECK(dst);
    CUTIL_NULL_CHECK(src);

    dst->str = src->str;
    dst->length = src->length;
}

cutil_Bool
cutil_StringView_deep_equals(
  const cutil_StringView *lhs, const cutil_StringView *rhs
)
{
    return cutil_StringView_deep_equals_generic(lhs, rhs);
}

int
cutil_StringView_compare(
  const cutil_StringView *lhs, const cutil_StringView *rhs
)
{
    return cutil_StringView_compare_generic(lhs, rhs);
}

cutil_hash_t
cutil_StringView_hash(const cutil_StringView *sv)
{
    return cutil_StringView_hash_generic(sv);
}

size_t
cutil_StringView_to_string(const cutil_StringView *sv, char *buf, size_t buflen)
{
    return cutil_StringView_to_string_generic(sv, buf, buflen);
}

extern inline void
cutil_StringView_clear_generic(void *obj);

extern inline void
cutil_StringView_copy_generic(void *dst, const void *src);

cutil_Bool
cutil_StringView_deep_equals_generic(const void *vlhs, const void *vrhs)
{
    const cutil_StringView *const lhs = vlhs;
    const cutil_StringView *const rhs = vrhs;
    if (lhs == rhs) {
        return true;
    }
    if (lhs == NULL || rhs == NULL) {
        return false;
    }
    if (lhs->str == rhs->str) {
        return true;
    }
    if (lhs->str == NULL || rhs->str == NULL) {
        return false;
    }
    return (strcmp(lhs->str, rhs->str) == 0);
}

int
cutil_StringView_compare_generic(const void *vlhs, const void *vrhs)
{
    const cutil_StringView *const lhs = vlhs;
    const cutil_StringView *const rhs = vrhs;
    if (lhs == rhs) {
        return 0;
    }
    if (lhs == NULL || rhs == NULL) {
        return (rhs == NULL) - (lhs == NULL);
    }
    if (lhs->str == rhs->str) {
        return 0;
    }
    if (lhs->str == NULL || rhs->str == NULL) {
        return (rhs->str == NULL) - (lhs->str == NULL);
    }
    return strcmp(lhs->str, rhs->str);
}

cutil_hash_t
cutil_StringView_hash_generic(const void *vsv)
{
    const cutil_StringView *const sv = vsv;
    if (sv == NULL || sv->length == 0) {
        return CUTIL_HASH_C(0);
    }
    return cutil_hash_str(sv->str);
}

size_t
cutil_StringView_to_string_generic(const void *vsv, char *buf, size_t buflen)
{
    const cutil_StringView *const sv = vsv;
    if (sv == NULL || sv->str == NULL) {
        const int res = snprintf(buf, buflen, "NULL");
        return (size_t) CUTIL_MAX(0, res);
    }
    if (sv->length + 1UL > buflen) {
        cutil_log_warn("Cannot serialize cutil_StringView: buffer too small");
        return 0UL;
    }
    return snprintf(buf, buflen, "%.*s", (int) sv->length, sv->str);
}

static void
_cutil_StringView_init_default(void *vsv)
{
    cutil_StringView *const sv = vsv;
    memset(sv, 0, sizeof *sv);
}

static const cutil_GenericType CUTIL_GENERIC_TYPE_STRING_VIEW_INSTANCE = {
  .name = "cutil_StringView",
  .size = sizeof(cutil_StringView),
  .init = &_cutil_StringView_init_default,
  .clear = &cutil_StringView_clear_generic,
  .copy = &cutil_StringView_copy_generic,
  .deep_equals = &cutil_StringView_deep_equals_generic,
  .comp = &cutil_StringView_compare_generic,
  .hash = &cutil_StringView_hash_generic,
  .to_string = &cutil_StringView_to_string_generic
};
const cutil_GenericType *const CUTIL_GENERIC_TYPE_STRING_VIEW
  = &CUTIL_GENERIC_TYPE_STRING_VIEW_INSTANCE;

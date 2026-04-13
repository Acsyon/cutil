#include <cutil/data/generic/object.h>

#include <cutil/io/log.h>
#include <cutil/std/stdio.h>
#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

static void
_cutil_GenericObject_init_default(void *vobj)
{
    cutil_GenericObject *const obj = vobj;

    obj->type = NULL;
    obj->data = NULL;
}

cutil_GenericObject *
cutil_GenericObject_alloc(void)
{
    cutil_GenericObject *const obj = CUTIL_MALLOC_OBJECT(obj);

    _cutil_GenericObject_init_default(obj);
    
    return obj;
}

cutil_GenericObject *
cutil_GenericObject_create(const cutil_GenericType *type, const void *data)
{
    if (!cutil_GenericType_is_valid(type)) {
        cutil_log_warn("Invalid type for GenericObject");
        return NULL;
    }

    cutil_GenericObject *const obj = cutil_GenericObject_alloc();

    obj->type = type;
    if (data != NULL) {
        obj->data = calloc(1UL, type->size);
        cutil_GenericType_apply_copy(type, obj->data, data);
    } 

    return obj;
}

void
cutil_GenericObject_free(cutil_GenericObject *obj)
{
    CUTIL_RETURN_IF_NULL(obj);

    cutil_GenericObject_clear(obj);

    free(obj);
}

void
cutil_GenericObject_clear(cutil_GenericObject *obj)
{
    CUTIL_RETURN_IF_NULL(obj);

    if (obj->data != NULL && obj->type != NULL) {
        cutil_GenericType_apply_clear(obj->type, obj->data);
        free(obj->data);
    }

    obj->type = NULL;
    obj->data = NULL;
}

void
cutil_GenericObject_copy(
  cutil_GenericObject *dst, const cutil_GenericObject *src
)
{
    CUTIL_NULL_CHECK(dst);
    CUTIL_NULL_CHECK(src);

    cutil_GenericObject_clear(dst);

    if (src->type != NULL && src->data != NULL) {
        dst->type = src->type;
        dst->data = calloc(1UL, src->type->size);
        cutil_GenericType_apply_copy(src->type, dst->data, src->data);
    }
}

const cutil_GenericType *
cutil_GenericObject_get_type(const cutil_GenericObject *obj)
{
    CUTIL_NULL_CHECK(obj);
    return obj->type;
}

const void *
cutil_GenericObject_get_data(const cutil_GenericObject *obj)
{
    CUTIL_NULL_CHECK(obj);
    return obj->data;
}

extern inline void
cutil_GenericObject_clear_generic(void *obj);

extern inline void
cutil_GenericObject_copy_generic(void *dst, const void *src);

cutil_Bool
cutil_GenericObject_deep_equals_generic(const void *vlhs, const void *vrhs)
{
    const cutil_GenericObject *const lhs = vlhs;
    const cutil_GenericObject *const rhs = vrhs;
    if (lhs == rhs) {
        return true;
    }
    if (lhs == NULL || rhs == NULL) {
        return false;
    }
    if (lhs->type != rhs->type) {
        return false;
    }
    if (lhs->data == rhs->data) {
        return true;
    }
    if (lhs->type == NULL) {
        return false;
    }
    return cutil_GenericType_apply_deep_equals(lhs->type, lhs->data, rhs->data);
}

int
cutil_GenericObject_compare_generic(const void *vlhs, const void *vrhs)
{
    const cutil_GenericObject *const lhs = vlhs;
    const cutil_GenericObject *const rhs = vrhs;
    if (lhs == rhs) {
        return 0;
    }
    if (lhs == NULL || rhs == NULL) {
        return (rhs == NULL) - (lhs == NULL);
    }
    if (lhs->type == NULL || rhs->type == NULL) {
        return (rhs->type == NULL) - (lhs->type == NULL);
    }
    /* Different types: order by type name */
    if (!cutil_GenericType_equals(lhs->type, rhs->type)) {
        const int name_cmp = strcmp(lhs->type->name, rhs->type->name);
        if (name_cmp != 0) {
            return name_cmp;
        }
        /* Same name, different pointer: deterministic by pointer */
        return (lhs->type > rhs->type) - (lhs->type < rhs->type);
    }
    /* Same type: compare inner data */
    return cutil_GenericType_apply_compare(lhs->type, lhs->data, rhs->data);
}

cutil_hash_t
cutil_GenericObject_hash_generic(const void *vobj)
{
    const cutil_GenericObject *const obj = vobj;
    if (obj == NULL || obj->type == NULL) {
        return CUTIL_HASH_C(0);
    }
    return cutil_GenericType_apply_hash(obj->type, obj->data);
}

size_t
cutil_GenericObject_to_string_generic(
  const void *vobj, char *buf, size_t buflen
)
{
    const cutil_GenericObject *const obj = vobj;
    if (obj == NULL) {
        return snprintf(buf, buflen, "NULL");
    }
    if (obj->type == NULL) {
        return snprintf(buf, buflen, "GenericObject<NULL>");
    }
    return cutil_GenericType_apply_to_string(obj->type, obj->data, buf, buflen);
}

static const cutil_GenericType CUTIL_GENERIC_TYPE_GENERIC_OBJECT_INSTANCE = {
  .name = "cutil_GenericObject",
  .size = sizeof(cutil_GenericObject),
  .init = &_cutil_GenericObject_init_default,
  .clear = &cutil_GenericObject_clear_generic,
  .copy = &cutil_GenericObject_copy_generic,
  .deep_equals = &cutil_GenericObject_deep_equals_generic,
  .comp = &cutil_GenericObject_compare_generic,
  .hash = &cutil_GenericObject_hash_generic,
  .to_string = &cutil_GenericObject_to_string_generic
};
const cutil_GenericType *const CUTIL_GENERIC_TYPE_GENERIC_OBJECT
  = &CUTIL_GENERIC_TYPE_GENERIC_OBJECT_INSTANCE;

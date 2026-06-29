#include <cutil/serial/serial.h>

#include <cutil/core/io/log.h>
#include <cutil/core/status.h>
#include <cutil/core/std/stdio.h>
#include <cutil/core/std/stdlib.h>
#include <cutil/core/std/string.h>
#include <cutil/core/util/macro.h>

cutil_SerialNode *
cutil_SerialNode_calloc(const cutil_SerialType *type)
{
    CUTIL_RETURN_NULL_IF_NULL(type);
    cutil_SerialNode *const node = CUTIL_CALLOC_OBJECT(node);
    node->type = type;
    node->node_type = CUTIL_SERIAL_NODE_UNINITIALIZED;
    node->key = NULL;
    node->value.string_view = NULL;
    node->node = NULL;
    return node;
}

cutil_SerialNode *
cutil_SerialNode_from_file(const cutil_SerialType *type, FILE *file)
{
    CUTIL_RETURN_NULL_IF_NULL(type);
    CUTIL_RETURN_NULL_IF_NULL(file);
    if (type->from_file == NULL) {
        cutil_log_error("Serial type does not support from_file");
        return NULL;
    }
    cutil_SerialNode *const node = type->from_file(file);
    if (node == NULL) {
        cutil_log_error("Failed to initialize %s node from file", type->name);
        return NULL;
    }
    return node;
}

cutil_SerialNode *
cutil_SerialNode_from_nstring(
  const cutil_SerialType *type, const char *str, size_t len
)
{
    CUTIL_RETURN_NULL_IF_NULL(type);
    CUTIL_RETURN_NULL_IF_NULL(str);
    if (type->from_nstring == NULL) {
        cutil_log_error("Serial type does not support from_nstring");
        return NULL;
    }
    cutil_SerialNode *const node = type->from_nstring(str, len);
    if (node == NULL) {
        cutil_log_error("Failed to initialize %s node from string", type->name);
        return NULL;
    }
    return node;
}

cutil_SerialNode *
cutil_SerialNode_from_string(const cutil_SerialType *type, const char *str)
{
    const size_t len = (str != NULL) ? strlen(str) : 0;
    return cutil_SerialNode_from_nstring(type, str, len);
}

extern inline cutil_SerialNode *
cutil_SerialNode_dup(const cutil_SerialNode *node);

extern inline void
cutil_SerialNode_free(cutil_SerialNode *node);

extern inline const cutil_SerialType *
cutil_SerialNode_get_serial_type(const cutil_SerialNode *node);

extern inline cutil_SerialNodeType
cutil_SerialNode_get_node_type(const cutil_SerialNode *node);

extern inline cutil_Bool
cutil_SerialNode_get_string_value(
  const cutil_SerialNode *node, const char **res
);

extern inline cutil_Bool
cutil_SerialNode_get_number_value(const cutil_SerialNode *node, double *res);

extern inline cutil_Bool
cutil_SerialNode_get_bool_value(const cutil_SerialNode *node, cutil_Bool *res);

extern inline size_t
cutil_SerialNode_get_sequence_length(const cutil_SerialNode *node);

extern inline cutil_Bool
cutil_SerialNode_get_sequence_item(
  const cutil_SerialNode *node, size_t idx, cutil_SerialNode *res
);

extern inline cutil_Bool
cutil_SerialNode_get_child(
  const cutil_SerialNode *node, const char *key, cutil_SerialNode *res
);

extern inline cutil_Bool
cutil_SerialNode_has_child(const cutil_SerialNode *node, const char *key);

extern inline cutil_SerialNode *
cutil_SerialNode_create_scalar(
  const cutil_SerialType *type,
  cutil_SerialNodeValueType vtype,
  const void *value
);

extern inline cutil_SerialNode *
cutil_SerialNode_create_sequence(
  const cutil_SerialType *type,
  const cutil_SerialNodeValueType *types,
  const void *const *values,
  size_t count
);

extern inline cutil_SerialNode *
cutil_SerialNode_create_mapping(const cutil_SerialType *type);

extern inline cutil_Bool
cutil_SerialNode_add_sequence_item(
  cutil_SerialNode *node, cutil_SerialNodeValueType vtype, const void *value
);

extern inline cutil_Bool
cutil_SerialNode_add_child(
  cutil_SerialNode *node, const char *key, cutil_SerialNode *child
);

extern inline char *
cutil_SerialNode_to_string(cutil_SerialNode *node, uint32_t write_opts);

const char *
cutil_SerialNode_get_string(
  const cutil_SerialNode *node, const char *key, const char *default_val
)
{
    CUTIL_RETURN_VAL_IF_NULL(node, default_val);
    cutil_SerialNode *const child = cutil_SerialNode_calloc(node->type);
    if (child == NULL) {
        return default_val;
    }
    if (!cutil_SerialNode_get_child(node, key, child)) {
        cutil_SerialNode_free(child);
        return default_val;
    }
    const char *str = NULL;
    const cutil_Bool got = cutil_SerialNode_get_string_value(child, &str);
    cutil_SerialNode_free(child);
    return (got && str != NULL) ? str : default_val;
}

double
cutil_SerialNode_get_double(
  const cutil_SerialNode *node, const char *key, double default_val
)
{
    CUTIL_RETURN_VAL_IF_NULL(node, default_val);
    cutil_SerialNode *const child = cutil_SerialNode_calloc(node->type);
    if (child == NULL) {
        return default_val;
    }
    if (!cutil_SerialNode_get_child(node, key, child)) {
        cutil_SerialNode_free(child);
        return default_val;
    }
    double val = default_val;
    cutil_SerialNode_get_number_value(child, &val);
    cutil_SerialNode_free(child);
    return val;
}

int
cutil_SerialNode_get_int(
  const cutil_SerialNode *node, const char *key, int default_val
)
{
    return (int) cutil_SerialNode_get_double(node, key, (double) default_val);
}

bool
cutil_SerialNode_get_bool(
  const cutil_SerialNode *node, const char *key, bool default_val
)
{
    CUTIL_RETURN_VAL_IF_NULL(node, default_val);
    cutil_SerialNode *const child = cutil_SerialNode_calloc(node->type);
    if (child == NULL) {
        return default_val;
    }
    if (!cutil_SerialNode_get_child(node, key, child)) {
        cutil_SerialNode_free(child);
        return default_val;
    }
    cutil_Bool val = default_val ? CUTIL_TRUE : CUTIL_FALSE;
    cutil_SerialNode_get_bool_value(child, &val);
    cutil_SerialNode_free(child);
    return val == CUTIL_TRUE;
}

cutil_Status
cutil_serial_read_file(
  const cutil_SerialType *type,
  void *obj,
  FILE *file,
  cutil_SerialReadCallback *cb
)
{
    if (cb == NULL) {
        cutil_log_error("Callback is NULL");
        return CUTIL_STATUS_FAILURE;
    }
    cutil_SerialNode *const node = cutil_SerialNode_from_file(type, file);
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_STATUS_FAILURE);
    const cutil_Status status = cb(obj, node);
    cutil_SerialNode_free(node);
    return status;
}

cutil_Status
cutil_serial_read_string(
  const cutil_SerialType *type,
  void *obj,
  const char *str,
  cutil_SerialReadCallback *cb
)
{
    if (cb == NULL) {
        cutil_log_error("Callback is NULL");
        return CUTIL_STATUS_FAILURE;
    }
    cutil_SerialNode *const node = cutil_SerialNode_from_string(type, str);
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_STATUS_FAILURE);
    const cutil_Status status = cb(obj, node);
    cutil_SerialNode_free(node);
    return status;
}

/**
 * Reads a serial string of known length and invokes the callback.
 *
 * @param[out] obj target object to populate
 * @param[in]  str serial string
 * @param[in]  len length of the string in bytes
 * @param[in]  cb  callback invoked with the parsed content
 *
 * @return the status returned by cb, other status on error
 */
cutil_Status
cutil_serial_read_nstring(
  const cutil_SerialType *type,
  void *obj,
  const char *str,
  size_t len,
  cutil_SerialReadCallback *cb
)
{
    if (cb == NULL) {
        cutil_log_error("Callback is NULL");
        return CUTIL_STATUS_FAILURE;
    }
    cutil_SerialNode *const node
      = cutil_SerialNode_from_nstring(type, str, len);
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_STATUS_FAILURE);
    const cutil_Status status = cb(obj, node);
    cutil_SerialNode_free(node);
    return status;
}

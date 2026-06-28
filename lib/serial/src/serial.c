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

extern inline cutil_Bool
cutil_SerialNode_set_string_value(cutil_SerialNode *node, const char *value);

extern inline cutil_Bool
cutil_SerialNode_set_number_value(cutil_SerialNode *node, double value);

extern inline cutil_Bool
cutil_SerialNode_set_bool_value(cutil_SerialNode *node, cutil_Bool value);

extern inline cutil_Bool
cutil_SerialNode_add_sequence_item(
  cutil_SerialNode *node, cutil_SerialNode *val
);

extern inline cutil_Bool
cutil_SerialNode_add_child(
  cutil_SerialNode *node, const char *key, const cutil_SerialNode *child
);

const char *
cutil_SerialNode_get_string(
  const cutil_SerialNode *node, const char *key, const char *default_val
)
{
    const char *const res = cutil_SerialNode_get_scalar_value_by_key(node, key);
    return (res != NULL) ? res : default_val;
}

#if 0

double
cutil_SerialNode_get_double(
  const cutil_SerialNode *node, const char *key, double default_val
)
{
    const char *const res = cutil_SerialNode_get_scalar_value_by_key(node, key);
    return (res != NULL) ? atof(res) : default_val;
}

int
cutil_SerialNode_get_int(
  const cutil_SerialNode *node, const char *key, int default_val
)
{
    const char *const res = cutil_SerialNode_get_scalar_value_by_key(node, key);
    return (res != NULL) ? atoi(res) : default_val;
}

bool
cutil_SerialNode_get_bool(
  const cutil_SerialNode *node, const char *key, bool default_val
)
{
    const char *const res = cutil_SerialNode_get_scalar_value_by_key(node, key);
    return (res != NULL) ? (cutil_atobool(res) == CUTIL_TRUE) : default_val;
}

#endif

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

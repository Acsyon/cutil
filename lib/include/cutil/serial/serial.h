/** cutil/serial/serial.h
 *
 * Header file for YAML parsing utilities.
 */

#ifndef CUTIL_SERIAL_SERIAL_H_INCLUDED
#define CUTIL_SERIAL_SERIAL_H_INCLUDED

#include <cutil/core/debug/null.h>
#include <cutil/core/status.h>
#include <cutil/core/std/inttypes.h>
#include <cutil/core/std/stdbool.h>
#include <cutil/core/std/stdio.h>
#include <cutil/core/util/macro.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Type of a serial node (scalar, sequence or composite).
 */
typedef enum {
    CUTIL_SERIAL_NODE_UNINITIALIZED = -2,
    CUTIL_SERIAL_NODE_UNDEFINED = -1,
    CUTIL_SERIAL_NODE_EMPTY,
    CUTIL_SERIAL_NODE_STRING,
    CUTIL_SERIAL_NODE_NUMBER,
    CUTIL_SERIAL_NODE_BOOL,
    CUTIL_SERIAL_NODE_SEQUENCE,
    CUTIL_SERIAL_NODE_COMPOSITE,
} cutil_SerialNodeType;

/**
 * Alias for cutil_SerialNodeType used as a scalar value type discriminator
 * in write operations. Only STRING, NUMBER, and BOOL values are meaningful
 * here.
 */
typedef cutil_SerialNodeType cutil_SerialNodeValueType;

/**
 * Forward declaration of opaque serial node type.
 */
typedef struct cutil_SerialNode cutil_SerialNode;

/**
 * Callback type for structured deserialization callback function.
 *
 * @param[out] obj  target object to populate
 * @param[in]  node parsed serial node to read from
 *
 * @return CUTIL_STATUS_SUCCESS on success, other status on error
 */
typedef cutil_Status
cutil_SerialReadCallback(void *obj, const cutil_SerialNode *node);

/**
 * Typedef for structured serialization callback function.
 *
 * @param[in] obj object to read values from
 *
 * @return newly allocated cutil_SerialNode on success, NULL on error
 */
typedef cutil_SerialNode *
cutil_SerialWriteCallback(const void *obj);

/**
 * Vtable for serial node operations. This allows for a common interface for
 * different serialization formats (e.g., YAML, JSON) and enables code reuse in
 * higher-level serialization utilities.
 */
typedef struct {
    const char *const name;
    cutil_SerialNode *(*const from_file)(FILE *file);
    cutil_SerialNode *(*const from_nstring)(const char *str, size_t len);
    cutil_SerialNode *(*const dup)(const cutil_SerialNode *node);
    void (*const free)(cutil_SerialNode *node);
    cutil_Bool (*const get_string_value)(
      const cutil_SerialNode *node, const char **res
    );
    cutil_Bool (*const get_number_value)(
      const cutil_SerialNode *node, double *res
    );
    cutil_Bool (*const get_bool_value)(
      const cutil_SerialNode *node, cutil_Bool *res
    );
    size_t (*const get_sequence_length)(const cutil_SerialNode *node);
    cutil_Bool (*const get_sequence_item)(
      const cutil_SerialNode *node, size_t idx, cutil_SerialNode *res
    );
    cutil_Bool (*const get_child)(
      const cutil_SerialNode *node, const char *key, cutil_SerialNode *res
    );
    /* WRITE operations — construct-then-add pattern */

    /**
     * Construct a standalone scalar node.
     * @param vtype  CUTIL_SERIAL_NODE_STRING, NUMBER, or BOOL
     * @param value  for STRING: const char*; NUMBER: const double*; BOOL: const
     * cutil_Bool*
     */
    cutil_SerialNode *(*const create_scalar)(
      cutil_SerialNodeValueType vtype, const void *value
    );

    /**
     * Construct a sequence pre-populated with primitive items.
     * @param types  array of value-type discriminators, length count
     * @param values array of void* pointers to each item's value, length count
     * @param count  number of items (0 creates an empty sequence)
     */
    cutil_SerialNode *(*const create_sequence)(
      const cutil_SerialNodeValueType *types,
      const void *const *values,
      size_t count
    );

    /**
     * Append one primitive item to an existing sequence node.
     * @param seq    the target sequence node
     * @param vtype  item type (STRING, NUMBER, or BOOL)
     * @param value  pointer to the item value (same convention as
     * create_scalar)
     */
    cutil_Bool (*const add_sequence_item)(
      cutil_SerialNode *seq, cutil_SerialNodeValueType vtype, const void *value
    );

    /**
     * Construct an empty mapping/object node.
     */
    cutil_SerialNode *(*const create_mapping)(void);

    /**
     * Attach a pre-built child node to a composite parent under key.
     * The child's content is copied into the parent's backend document.
     * Type validation: parent must be COMPOSITE, child may be any type.
     */
    cutil_Bool (*const add_child)(
      cutil_SerialNode *parent, const char *key, cutil_SerialNode *child
    );

    /**
     * Serialise the node tree to a newly allocated NUL-terminated string.
     * Caller must free() the returned pointer.
     * @param write_opts  bitmap of CUTIL_SERIAL_WRITE_OPT_* flags (see
     * serial.h)
     */
    char *(*const to_string)(cutil_SerialNode *node, uint32_t write_opts);
} cutil_SerialType;

/*
union cutil_SerialNodeScalarValue {
    const char *string_view;
    char *string;
    double number;
    cutil_Bool boolean;
};
*/

/**
 * Opaque serial node type.
 */
struct cutil_SerialNode {
    const cutil_SerialType *type;
    cutil_SerialNodeType node_type;
    char *key;
    union {
        const char *string_view;
        char *string;
        double number;
        cutil_Bool boolean;
    } value;
    void *node;
};

/**
 * Convenience MACRO to check for NULLs in `SERIAL` in debug mode.
 *
 * @param[in] SERIAL cutil_SerialDocument or cutil_SerialNode to check for NULLs
 */
#define CUTIL_NULL_CHECKS_SERIAL(SERIAL)                                       \
    do {                                                                       \
        CUTIL_NULL_CHECK(SERIAL);                                              \
        CUTIL_NULL_CHECK(SERIAL->type);                                        \
    } while (0)

/**
 * Allocates and zero-initializes a new cutil_SerialNode object of type `type`.
 *
 * @param[in] type type of the serial node to allocate
 *
 * @return newly allocated cutil_SerialNode
 */
cutil_SerialNode *
cutil_SerialNode_calloc(const cutil_SerialType *type);

/**
 * Initializes a cutil_SerialNode from a file.
 *
 * @param[in] file open FILE handle to read from
 *
 * @return newly allocated cutil_SerialNode on success, NULL on error
 */
cutil_SerialNode *
cutil_SerialNode_from_file(const cutil_SerialType *type, FILE *file);

/**
 * Initializes a cutil_SerialNode from a string of known length.
 *
 * @param[in] str string to parse
 * @param[in] len length of the string in bytes
 *
 * @return newly allocated cutil_SerialNode on success, NULL on error
 */
cutil_SerialNode *
cutil_SerialNode_from_nstring(
  const cutil_SerialType *type, const char *str, size_t len
);

/**
 * Initializes a cutil_SerialNode from a NUL-terminated string.
 *
 * @param[in] str NUL-terminated string to parse
 *
 * @return newly allocated cutil_SerialNode on success, NULL on error
 */
cutil_SerialNode *
cutil_SerialNode_from_string(const cutil_SerialType *type, const char *str);

/**
 * Duplicates a cutil_SerialNode object, including its content.
 *
 * @param[in] node cutil_SerialNode to duplicate
 *
 * @return newly allocated duplicate of node, or NULL on error
 */
inline cutil_SerialNode *
cutil_SerialNode_dup(const cutil_SerialNode *node)
{
    CUTIL_RETURN_NULL_IF_NULL(node);
    CUTIL_NULL_CHECKS_SERIAL(node);
    CUTIL_NULL_CHECK_VTABLE(node->type, dup);
    return node->type->dup(node);
}

/**
 * Frees an cutil_SerialNode object. No-op on NULL.
 *
 * @param[in] node object to be freed
 */
inline void
cutil_SerialNode_free(cutil_SerialNode *node)
{
    CUTIL_RETURN_IF_NULL(node);
    CUTIL_NULL_CHECKS_SERIAL(node);
    CUTIL_NULL_CHECK_VTABLE(node->type, free);
    node->type->free(node);
}

/**
 * Gets the cutil_SerialType of a cutil_SerialNode node.
 *
 * @param[in] node serial node to get type of
 *
 * @return cutil_SerialType of the node, or NULL if node is NULL
 */
inline const cutil_SerialType *
cutil_SerialNode_get_serial_type(const cutil_SerialNode *node)
{
    CUTIL_RETURN_NULL_IF_NULL(node);
    return node->type;
}

/**
 * Gets the type of a serial node.
 *
 * @param[in] node serial node to get type of
 *
 * @return type of the node, or CUTIL_SERIAL_NODE_UNDEFINED if node is NULL
 */
inline cutil_SerialNodeType
cutil_SerialNode_get_node_type(const cutil_SerialNode *node)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_SERIAL_NODE_UNDEFINED);
    return node->node_type;
}

/**
 * Gets the scalar string value of a serial node.
 *
 * @param[in]  node serial node to get string value of
 * @param[out] res  output buffer to store the string value, may be NULL
 *
 * @return CUTIL_TRUE if node contains a string value, CUTIL_FALSE otherwise
 */
inline cutil_Bool
cutil_SerialNode_get_string_value(
  const cutil_SerialNode *node, const char **res
)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_FALSE);
    CUTIL_NULL_CHECKS_SERIAL(node);
    CUTIL_NULL_CHECK_VTABLE(node->type, get_string_value);
    return node->type->get_string_value(node, res);
}

/**
 * Gets the scalar number value of a serial node as a double.
 *
 * @param[in]  node serial node to get number value of
 * @param[out] res  output buffer to store the number value, may be NULL
 *
 * @return CUTIL_TRUE if node contains a number value, CUTIL_FALSE otherwise
 */
inline cutil_Bool
cutil_SerialNode_get_number_value(const cutil_SerialNode *node, double *res)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_FALSE);
    CUTIL_NULL_CHECKS_SERIAL(node);
    CUTIL_NULL_CHECK_VTABLE(node->type, get_number_value);
    return node->type->get_number_value(node, res);
}

/**
 * Gets the scalar bool value of a serial node.
 *
 * @param[in]  node serial node to get bool value of
 * @param[out] res  output buffer to store the bool value, may be NULL
 *
 * @return CUTIL_TRUE if node contains a bool value, CUTIL_FALSE otherwise
 */
inline cutil_Bool
cutil_SerialNode_get_bool_value(const cutil_SerialNode *node, cutil_Bool *res)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_FALSE);
    CUTIL_NULL_CHECKS_SERIAL(node);
    CUTIL_NULL_CHECK_VTABLE(node->type, get_bool_value);
    return node->type->get_bool_value(node, res);
}

/**
 * Returns the number of items in a serial sequence node.
 *
 * @param[in] node serial node to get sequence length from
 *
 * @return number of items in the sequence, or 0 if node is NULL or not a
 *         sequence node
 */
inline size_t
cutil_SerialNode_get_sequence_length(const cutil_SerialNode *node)
{
    CUTIL_RETURN_VAL_IF_NULL(node, 0UL);
    CUTIL_NULL_CHECKS_SERIAL(node);
    CUTIL_NULL_CHECK_VTABLE(node->type, get_sequence_length);
    return node->type->get_sequence_length(node);
}

/**
 * Gets a sequence item by index from a serial sequence node.
 *
 * @param[in]  node serial sequence node to get item from
 * @param[in]  idx  index of the item to get (0-based)
 * @param[out] res  output buffer to store the item node, may be NULL
 *
 * @return CUTIL_TRUE if item is found, CUTIL_FALSE otherwise (NULL node,
 *         non-sequence node, or out-of-bounds index)
 */
inline cutil_Bool
cutil_SerialNode_get_sequence_item(
  const cutil_SerialNode *node, size_t idx, cutil_SerialNode *res
)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_FALSE);
    CUTIL_NULL_CHECKS_SERIAL(node);
    CUTIL_NULL_CHECK_VTABLE(node->type, get_sequence_item);
    return node->type->get_sequence_item(node, idx, res);
}

/**
 * Gets a child node by key from a serial node.
 *
 * @param[in] node parent node to get child node from
 * @param[in] key key of the child node to get
 * @param[out] res output buffer to store the child node, may be NULL
 *
 * @return CUTIL_TRUE if child node is found, CUTIL_FALSE otherwise
 */
inline cutil_Bool
cutil_SerialNode_get_child(
  const cutil_SerialNode *node, const char *key, cutil_SerialNode *res
)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_FALSE);
    CUTIL_NULL_CHECKS_SERIAL(node);
    CUTIL_NULL_CHECK_VTABLE(node->type, get_child);
    return node->type->get_child(node, key, res);
}

/**
 * Checks if a serial node has a child node with the given key.
 *
 * @param[in] node parent node to check for child node
 * @param[in] key key of the child node to check for
 *
 * @return CUTIL_TRUE if child node is found, CUTIL_FALSE otherwise
 */
inline cutil_Bool
cutil_SerialNode_has_child(const cutil_SerialNode *node, const char *key)
{
    return cutil_SerialNode_get_child(node, key, NULL);
}

inline cutil_SerialNode *
cutil_SerialNode_create_scalar(
  const cutil_SerialType *type,
  cutil_SerialNodeValueType vtype,
  const void *value
)
{
    CUTIL_RETURN_NULL_IF_NULL(type);
    CUTIL_NULL_CHECK_VTABLE(type, create_scalar);
    return type->create_scalar(vtype, value);
}

inline cutil_SerialNode *
cutil_SerialNode_create_sequence(
  const cutil_SerialType *type,
  const cutil_SerialNodeValueType *types,
  const void *const *values,
  size_t count
)
{
    CUTIL_RETURN_NULL_IF_NULL(type);
    CUTIL_NULL_CHECK_VTABLE(type, create_sequence);
    return type->create_sequence(types, values, count);
}

inline cutil_SerialNode *
cutil_SerialNode_create_mapping(const cutil_SerialType *type)
{
    CUTIL_RETURN_NULL_IF_NULL(type);
    CUTIL_NULL_CHECK_VTABLE(type, create_mapping);
    return type->create_mapping();
}

inline cutil_Bool
cutil_SerialNode_add_sequence_item(
  cutil_SerialNode *node, cutil_SerialNodeValueType vtype, const void *value
)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_FALSE);
    CUTIL_NULL_CHECKS_SERIAL(node);
    CUTIL_NULL_CHECK_VTABLE(node->type, add_sequence_item);
    return node->type->add_sequence_item(node, vtype, value);
}

inline cutil_Bool
cutil_SerialNode_add_child(
  cutil_SerialNode *node, const char *key, cutil_SerialNode *child
)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_FALSE);
    CUTIL_NULL_CHECKS_SERIAL(node);
    CUTIL_NULL_CHECK_VTABLE(node->type, add_child);
    return node->type->add_child(node, key, child);
}

inline char *
cutil_SerialNode_to_string(cutil_SerialNode *node, uint32_t write_opts)
{
    CUTIL_RETURN_NULL_IF_NULL(node);
    CUTIL_NULL_CHECKS_SERIAL(node);
    CUTIL_NULL_CHECK_VTABLE(node->type, to_string);
    return node->type->to_string(node, write_opts);
}

/**
 * Reads a file and invokes the callback with the parsed content.
 *
 * @param[in]  type  type of the serial format to read
 * @param[out] obj   target object to populate
 * @param[in]  file  open FILE handle to read from
 * @param[in]  cb    callback invoked with the parsed content
 *
 * @return the status returned by cb, other status on error
 */
cutil_Status
cutil_serial_read_file(
  const cutil_SerialType *type,
  void *obj,
  FILE *file,
  cutil_SerialReadCallback *cb
);

/**
 * Reads a NUL-terminated serial string and invokes the callback.
 *
 * @param[in]  type type of the serial format to read
 * @param[out] obj  target object to populate
 * @param[in]  str  NUL-terminated serial string
 * @param[in]  cb   callback invoked with the parsed content
 *
 * @return the status returned by cb, other status on error
 */
cutil_Status
cutil_serial_read_string(
  const cutil_SerialType *type,
  void *obj,
  const char *str,
  cutil_SerialReadCallback *cb
);

/**
 * Reads a serial string of known length and invokes the callback.
 *
 * @param[in]  type type of the serial format to read
 * @param[out] obj  target object to populate
 * @param[in]  str  serial string
 * @param[in]  len  length of the string in bytes
 * @param[in]  cb   callback invoked with the parsed content
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
);

/**
 * Gets a string child value by key. Returns default_val if not found or
 * the value is not a string.
 */
const char *
cutil_SerialNode_get_string(
  const cutil_SerialNode *node, const char *key, const char *default_val
);

/**
 * Gets a numeric child value by key, returned as double.
 * Returns default_val if not found or not a number.
 */
double
cutil_SerialNode_get_double(
  const cutil_SerialNode *node, const char *key, double default_val
);

/**
 * Gets a numeric child value by key, returned as int (truncates double).
 * Returns default_val if not found or not a number.
 */
int
cutil_SerialNode_get_int(
  const cutil_SerialNode *node, const char *key, int default_val
);

/**
 * Gets a boolean child value by key.
 * Returns default_val if not found or not a bool.
 */
bool
cutil_SerialNode_get_bool(
  const cutil_SerialNode *node, const char *key, bool default_val
);

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_SERIAL_SERIAL_H_INCLUDED */

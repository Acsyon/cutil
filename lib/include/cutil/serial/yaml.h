/** cutil/serial/yaml.h
 *
 * Header file for YAML parsing utilities.
 */

#ifndef CUTIL_SERIAL_YAML_H_INCLUDED
#define CUTIL_SERIAL_YAML_H_INCLUDED

#include <cutil/core/status.h>
#include <cutil/core/std/stdbool.h>
#include <cutil/core/std/stddef.h>
#include <cutil/core/std/stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CUTIL_YAML_NODE_SCALAR,
    CUTIL_YAML_NODE_SEQUENCE,
    CUTIL_YAML_NODE_COMPOSITE,
} cutil_YamlNodeType;

/**
 * Opaque YAML document type.
 */
typedef struct cutil_Yaml cutil_Yaml;

/**
 * Opaque YAML node type.
 */
typedef struct cutil_YamlNode cutil_YamlNode;

/**
 * Callback type for structured YAML reading.
 *
 * @param[out] obj  target object to populate
 * @param[in]  node parsed YAML node to read from
 *
 * @return CUTIL_STATUS_SUCCESS on success, other status on error
 */
typedef cutil_Status
cutil_YamlReadCallback(void *obj, const cutil_YamlNode *node);

/**
 * Parses a YAML file into a newly allocated cutil_Yaml object.
 *
 * @param[in] file open FILE handle to read from
 *
 * @return newly allocated cutil_Yaml or NULL
 */
cutil_Yaml *
cutil_Yaml_from_file(FILE *file);

/**
 * Parses a NUL-terminated YAML string into a newly allocated cutil_Yaml object.
 *
 * @param[in] str NUL-terminated YAML string to parse
 *
 * @return newly allocated cutil_Yaml or NULL
 */
cutil_Yaml *
cutil_Yaml_from_string(const char *str);

/**
 * Parses a YAML string of known length into a newly allocated cutil_Yaml
 * object.
 *
 * @param[in] str YAML string to parse
 * @param[in] len length of the string in bytes
 *
 * @return newly allocated cutil_Yaml or NULL
 */
cutil_Yaml *
cutil_Yaml_from_nstring(const char *str, size_t len);

/**
 * Frees an cutil_Yaml object. No-op on NULL.
 *
 * @param[in] yaml object to be freed
 */
void
cutil_Yaml_free(cutil_Yaml *yaml);

/**
 * Gets the root node of a parsed YAML object.
 *
 * @param[in] yaml parsed YAML object
 * @param[out] res output buffer to store the root node, may be NULL
 *
 * @return CUTIL_TRUE if root node is found, CUTIL_FALSE otherwise
 */
cutil_Bool
cutil_Yaml_get_root_node(const cutil_Yaml *yaml, cutil_YamlNode *res);

/**
 * Reads a YAML file and invokes the callback with the parsed content.
 *
 * @param[out] obj  target object to populate
 * @param[in]  file open FILE handle to read from
 * @param[in]  cb   callback invoked with the parsed content
 *
 * @return the status returned by cb, other status on error
 */
cutil_Status
cutil_Yaml_read_file(void *obj, FILE *file, cutil_YamlReadCallback *cb);

/**
 * Reads a NUL-terminated YAML string and invokes the callback.
 *
 * @param[out] obj target object to populate
 * @param[in]  str NUL-terminated YAML string
 * @param[in]  cb  callback invoked with the parsed content
 *
 * @return the status returned by cb, other status on error
 */
cutil_Status
cutil_Yaml_read_string(void *obj, const char *str, cutil_YamlReadCallback *cb);

/**
 * Reads a YAML string of known length and invokes the callback.
 *
 * @param[out] obj target object to populate
 * @param[in]  str YAML string
 * @param[in]  len length of the string in bytes
 * @param[in]  cb  callback invoked with the parsed content
 *
 * @return the status returned by cb, other status on error
 */
cutil_Status
cutil_Yaml_read_nstring(
  void *obj, const char *str, size_t len, cutil_YamlReadCallback *cb
);

/**
 * Allocates and zero-initializes a new cutil_YamlNode object.
 *
 * @return newly allocated cutil_YamlNode
 */
cutil_YamlNode *
cutil_YamlNode_calloc(void);

/**
 * Frees an cutil_YamlNode object. No-op on NULL.
 *
 * @param[in] node object to be freed
 */
void
cutil_YamlNode_free(cutil_YamlNode *node);

/**
 * Gets the type of a YAML node.
 *
 * @param[in] node YAML node to get type of
 *
 * @return type of the node, or CUTIL_YAML_NODE_SCALAR if node is NULL
 *         (since NULL is often used to indicate missing nodes, treating it as
 *         a scalar with default value is more convenient for callers)
 */
cutil_YamlNodeType
cutil_YamlNode_get_type(const cutil_YamlNode *node);

/**
 * Gets a child node by key from a YAML node from parsed document.
 *
 * @param[in] node parent node to get child node from
 * @param[in] key key of the child node to get
 * @param[out] res output buffer to store the child node, may be NULL
 *
 * @return CUTIL_TRUE if child node is found, CUTIL_FALSE otherwise
 */
cutil_Bool
cutil_YamlNode_get_child(
  const cutil_YamlNode *node, const char *key, cutil_YamlNode *res
);

/**
 * Checks if a YAML node has a child node with the given key.
 *
 * @param[in] node parent node to check for child node
 * @param[in] key key of the child node to check for
 *
 * @return CUTIL_TRUE if child node is found, CUTIL_FALSE otherwise
 */
inline cutil_Bool
cutil_YamlNode_has_child(const cutil_YamlNode *node, const char *key)
{
    return cutil_YamlNode_get_child(node, key, NULL);
}

/**
 * Reads a string value by key.
 *
 * @param[in] node        YAML node to get value from
 * @param[in] key         key to look up
 * @param[in] default_val value returned when key is not found or node is NULL
 *
 * @return value for key or default_val
 */
const char *
cutil_YamlNode_get_string(
  const cutil_YamlNode *node, const char *key, const char *default_val
);

/**
 * Reads a double value by key.
 *
 * @param[in] node        YAML node to get value from
 * @param[in] key         key to look up
 * @param[in] default_val value returned when key is not found or node is NULL
 *
 * @return value for key or default_val
 */
double
cutil_YamlNode_get_double(
  const cutil_YamlNode *node, const char *key, double default_val
);

/**
 * Reads an int value by key.
 *
 * @param[in] node        YAML node to get value from
 * @param[in] key         key to look up
 * @param[in] default_val value returned when key is not found or node is NULL
 *
 * @return value for key or default_val
 */
int
cutil_YamlNode_get_int(
  const cutil_YamlNode *node, const char *key, int default_val
);

/**
 * Reads a bool value by key.
 *
 * @param[in] node        YAML node to get value from
 * @param[in] key         key to look up
 * @param[in] default_val value returned when key is not found or node is NULL
 *
 * @return value for key or default_val
 */
bool
cutil_YamlNode_get_bool(
  const cutil_YamlNode *node, const char *key, bool default_val
);

/**
 * Returns the number of items in a YAML sequence node.
 *
 * @param[in] node YAML node to get sequence length from
 *
 * @return number of items in the sequence, or 0 if node is NULL or not a
 *         sequence node
 */
size_t
cutil_YamlNode_get_sequence_length(const cutil_YamlNode *node);

/**
 * Gets a sequence item by index from a YAML sequence node.
 *
 * @param[in]  node YAML sequence node to get item from
 * @param[in]  idx  index of the item to get (0-based)
 * @param[out] res  output buffer to store the item node, may be NULL
 *
 * @return CUTIL_TRUE if item is found, CUTIL_FALSE otherwise (NULL node,
 *         non-sequence node, or out-of-bounds index)
 */
cutil_Bool
cutil_YamlNode_get_sequence_item(
  const cutil_YamlNode *node, size_t idx, cutil_YamlNode *res
);

/**
 * Gets the scalar string value of a YAML scalar node.
 *
 * Unlike cutil_YamlNode_get_string which looks up a value by key within a
 * mapping node, this function returns the string value of the node itself.
 * This is intended for use with scalar sequence items.
 *
 * @param[in] node        YAML scalar node
 * @param[in] default_val value returned when node is NULL or not a scalar
 *
 * @return scalar string value or default_val
 */
const char *
cutil_YamlNode_get_scalar_value(
  const cutil_YamlNode *node, const char *default_val
);

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_SERIAL_YAML_H_INCLUDED */

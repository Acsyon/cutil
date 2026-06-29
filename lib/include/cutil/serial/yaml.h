/** cutil/serial/yaml.h
 *
 * Header file for YAML parsing utilities.
 */

#ifndef CUTIL_SERIAL_YAML_H_INCLUDED
#define CUTIL_SERIAL_YAML_H_INCLUDED

#include <cutil/serial/serial.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * cutil_SerialType for YAML serial nodes.
 */
extern const cutil_SerialType *const CUTIL_SERIAL_TYPE_YAML;

/**
 * Helper function to allocate a cutil_SerialNode with the YAML type.
 *
 * @return newly allocated cutil_SerialNode with YAML type, or NULL on error
 */
inline cutil_SerialNode *
cutil_SerialNode_calloc_yaml(void)
{
    return cutil_SerialNode_calloc(CUTIL_SERIAL_TYPE_YAML);
}

/**
 * Creates a new root YAML mapping node (empty document).
 *
 * @return newly allocated root cutil_SerialNode, or NULL on error
 */
cutil_SerialNode *
cutil_SerialNode_yaml_create_root(void);

/**
 * Creates a new YAML scalar string node.
 */
cutil_SerialNode *
cutil_SerialNode_yaml_create_string(const char *value);

/**
 * Creates a new YAML scalar number node.
 */
cutil_SerialNode *
cutil_SerialNode_yaml_create_number(double value);

/**
 * Creates a new YAML scalar boolean node.
 */
cutil_SerialNode *
cutil_SerialNode_yaml_create_bool(cutil_Bool value);

/**
 * Creates a new empty YAML sequence node.
 */
cutil_SerialNode *
cutil_SerialNode_yaml_create_empty_sequence(void);

#ifdef __cplusplus
}
#endif

#endif /* CUTIL_SERIAL_YAML_H_INCLUDED */

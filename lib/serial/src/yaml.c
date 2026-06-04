#include <cutil/serial/yaml.h>

#include <yaml.h>

#include <cutil/core/io/log.h>
#include <cutil/core/status.h>
#include <cutil/core/std/stdio.h>
#include <cutil/core/std/stdlib.h>
#include <cutil/core/std/string.h>
#include <cutil/core/util/macro.h>

struct cutil_Yaml {
    yaml_document_t doc;
};

struct cutil_YamlNode {
    const yaml_document_t *doc;
    yaml_node_t node;
};

cutil_Yaml *
cutil_Yaml_from_file(FILE *file)
{
    if (file == NULL) {
        cutil_log_error("YAML file handle is NULL");
        return NULL;
    }

    cutil_Yaml *const yaml = CUTIL_MALLOC_OBJECT(yaml);

    yaml_parser_t parser;
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, file);
    yaml_parser_load(&parser, &yaml->doc);
    yaml_parser_delete(&parser);

    return yaml;
}

cutil_Yaml *
cutil_Yaml_from_string(const char *str)
{
    if (str == NULL) {
        cutil_log_error("YAML string is NULL");
        return NULL;
    }

    cutil_Yaml *const yaml = CUTIL_CALLOC_OBJECT(yaml);

    yaml_parser_t parser;
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_string(
      &parser, (const unsigned char *) str, strlen(str)
    );
    if (!yaml_parser_load(&parser, &yaml->doc)) {
        yaml_parser_delete(&parser);
        yaml_document_delete(&yaml->doc);
        free(yaml);
        return NULL;
    }
    yaml_parser_delete(&parser);

    return yaml;
}

cutil_Yaml *
cutil_Yaml_from_nstring(const char *str, size_t len)
{
    if (str == NULL) {
        cutil_log_error("YAML string is NULL");
        return NULL;
    }

    cutil_Yaml *const yaml = CUTIL_CALLOC_OBJECT(yaml);

    yaml_parser_t parser;
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_string(&parser, (const unsigned char *) str, len);
    if (!yaml_parser_load(&parser, &yaml->doc)) {
        yaml_parser_delete(&parser);
        yaml_document_delete(&yaml->doc);
        free(yaml);
        return NULL;
    }
    yaml_parser_delete(&parser);

    return yaml;
}

void
cutil_Yaml_free(cutil_Yaml *yaml)
{
    CUTIL_RETURN_IF_NULL(yaml);

    yaml_document_delete(&yaml->doc);

    free(yaml);
}

cutil_Bool
cutil_Yaml_get_root_node(const cutil_Yaml *yaml, cutil_YamlNode *res)
{
    CUTIL_RETURN_VAL_IF_NULL(yaml, CUTIL_FALSE);
    yaml_document_t *const doc = (yaml_document_t *) &yaml->doc;
    yaml_node_t *const node = yaml_document_get_root_node(doc);
    if (node == NULL) {
        return CUTIL_FALSE;
    }
    if (res != NULL) {
        res->doc = doc;
        res->node = *node;
    }
    return CUTIL_TRUE;
}

cutil_Status
cutil_Yaml_read_file(void *obj, FILE *file, cutil_YamlReadCallback *cb)
{
    cutil_Yaml *const yaml = cutil_Yaml_from_file(file);
    if (yaml == NULL) {
        cutil_log_error("Failed to read YAML from file");
        return CUTIL_STATUS_FAILURE;
    }
    if (cb == NULL) {
        cutil_log_error("Callback is NULL");
        cutil_Yaml_free(yaml);
        return CUTIL_STATUS_FAILURE;
    }
    cutil_YamlNode buf;
    cutil_Yaml_get_root_node(yaml, &buf);
    const cutil_Status status = cb(obj, &buf);
    cutil_Yaml_free(yaml);
    return status;
}

cutil_Status
cutil_Yaml_read_string(void *obj, const char *str, cutil_YamlReadCallback *cb)
{
    cutil_Yaml *const yaml = cutil_Yaml_from_string(str);
    if (yaml == NULL) {
        cutil_log_error("Failed to read YAML from string");
        return CUTIL_STATUS_FAILURE;
    }
    if (cb == NULL) {
        cutil_log_error("Callback is NULL");
        cutil_Yaml_free(yaml);
        return CUTIL_STATUS_FAILURE;
    }
    cutil_YamlNode buf;
    cutil_Yaml_get_root_node(yaml, &buf);
    const cutil_Status status = cb(obj, &buf);
    cutil_Yaml_free(yaml);
    return status;
}

cutil_Status
cutil_Yaml_read_nstring(
  void *obj, const char *str, size_t len, cutil_YamlReadCallback *cb
)
{
    cutil_Yaml *const yaml = cutil_Yaml_from_nstring(str, len);
    if (yaml == NULL) {
        cutil_log_error("Failed to read YAML from string");
        return CUTIL_STATUS_FAILURE;
    }
    if (cb == NULL) {
        cutil_log_error("Callback is NULL");
        cutil_Yaml_free(yaml);
        return CUTIL_STATUS_FAILURE;
    }
    cutil_YamlNode buf;
    cutil_Yaml_get_root_node(yaml, &buf);
    const cutil_Status status = cb(obj, &buf);
    cutil_Yaml_free(yaml);
    return status;
}

cutil_YamlNode *
cutil_YamlNode_calloc(void)
{
    cutil_YamlNode *const node = CUTIL_CALLOC_OBJECT(node);
    return node;
}

void
cutil_YamlNode_free(cutil_YamlNode *node)
{
    CUTIL_RETURN_IF_NULL(node);
    free(node);
}

cutil_YamlNodeType
cutil_YamlNode_get_type(const cutil_YamlNode *node)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_YAML_NODE_SCALAR);
    switch (node->node.type) {
    case YAML_SCALAR_NODE:
        return CUTIL_YAML_NODE_SCALAR;
    case YAML_SEQUENCE_NODE:
        return CUTIL_YAML_NODE_SEQUENCE;
    case YAML_MAPPING_NODE:
        return CUTIL_YAML_NODE_COMPOSITE;
    default:
        return CUTIL_YAML_NODE_SCALAR;
    }
}

static yaml_node_t *
sf_cutil_YamlNode_get_child_by_key(const cutil_YamlNode *ynode, const char *key)
{
    CUTIL_RETURN_NULL_IF_NULL(ynode);
    CUTIL_RETURN_NULL_IF_NULL(key);
    yaml_document_t *const doc = (yaml_document_t *) ynode->doc;
    const yaml_node_t *const node = &ynode->node;
    if (node == NULL || node->type != YAML_MAPPING_NODE) {
        return NULL;
    }
    for (yaml_node_pair_t *pair = node->data.mapping.pairs.start;
         pair < node->data.mapping.pairs.top; ++pair)
    {
        yaml_node_t *const key_node = yaml_document_get_node(doc, pair->key);
        if (key_node != NULL && key_node->type == YAML_SCALAR_NODE) {
            if (strcmp((const char *) key_node->data.scalar.value, key) == 0) {
                return yaml_document_get_node(doc, pair->value);
            }
        }
    }
    return NULL;
}

cutil_Bool
cutil_YamlNode_get_child(
  const cutil_YamlNode *node, const char *key, cutil_YamlNode *res
)
{
    yaml_node_t *const child = sf_cutil_YamlNode_get_child_by_key(node, key);
    if (child == NULL) {
        return CUTIL_FALSE;
    }
    if (res != NULL) {
        res->doc = node->doc;
        res->node = *child;
    }
    return CUTIL_TRUE;
}

extern inline cutil_Bool
cutil_YamlNode_has_child(const cutil_YamlNode *node, const char *key);

static const char *
sf_get_scalar_node(const yaml_node_t *node)
{
    if (node == NULL || node->type != YAML_SCALAR_NODE) {
        return NULL;
    }
    return (const char *) node->data.scalar.value;
}

static const char *
sf_cutil_YamlNode_get_scalar_value_by_key(
  const cutil_YamlNode *node, const char *key
)
{
    const yaml_node_t *const child
      = sf_cutil_YamlNode_get_child_by_key(node, key);
    if (child == NULL || child->type != YAML_SCALAR_NODE) {
        return NULL;
    }
    return sf_get_scalar_node(child);
}

const char *
cutil_YamlNode_get_string(
  const cutil_YamlNode *node, const char *key, const char *default_val
)
{
    const char *const res
      = sf_cutil_YamlNode_get_scalar_value_by_key(node, key);
    return (res != NULL) ? res : default_val;
}

double
cutil_YamlNode_get_double(
  const cutil_YamlNode *node, const char *key, double default_val
)
{
    const char *const res
      = sf_cutil_YamlNode_get_scalar_value_by_key(node, key);
    return (res != NULL) ? atof(res) : default_val;
}

int
cutil_YamlNode_get_int(
  const cutil_YamlNode *node, const char *key, int default_val
)
{
    const char *const res
      = sf_cutil_YamlNode_get_scalar_value_by_key(node, key);
    return (res != NULL) ? atoi(res) : default_val;
}

static bool
_atobool(const char *str)
{
    if (atoi(str) != 0) {
        return true;
    }
    return (
      strcmp(str, "true") == 0 || strcmp(str, "True") == 0
      || strcmp(str, "TRUE") == 0
    );
}

bool
cutil_YamlNode_get_bool(
  const cutil_YamlNode *node, const char *key, bool default_val
)
{
    const char *const res
      = sf_cutil_YamlNode_get_scalar_value_by_key(node, key);
    return (res != NULL) ? _atobool(res) : default_val;
}

size_t
cutil_YamlNode_get_sequence_length(const cutil_YamlNode *node)
{
    CUTIL_RETURN_VAL_IF_NULL(node, 0UL);
    const yaml_node_t *const ynode = &node->node;
    if (ynode->type != YAML_SEQUENCE_NODE) {
        return 0UL;
    }
    return (size_t) (ynode->data.sequence.items.top
                     - ynode->data.sequence.items.start);
}

cutil_Bool
cutil_YamlNode_get_sequence_item(
  const cutil_YamlNode *node, size_t idx, cutil_YamlNode *res
)
{
    if (idx >= cutil_YamlNode_get_sequence_length(node)) {
        return CUTIL_FALSE;
    }
    const yaml_node_t *const ynode = &node->node;
    yaml_document_t *const doc = (yaml_document_t *) node->doc;
    const yaml_node_item_t item = ynode->data.sequence.items.start[idx];
    yaml_node_t *const item_node = yaml_document_get_node(doc, item);
    if (item_node == NULL) {
        return CUTIL_FALSE;
    }
    if (res != NULL) {
        res->doc = node->doc;
        res->node = *item_node;
    }
    return CUTIL_TRUE;
}

const char *
cutil_YamlNode_get_scalar_value(
  const cutil_YamlNode *node, const char *default_val
)
{
    if (node == NULL || node->node.type != YAML_SCALAR_NODE) {
        return default_val;
    }
    return sf_get_scalar_node(&node->node);
}

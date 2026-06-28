#include <cutil/serial/yaml.h>

#include <yaml.h>

#include <cutil/core/debug/null.h>
#include <cutil/core/io/log.h>
#include <cutil/core/status.h>
#include <cutil/core/std/stdbool.h>
#include <cutil/core/std/stdio.h>
#include <cutil/core/std/stdlib.h>
#include <cutil/core/std/string.h>
#include <cutil/core/util/macro.h>
#include <cutil/serial/serial.h>

typedef struct {
    yaml_document_t *doc;
    yaml_node_t node;
    cutil_Bool is_root;
} s_YamlNode;

static s_YamlNode *
sf_YamlNode_calloc(void)
{
    s_YamlNode *const ynode = CUTIL_CALLOC_OBJECT(ynode);
    return ynode;
}

static void
sf_YamlNode_free(s_YamlNode *ynode)
{
    CUTIL_RETURN_IF_NULL(ynode);
    if (ynode->is_root) {
        yaml_document_delete(ynode->doc);
        free(ynode->doc);
    }
    free(ynode);
}

static cutil_SerialNode *
sf_cutil_SerialNode_yaml_create(s_YamlNode *ynode)
{
    cutil_SerialNode *const node = cutil_SerialNode_calloc_yaml();
    if (node == NULL) {
        sf_YamlNode_free(ynode);
        return NULL;
    }
    node->node = ynode;
    return node;
}

typedef void
sf_parser_init_func(yaml_parser_t *parser, void *params);

static void
sf_parser_init_from_file(yaml_parser_t *parser, void *params)
{
    FILE *const file = params;
    yaml_parser_set_input_file(parser, file);
}

struct s_InitParamsStr {
    size_t len;
    const unsigned char *str;
};

static void
sf_parser_init_from_string(yaml_parser_t *parser, void *params)
{
    struct s_InitParamsStr *const init_params = params;
    yaml_parser_set_input_string(parser, init_params->str, init_params->len);
}

static cutil_SerialNode *
sf_cutil_SerialNode_yaml_init(sf_parser_init_func *init_func, void *params)
{
    yaml_parser_t parser;
    yaml_parser_initialize(&parser);
    init_func(&parser, params);
    s_YamlNode *const ynode = sf_YamlNode_calloc();
    ynode->doc = CUTIL_MALLOC_OBJECT(ynode->doc);
    if (!yaml_parser_load(&parser, ynode->doc)) {
        yaml_parser_delete(&parser);
        free(ynode->doc);
        free(ynode);
        return NULL;
    }
    yaml_parser_delete(&parser);
    ynode->node = *yaml_document_get_root_node(ynode->doc);
    ynode->is_root = CUTIL_TRUE;
    return sf_cutil_SerialNode_yaml_create(ynode);
}

static cutil_SerialNode *
sf_cutil_SerialNode_yaml_from_file(FILE *file)
{
    return sf_cutil_SerialNode_yaml_init(&sf_parser_init_from_file, file);
}

static cutil_SerialNode *
sf_cutil_SerialNode_yaml_from_nstring(const char *str, size_t len)
{
    return sf_cutil_SerialNode_yaml_init(
      &sf_parser_init_from_string,
      &(struct s_InitParamsStr) {
        .len = len,
        .str = (const unsigned char *) str,
      }
    );
}

static cutil_SerialNode *
sf_cutil_SerialNode_yaml_dup(const cutil_SerialNode *node)
{
    CUTIL_RETURN_NULL_IF_NULL(node);
    const s_YamlNode *const ynode = node->node;
    if (ynode->is_root) {
        cutil_log_error("Root YAML node cannot be duplicated");
        return NULL;
    }
    s_YamlNode *const dup_node = sf_YamlNode_calloc();
    dup_node->doc = ynode->doc;
    dup_node->node = ynode->node;
    dup_node->is_root = ynode->is_root;
    return sf_cutil_SerialNode_yaml_create(dup_node);
}

static void
sf_cutil_SerialNode_yaml_free(cutil_SerialNode *node)
{
    CUTIL_RETURN_IF_NULL(node);
    free(node->key);
    if (node->node_type == CUTIL_SERIAL_NODE_STRING) {
        free(node->value.string);
    }
    sf_YamlNode_free(node->node);
    free(node);
}

#define YAML_STRODOB_CHECK(STR, ENDPTR, ENTRY, RETVAL)                         \
    do {                                                                       \
        if (strcmp((STR), (ENTRY))) {                                          \
            *(ENDPTR) = (char *) (STR) + sizeof(ENTRY);                        \
            return (RETVAL);                                                   \
        }                                                                      \
    } while (0)

static inline cutil_Bool
sf_yaml_strtob(const char *CUTIL_RESTRICT str, char **CUTIL_RESTRICT endptr)
{
    YAML_STRODOB_CHECK(str, endptr, "True", CUTIL_TRUE);
    YAML_STRODOB_CHECK(str, endptr, "On", CUTIL_TRUE);
    YAML_STRODOB_CHECK(str, endptr, "Yes", CUTIL_TRUE);
    YAML_STRODOB_CHECK(str, endptr, "False", CUTIL_FALSE);
    YAML_STRODOB_CHECK(str, endptr, "Off", CUTIL_FALSE);
    YAML_STRODOB_CHECK(str, endptr, "No", CUTIL_FALSE);
    *endptr = (char *) str;
    return CUTIL_FALSE;
}

static cutil_SerialNodeType
sf_cutil_SerialNode_yaml_get_scalar_node_type(const s_YamlNode *const ynode)
{
    const char *const str = (const char *) ynode->node.data.scalar.value;
    char *endptr = NULL;
    CUTIL_UNUSED(strtod(str, &endptr));
    if (str != endptr) {
        return CUTIL_SERIAL_NODE_NUMBER;
    }
    CUTIL_UNUSED(sf_yaml_strtob(str, &endptr));
    if (str != endptr) {
        return CUTIL_SERIAL_NODE_BOOL;
    }
    return CUTIL_SERIAL_NODE_STRING;
}

static cutil_SerialNodeType
sf_cutil_SerialNode_yaml_get_node_type(const cutil_SerialNode *node)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_SERIAL_NODE_UNDEFINED);
    const s_YamlNode *const ynode = node->node;
    CUTIL_RETURN_VAL_IF_NULL(ynode, CUTIL_SERIAL_NODE_UNDEFINED);
    switch (ynode->node.type) {
    case YAML_SCALAR_NODE:
        return sf_cutil_SerialNode_yaml_get_scalar_node_type(ynode);
    case YAML_SEQUENCE_NODE:
        return CUTIL_SERIAL_NODE_SEQUENCE;
    case YAML_MAPPING_NODE:
        return CUTIL_SERIAL_NODE_COMPOSITE;
    default:
        return CUTIL_SERIAL_NODE_UNDEFINED;
    }
}

static cutil_Bool
sf_cutil_SerialNode_yaml_get_string_value(
  const cutil_SerialNode *node, const char **res
)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_FALSE);
    const s_YamlNode *const ynode = node->node;
    if (ynode == NULL || ynode->node.type != YAML_SCALAR_NODE) {
        return CUTIL_FALSE;
    }
    if (res != NULL) {
        *res = (const char *) ynode->node.data.scalar.value;
    }
    return CUTIL_TRUE;
}

static cutil_Bool
sf_cutil_SerialNode_yaml_get_number_value(
  const cutil_SerialNode *node, double *res
)
{
    const char *str = NULL;
    if (!sf_cutil_SerialNode_yaml_get_string_value(node, &str)) {
        return CUTIL_FALSE;
    }
    char *endptr = NULL;
    const double val = strtod(str, &endptr);
    if (str == endptr) {
        return CUTIL_FALSE;
    }
    if (res != NULL) {
        *res = val;
    }
    return CUTIL_TRUE;
}

static cutil_Bool
sf_cutil_SerialNode_yaml_get_bool_value(
  const cutil_SerialNode *node, cutil_Bool *res
)
{
    const char *str = NULL;
    if (!sf_cutil_SerialNode_yaml_get_string_value(node, &str)) {
        return CUTIL_FALSE;
    }
    char *endptr = NULL;
    const cutil_Bool val = sf_yaml_strtob(str, &endptr);
    if (str == endptr) {
        return CUTIL_FALSE;
    }
    if (res != NULL) {
        *res = val;
    }
    return CUTIL_TRUE;
}

static size_t
sf_cutil_SerialNode_yaml_get_sequence_length(const cutil_SerialNode *node)
{
    CUTIL_RETURN_VAL_IF_NULL(node, 0UL);
    const s_YamlNode *const ynode = node->node;
    const yaml_node_t *const yaml_node = &ynode->node;
    if (yaml_node->type != YAML_SEQUENCE_NODE) {
        return 0UL;
    }
    return (size_t) (yaml_node->data.sequence.items.top
                     - yaml_node->data.sequence.items.start);
}

static cutil_Bool
sf_cutil_SerialNode_yaml_set_to_res(
  cutil_SerialNode *res, yaml_document_t *yaml_doc, yaml_node_t *yaml_node
)
{
    CUTIL_NULL_CHECK(yaml_doc);
    CUTIL_RETURN_VAL_IF_NULL(yaml_node, CUTIL_FALSE);
    CUTIL_RETURN_VAL_IF_NULL(res, CUTIL_TRUE);
    if (res->node == NULL) {
        res->node = sf_YamlNode_calloc();
    }
    s_YamlNode *const yres = res->node;
    yres->doc = yaml_doc;
    yres->node = *yaml_node;
    yres->is_root = CUTIL_FALSE;
    return CUTIL_TRUE;
}

static cutil_Bool
sf_cutil_SerialNode_yaml_get_sequence_item(
  const cutil_SerialNode *node, size_t idx, cutil_SerialNode *res
)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_FALSE);
    const s_YamlNode *const ynode = node->node;
    if (idx >= sf_cutil_SerialNode_yaml_get_sequence_length(node)) {
        return CUTIL_FALSE;
    }
    const yaml_node_t *const yaml_node = &ynode->node;
    yaml_document_t *const doc = ynode->doc;
    const yaml_node_item_t item = yaml_node->data.sequence.items.start[idx];
    yaml_node_t *const item_node = yaml_document_get_node(doc, item);
    return sf_cutil_SerialNode_yaml_set_to_res(res, ynode->doc, item_node);
}

static yaml_node_t *
sf_YamlNode_get_child_by_key(const s_YamlNode *ynode, const char *key)
{
    CUTIL_RETURN_NULL_IF_NULL(ynode);
    CUTIL_RETURN_NULL_IF_NULL(key);
    yaml_document_t *const doc = ynode->doc;
    const yaml_node_t *const yaml_node = &ynode->node;
    if (ynode == NULL || yaml_node->type != YAML_MAPPING_NODE) {
        return NULL;
    }
    for (yaml_node_pair_t *pair = yaml_node->data.mapping.pairs.start;
         pair < yaml_node->data.mapping.pairs.top; ++pair)
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

static cutil_Bool
sf_cutil_SerialNode_yaml_get_child(
  const cutil_SerialNode *node, const char *key, cutil_SerialNode *res
)
{
    const s_YamlNode *const ynode = node->node;
    yaml_node_t *const child = sf_YamlNode_get_child_by_key(ynode, key);
    return sf_cutil_SerialNode_yaml_set_to_res(res, ynode->doc, child);
}

static cutil_Bool
sf_cutil_SerialNode_yaml_set_string_value(
  cutil_SerialNode *node, const char *key, const char *value
)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_FALSE);
    s_YamlNode *const ynode = node->node;
    if (ynode == NULL || ynode->node.type != YAML_MAPPING_NODE) {
        return CUTIL_FALSE;
    }
    yaml_document_t *const doc = ynode->doc;
    yaml_node_t *const yaml_node = &ynode->node;
    yaml_node_pair_t *target_pair = NULL;
    for (yaml_node_pair_t *pair = yaml_node->data.mapping.pairs.start;
         pair < yaml_node->data.mapping.pairs.top; ++pair)
    {
        yaml_node_t *const key_node = yaml_document_get_node(doc, pair->key);
        if (key_node != NULL && key_node->type == YAML_SCALAR_NODE) {
            if (strcmp((const char *) key_node->data.scalar.value, key) == 0) {
                target_pair = pair;
                break;
            }
        }
    }
    if (target_pair == NULL) {
        const int key_idx = yaml_document_add_scalar(
          doc, NULL, (const unsigned char *) key, (int) strlen(key),
          YAML_ANY_SCALAR_STYLE
        );
        if (key_idx == 0) {
            return CUTIL_FALSE;
        }
        const int value_idx = yaml_document_add_scalar(
          doc, NULL, (const unsigned char *) value, (int) strlen(value),
          YAML_ANY_SCALAR_STYLE
        );
        if (value_idx == 0) {
            return CUTIL_FALSE;
        }
        target_pair = yaml_document_append_mapping_pair(doc, yaml_node);
        if (target_pair == NULL) {
            return CUTIL_FALSE;
        }
        target_pair->key = key_idx;
        target_pair->value = value_idx;
    } else {
        const int value_idx = yaml_document_add_scalar(
          doc, NULL, (const unsigned char *) value, (int) strlen(value),
          YAML_ANY_SCALAR_STYLE
        );
        if (value_idx == 0) {
            return CUTIL_FALSE;
        }
        target_pair->value = value_idx;
    }
    return CUTIL_TRUE;
}

static const cutil_SerialType CUTIL_SERIAL_TYPE_YAML_OBJECT = {
  .name = "YAML",
  .from_file = &sf_cutil_SerialNode_yaml_from_file,
  .from_nstring = &sf_cutil_SerialNode_yaml_from_nstring,
  .dup = &sf_cutil_SerialNode_yaml_dup,
  .free = &sf_cutil_SerialNode_yaml_free,
  .get_string_value = &sf_cutil_SerialNode_yaml_get_string_value,
  .get_number_value = &sf_cutil_SerialNode_yaml_get_number_value,
  .get_bool_value = &sf_cutil_SerialNode_yaml_get_bool_value,
  .get_sequence_length = &sf_cutil_SerialNode_yaml_get_sequence_length,
  .get_sequence_item = &sf_cutil_SerialNode_yaml_get_sequence_item,
  .get_child = &sf_cutil_SerialNode_yaml_get_child,
  .set_string_value = &sf_cutil_SerialNode_yaml_set_string_value,
  .set_number_value = &sf_cutil_SerialNode_yaml_set_number_value,
  .set_bool_value = &sf_cutil_SerialNode_yaml_set_bool_value,
  .add_sequence_item = &sf_cutil_SerialNode_yaml_add_sequence_item,
  .add_child = &sf_cutil_SerialNode_yaml_add_child,
};

const cutil_SerialType *const CUTIL_SERIAL_TYPE_YAML
  = &CUTIL_SERIAL_TYPE_YAML_OBJECT;

extern inline cutil_SerialNode *
cutil_SerialNode_calloc_yaml(void);

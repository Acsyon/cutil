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
    int node_idx; /* 1-based index into doc->nodes; used for write operations */
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
    const yaml_node_t *const root = yaml_document_get_root_node(ynode->doc);
    ynode->node_idx = (int) (root - ynode->doc->nodes.start) + 1;
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
    dup_node->node_idx = ynode->node_idx;
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

static cutil_Bool
sf_cutil_SerialNode_yaml_get_string_value(
  const cutil_SerialNode *node, const char **res
)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_FALSE);
    const s_YamlNode *const ynode = node->node;
    if (ynode == NULL) {
        return CUTIL_FALSE;
    }
    const yaml_node_t *const yaml_node
      = yaml_document_get_node(ynode->doc, ynode->node_idx);
    if (yaml_node == NULL || yaml_node->type != YAML_SCALAR_NODE) {
        return CUTIL_FALSE;
    }
    if (res != NULL) {
        *res = (const char *) yaml_node->data.scalar.value;
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
    const yaml_node_t *const yaml_node
      = yaml_document_get_node(ynode->doc, ynode->node_idx);
    if (yaml_node == NULL || yaml_node->type != YAML_SEQUENCE_NODE) {
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
    yres->node_idx = (int) (yaml_node - yaml_doc->nodes.start) + 1;
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
    yaml_document_t *const doc = ynode->doc;
    const yaml_node_t *const yaml_node
      = yaml_document_get_node(doc, ynode->node_idx);
    if (yaml_node == NULL) {
        return CUTIL_FALSE;
    }
    const yaml_node_item_t item = yaml_node->data.sequence.items.start[idx];
    yaml_node_t *const item_node = yaml_document_get_node(doc, item);
    return sf_cutil_SerialNode_yaml_set_to_res(res, doc, item_node);
}

static yaml_node_t *
sf_YamlNode_get_child_by_key(const s_YamlNode *ynode, const char *key)
{
    CUTIL_RETURN_NULL_IF_NULL(ynode);
    CUTIL_RETURN_NULL_IF_NULL(key);
    yaml_document_t *const doc = ynode->doc;
    const yaml_node_t *const yaml_node
      = yaml_document_get_node(doc, ynode->node_idx);
    if (yaml_node == NULL || yaml_node->type != YAML_MAPPING_NODE) {
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

/**
 * Recursively copy a node from src_doc (identified by src_idx) into dst_doc.
 * Returns the new node index in dst_doc, or 0 on failure.
 */
static int
sf_yaml_copy_node(
  yaml_document_t *dst_doc, yaml_document_t *src_doc, int src_idx
)
{
    yaml_node_t *const src = yaml_document_get_node(src_doc, src_idx);
    if (src == NULL) {
        return 0;
    }
    switch (src->type) {
    case YAML_SCALAR_NODE:
        return yaml_document_add_scalar(
          dst_doc, src->tag, src->data.scalar.value, src->data.scalar.length,
          src->data.scalar.style
        );
    case YAML_SEQUENCE_NODE: {
        const int seq_idx = yaml_document_add_sequence(
          dst_doc, src->tag, src->data.sequence.style
        );
        if (seq_idx == 0) {
            return 0;
        }
        for (yaml_node_item_t *item = src->data.sequence.items.start;
             item < src->data.sequence.items.top; ++item)
        {
            const int item_idx = sf_yaml_copy_node(dst_doc, src_doc, *item);
            if (item_idx == 0) {
                return 0;
            }
            if (!yaml_document_append_sequence_item(dst_doc, seq_idx, item_idx))
            {
                return 0;
            }
        }
        return seq_idx;
    }
    case YAML_MAPPING_NODE: {
        const int map_idx = yaml_document_add_mapping(
          dst_doc, src->tag, src->data.mapping.style
        );
        if (map_idx == 0) {
            return 0;
        }
        for (yaml_node_pair_t *pair = src->data.mapping.pairs.start;
             pair < src->data.mapping.pairs.top; ++pair)
        {
            const int key_idx = sf_yaml_copy_node(dst_doc, src_doc, pair->key);
            const int val_idx
              = sf_yaml_copy_node(dst_doc, src_doc, pair->value);
            if (key_idx == 0 || val_idx == 0) {
                return 0;
            }
            if (!yaml_document_append_mapping_pair(
                  dst_doc, map_idx, key_idx, val_idx
                ))
            {
                return 0;
            }
        }
        return map_idx;
    }
    default:
        return 0;
    }
}

/* Dynamic output buffer for the emitter */
typedef struct {
    char *data;
    size_t len;
    size_t cap;
} s_YamlStrBuf;

static int
sf_yaml_write_handler(void *data, unsigned char *buffer, size_t size)
{
    s_YamlStrBuf *const buf = data;
    if (buf->len + size >= buf->cap) {
        const size_t new_cap = (buf->len + size + 1) * 2;
        char *const new_data = realloc(buf->data, new_cap);
        if (new_data == NULL) {
            return 0;
        }
        buf->data = new_data;
        buf->cap = new_cap;
    }
    memcpy(buf->data + buf->len, buffer, size);
    buf->len += size;
    buf->data[buf->len] = '\0';
    return 1;
}

static cutil_Bool
sf_yaml_emit_node(yaml_emitter_t *emitter, yaml_document_t *doc, int node_idx)
{
    yaml_node_t *const node = yaml_document_get_node(doc, node_idx);
    if (node == NULL) {
        return CUTIL_FALSE;
    }
    yaml_event_t ev;
    switch (node->type) {
    case YAML_SCALAR_NODE:
        if (!yaml_scalar_event_initialize(
              &ev, NULL, node->tag, node->data.scalar.value,
              node->data.scalar.length, 1, 1, node->data.scalar.style
            ))
        {
            return CUTIL_FALSE;
        }
        return (cutil_Bool) yaml_emitter_emit(emitter, &ev);

    case YAML_SEQUENCE_NODE:
        if (!yaml_sequence_start_event_initialize(
              &ev, NULL, node->tag, 1, node->data.sequence.style
            ))
        {
            return CUTIL_FALSE;
        }
        if (!yaml_emitter_emit(emitter, &ev)) {
            return CUTIL_FALSE;
        }
        for (yaml_node_item_t *item = node->data.sequence.items.start;
             item < node->data.sequence.items.top; ++item)
        {
            if (!sf_yaml_emit_node(emitter, doc, *item)) {
                return CUTIL_FALSE;
            }
        }
        yaml_sequence_end_event_initialize(&ev);
        return (cutil_Bool) yaml_emitter_emit(emitter, &ev);

    case YAML_MAPPING_NODE:
        if (!yaml_mapping_start_event_initialize(
              &ev, NULL, node->tag, 1, node->data.mapping.style
            ))
        {
            return CUTIL_FALSE;
        }
        if (!yaml_emitter_emit(emitter, &ev)) {
            return CUTIL_FALSE;
        }
        for (yaml_node_pair_t *pair = node->data.mapping.pairs.start;
             pair < node->data.mapping.pairs.top; ++pair)
        {
            if (
              !sf_yaml_emit_node(emitter, doc, pair->key)
              || !sf_yaml_emit_node(emitter, doc, pair->value)
            )
            {
                return CUTIL_FALSE;
            }
        }
        yaml_mapping_end_event_initialize(&ev);
        return (cutil_Bool) yaml_emitter_emit(emitter, &ev);

    default:
        return CUTIL_FALSE;
    }
}

static cutil_SerialNode *
sf_cutil_SerialNode_yaml_create_scalar(
  cutil_SerialNodeValueType vtype, const void *value
)
{
    yaml_document_t *const doc = CUTIL_MALLOC_OBJECT(doc);
    if (doc == NULL) {
        return NULL;
    }
    if (!yaml_document_initialize(doc, NULL, NULL, NULL, 0, 0)) {
        free(doc);
        return NULL;
    }
    char buf[64];
    const char *str = NULL;
    switch (vtype) {
    case CUTIL_SERIAL_NODE_STRING:
        str = (const char *) value;
        break;
    case CUTIL_SERIAL_NODE_NUMBER:
        snprintf(buf, sizeof(buf), "%.17g", *(const double *) value);
        str = buf;
        break;
    case CUTIL_SERIAL_NODE_BOOL:
        str = (*(const cutil_Bool *) value) ? "true" : "false";
        break;
    default:
        cutil_log_error(
          "create_scalar: unsupported value type %d", (int) vtype
        );
        yaml_document_delete(doc);
        free(doc);
        return NULL;
    }
    const int node_idx = yaml_document_add_scalar(
      doc, NULL, (const unsigned char *) str, (int) strlen(str),
      YAML_PLAIN_SCALAR_STYLE
    );
    if (node_idx == 0) {
        yaml_document_delete(doc);
        free(doc);
        return NULL;
    }
    s_YamlNode *const ynode = sf_YamlNode_calloc();
    ynode->doc = doc;
    ynode->node_idx = node_idx;
    ynode->is_root = CUTIL_TRUE;
    cutil_SerialNode *const node = sf_cutil_SerialNode_yaml_create(ynode);
    if (node != NULL) {
        node->node_type = vtype;
    }
    return node;
}

static cutil_SerialNode *
sf_cutil_SerialNode_yaml_create_sequence(
  const cutil_SerialNodeValueType *types,
  const void *const *values,
  size_t count
)
{
    yaml_document_t *const doc = CUTIL_MALLOC_OBJECT(doc);
    if (doc == NULL) {
        return NULL;
    }
    if (!yaml_document_initialize(doc, NULL, NULL, NULL, 0, 0)) {
        free(doc);
        return NULL;
    }
    const int seq_idx
      = yaml_document_add_sequence(doc, NULL, YAML_BLOCK_SEQUENCE_STYLE);
    if (seq_idx == 0) {
        yaml_document_delete(doc);
        free(doc);
        return NULL;
    }
    for (size_t i = 0; i < count; ++i) {
        char buf[64];
        const char *str = NULL;
        switch (types[i]) {
        case CUTIL_SERIAL_NODE_STRING:
            str = (const char *) values[i];
            break;
        case CUTIL_SERIAL_NODE_NUMBER:
            snprintf(buf, sizeof(buf), "%.17g", *(const double *) values[i]);
            str = buf;
            break;
        case CUTIL_SERIAL_NODE_BOOL:
            str = (*(const cutil_Bool *) values[i]) ? "true" : "false";
            break;
        default:
            cutil_log_error(
              "create_sequence: unsupported item type at index %zu", i
            );
            yaml_document_delete(doc);
            free(doc);
            return NULL;
        }
        const int item_idx = yaml_document_add_scalar(
          doc, NULL, (const unsigned char *) str, (int) strlen(str),
          YAML_PLAIN_SCALAR_STYLE
        );
        if (
          item_idx == 0
          || !yaml_document_append_sequence_item(doc, seq_idx, item_idx)
        )
        {
            yaml_document_delete(doc);
            free(doc);
            return NULL;
        }
    }
    s_YamlNode *const ynode = sf_YamlNode_calloc();
    ynode->doc = doc;
    ynode->node_idx = seq_idx;
    ynode->is_root = CUTIL_TRUE;
    cutil_SerialNode *const node = sf_cutil_SerialNode_yaml_create(ynode);
    if (node != NULL) {
        node->node_type = CUTIL_SERIAL_NODE_SEQUENCE;
    }
    return node;
}

static cutil_Bool
sf_cutil_SerialNode_yaml_add_sequence_item(
  cutil_SerialNode *seq, cutil_SerialNodeValueType vtype, const void *value
)
{
    CUTIL_RETURN_VAL_IF_NULL(seq, CUTIL_FALSE);
    s_YamlNode *const ynode = seq->node;
    yaml_node_t *const yaml_node
      = yaml_document_get_node(ynode->doc, ynode->node_idx);
    if (yaml_node == NULL || yaml_node->type != YAML_SEQUENCE_NODE) {
        cutil_log_error("add_sequence_item: node is not a sequence");
        return CUTIL_FALSE;
    }
    char buf[64];
    const char *str = NULL;
    switch (vtype) {
    case CUTIL_SERIAL_NODE_STRING:
        str = (const char *) value;
        break;
    case CUTIL_SERIAL_NODE_NUMBER:
        snprintf(buf, sizeof(buf), "%.17g", *(const double *) value);
        str = buf;
        break;
    case CUTIL_SERIAL_NODE_BOOL:
        str = (*(const cutil_Bool *) value) ? "true" : "false";
        break;
    default:
        cutil_log_error(
          "add_sequence_item: unsupported value type %d", (int) vtype
        );
        return CUTIL_FALSE;
    }
    const int item_idx = yaml_document_add_scalar(
      ynode->doc, NULL, (const unsigned char *) str, (int) strlen(str),
      YAML_PLAIN_SCALAR_STYLE
    );
    if (item_idx == 0) {
        return CUTIL_FALSE;
    }
    return (cutil_Bool) yaml_document_append_sequence_item(
      ynode->doc, ynode->node_idx, item_idx
    );
}

static cutil_SerialNode *
sf_cutil_SerialNode_yaml_create_mapping(void)
{
    yaml_document_t *const doc = CUTIL_MALLOC_OBJECT(doc);
    if (doc == NULL) {
        return NULL;
    }
    if (!yaml_document_initialize(doc, NULL, NULL, NULL, 0, 0)) {
        free(doc);
        return NULL;
    }
    const int map_idx
      = yaml_document_add_mapping(doc, NULL, YAML_BLOCK_MAPPING_STYLE);
    if (map_idx == 0) {
        yaml_document_delete(doc);
        free(doc);
        return NULL;
    }
    s_YamlNode *const ynode = sf_YamlNode_calloc();
    ynode->doc = doc;
    ynode->node_idx = map_idx;
    ynode->is_root = CUTIL_TRUE;
    cutil_SerialNode *const node = sf_cutil_SerialNode_yaml_create(ynode);
    if (node != NULL) {
        node->node_type = CUTIL_SERIAL_NODE_COMPOSITE;
    }
    return node;
}

static cutil_Bool
sf_cutil_SerialNode_yaml_add_child(
  cutil_SerialNode *parent, const char *key, cutil_SerialNode *child
)
{
    CUTIL_RETURN_VAL_IF_NULL(parent, CUTIL_FALSE);
    CUTIL_RETURN_VAL_IF_NULL(child, CUTIL_FALSE);
    CUTIL_RETURN_VAL_IF_NULL(key, CUTIL_FALSE);
    s_YamlNode *const parent_ynode = parent->node;
    s_YamlNode *const child_ynode = child->node;
    yaml_node_t *const parent_node
      = yaml_document_get_node(parent_ynode->doc, parent_ynode->node_idx);
    if (parent_node == NULL || parent_node->type != YAML_MAPPING_NODE) {
        cutil_log_error("add_child: parent is not a mapping node");
        return CUTIL_FALSE;
    }
    const int key_idx = yaml_document_add_scalar(
      parent_ynode->doc, NULL, (const unsigned char *) key, (int) strlen(key),
      YAML_PLAIN_SCALAR_STYLE
    );
    if (key_idx == 0) {
        return CUTIL_FALSE;
    }
    const int val_idx = sf_yaml_copy_node(
      parent_ynode->doc, child_ynode->doc, child_ynode->node_idx
    );
    if (val_idx == 0) {
        return CUTIL_FALSE;
    }
    return (cutil_Bool) yaml_document_append_mapping_pair(
      parent_ynode->doc, parent_ynode->node_idx, key_idx, val_idx
    );
}

static char *
sf_cutil_SerialNode_yaml_to_string(cutil_SerialNode *node, uint32_t write_opts)
{
    CUTIL_RETURN_NULL_IF_NULL(node);
    s_YamlNode *const ynode = node->node;
    s_YamlStrBuf buf = {.data = NULL, .len = 0, .cap = 0};
    yaml_emitter_t emitter;
    yaml_emitter_initialize(&emitter);
    yaml_emitter_set_output(&emitter, sf_yaml_write_handler, &buf);
    CUTIL_UNUSED(write_opts); /* flow-style option reserved for future step */
    yaml_event_t ev;
    cutil_Bool ok = CUTIL_TRUE;
    yaml_stream_start_event_initialize(&ev, YAML_UTF8_ENCODING);
    ok = ok && (cutil_Bool) yaml_emitter_emit(&emitter, &ev);
    yaml_document_start_event_initialize(&ev, NULL, NULL, NULL, 1);
    ok = ok && (cutil_Bool) yaml_emitter_emit(&emitter, &ev);
    ok = ok && sf_yaml_emit_node(&emitter, ynode->doc, ynode->node_idx);
    yaml_document_end_event_initialize(&ev, 1);
    ok = ok && (cutil_Bool) yaml_emitter_emit(&emitter, &ev);
    yaml_stream_end_event_initialize(&ev);
    ok = ok && (cutil_Bool) yaml_emitter_emit(&emitter, &ev);
    yaml_emitter_delete(&emitter);
    if (!ok) {
        free(buf.data);
        return NULL;
    }
    return buf.data;
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
  .create_scalar = &sf_cutil_SerialNode_yaml_create_scalar,
  .create_sequence = &sf_cutil_SerialNode_yaml_create_sequence,
  .add_sequence_item = &sf_cutil_SerialNode_yaml_add_sequence_item,
  .create_mapping = &sf_cutil_SerialNode_yaml_create_mapping,
  .add_child = &sf_cutil_SerialNode_yaml_add_child,
  .to_string = &sf_cutil_SerialNode_yaml_to_string,
};

const cutil_SerialType *const CUTIL_SERIAL_TYPE_YAML
  = &CUTIL_SERIAL_TYPE_YAML_OBJECT;

extern inline cutil_SerialNode *
cutil_SerialNode_calloc_yaml(void);

cutil_SerialNode *
cutil_SerialNode_yaml_create_root(void)
{
    yaml_document_t *const doc = CUTIL_MALLOC_OBJECT(doc);
    if (doc == NULL) {
        return NULL;
    }
    if (!yaml_document_initialize(doc, NULL, NULL, NULL, 0, 0)) {
        free(doc);
        return NULL;
    }
    const int root_idx
      = yaml_document_add_mapping(doc, NULL, YAML_BLOCK_MAPPING_STYLE);
    if (root_idx == 0) {
        yaml_document_delete(doc);
        free(doc);
        return NULL;
    }
    s_YamlNode *const ynode = sf_YamlNode_calloc();
    ynode->doc = doc;
    ynode->node_idx = root_idx;
    ynode->is_root = CUTIL_TRUE;
    cutil_SerialNode *const node = sf_cutil_SerialNode_yaml_create(ynode);
    if (node != NULL) {
        node->node_type = CUTIL_SERIAL_NODE_COMPOSITE;
    }
    return node;
}

cutil_SerialNode *
cutil_SerialNode_yaml_create_string(const char *value)
{
    return cutil_SerialNode_create_scalar(
      CUTIL_SERIAL_TYPE_YAML, CUTIL_SERIAL_NODE_STRING, value
    );
}

cutil_SerialNode *
cutil_SerialNode_yaml_create_number(double value)
{
    return cutil_SerialNode_create_scalar(
      CUTIL_SERIAL_TYPE_YAML, CUTIL_SERIAL_NODE_NUMBER, &value
    );
}

cutil_SerialNode *
cutil_SerialNode_yaml_create_bool(cutil_Bool value)
{
    return cutil_SerialNode_create_scalar(
      CUTIL_SERIAL_TYPE_YAML, CUTIL_SERIAL_NODE_BOOL, &value
    );
}

cutil_SerialNode *
cutil_SerialNode_yaml_create_empty_sequence(void)
{
    return cutil_SerialNode_create_sequence(
      CUTIL_SERIAL_TYPE_YAML, NULL, NULL, 0
    );
}

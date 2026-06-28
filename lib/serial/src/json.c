#include "cutil/serial/serial.h"
#include <cutil/serial/json.h>

#include <cutil/core/io/log.h>
#include <cutil/core/std/stdlib.h>
#include <cutil/core/util/macro.h>

#include <cutil/core/std/inttypes.h>
#include <cutil/core/std/stdbool.h>
#include <cutil/core/std/stdio.h>

#include <cjson/cJSON.h>
#include <stddef.h>

static char *
sf_file_to_str(FILE *in)
{
    CUTIL_RETURN_NULL_IF_NULL(in);

    fseek(in, 0L, SEEK_END);
    const long fsize = ftell(in) + 1;
    rewind(in);

    char *const fstr = malloc(fsize * sizeof *fstr);
    if (fstr == NULL) {
        cutil_log_error("Cannot allocate memory for copy of file");
        return NULL;
    }
    const size_t size = fread(fstr, 1UL, fsize, in);
    if (size != (size_t) fsize - 1) {
        cutil_log_error("Cannot copy contents of file");
        free(fstr);
        return NULL;
    }
    return fstr;
}

typedef struct {
    cJSON *json;
    cutil_Bool is_root;
} s_JsonNode;

static s_JsonNode *
sf_JsonNode_create(cJSON *json, cutil_Bool is_root)
{
    s_JsonNode *const jnode = CUTIL_CALLOC_OBJECT(jnode);
    CUTIL_RETURN_NULL_IF_NULL(jnode);
    jnode->json = json;
    jnode->is_root = is_root;
    return jnode;
}

static void
sf_JsonNode_free(s_JsonNode *jnode)
{
    CUTIL_RETURN_IF_NULL(jnode);
    if (jnode->is_root) {
        cJSON_Delete(jnode->json);
    }
    free(jnode);
}

static cutil_SerialNode *
sf_cutil_SerialNode_json_create(cJSON *json)
{
    if (json == NULL) {
        const char *errptr = cJSON_GetErrorPtr();
        cutil_log_error("Error while reading JSON before '%s'", errptr);
        return NULL;
    }
    cutil_SerialNode *const node = cutil_SerialNode_calloc_json();
    if (node == NULL) {
        cJSON_Delete(json);
        return NULL;
    }
    node->node = sf_JsonNode_create(json, CUTIL_TRUE);
    return node;
}

static cutil_SerialNode *
sf_cutil_SerialNode_json_from_file(FILE *file)
{
    char *const str = sf_file_to_str(file);
    cJSON *const json = cJSON_Parse(str);
    free(str);
    return sf_cutil_SerialNode_json_create(json);
}

static cutil_SerialNode *
sf_cutil_SerialNode_json_from_nstring(const char *str, size_t len)
{
    cJSON *const json = cJSON_ParseWithLength(str, len);
    return sf_cutil_SerialNode_json_create(json);
}

static cutil_SerialNode *
sf_cutil_SerialNode_json_dup(const cutil_SerialNode *node)
{
    CUTIL_RETURN_NULL_IF_NULL(node);
    const s_JsonNode *const jnode = node->node;
    cutil_SerialNode *const dup_node = cutil_SerialNode_calloc_json();
    cJSON *const dup_json = cJSON_Duplicate(jnode->json, cJSON_True);
    dup_node->node = sf_JsonNode_create(dup_json, jnode->is_root);
    return dup_node;
}

static void
sf_cutil_SerialNode_json_free(cutil_SerialNode *node)
{
    CUTIL_RETURN_IF_NULL(node);
    sf_JsonNode_free(node->node);
    free(node);
}

static cutil_SerialNodeType
sf_cutil_SerialNode_json_get_node_type(const cutil_SerialNode *node)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_SERIAL_NODE_UNDEFINED);
    const s_JsonNode *const jnode = node->node;
    CUTIL_RETURN_VAL_IF_NULL(jnode, CUTIL_SERIAL_NODE_UNDEFINED);
    const cJSON *const json = jnode->json;
    CUTIL_RETURN_VAL_IF_NULL(json, CUTIL_SERIAL_NODE_UNDEFINED);
    if (cJSON_IsNull(json)) {
        return CUTIL_SERIAL_NODE_EMPTY;
    }
    if (cJSON_IsArray(json)) {
        return CUTIL_SERIAL_NODE_SEQUENCE;
    }
    if (cJSON_IsObject(json)) {
        return CUTIL_SERIAL_NODE_COMPOSITE;
    }
    if (cJSON_IsInvalid(json)) {
        return CUTIL_SERIAL_NODE_UNDEFINED;
    }
    return CUTIL_SERIAL_NODE_UNDEFINED;
}

static const char *
sf_cutil_SerialNode_json_get_scalar_value(const cutil_SerialNode *node)
{
    CUTIL_RETURN_NULL_IF_NULL(node);
    const s_JsonNode *const jnode = node->node;
    if (jnode == NULL || !cJSON_IsString(jnode->json)) {
        return NULL;
    }
    return cJSON_GetStringValue(jnode->json);
}

static size_t
sf_cutil_SerialNode_json_get_sequence_length(const cutil_SerialNode *node)
{
    CUTIL_RETURN_VAL_IF_NULL(node, 0UL);
    const s_JsonNode *const jnode = node->node;
    if (jnode == NULL || !cJSON_IsArray(jnode->json)) {
        return 0UL;
    }
    return (size_t) cJSON_GetArraySize(jnode->json);
}

static cutil_Bool
sf_cutil_SerialNode_json_set_to_res(cutil_SerialNode *res, cJSON *json)
{
    CUTIL_RETURN_VAL_IF_NULL(json, CUTIL_FALSE);
    CUTIL_RETURN_VAL_IF_NULL(res, CUTIL_TRUE);
    if (res->node != NULL) {
        sf_JsonNode_free(res->node);
    }
    res->node = sf_JsonNode_create(json, CUTIL_FALSE);
    return CUTIL_TRUE;
}

static cutil_Bool
sf_cutil_SerialNode_json_get_sequence_item(
  const cutil_SerialNode *node, size_t idx, cutil_SerialNode *res
)
{
    CUTIL_RETURN_VAL_IF_NULL(node, CUTIL_FALSE);
    const cJSON *const jnode = node->node;
    if (idx >= sf_cutil_SerialNode_json_get_sequence_length(node)) {
        return CUTIL_FALSE;
    }
    cJSON *const item_node = cJSON_GetArrayItem(jnode, (int) idx);
    return sf_cutil_SerialNode_json_set_to_res(res, item_node);
}

static cutil_Bool
sf_cutil_SerialNode_json_get_child(
  const cutil_SerialNode *node, const char *key, cutil_SerialNode *res
)
{
    const cJSON *const jnode = node->node;
    cJSON *const child = cJSON_GetObjectItemCaseSensitive(jnode, key);
    return sf_cutil_SerialNode_json_set_to_res(res, child);
}

static const char *
sf_cutil_SerialNode_json_get_scalar_value_by_key(
  const cutil_SerialNode *node, const char *key
)
{
    const cJSON *const jnode = node->node;
    cJSON *const child = cJSON_GetObjectItemCaseSensitive(jnode, key);
    if (child == NULL) {
        return NULL;
    }
    return cJSON_GetStringValue(child);
}

static const cutil_SerialType CUTIL_SERIAL_TYPE_JSON_OBJECT = {
  .name = "JSON",
  .from_file = &sf_cutil_SerialNode_json_from_file,
  .from_nstring = &sf_cutil_SerialNode_json_from_nstring,
  .dup = &sf_cutil_SerialNode_json_dup,
  .free = &sf_cutil_SerialNode_json_free,
  .get_node_type = &sf_cutil_SerialNode_json_get_node_type,
  .get_scalar_value = &sf_cutil_SerialNode_json_get_scalar_value,
  .get_sequence_length = &sf_cutil_SerialNode_json_get_sequence_length,
  .get_sequence_item = &sf_cutil_SerialNode_json_get_sequence_item,
  .get_child = &sf_cutil_SerialNode_json_get_child,
  .get_scalar_value_by_key = &sf_cutil_SerialNode_json_get_scalar_value_by_key,
};

const cutil_SerialType *const CUTIL_SERIAL_TYPE_JSON
  = &CUTIL_SERIAL_TYPE_JSON_OBJECT;

extern inline cutil_SerialNode *
cutil_SerialNode_calloc_json(void);

cutil_Json *
cutil_Json_create(void)
{
    return cJSON_CreateObject();
}

cutil_Json *
cutil_Json_from_string(const char *str)
{
    cJSON *const json = cJSON_Parse(str);
    if (json == NULL) {
        const char *errptr = cJSON_GetErrorPtr();
        cutil_log_error("Error while reading JSON before '%s'", errptr);
    }
    return json;
}

void
cutil_Json_free(cutil_Json *json)
{
    cJSON_Delete(json);
}

char *
cutil_Json_to_string(const cutil_Json *json)
{
    return cJSON_Print(json);
}

bool
cutil_Json_compare(
  const cutil_Json *lhs, const cutil_Json *rhs, bool case_sensitive
)
{
    return cJSON_Compare(lhs, rhs, case_sensitive);
}

#define DEFINE_JSON_TO_TYPE_FUNC(TYPE)                                         \
    int cutil_Json_elem_to_##TYPE(                                             \
      const cutil_Json *json, const char *elem, TYPE *ptr                      \
    )                                                                          \
    {                                                                          \
        const cJSON *const item                                                \
          = cJSON_GetObjectItemCaseSensitive(json, elem);                      \
        if (item == NULL) {                                                    \
            return EXIT_FAILURE;                                               \
        }                                                                      \
        *ptr = (TYPE) cJSON_GetNumberValue(item);                              \
        return EXIT_SUCCESS;                                                   \
    }

DEFINE_JSON_TO_TYPE_FUNC(uint8_t)
DEFINE_JSON_TO_TYPE_FUNC(uint16_t)
DEFINE_JSON_TO_TYPE_FUNC(size_t)
DEFINE_JSON_TO_TYPE_FUNC(double)

int
cutil_Json_elem_to_str(const cutil_Json *json, const char *elem, char **ptr)
{
    const cJSON *const item = cJSON_GetObjectItemCaseSensitive(json, elem);
    if (item == NULL) {
        return EXIT_FAILURE;
    }
    *ptr = cJSON_GetStringValue(item);
    return EXIT_SUCCESS;
}

#define DEFINE_JSON_ADD_TYPE_FUNC(TYPE)                                        \
    int cutil_Json_add_##TYPE(cutil_Json *json, const char *elem, TYPE var)    \
    {                                                                          \
        cJSON *const item = cJSON_CreateNumber((double) var);                  \
        if (item == NULL) {                                                    \
            return EXIT_FAILURE;                                               \
        }                                                                      \
        const cJSON_bool status = cJSON_AddItemToObject(json, elem, item);     \
        if (!status) {                                                         \
            return EXIT_FAILURE;                                               \
        }                                                                      \
        return EXIT_SUCCESS;                                                   \
    }

DEFINE_JSON_ADD_TYPE_FUNC(uint8_t)
DEFINE_JSON_ADD_TYPE_FUNC(uint16_t)
DEFINE_JSON_ADD_TYPE_FUNC(size_t)
DEFINE_JSON_ADD_TYPE_FUNC(double)

int
cutil_Json_add_str(cutil_Json *json, const char *elem, const char *var)
{
    cJSON *const item = cJSON_CreateString(var);
    if (item == NULL) {
        return EXIT_FAILURE;
    }
    const cJSON_bool status = cJSON_AddItemToObject(json, elem, item);
    if (!status) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void
cutil_jsonutil_fill_from_string(
  void *obj, const char *str, cutil_JsonUtilReadCallback *callback
)
{
    cutil_Json *const json = cutil_Json_from_string(str);
    if (json == NULL) {
        return;
    }
    callback(obj, json);
}

void
cutil_jsonutil_fread(void *obj, FILE *in, cutil_JsonUtilReadCallback *callback)
{
    char *const str = sf_file_to_str(in);
    cutil_jsonutil_fill_from_string(obj, str, callback);
    free(str);
}

void
cutil_jsonutil_read(
  void *obj, const char *fname, cutil_JsonUtilReadCallback *callback
)
{
    FILE *const in = fopen(fname, "r");
    if (in == NULL) {
        cutil_log_error("Cannot open file '%s'", fname);
        return;
    }
    cutil_jsonutil_fread(obj, in, callback);
    fclose(in);
}

char *
cutil_jsonutil_to_string(const void *obj, cutil_JsonUtilWriteCallback *callback)
{
    cutil_Json *const json = callback(obj);
    if (json == NULL) {
        return NULL;
    }
    char *const str = cutil_Json_to_string(json);
    cutil_Json_free(json);
    return str;
}

void
cutil_jsonutil_fwrite(
  const void *obj, FILE *out, cutil_JsonUtilWriteCallback *callback
)
{
    char *const str = cutil_jsonutil_to_string(obj, callback);
    if (str == NULL) {
        return;
    }
    fprintf(out, "%s\n", str);
    free(str);
}

void
cutil_jsonutil_write(
  const void *obj, const char *fname, cutil_JsonUtilWriteCallback *callback
)
{
    FILE *const out = fopen(fname, "w");
    if (out == NULL) {
        cutil_log_error("Cannot open file '%s'", fname);
        return;
    }
    cutil_jsonutil_fwrite(obj, out, callback);
    fclose(out);
}

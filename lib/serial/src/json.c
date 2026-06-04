#include <cutil/serial/json.h>

#include <cutil/core/io/log.h>
#include <cutil/core/std/stdlib.h>
#include <cutil/core/util/macro.h>

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

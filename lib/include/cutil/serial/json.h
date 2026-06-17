/** cutil/serial/json.h
 *
 * Header for handling JSON files
 *
 */

#ifndef CUTIL_SERIAL_JSON_H_INCLUDED
#define CUTIL_SERIAL_JSON_H_INCLUDED

#include <cutil/serial/serial.h>

/**
 * cutil_SerialType for JSON serial nodes.
 */
extern const cutil_SerialType *const CUTIL_SERIAL_TYPE_JSON;

/**
 * Helper function to allocate a cutil_SerialNode with the JSON type.
 *
 * @return newly allocated cutil_SerialNode with JSON type, or NULL on error
 */
inline cutil_SerialNode *
cutil_SerialNode_calloc_json(void)
{
    return cutil_SerialNode_calloc(CUTIL_SERIAL_TYPE_JSON);
}

#include <cjson/cJSON.h>

#include <cutil/core/std/inttypes.h>
#include <cutil/core/std/stdbool.h>
#include <cutil/core/std/stdio.h>

/**
 * JSON object type (wrapper around cJSON)
 */
typedef cJSON cutil_Json;

/**
 * Creates newly malloc'd cutil_Json object.
 *
 * @return newly malloc'd cutil_Json object
 */
cutil_Json *
cutil_Json_create(void);

/**
 * Creates newly malloc'd cutil_Json object whose contents are read from string
 * `str`.
 *
 * @param[in] str string to read contents of cutil_Json object from
 *
 * @return newly malloc'd cutil_Json object with contents read from string
 */
cutil_Json *
cutil_Json_from_string(const char *str);

/**
 * Destroys cutil_Json object `json` and frees memory.
 *
 * @param[in] newly cutil_Json object to be destroyed
 */
void
cutil_Json_free(cutil_Json *json);

/**
 * Writes contents of `json` to newly malloc'd string.
 *
 * @param[in] json cutil_Json object to write
 *
 * @return newly malloc'd string with contents of `json`
 */
char *
cutil_Json_to_string(const cutil_Json *json);

/**
 * Compares the cutil_Json objects `lhs` and `rhs` by either taking case
 * sensitivity into account or not (depending on `case_sensitive`) and returns
 * if the objects are equal. Invalid and NULL input always compares to false.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 * @param[in] case_sensitive Should comparison be performed case-sensitively?
 *
 * @return Does `lhs` equal `rhs`?
 */
bool
cutil_Json_compare(
  const cutil_Json *lhs, const cutil_Json *rhs, bool case_sensitive
);

/**
 * Reads element `elem` from `json` interpreted as a TYPE and writes it to
 * memory pointed to by `ptr`. Returns error code (EXIT_SUCESS or EXIT_FAILURE).
 *
 * @param[in] json cutil_Json object to read from
 * @param[in] elem name of element to read
 * @param[out] ptr pointer to memory to write to
 *
 * @return error code (EXIT_SUCESS or EXIT_FAILURE).
 */
#define DECLARE_JSON_TO_TYPE_FUNC(TYPE)                                        \
    int cutil_Json_elem_to_##TYPE(                                             \
      const cutil_Json *json, const char *elem, TYPE *ptr                      \
    );

DECLARE_JSON_TO_TYPE_FUNC(uint8_t)
DECLARE_JSON_TO_TYPE_FUNC(uint16_t)
DECLARE_JSON_TO_TYPE_FUNC(size_t)
DECLARE_JSON_TO_TYPE_FUNC(double)

#undef DECLARE_JSON_TO_TYPE_FUNC

/**
 * Reads element `elem` from `json` interpreted as a string and writes it to
 * memory pointed to by `ptr`. Returns error code (EXIT_SUCESS or EXIT_FAILURE).
 *
 * @param[in] json cutil_Json object to read from
 * @param[in] elem name of element to read
 * @param[out] ptr pointer to memory to write to
 *
 * @return error code (EXIT_SUCESS or EXIT_FAILURE).
 */
int
cutil_Json_elem_to_str(const cutil_Json *json, const char *elem, char **ptr);

/**
 * Adds TYPE `var` as element with name `elem` to cutil_Json object pointed to
 * by `json`. Returns error code (EXIT_SUCESS or EXIT_FAILURE).
 *
 * @param[in] json cutil_Json object to add element to
 * @param[in] elem name of element to add
 * @param[in] var value of element to add
 *
 * @return error code (EXIT_SUCESS or EXIT_FAILURE).
 */
#define DECLARE_JSON_ADD_TYPE_FUNC(TYPE)                                       \
    int cutil_Json_add_##TYPE(cutil_Json *json, const char *elem, TYPE var);

DECLARE_JSON_ADD_TYPE_FUNC(uint8_t)
DECLARE_JSON_ADD_TYPE_FUNC(uint16_t)
DECLARE_JSON_ADD_TYPE_FUNC(size_t)
DECLARE_JSON_ADD_TYPE_FUNC(double)

#undef DECLARE_JSON_ADD_TYPE_FUNC

/**
 * Adds string `var` as element with name `elem` to cutil_Json object pointed to
 * by `json`. Returns error code (EXIT_SUCESS or EXIT_FAILURE).
 *
 * @param[in] json cutil_Json object to add element to
 * @param[in] elem name of element to add
 * @param[in] var value of element to add
 *
 * @return error code (EXIT_SUCESS or EXIT_FAILURE).
 */
int
cutil_Json_add_str(cutil_Json *json, const char *elem, const char *var);

/**
 * Typedef for JSON reading callback function
 *
 * @param[out] obj object to write values into
 * @param[in] json cutil_Json object to read from
 */
typedef void
cutil_JsonUtilReadCallback(void *obj, const cutil_Json *json);

/**
 * Fills object `obj` with the values in the JSON config file whose contents are
 * stored in string `str`. Reading is performed by the callback function
 * `callback`.
 *
 * @param[out] obj object to write values into
 * @param[in] str contents of config file to read values from
 * @param[in] callback callback function for reading
 */
void
cutil_jsonutil_fill_from_string(
  void *obj, const char *str, cutil_JsonUtilReadCallback *callback
);

/**
 * Fills object `obj` with the values in the JSON config file opened in file
 * stream `in`. Reading is performed by the callback function `callback`.
 *
 * @param[out] obj object to write values into
 * @param[in] in stream of config file to read values from
 * @param[in] callback callback function for reading
 */
void
cutil_jsonutil_fread(void *obj, FILE *in, cutil_JsonUtilReadCallback *callback);

/**
 * Fills object `obj` with the values in the JSON config file in `fname`.
 * Reading is performed by the callback function `callback`.
 *
 * @param[out] obj object to write values into
 * @param[in] fname file name of config file to read values from
 * @param[in] callback callback function for reading
 */
void
cutil_jsonutil_read(
  void *obj, const char *fname, cutil_JsonUtilReadCallback *callback
);

/**
 * Typedef for JSON writing callback function
 *
 * @param[in] obj object to read values from
 *
 * @return cutil_Json object to write data to
 */
typedef cutil_Json *
cutil_JsonUtilWriteCallback(const void *obj);

/**
 * Creates newly malloc'd string containing `obj` as a JSON object. Writing is
 * performed by the callback function `callback`.
 *
 * @param[in] obj object to convert to string
 * @param[in] callback callback function for writing
 *
 * @return newly malloc'd string containing `obj` as a JSON object
 */
char *
cutil_jsonutil_to_string(
  const void *obj, cutil_JsonUtilWriteCallback *callback
);

/**
 * Writes `obj` as a JSON object to output stream `out`. Writing is performed by
 * the callback function `callback`.
 *
 * @param[in] obj object to write
 * @param[out] out output stream to write into
 * @param[in] callback callback function for writing
 */
void
cutil_jsonutil_fwrite(
  const void *obj, FILE *out, cutil_JsonUtilWriteCallback *callback
);

/**
 * Writes `obj` as a JSON object to file `fname`. Writing is performed by the
 * callback function `callback`.
 *
 * @param[in] obj object to write
 * @param[in] out name of file to write into
 * @param[in] callback callback function for writing
 */
void
cutil_jsonutil_write(
  const void *obj, const char *fname, cutil_JsonUtilWriteCallback *callback
);

#endif /* CUTIL_SERIAL_JSON_H_INCLUDED */

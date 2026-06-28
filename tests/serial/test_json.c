#include "unity.h"
#include <cutil/serial/json.h>

#include <cutil/core/std/stdlib.h>
#include <cutil/core/std/string.h>

static void
test_should_createJsonObject_when_callJsonCreate(void)
{
    /* Act */
    cutil_Json *const json = cutil_Json_create();

    /* Assert */
    TEST_ASSERT_NOT_NULL(json);

    /* Cleanup */
    cutil_Json_free(json);
}

static void
test_should_parseJsonFromString_when_callJsonFromString(void)
{
    /* Arrange */
    const char *const json_str = "{\"key\": \"value\"}";

    /* Act */
    cutil_Json *const json = cutil_Json_from_string(json_str);

    /* Assert */
    TEST_ASSERT_NOT_NULL(json);

    /* Cleanup */
    cutil_Json_free(json);
}

static void
test_should_returnNull_when_callJsonFromStringWithInvalidInput(void)
{
    const char *invalid_json_str = "{invalid_json}";
    cutil_Json *json = cutil_Json_from_string(invalid_json_str);
    TEST_ASSERT_NULL(json);
}

static void
test_should_convertJsonToString_when_callJsonToString(void)
{
    /* Arrange */
    cutil_Json *const json = cutil_Json_create();
    cutil_Json_add_str(json, "key", "value");

    /* Act */
    char *const json_str = cutil_Json_to_string(json);

    /* Assert */
    TEST_ASSERT_NOT_NULL(json_str);
    TEST_ASSERT_EQUAL_STRING("{\n\t\"key\":\t\"value\"\n}", json_str);

    /* Cleanup */
    free(json_str);
    cutil_Json_free(json);
}

static void
test_should_compareJsonObjects_when_callJsonCompare(void)
{
    /* Arrange */
    cutil_Json *const json1 = cutil_Json_create();
    cutil_Json_add_str(json1, "key", "value");
    cutil_Json *const json2 = cutil_Json_create();
    cutil_Json_add_str(json2, "key", "value");

    /* Act */
    const bool result = cutil_Json_compare(json1, json2, true);

    /* Assert */
    TEST_ASSERT_TRUE(result);

    /* Cleanup */
    cutil_Json_free(json1);
    cutil_Json_free(json2);
}

static void
test_should_readU8FromJson_when_callJsonElemToU8(void)
{
    /* Arrange */
    cutil_Json *json = cutil_Json_create();
    cutil_Json_add_uint8_t(json, "key", 42);
    uint8_t value = 0;

    /* Act */
    const int result = cutil_Json_elem_to_uint8_t(json, "key", &value);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT8(42, value);

    /* Cleanup */
    cutil_Json_free(json);
}

static void
test_should_readStringFromJson_when_callJsonElemToStr(void)
{
    /* Arrange */
    cutil_Json *const json = cutil_Json_create();
    cutil_Json_add_str(json, "key", "value");
    char *value = NULL;

    /* Act */
    const int result = cutil_Json_elem_to_str(json, "key", &value);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, result);
    TEST_ASSERT_EQUAL_STRING("value", value);

    /* Cleanup */
    cutil_Json_free(json);
}

static void
test_should_addU8ToJson_when_callJsonAddU8(void)
{
    /* Arrange */
    cutil_Json *const json = cutil_Json_create();

    /* Act */
    const int result = cutil_Json_add_uint8_t(json, "key", 42);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, result);

    /* Cleanup */
    cutil_Json_free(json);
}

static void
test_should_addStringToJson_when_callJsonAddStr(void)
{
    /* Arrange */
    cutil_Json *const json = cutil_Json_create();

    /* Act */
    const int result = cutil_Json_add_str(json, "key", "value");

    /* Assert */
    TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, result);

    /* Cleanup */
    cutil_Json_free(json);
}

static void
_callback(void *obj, const cutil_Json *json)
{
    char **value = obj;
    cutil_Json_elem_to_str(json, "key", value);
}

static void
test_should_fillObjectFromJsonString_when_callJsonUtilFillFromString(void)
{
    /* Arrange */
    const char *json_str = "{\"key\": \"value\"}";
    char buffer[10] = {0};
    char *value = &buffer[0];

    /* Act */
    cutil_jsonutil_fill_from_string(&value, json_str, &_callback);

    /* Cleanup */
    TEST_ASSERT_EQUAL_STRING("value", value);
}

void
setUp(void)
{}

void
tearDown(void)
{}

#include <cutil/core/status.h>
#include <cutil/core/std/stdbool.h>
#include <cutil/core/std/stddef.h>
#include <cutil/core/std/stdio.h>
#include <cutil/core/std/string.h>

/* Inline JSON fixture (replaces the basic.json file) */
static const char *const BASIC_JSON
  = "{\n"
    "\"string_key\": \"hello\",\n"
    "\"double_key\": 3.14,\n"
    "\"int_key\": 42,\n"
    "\"bool_key\": true,\n"
    "\"child\":\n"
    "  { \"nested_key\": \"nested_value\"}\n"
    "}\n";

/* Malformed JSON fragment for negative tests */
static const char *const INVALID_JSON = "{";

/* No-op callback used by cutil_json_read_* tests */
static cutil_Status
sf_noop_callback(void *obj, const cutil_SerialNode *node)
{
    (void) obj;
    (void) node;
    return CUTIL_STATUS_SUCCESS;
}

static void
test_should_returnNonNull_when_validJsonString(void)
{
    /* Act */
    cutil_SerialNode *const json
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_JSON, BASIC_JSON);

    /* Assert */
    TEST_ASSERT_NOT_NULL(json);

    /* Cleanup */
    cutil_SerialNode_free(json);
}

static void
test_should_returnNull_when_invalidJsonString(void)
{
    /* Act */
    cutil_SerialNode *const json
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_JSON, INVALID_JSON);

    /* Assert */
    TEST_ASSERT_NULL(json);
}

static void
test_should_returnNonNull_when_validJsonNstring(void)
{
    /* Arrange */
    const size_t len = strlen(BASIC_JSON);

    /* Act */
    cutil_SerialNode *const json
      = cutil_SerialNode_from_nstring(CUTIL_SERIAL_TYPE_JSON, BASIC_JSON, len);

    /* Assert */
    TEST_ASSERT_NOT_NULL(json);

    /* Cleanup */
    cutil_SerialNode_free(json);
}

static void
test_should_returnNull_when_invalidJsonNstring(void)
{
    /* Arrange */
    const size_t len = strlen(INVALID_JSON);

    /* Act */
    cutil_SerialNode *const json = cutil_SerialNode_from_nstring(
      CUTIL_SERIAL_TYPE_JSON, INVALID_JSON, len
    );

    /* Assert */
    TEST_ASSERT_NULL(json);
}

static void
test_should_returnNonNull_when_validFile(void)
{
    /* Arrange */
    FILE *const f = tmpfile();
    fputs(BASIC_JSON, f);
    rewind(f);

    /* Act */
    cutil_SerialNode *const json
      = cutil_SerialNode_from_file(CUTIL_SERIAL_TYPE_JSON, f);
    fclose(f);

    /* Assert */
    TEST_ASSERT_NOT_NULL(json);

    /* Cleanup */
    cutil_SerialNode_free(json);
}

static void
test_should_returnNull_when_fileIsNull(void)
{
    /* Act */
    cutil_SerialNode *const json
      = cutil_SerialNode_from_file(CUTIL_SERIAL_TYPE_JSON, NULL);

    /* Assert */
    TEST_ASSERT_NULL(json);
}

static void
test_should_returnSuccess_when_validFile(void)
{
    /* Arrange */
    FILE *const f = tmpfile();
    fputs(BASIC_JSON, f);
    rewind(f);

    /* Act */
    const cutil_Status status = cutil_serial_read_file(
      CUTIL_SERIAL_TYPE_JSON, NULL, f, &sf_noop_callback
    );
    fclose(f);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, status);
}

static void
test_should_returnFailure_when_fileIsNull(void)
{
    /* Act */
    const cutil_Status status = cutil_serial_read_file(
      CUTIL_SERIAL_TYPE_JSON, NULL, NULL, &sf_noop_callback
    );

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, status);
}

static void
test_should_returnSuccess_when_validString(void)
{
    /* Act */
    const cutil_Status status = cutil_serial_read_string(
      CUTIL_SERIAL_TYPE_JSON, NULL, BASIC_JSON, &sf_noop_callback
    );

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, status);
}

static void
test_should_returnFailure_when_stringIsInvalid(void)
{
    /* Act */
    const cutil_Status status = cutil_serial_read_string(
      CUTIL_SERIAL_TYPE_JSON, NULL, INVALID_JSON, &sf_noop_callback
    );

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, status);
}

static void
test_should_returnSuccess_when_validNstring(void)
{
    /* Arrange */
    const size_t len = strlen(BASIC_JSON);

    /* Act */
    const cutil_Status status = cutil_serial_read_nstring(
      CUTIL_SERIAL_TYPE_JSON, NULL, BASIC_JSON, len, &sf_noop_callback
    );

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, status);
}

static void
test_should_returnFailure_when_nstringIsInvalid(void)
{
    /* Arrange */
    const size_t len = strlen(INVALID_JSON);

    /* Act */
    const cutil_Status status = cutil_serial_read_nstring(
      CUTIL_SERIAL_TYPE_JSON, NULL, INVALID_JSON, len, &sf_noop_callback
    );

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, status);
}

static void
test_should_returnExpectedString_when_keyExists(void)
{
    /* Arrange */
    cutil_SerialNode *const json
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_JSON, BASIC_JSON);

    /* Act */
    const char *const result
      = cutil_SerialNode_get_string(json, "string_key", "default");

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("hello", result);

    /* Cleanup */
    cutil_SerialNode_free(json);
}

static void
test_should_returnDefault_when_stringKeyMissing(void)
{
    /* Arrange */
    cutil_SerialNode *const json
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_JSON, BASIC_JSON);

    /* Act */
    const char *const result
      = cutil_SerialNode_get_string(json, "no_such_key", "fallback");

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("fallback", result);

    /* Cleanup */
    cutil_SerialNode_free(json);
}

static void
test_should_returnExpectedDouble_when_keyExists(void)
{
    /* Arrange */
    cutil_SerialNode *const json
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_JSON, BASIC_JSON);

    /* Act */
    const double result = cutil_SerialNode_get_double(json, "double_key", 0.0);

    /* Assert */
    TEST_ASSERT_EQUAL_DOUBLE(3.14, result);

    /* Cleanup */
    cutil_SerialNode_free(json);
}

static void
test_should_returnExpectedInt_when_keyExists(void)
{
    /* Arrange */
    cutil_SerialNode *const json
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_JSON, BASIC_JSON);

    /* Act */
    const int result = cutil_SerialNode_get_int(json, "int_key", 0);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(42, result);

    /* Cleanup */
    cutil_SerialNode_free(json);
}

static void
test_should_returnTrue_when_boolKeyIsTrue(void)
{
    /* Arrange */
    cutil_SerialNode *const json
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_JSON, BASIC_JSON);

    /* Act */
    const bool result = cutil_SerialNode_get_bool(json, "bool_key", false);

    /* Assert */
    TEST_ASSERT_TRUE(result);

    /* Cleanup */
    cutil_SerialNode_free(json);
}

static void
test_should_returnNonNull_when_childKeyExists(void)
{
    /* Arrange */
    cutil_SerialNode *const json
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_JSON, BASIC_JSON);
    cutil_SerialNode *const child
      = cutil_SerialNode_calloc(CUTIL_SERIAL_TYPE_JSON);

    /* Act */
    const cutil_Bool success = cutil_SerialNode_get_child(json, "child", child);

    /* Assert */
    TEST_ASSERT_NOT_NULL(child);
    TEST_ASSERT_TRUE(success);

    /* Cleanup */
    cutil_SerialNode_free(child);
    cutil_SerialNode_free(json);
}

static void
test_should_returnNull_when_childKeyMissing(void)
{
    /* Arrange */
    cutil_SerialNode *const json
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_JSON, BASIC_JSON);
    cutil_SerialNode *const child
      = cutil_SerialNode_calloc(CUTIL_SERIAL_TYPE_JSON);

    /* Act */
    const cutil_Bool success
      = cutil_SerialNode_get_child(json, "no_such_key", child);

    /* Assert */
    TEST_ASSERT_NOT_NULL(child);
    TEST_ASSERT_FALSE(success);

    /* Cleanup */
    cutil_SerialNode_free(child);
    cutil_SerialNode_free(json);
}

static void
test_should_returnNestedString_when_navigatingViaChild(void)
{
    /* Arrange */
    cutil_SerialNode *const json
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_JSON, BASIC_JSON);
    cutil_SerialNode *const child
      = cutil_SerialNode_calloc(CUTIL_SERIAL_TYPE_JSON);
    cutil_SerialNode_get_child(json, "child", child);

    /* Act */
    const char *const result
      = cutil_SerialNode_get_string(child, "nested_key", "default");

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("nested_value", result);

    /* Cleanup */
    cutil_SerialNode_free(child);
    cutil_SerialNode_free(json);
}

int
main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_should_createJsonObject_when_callJsonCreate);
    RUN_TEST(test_should_parseJsonFromString_when_callJsonFromString);
    RUN_TEST(test_should_returnNull_when_callJsonFromStringWithInvalidInput);
    RUN_TEST(test_should_convertJsonToString_when_callJsonToString);
    RUN_TEST(test_should_compareJsonObjects_when_callJsonCompare);
    RUN_TEST(test_should_readU8FromJson_when_callJsonElemToU8);
    RUN_TEST(test_should_readStringFromJson_when_callJsonElemToStr);
    RUN_TEST(test_should_addU8ToJson_when_callJsonAddU8);
    RUN_TEST(test_should_addStringToJson_when_callJsonAddStr);
    RUN_TEST(
      test_should_fillObjectFromJsonString_when_callJsonUtilFillFromString
    );

    RUN_TEST(test_should_returnNonNull_when_validJsonString);
    RUN_TEST(test_should_returnNull_when_invalidJsonString);
    RUN_TEST(test_should_returnNonNull_when_validJsonNstring);
    RUN_TEST(test_should_returnNull_when_invalidJsonNstring);
    RUN_TEST(test_should_returnNonNull_when_validFile);
    RUN_TEST(test_should_returnNull_when_fileIsNull);
    RUN_TEST(test_should_returnSuccess_when_validFile);
    RUN_TEST(test_should_returnFailure_when_fileIsNull);
    RUN_TEST(test_should_returnSuccess_when_validString);
    RUN_TEST(test_should_returnFailure_when_stringIsInvalid);
    RUN_TEST(test_should_returnSuccess_when_validNstring);
    RUN_TEST(test_should_returnFailure_when_nstringIsInvalid);
    RUN_TEST(test_should_returnExpectedString_when_keyExists);
    RUN_TEST(test_should_returnDefault_when_stringKeyMissing);
    RUN_TEST(test_should_returnExpectedDouble_when_keyExists);
    RUN_TEST(test_should_returnExpectedInt_when_keyExists);
    RUN_TEST(test_should_returnTrue_when_boolKeyIsTrue);
    RUN_TEST(test_should_returnNonNull_when_childKeyExists);
    RUN_TEST(test_should_returnNull_when_childKeyMissing);
    RUN_TEST(test_should_returnNestedString_when_navigatingViaChild);

    return UNITY_END();
}

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

    return UNITY_END();
}

#include "unity.h"
#include <cutil/serial/yaml.h>

#include <cutil/core/status.h>
#include <cutil/core/std/stdbool.h>
#include <cutil/core/std/stddef.h>
#include <cutil/core/std/stdio.h>
#include <cutil/core/std/string.h>

/* Inline YAML fixture (replaces the basic.yaml file) */
static const char *const BASIC_YAML
  = "string_key: hello\n"
    "double_key: 3.14\n"
    "int_key: 42\n"
    "bool_key: true\n"
    "child:\n"
    "  nested_key: nested_value\n";

/* Malformed YAML fragment for negative tests */
static const char *const INVALID_YAML = "{";

/* No-op callback used by cutil_yaml_read_* tests */
static cutil_Status
sf_noop_callback(void *obj, const cutil_SerialNode *node)
{
    (void) obj;
    (void) node;
    return CUTIL_STATUS_SUCCESS;
}

static void
test_should_returnNonNull_when_validYamlString(void)
{
    /* Act */
    cutil_SerialNode *const yaml
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_YAML, BASIC_YAML);

    /* Assert */
    TEST_ASSERT_NOT_NULL(yaml);

    /* Cleanup */
    cutil_SerialNode_free(yaml);
}

static void
test_should_returnNull_when_invalidYamlString(void)
{
    /* Act */
    cutil_SerialNode *const yaml
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_YAML, INVALID_YAML);

    /* Assert */
    TEST_ASSERT_NULL(yaml);
}

static void
test_should_returnNonNull_when_validYamlNstring(void)
{
    /* Arrange */
    const size_t len = strlen(BASIC_YAML);

    /* Act */
    cutil_SerialNode *const yaml
      = cutil_SerialNode_from_nstring(CUTIL_SERIAL_TYPE_YAML, BASIC_YAML, len);

    /* Assert */
    TEST_ASSERT_NOT_NULL(yaml);

    /* Cleanup */
    cutil_SerialNode_free(yaml);
}

static void
test_should_returnNull_when_invalidYamlNstring(void)
{
    /* Arrange */
    const size_t len = strlen(INVALID_YAML);

    /* Act */
    cutil_SerialNode *const yaml = cutil_SerialNode_from_nstring(
      CUTIL_SERIAL_TYPE_YAML, INVALID_YAML, len
    );

    /* Assert */
    TEST_ASSERT_NULL(yaml);
}

static void
test_should_returnNonNull_when_validFile(void)
{
    /* Arrange */
    FILE *const f = tmpfile();
    fputs(BASIC_YAML, f);
    rewind(f);

    /* Act */
    cutil_SerialNode *const yaml
      = cutil_SerialNode_from_file(CUTIL_SERIAL_TYPE_YAML, f);
    fclose(f);

    /* Assert */
    TEST_ASSERT_NOT_NULL(yaml);

    /* Cleanup */
    cutil_SerialNode_free(yaml);
}

static void
test_should_returnNull_when_fileIsNull(void)
{
    /* Act */
    cutil_SerialNode *const yaml
      = cutil_SerialNode_from_file(CUTIL_SERIAL_TYPE_YAML, NULL);

    /* Assert */
    TEST_ASSERT_NULL(yaml);
}

static void
test_should_returnSuccess_when_validFile(void)
{
    /* Arrange */
    FILE *const f = tmpfile();
    fputs(BASIC_YAML, f);
    rewind(f);

    /* Act */
    const cutil_Status status = cutil_serial_read_file(
      CUTIL_SERIAL_TYPE_YAML, NULL, f, &sf_noop_callback
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
      CUTIL_SERIAL_TYPE_YAML, NULL, NULL, &sf_noop_callback
    );

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, status);
}

static void
test_should_returnSuccess_when_validString(void)
{
    /* Act */
    const cutil_Status status = cutil_serial_read_string(
      CUTIL_SERIAL_TYPE_YAML, NULL, BASIC_YAML, &sf_noop_callback
    );

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, status);
}

static void
test_should_returnFailure_when_stringIsInvalid(void)
{
    /* Act */
    const cutil_Status status = cutil_serial_read_string(
      CUTIL_SERIAL_TYPE_YAML, NULL, INVALID_YAML, &sf_noop_callback
    );

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, status);
}

static void
test_should_returnSuccess_when_validNstring(void)
{
    /* Arrange */
    const size_t len = strlen(BASIC_YAML);

    /* Act */
    const cutil_Status status = cutil_serial_read_nstring(
      CUTIL_SERIAL_TYPE_YAML, NULL, BASIC_YAML, len, &sf_noop_callback
    );

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, status);
}

static void
test_should_returnFailure_when_nstringIsInvalid(void)
{
    /* Arrange */
    const size_t len = strlen(INVALID_YAML);

    /* Act */
    const cutil_Status status = cutil_serial_read_nstring(
      CUTIL_SERIAL_TYPE_YAML, NULL, INVALID_YAML, len, &sf_noop_callback
    );

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, status);
}

static void
test_should_returnExpectedString_when_keyExists(void)
{
    /* Arrange */
    cutil_SerialNode *const yaml
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_YAML, BASIC_YAML);

    /* Act */
    const char *const result
      = cutil_SerialNode_get_string(yaml, "string_key", "default");

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("hello", result);

    /* Cleanup */
    cutil_SerialNode_free(yaml);
}

static void
test_should_returnDefault_when_stringKeyMissing(void)
{
    /* Arrange */
    cutil_SerialNode *const yaml
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_YAML, BASIC_YAML);

    /* Act */
    const char *const result
      = cutil_SerialNode_get_string(yaml, "no_such_key", "fallback");

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("fallback", result);

    /* Cleanup */
    cutil_SerialNode_free(yaml);
}

static void
test_should_returnExpectedDouble_when_keyExists(void)
{
    /* Arrange */
    cutil_SerialNode *const yaml
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_YAML, BASIC_YAML);

    /* Act */
    const double result = cutil_SerialNode_get_double(yaml, "double_key", 0.0);

    /* Assert */
    TEST_ASSERT_EQUAL_DOUBLE(3.14, result);

    /* Cleanup */
    cutil_SerialNode_free(yaml);
}

static void
test_should_returnExpectedInt_when_keyExists(void)
{
    /* Arrange */
    cutil_SerialNode *const yaml
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_YAML, BASIC_YAML);

    /* Act */
    const int result = cutil_SerialNode_get_int(yaml, "int_key", 0);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(42, result);

    /* Cleanup */
    cutil_SerialNode_free(yaml);
}

static void
test_should_returnTrue_when_boolKeyIsTrue(void)
{
    /* Arrange */
    cutil_SerialNode *const yaml
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_YAML, BASIC_YAML);

    /* Act */
    const bool result = cutil_SerialNode_get_bool(yaml, "bool_key", false);

    /* Assert */
    TEST_ASSERT_TRUE(result);

    /* Cleanup */
    cutil_SerialNode_free(yaml);
}

static void
test_should_returnNonNull_when_childKeyExists(void)
{
    /* Arrange */
    cutil_SerialNode *const yaml
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_YAML, BASIC_YAML);
    cutil_SerialNode *const child
      = cutil_SerialNode_calloc(CUTIL_SERIAL_TYPE_YAML);

    /* Act */
    const cutil_Bool success = cutil_SerialNode_get_child(yaml, "child", child);

    /* Assert */
    TEST_ASSERT_NOT_NULL(child);
    TEST_ASSERT_TRUE(success);

    /* Cleanup */
    cutil_SerialNode_free(child);
    cutil_SerialNode_free(yaml);
}

static void
test_should_returnNull_when_childKeyMissing(void)
{
    /* Arrange */
    cutil_SerialNode *const yaml
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_YAML, BASIC_YAML);
    cutil_SerialNode *const child
      = cutil_SerialNode_calloc(CUTIL_SERIAL_TYPE_YAML);

    /* Act */
    const cutil_Bool success
      = cutil_SerialNode_get_child(yaml, "no_such_key", child);

    /* Assert */
    TEST_ASSERT_NOT_NULL(child);
    TEST_ASSERT_FALSE(success);

    /* Cleanup */
    cutil_SerialNode_free(child);
    cutil_SerialNode_free(yaml);
}

static void
test_should_returnNestedString_when_navigatingViaChild(void)
{
    /* Arrange */
    cutil_SerialNode *const yaml
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_YAML, BASIC_YAML);
    cutil_SerialNode *const child
      = cutil_SerialNode_calloc(CUTIL_SERIAL_TYPE_YAML);
    cutil_SerialNode_get_child(yaml, "child", child);

    /* Act */
    const char *const result
      = cutil_SerialNode_get_string(child, "nested_key", "default");

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("nested_value", result);

    /* Cleanup */
    cutil_SerialNode_free(child);
    cutil_SerialNode_free(yaml);
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

    RUN_TEST(test_should_returnNonNull_when_validYamlString);
    RUN_TEST(test_should_returnNull_when_invalidYamlString);
    RUN_TEST(test_should_returnNonNull_when_validYamlNstring);
    RUN_TEST(test_should_returnNull_when_invalidYamlNstring);
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

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

/* No-op callback used by cutil_Yaml_read_* tests */
static cutil_Status
sf_noop_callback(void *obj, const cutil_YamlNode *node)
{
    (void) obj;
    (void) node;
    return CUTIL_STATUS_SUCCESS;
}

static void
test_should_returnNonNull_when_validYamlString(void)
{
    /* Act */
    cutil_Yaml *const yaml = cutil_Yaml_from_string(BASIC_YAML);

    /* Assert */
    TEST_ASSERT_NOT_NULL(yaml);

    /* Cleanup */
    cutil_Yaml_free(yaml);
}

static void
test_should_returnNull_when_invalidYamlString(void)
{
    /* Act */
    cutil_Yaml *const yaml = cutil_Yaml_from_string(INVALID_YAML);

    /* Assert */
    TEST_ASSERT_NULL(yaml);
}

static void
test_should_returnNonNull_when_validYamlNstring(void)
{
    /* Arrange */
    const size_t len = strlen(BASIC_YAML);

    /* Act */
    cutil_Yaml *const yaml = cutil_Yaml_from_nstring(BASIC_YAML, len);

    /* Assert */
    TEST_ASSERT_NOT_NULL(yaml);

    /* Cleanup */
    cutil_Yaml_free(yaml);
}

static void
test_should_returnNull_when_invalidYamlNstring(void)
{
    /* Arrange */
    const size_t len = strlen(INVALID_YAML);

    /* Act */
    cutil_Yaml *const yaml = cutil_Yaml_from_nstring(INVALID_YAML, len);

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
    cutil_Yaml *const yaml = cutil_Yaml_from_file(f);
    fclose(f);

    /* Assert */
    TEST_ASSERT_NOT_NULL(yaml);

    /* Cleanup */
    cutil_Yaml_free(yaml);
}

static void
test_should_returnNull_when_fileIsNull(void)
{
    /* Act */
    cutil_Yaml *const yaml = cutil_Yaml_from_file(NULL);

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
    const cutil_Status status = cutil_Yaml_read_file(NULL, f, sf_noop_callback);
    fclose(f);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, status);
}

static void
test_should_returnFailure_when_fileIsNull(void)
{
    /* Act */
    const cutil_Status status
      = cutil_Yaml_read_file(NULL, NULL, sf_noop_callback);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, status);
}

static void
test_should_returnSuccess_when_validString(void)
{
    /* Act */
    const cutil_Status status
      = cutil_Yaml_read_string(NULL, BASIC_YAML, sf_noop_callback);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, status);
}

static void
test_should_returnFailure_when_stringIsInvalid(void)
{
    /* Act */
    const cutil_Status status
      = cutil_Yaml_read_string(NULL, INVALID_YAML, sf_noop_callback);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, status);
}

static void
test_should_returnSuccess_when_validNstring(void)
{
    /* Arrange */
    const size_t len = strlen(BASIC_YAML);

    /* Act */
    const cutil_Status status
      = cutil_Yaml_read_nstring(NULL, BASIC_YAML, len, sf_noop_callback);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, status);
}

static void
test_should_returnFailure_when_nstringIsInvalid(void)
{
    /* Arrange */
    const size_t len = strlen(INVALID_YAML);

    /* Act */
    const cutil_Status status
      = cutil_Yaml_read_nstring(NULL, INVALID_YAML, len, sf_noop_callback);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, status);
}

static void
test_should_returnNonNull_when_gettingRootNode(void)
{
    /* Arrange */
    cutil_Yaml *const yaml = cutil_Yaml_from_string(BASIC_YAML);
    cutil_YamlNode *const root = cutil_YamlNode_calloc();

    /* Act */
    const cutil_Bool success = cutil_Yaml_get_root_node(yaml, root);

    /* Assert */
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_TRUE(success);

    /* Cleanup */
    cutil_YamlNode_free(root);
    cutil_Yaml_free(yaml);
}

static void
test_should_returnExpectedString_when_keyExists(void)
{
    /* Arrange */
    cutil_Yaml *const yaml = cutil_Yaml_from_string(BASIC_YAML);
    cutil_YamlNode *const root = cutil_YamlNode_calloc();
    cutil_Yaml_get_root_node(yaml, root);

    /* Act */
    const char *const result
      = cutil_YamlNode_get_string(root, "string_key", "default");

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("hello", result);

    /* Cleanup */
    cutil_YamlNode_free(root);
    cutil_Yaml_free(yaml);
}

static void
test_should_returnDefault_when_stringKeyMissing(void)
{
    /* Arrange */
    cutil_Yaml *const yaml = cutil_Yaml_from_string(BASIC_YAML);
    cutil_YamlNode *const root = cutil_YamlNode_calloc();
    cutil_Yaml_get_root_node(yaml, root);

    /* Act */
    const char *const result
      = cutil_YamlNode_get_string(root, "no_such_key", "fallback");

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("fallback", result);

    /* Cleanup */
    cutil_YamlNode_free(root);
    cutil_Yaml_free(yaml);
}

static void
test_should_returnExpectedDouble_when_keyExists(void)
{
    /* Arrange */
    cutil_Yaml *const yaml = cutil_Yaml_from_string(BASIC_YAML);
    cutil_YamlNode *const root = cutil_YamlNode_calloc();
    cutil_Yaml_get_root_node(yaml, root);

    /* Act */
    const double result = cutil_YamlNode_get_double(root, "double_key", 0.0);

    /* Assert */
    TEST_ASSERT_EQUAL_DOUBLE(3.14, result);

    /* Cleanup */
    cutil_YamlNode_free(root);
    cutil_Yaml_free(yaml);
}

static void
test_should_returnExpectedInt_when_keyExists(void)
{
    /* Arrange */
    cutil_Yaml *const yaml = cutil_Yaml_from_string(BASIC_YAML);
    cutil_YamlNode *const root = cutil_YamlNode_calloc();
    cutil_Yaml_get_root_node(yaml, root);

    /* Act */
    const int result = cutil_YamlNode_get_int(root, "int_key", 0);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(42, result);

    /* Cleanup */
    cutil_YamlNode_free(root);
    cutil_Yaml_free(yaml);
}

static void
test_should_returnTrue_when_boolKeyIsTrue(void)
{
    /* Arrange */
    cutil_Yaml *const yaml = cutil_Yaml_from_string(BASIC_YAML);
    cutil_YamlNode *const root = cutil_YamlNode_calloc();
    cutil_Yaml_get_root_node(yaml, root);

    /* Act */
    const bool result = cutil_YamlNode_get_bool(root, "bool_key", false);

    /* Assert */
    TEST_ASSERT_TRUE(result);

    /* Cleanup */
    cutil_YamlNode_free(root);
    cutil_Yaml_free(yaml);
}

static void
test_should_returnNonNull_when_childKeyExists(void)
{
    /* Arrange */
    cutil_Yaml *const yaml = cutil_Yaml_from_string(BASIC_YAML);
    cutil_YamlNode *const root = cutil_YamlNode_calloc();
    cutil_Yaml_get_root_node(yaml, root);
    cutil_YamlNode *const child = cutil_YamlNode_calloc();

    /* Act */
    const cutil_Bool success = cutil_YamlNode_get_child(root, "child", child);

    /* Assert */
    TEST_ASSERT_NOT_NULL(child);
    TEST_ASSERT_TRUE(success);

    /* Cleanup */
    cutil_YamlNode_free(child);
    cutil_YamlNode_free(root);
    cutil_Yaml_free(yaml);
}

static void
test_should_returnNull_when_childKeyMissing(void)
{
    /* Arrange */
    cutil_Yaml *const yaml = cutil_Yaml_from_string(BASIC_YAML);
    cutil_YamlNode *const root = cutil_YamlNode_calloc();
    cutil_Yaml_get_root_node(yaml, root);
    cutil_YamlNode *const child = cutil_YamlNode_calloc();

    /* Act */
    const cutil_Bool success
      = cutil_YamlNode_get_child(root, "no_such_key", child);

    /* Assert */
    TEST_ASSERT_NOT_NULL(child);
    TEST_ASSERT_FALSE(success);

    /* Cleanup */
    cutil_YamlNode_free(child);
    cutil_YamlNode_free(root);
    cutil_Yaml_free(yaml);
}

static void
test_should_returnNestedString_when_navigatingViaChild(void)
{
    /* Arrange */
    cutil_Yaml *const yaml = cutil_Yaml_from_string(BASIC_YAML);
    cutil_YamlNode *const root = cutil_YamlNode_calloc();
    cutil_Yaml_get_root_node(yaml, root);
    cutil_YamlNode *const child = cutil_YamlNode_calloc();
    cutil_YamlNode_get_child(root, "child", child);

    /* Act */
    const char *const result
      = cutil_YamlNode_get_string(child, "nested_key", "default");

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("nested_value", result);

    /* Cleanup */
    cutil_YamlNode_free(child);
    cutil_YamlNode_free(root);
    cutil_Yaml_free(yaml);
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
    RUN_TEST(test_should_returnNonNull_when_gettingRootNode);
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

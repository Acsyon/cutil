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

/* ------------------------------------------------------------------ */
/* Write tests                                                          */
/* ------------------------------------------------------------------ */

static void
test_yaml_createRoot_should_returnNonNull(void)
{
    /* Act */
    cutil_SerialNode *const root = cutil_SerialNode_yaml_create_root();

    /* Assert */
    TEST_ASSERT_NOT_NULL(root);

    /* Cleanup */
    cutil_SerialNode_free(root);
}

static void
test_yaml_createScalar_should_returnStringNode_when_typeIsString(void)
{
    /* Arrange */
    const char *const val = "hello";

    /* Act */
    cutil_SerialNode *const node = cutil_SerialNode_create_scalar(
      CUTIL_SERIAL_TYPE_YAML, CUTIL_SERIAL_NODE_STRING, val
    );

    /* Assert */
    TEST_ASSERT_NOT_NULL(node);
    const char *str = NULL;
    TEST_ASSERT_TRUE(cutil_SerialNode_get_string_value(node, &str));
    TEST_ASSERT_EQUAL_STRING("hello", str);

    /* Cleanup */
    cutil_SerialNode_free(node);
}

static void
test_yaml_createScalar_should_returnNumberNode_when_typeIsNumber(void)
{
    /* Arrange */
    const double val = 3.14;

    /* Act */
    cutil_SerialNode *const node = cutil_SerialNode_create_scalar(
      CUTIL_SERIAL_TYPE_YAML, CUTIL_SERIAL_NODE_NUMBER, &val
    );

    /* Assert */
    TEST_ASSERT_NOT_NULL(node);
    double result = 0.0;
    TEST_ASSERT_TRUE(cutil_SerialNode_get_number_value(node, &result));
    TEST_ASSERT_EQUAL_DOUBLE(3.14, result);

    /* Cleanup */
    cutil_SerialNode_free(node);
}

static void
test_yaml_createScalar_should_returnBoolNode_when_typeIsBool(void)
{
    /* Arrange */
    const cutil_Bool val = CUTIL_TRUE;

    /* Act */
    cutil_SerialNode *const node = cutil_SerialNode_create_scalar(
      CUTIL_SERIAL_TYPE_YAML, CUTIL_SERIAL_NODE_BOOL, &val
    );

    /* Assert */
    TEST_ASSERT_NOT_NULL(node);
    cutil_Bool result = CUTIL_FALSE;
    TEST_ASSERT_TRUE(cutil_SerialNode_get_bool_value(node, &result));
    TEST_ASSERT_TRUE(result);

    /* Cleanup */
    cutil_SerialNode_free(node);
}

static void
test_yaml_createString_should_returnValidNode(void)
{
    cutil_SerialNode *const node = cutil_SerialNode_yaml_create_string("world");
    TEST_ASSERT_NOT_NULL(node);
    const char *str = NULL;
    TEST_ASSERT_TRUE(cutil_SerialNode_get_string_value(node, &str));
    TEST_ASSERT_EQUAL_STRING("world", str);
    cutil_SerialNode_free(node);
}

static void
test_yaml_createNumber_should_returnValidNode(void)
{
    cutil_SerialNode *const node = cutil_SerialNode_yaml_create_number(42.0);
    TEST_ASSERT_NOT_NULL(node);
    double result = 0.0;
    TEST_ASSERT_TRUE(cutil_SerialNode_get_number_value(node, &result));
    TEST_ASSERT_EQUAL_DOUBLE(42.0, result);
    cutil_SerialNode_free(node);
}

static void
test_yaml_createBool_should_returnValidNode(void)
{
    cutil_SerialNode *const node
      = cutil_SerialNode_yaml_create_bool(CUTIL_TRUE);
    TEST_ASSERT_NOT_NULL(node);
    cutil_Bool result = CUTIL_FALSE;
    TEST_ASSERT_TRUE(cutil_SerialNode_get_bool_value(node, &result));
    TEST_ASSERT_TRUE(result);
    cutil_SerialNode_free(node);
}

static void
test_yaml_createMapping_should_returnCompositeNode(void)
{
    cutil_SerialNode *const node
      = cutil_SerialNode_create_mapping(CUTIL_SERIAL_TYPE_YAML);
    TEST_ASSERT_NOT_NULL(node);
    cutil_SerialNode_free(node);
}

static void
test_yaml_addChild_should_returnTrue_when_addingScalarToMapping(void)
{
    /* Arrange */
    cutil_SerialNode *const parent
      = cutil_SerialNode_create_mapping(CUTIL_SERIAL_TYPE_YAML);
    cutil_SerialNode *const child
      = cutil_SerialNode_yaml_create_string("value");

    /* Act */
    const cutil_Bool ok = cutil_SerialNode_add_child(parent, "key", child);

    /* Assert */
    TEST_ASSERT_TRUE(ok);

    /* Cleanup */
    cutil_SerialNode_free(child);
    cutil_SerialNode_free(parent);
}

static void
test_yaml_addChild_should_returnFalse_when_parentIsNull(void)
{
    cutil_SerialNode *const child
      = cutil_SerialNode_yaml_create_string("value");
    const cutil_Bool ok = cutil_SerialNode_add_child(NULL, "key", child);
    TEST_ASSERT_FALSE(ok);
    cutil_SerialNode_free(child);
}

static void
test_yaml_addChild_should_returnFalse_when_childIsNull(void)
{
    cutil_SerialNode *const parent
      = cutil_SerialNode_create_mapping(CUTIL_SERIAL_TYPE_YAML);
    const cutil_Bool ok = cutil_SerialNode_add_child(parent, "key", NULL);
    TEST_ASSERT_FALSE(ok);
    cutil_SerialNode_free(parent);
}

static void
test_yaml_createEmptySequence_should_returnSequenceNode(void)
{
    cutil_SerialNode *const node
      = cutil_SerialNode_yaml_create_empty_sequence();
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL_INT(0, (int) cutil_SerialNode_get_sequence_length(node));
    cutil_SerialNode_free(node);
}

static void
test_yaml_createSequence_should_populateItems_when_countIsNonzero(void)
{
    /* Arrange */
    const cutil_SerialNodeValueType types[3] = {
      CUTIL_SERIAL_NODE_STRING, CUTIL_SERIAL_NODE_STRING,
      CUTIL_SERIAL_NODE_STRING
    };
    const char *const s0 = "a";
    const char *const s1 = "b";
    const char *const s2 = "c";
    const void *const values[3] = {s0, s1, s2};

    /* Act */
    cutil_SerialNode *const seq = cutil_SerialNode_create_sequence(
      CUTIL_SERIAL_TYPE_YAML, types, values, 3
    );

    /* Assert */
    TEST_ASSERT_NOT_NULL(seq);
    TEST_ASSERT_EQUAL_INT(3, (int) cutil_SerialNode_get_sequence_length(seq));

    /* Cleanup */
    cutil_SerialNode_free(seq);
}

static void
test_yaml_addSequenceItem_should_increaseLength(void)
{
    /* Arrange */
    cutil_SerialNode *const seq = cutil_SerialNode_yaml_create_empty_sequence();
    const char *const item = "hello";

    /* Act */
    const cutil_Bool ok
      = cutil_SerialNode_add_sequence_item(seq, CUTIL_SERIAL_NODE_STRING, item);

    /* Assert */
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_INT(1, (int) cutil_SerialNode_get_sequence_length(seq));

    /* Cleanup */
    cutil_SerialNode_free(seq);
}

static void
test_yaml_roundTrip_should_preserveStringValue(void)
{
    /* Arrange — build a mapping with one key */
    cutil_SerialNode *const mapping
      = cutil_SerialNode_create_mapping(CUTIL_SERIAL_TYPE_YAML);
    cutil_SerialNode *const value
      = cutil_SerialNode_yaml_create_string("roundtrip_value");
    cutil_SerialNode_add_child(mapping, "mykey", value);
    cutil_SerialNode_free(value);

    /* Act — serialise to string */
    char *const yaml_str
      = cutil_SerialNode_to_string(mapping, CUTIL_SERIAL_WRITE_OPT_NONE);
    TEST_ASSERT_NOT_NULL(yaml_str);

    /* Act — deserialise */
    cutil_SerialNode *const parsed
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_YAML, yaml_str);
    TEST_ASSERT_NOT_NULL(parsed);

    /* Assert */
    const char *const result
      = cutil_SerialNode_get_string(parsed, "mykey", "MISSING");
    TEST_ASSERT_EQUAL_STRING("roundtrip_value", result);

    /* Cleanup */
    cutil_SerialNode_free(parsed);
    free(yaml_str);
    cutil_SerialNode_free(mapping);
}

static void
test_yaml_roundTrip_should_preserveNumberValue(void)
{
    cutil_SerialNode *const mapping
      = cutil_SerialNode_create_mapping(CUTIL_SERIAL_TYPE_YAML);
    cutil_SerialNode *const value = cutil_SerialNode_yaml_create_number(99.5);
    cutil_SerialNode_add_child(mapping, "numkey", value);
    cutil_SerialNode_free(value);

    char *const yaml_str
      = cutil_SerialNode_to_string(mapping, CUTIL_SERIAL_WRITE_OPT_NONE);
    TEST_ASSERT_NOT_NULL(yaml_str);

    cutil_SerialNode *const parsed
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_YAML, yaml_str);
    TEST_ASSERT_NOT_NULL(parsed);

    const double result = cutil_SerialNode_get_double(parsed, "numkey", 0.0);
    TEST_ASSERT_EQUAL_DOUBLE(99.5, result);

    cutil_SerialNode_free(parsed);
    free(yaml_str);
    cutil_SerialNode_free(mapping);
}

static void
test_yaml_roundTrip_should_preserveNestedMapping(void)
{
    /* Arrange — build: { outer: { inner: "deep" } } */
    cutil_SerialNode *const inner_map
      = cutil_SerialNode_create_mapping(CUTIL_SERIAL_TYPE_YAML);
    cutil_SerialNode *const inner_val
      = cutil_SerialNode_yaml_create_string("deep");
    cutil_SerialNode_add_child(inner_map, "inner", inner_val);
    cutil_SerialNode_free(inner_val);

    cutil_SerialNode *const outer_map
      = cutil_SerialNode_create_mapping(CUTIL_SERIAL_TYPE_YAML);
    cutil_SerialNode_add_child(outer_map, "outer", inner_map);
    cutil_SerialNode_free(inner_map);

    char *const yaml_str
      = cutil_SerialNode_to_string(outer_map, CUTIL_SERIAL_WRITE_OPT_NONE);
    TEST_ASSERT_NOT_NULL(yaml_str);

    cutil_SerialNode *const parsed
      = cutil_SerialNode_from_string(CUTIL_SERIAL_TYPE_YAML, yaml_str);
    TEST_ASSERT_NOT_NULL(parsed);

    /* Navigate: parsed["outer"]["inner"] == "deep" */
    cutil_SerialNode *const outer_child
      = cutil_SerialNode_calloc(CUTIL_SERIAL_TYPE_YAML);
    TEST_ASSERT_TRUE(cutil_SerialNode_get_child(parsed, "outer", outer_child));
    const char *const result
      = cutil_SerialNode_get_string(outer_child, "inner", "MISSING");
    TEST_ASSERT_EQUAL_STRING("deep", result);

    cutil_SerialNode_free(outer_child);
    cutil_SerialNode_free(parsed);
    free(yaml_str);
    cutil_SerialNode_free(outer_map);
}

static void
test_yaml_toString_should_produceFlowStyle_when_optYamlFlowSet(void)
{
    cutil_SerialNode *const mapping
      = cutil_SerialNode_create_mapping(CUTIL_SERIAL_TYPE_YAML);
    cutil_SerialNode *const val = cutil_SerialNode_yaml_create_string("v");
    cutil_SerialNode_add_child(mapping, "k", val);
    cutil_SerialNode_free(val);

    char *const yaml_str
      = cutil_SerialNode_to_string(mapping, CUTIL_SERIAL_WRITE_OPT_YAML_FLOW);
    TEST_ASSERT_NOT_NULL(yaml_str);
    /* Flow-style output must contain '{' and '}' */
    TEST_ASSERT_NOT_NULL(strchr(yaml_str, '{'));

    free(yaml_str);
    cutil_SerialNode_free(mapping);
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

    RUN_TEST(test_yaml_createRoot_should_returnNonNull);
    RUN_TEST(test_yaml_createScalar_should_returnStringNode_when_typeIsString);
    RUN_TEST(test_yaml_createScalar_should_returnNumberNode_when_typeIsNumber);
    RUN_TEST(test_yaml_createScalar_should_returnBoolNode_when_typeIsBool);
    RUN_TEST(test_yaml_createString_should_returnValidNode);
    RUN_TEST(test_yaml_createNumber_should_returnValidNode);
    RUN_TEST(test_yaml_createBool_should_returnValidNode);
    RUN_TEST(test_yaml_createMapping_should_returnCompositeNode);
    RUN_TEST(test_yaml_addChild_should_returnTrue_when_addingScalarToMapping);
    RUN_TEST(test_yaml_addChild_should_returnFalse_when_parentIsNull);
    RUN_TEST(test_yaml_addChild_should_returnFalse_when_childIsNull);
    RUN_TEST(test_yaml_createEmptySequence_should_returnSequenceNode);
    RUN_TEST(test_yaml_createSequence_should_populateItems_when_countIsNonzero);
    RUN_TEST(test_yaml_addSequenceItem_should_increaseLength);
    RUN_TEST(test_yaml_roundTrip_should_preserveStringValue);
    RUN_TEST(test_yaml_roundTrip_should_preserveNumberValue);
    RUN_TEST(test_yaml_roundTrip_should_preserveNestedMapping);
    RUN_TEST(test_yaml_toString_should_produceFlowStyle_when_optYamlFlowSet);

    return UNITY_END();
}

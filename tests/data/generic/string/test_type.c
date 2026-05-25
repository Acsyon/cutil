#include "unity.h"
#include <cutil/data/generic/string/type.h>

#include <cutil/core/string/type.h>

static void
test_should_beValid_when_stringDescriptorIsChecked(void)
{
    /* Act */
    const cutil_GenericType *const type = CUTIL_GENERIC_TYPE_STRING;

    /* Assert */
    TEST_ASSERT_TRUE(cutil_GenericType_is_valid(type));
    TEST_ASSERT_EQUAL_STRING("cutil_String", cutil_GenericType_get_name(type));
    TEST_ASSERT_EQUAL_size_t(
      sizeof(cutil_String), cutil_GenericType_get_size(type)
    );
    TEST_ASSERT_NOT_NULL(type->init);
    TEST_ASSERT_NOT_NULL(type->clear);
    TEST_ASSERT_NOT_NULL(type->copy);
    TEST_ASSERT_NOT_NULL(type->deep_equals);
    TEST_ASSERT_NOT_NULL(type->comp);
    TEST_ASSERT_NOT_NULL(type->hash);
    TEST_ASSERT_NOT_NULL(type->to_string);
}

static void
test_should_beValid_when_viewDescriptorIsChecked(void)
{
    /* Act */
    const cutil_GenericType *const type = CUTIL_GENERIC_TYPE_STRING_VIEW;

    /* Assert */
    TEST_ASSERT_TRUE(cutil_GenericType_is_valid(type));
    TEST_ASSERT_EQUAL_STRING(
      "cutil_StringView", cutil_GenericType_get_name(type)
    );
    TEST_ASSERT_EQUAL_size_t(
      sizeof(cutil_StringView), cutil_GenericType_get_size(type)
    );
    TEST_ASSERT_NOT_NULL(type->init);
    TEST_ASSERT_NOT_NULL(type->clear);
    TEST_ASSERT_NOT_NULL(type->copy);
    TEST_ASSERT_NOT_NULL(type->deep_equals);
    TEST_ASSERT_NOT_NULL(type->comp);
    TEST_ASSERT_NOT_NULL(type->hash);
    TEST_ASSERT_NOT_NULL(type->to_string);
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

    RUN_TEST(test_should_beValid_when_stringDescriptorIsChecked);
    RUN_TEST(test_should_beValid_when_viewDescriptorIsChecked);

    return UNITY_END();
}

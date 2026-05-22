#include "unity.h"
#include <cutil/data/generic/string/builder.h>

#include <cutil/core/string/builder.h>

static void
_should_beValid_when_descriptorIsChecked(void)
{
    /* Act */
    const cutil_GenericType *const type = CUTIL_GENERIC_TYPE_STRING_BUILDER;

    /* Assert */
    TEST_ASSERT_TRUE(cutil_GenericType_is_valid(type));
    TEST_ASSERT_EQUAL_STRING(
      "cutil_StringBuilder", cutil_GenericType_get_name(type)
    );
    TEST_ASSERT_EQUAL_size_t(
      sizeof(cutil_StringBuilder), cutil_GenericType_get_size(type)
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

    RUN_TEST(_should_beValid_when_descriptorIsChecked);

    return UNITY_END();
}

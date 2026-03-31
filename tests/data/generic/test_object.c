#include "unity.h"

#include <cutil/data/generic/object.h>
#include <cutil/data/generic/type.h>
#include <cutil/std/string.h>
#include <cutil/string/type.h>

static void
_should_returnEmptyObject_when_alloc(void)
{
    /* Arrange / Act */
    cutil_GenericObject *const obj = cutil_GenericObject_alloc();

    /* Assert */
    TEST_ASSERT_NOT_NULL(obj);
    TEST_ASSERT_NULL(cutil_GenericObject_get_type(obj));
    TEST_ASSERT_NULL(cutil_GenericObject_get_data(obj));

    /* Cleanup */
    cutil_GenericObject_free(obj);
}

static void
_should_copyData_when_createWithValidTypeAndData(void)
{
    /* Arrange */
    const int value = 42;

    /* Act */
    cutil_GenericObject *const obj
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &value);

    /* Assert */
    TEST_ASSERT_NOT_NULL(obj);
    TEST_ASSERT_EQUAL_PTR(
      CUTIL_GENERIC_TYPE_INT, cutil_GenericObject_get_type(obj)
    );
    const int *const data = cutil_GenericObject_get_data(obj);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_INT(value, *data);
    /* Verify deep copy: not pointer-aliased */
    TEST_ASSERT_NOT_EQUAL(&value, data);

    /* Cleanup */
    cutil_GenericObject_free(obj);
}

static void
_should_initData_when_createWithNullData(void)
{
    /* Arrange / Act */
    cutil_GenericObject *const obj
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, NULL);

    /* Assert */
    TEST_ASSERT_NOT_NULL(obj);
    TEST_ASSERT_EQUAL_PTR(
      CUTIL_GENERIC_TYPE_INT, cutil_GenericObject_get_type(obj)
    );
    TEST_ASSERT_NULL(cutil_GenericObject_get_data(obj));

    /* Cleanup */
    cutil_GenericObject_free(obj);
}

static void
_should_resetToEmpty_when_clear(void)
{
    /* Arrange */
    const int value = 7;
    cutil_GenericObject *const obj
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &value);

    /* Act */
    cutil_GenericObject_clear(obj);

    /* Assert */
    TEST_ASSERT_NULL(cutil_GenericObject_get_type(obj));
    TEST_ASSERT_NULL(cutil_GenericObject_get_data(obj));

    /* Cleanup */
    cutil_GenericObject_free(obj);
}

static void
_should_beIdempotent_when_clearTwice(void)
{
    /* Arrange */
    const int value = 5;
    cutil_GenericObject *const obj
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &value);

    /* Act */
    cutil_GenericObject_clear(obj);
    cutil_GenericObject_clear(obj);

    /* Assert */
    TEST_ASSERT_NULL(cutil_GenericObject_get_type(obj));
    TEST_ASSERT_NULL(cutil_GenericObject_get_data(obj));

    /* Cleanup */
    cutil_GenericObject_free(obj);
}

static void
_should_deepCopyData_when_copy(void)
{
    /* Arrange */
    const int value = 123;
    cutil_GenericObject *const src
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &value);
    cutil_GenericObject *const dst = cutil_GenericObject_alloc();

    /* Act */
    cutil_GenericObject_copy(dst, src);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(
      CUTIL_GENERIC_TYPE_INT, cutil_GenericObject_get_type(dst)
    );
    const int *const dst_data = cutil_GenericObject_get_data(dst);
    TEST_ASSERT_NOT_NULL(dst_data);
    TEST_ASSERT_EQUAL_INT(value, *dst_data);
    /* Not pointer-aliased */
    TEST_ASSERT_NOT_EQUAL(cutil_GenericObject_get_data(src), dst_data);

    /* Cleanup */
    cutil_GenericObject_free(src);
    cutil_GenericObject_free(dst);
}

static void
_should_clearDest_when_copyFromEmptySource(void)
{
    /* Arrange */
    int value = 10;
    cutil_GenericObject *const dst
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &value);
    cutil_GenericObject *const src = cutil_GenericObject_alloc();

    /* Act */
    cutil_GenericObject_copy(dst, src);

    /* Assert */
    TEST_ASSERT_NULL(cutil_GenericObject_get_type(dst));
    TEST_ASSERT_NULL(cutil_GenericObject_get_data(dst));

    /* Cleanup */
    cutil_GenericObject_free(dst);
    cutil_GenericObject_free(src);
}

static void
_should_returnType_when_getTypeOnCreatedObject(void)
{
    /* Arrange */
    const int value = 1;
    cutil_GenericObject *const obj
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &value);

    /* Act */
    const cutil_GenericType *const type = cutil_GenericObject_get_type(obj);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(CUTIL_GENERIC_TYPE_INT, type);

    /* Cleanup */
    cutil_GenericObject_free(obj);
}

static void
_should_returnData_when_getDataOnCreatedObject(void)
{
    /* Arrange */
    const int value = 55;
    cutil_GenericObject *const obj
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &value);

    /* Act */
    const void *const data = cutil_GenericObject_get_data(obj);

    /* Assert */
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_INT(value, *(const int *) data);

    /* Cleanup */
    cutil_GenericObject_free(obj);
}

static void
_should_returnNull_when_getDataOnEmptyObject(void)
{
    /* Arrange */
    cutil_GenericObject *const obj = cutil_GenericObject_alloc();

    /* Act / Assert */
    TEST_ASSERT_NULL(cutil_GenericObject_get_data(obj));

    /* Cleanup */
    cutil_GenericObject_free(obj);
}

static void
_should_zeroFields_when_genericInit(void)
{
    /* Arrange */
    cutil_GenericObject *const obj = cutil_GenericObject_alloc();
    obj->type = CUTIL_GENERIC_TYPE_INT;
    obj->data = (void *) 0xDEADBEEF;

    /* Act */
    cutil_GenericType_apply_init(CUTIL_GENERIC_TYPE_GENERIC_OBJECT, obj);

    /* Assert */
    TEST_ASSERT_NULL(obj->type);
    TEST_ASSERT_NULL(obj->data);

    /* Cleanup */
    cutil_GenericObject_free(obj);
}

static void
_should_releaseData_when_genericClear(void)
{
    /* Arrange */
    const int value = 42;
    cutil_GenericObject *const obj
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &value);

    /* Act */
    cutil_GenericObject_clear_generic(obj);

    /* Assert */
    TEST_ASSERT_NULL(obj->type);
    TEST_ASSERT_NULL(obj->data);

    /* Cleanup */
    cutil_GenericObject_free(obj);
}

static void
_should_deepCopy_when_genericCopy(void)
{
    /* Arrange */
    const int value = 77;
    cutil_GenericObject *const src
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &value);
    cutil_GenericObject *const dst = cutil_GenericObject_alloc();

    /* Act */
    cutil_GenericObject_copy_generic(dst, src);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(CUTIL_GENERIC_TYPE_INT, dst->type);
    TEST_ASSERT_NOT_NULL(dst->data);
    TEST_ASSERT_EQUAL_INT(77, *(int *) dst->data);
    TEST_ASSERT_NOT_EQUAL(src->data, dst->data);

    /* Cleanup */
    cutil_GenericObject_free(src);
    cutil_GenericObject_free(dst);
}

static void
_should_returnTrue_when_genericDeepEqualsWithSameTypeAndEqualData(void)
{
    /* Arrange */
    const int value = 42;
    cutil_GenericObject *const a
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &value);
    cutil_GenericObject *const b
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &value);

    /* Act / Assert */
    TEST_ASSERT_TRUE(cutil_GenericObject_deep_equals_generic(a, b));

    /* Cleanup */
    cutil_GenericObject_free(a);
    cutil_GenericObject_free(b);
}

static void
_should_returnFalse_when_genericDeepEqualsWithDifferentTypes(void)
{
    /* Arrange */
    const int ival = 42;
    const double dval = 42.0;
    cutil_GenericObject *const a
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &ival);
    cutil_GenericObject *const b
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_DOUBLE, &dval);

    /* Act / Assert */
    TEST_ASSERT_FALSE(cutil_GenericObject_deep_equals_generic(a, b));

    /* Cleanup */
    cutil_GenericObject_free(a);
    cutil_GenericObject_free(b);
}

static void
_should_returnFalse_when_genericDeepEqualsWithDifferentData(void)
{
    /* Arrange */
    const int val1 = 1;
    const int val2 = 2;
    cutil_GenericObject *const a
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &val1);
    cutil_GenericObject *const b
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &val2);

    /* Act / Assert */
    TEST_ASSERT_FALSE(cutil_GenericObject_deep_equals_generic(a, b));

    /* Cleanup */
    cutil_GenericObject_free(a);
    cutil_GenericObject_free(b);
}

static void
_should_orderByTypeNameThenValue_when_genericCompare(void)
{
    /* Arrange */
    const int val1 = 1;
    const int val2 = 2;
    cutil_GenericObject *const a
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &val1);
    cutil_GenericObject *const b
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &val2);

    /* Act / Assert */
    TEST_ASSERT_TRUE(cutil_GenericObject_compare_generic(a, b) < 0);
    TEST_ASSERT_TRUE(cutil_GenericObject_compare_generic(b, a) > 0);
    TEST_ASSERT_EQUAL_INT(0, cutil_GenericObject_compare_generic(a, a));

    /* Different types */
    const double dval = 1.0;
    cutil_GenericObject *const c
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_DOUBLE, &dval);
    /* "cutil_double" < "cutil_int" lexicographically */
    TEST_ASSERT_TRUE(cutil_GenericObject_compare_generic(c, a) < 0);
    TEST_ASSERT_TRUE(cutil_GenericObject_compare_generic(a, c) > 0);

    /* Cleanup */
    cutil_GenericObject_free(a);
    cutil_GenericObject_free(b);
    cutil_GenericObject_free(c);
}

static void
_should_returnConsistentHash_when_genericHash(void)
{
    /* Arrange */
    const int value = 42;
    cutil_GenericObject *const a
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &value);
    cutil_GenericObject *const b
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &value);

    /* Act */
    cutil_hash_t hash_a = cutil_GenericObject_hash_generic(a);
    cutil_hash_t hash_b = cutil_GenericObject_hash_generic(b);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT64(hash_a, hash_b);

    cutil_GenericObject *const empty = cutil_GenericObject_alloc();
    TEST_ASSERT_EQUAL_UINT64(
      CUTIL_HASH_C(0), cutil_GenericObject_hash_generic(empty)
    );

    /* Cleanup */
    cutil_GenericObject_free(a);
    cutil_GenericObject_free(b);
    cutil_GenericObject_free(empty);
}

static void
_should_formatCorrectly_when_genericToString(void)
{
    /* Arrange */
    const int value = 42;
    cutil_GenericObject *const obj
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_INT, &value);
    char buf[256] = {0};

    /* Act */
    size_t len = cutil_GenericObject_to_string_generic(obj, buf, sizeof buf);

    /* Assert */
    TEST_ASSERT_TRUE(len > 0);
    TEST_ASSERT_EQUAL_STRING("42", buf);

    cutil_GenericObject *const empty = cutil_GenericObject_alloc();
    len = cutil_GenericObject_to_string_generic(empty, buf, sizeof buf);
    TEST_ASSERT_TRUE(len > 0);
    TEST_ASSERT_EQUAL_STRING("GenericObject<NULL>", buf);

    len = cutil_GenericObject_to_string_generic(NULL, buf, sizeof buf);
    TEST_ASSERT_TRUE(len > 0);
    TEST_ASSERT_EQUAL_STRING("NULL", buf);

    /* Cleanup */
    cutil_GenericObject_free(obj);
    cutil_GenericObject_free(empty);
}

static void
_should_haveCorrectNameAndSize_when_inspectTypeDescriptor(void)
{
    /* Act / Assert */
    TEST_ASSERT_EQUAL_STRING(
      "cutil_GenericObject", CUTIL_GENERIC_TYPE_GENERIC_OBJECT->name
    );
    TEST_ASSERT_EQUAL_size_t(
      sizeof(cutil_GenericObject), CUTIL_GENERIC_TYPE_GENERIC_OBJECT->size
    );
}

static void
_should_haveAllCallbacks_when_inspectTypeDescriptor(void)
{
    /* Act / Assert */
    TEST_ASSERT_NOT_NULL(CUTIL_GENERIC_TYPE_GENERIC_OBJECT->init);
    TEST_ASSERT_NOT_NULL(CUTIL_GENERIC_TYPE_GENERIC_OBJECT->clear);
    TEST_ASSERT_NOT_NULL(CUTIL_GENERIC_TYPE_GENERIC_OBJECT->copy);
    TEST_ASSERT_NOT_NULL(CUTIL_GENERIC_TYPE_GENERIC_OBJECT->deep_equals);
    TEST_ASSERT_NOT_NULL(CUTIL_GENERIC_TYPE_GENERIC_OBJECT->comp);
    TEST_ASSERT_NOT_NULL(CUTIL_GENERIC_TYPE_GENERIC_OBJECT->hash);
    TEST_ASSERT_NOT_NULL(CUTIL_GENERIC_TYPE_GENERIC_OBJECT->to_string);
}

static void
_should_deepCopyCompositeInner_when_createWithStringType(void)
{
    /* Arrange */
    cutil_String *const src_str = cutil_String_from_string("hello");
    cutil_GenericObject *const obj
      = cutil_GenericObject_create(CUTIL_GENERIC_TYPE_STRING, src_str);

    /* Assert */
    const cutil_String *const inner = cutil_GenericObject_get_data(obj);
    TEST_ASSERT_NOT_NULL(inner);
    TEST_ASSERT_EQUAL_STRING("hello", inner->str);
    TEST_ASSERT_NOT_EQUAL(src_str->str, inner->str);

    /* Copy the GenericObject */
    cutil_GenericObject *const copy = cutil_GenericObject_alloc();
    cutil_GenericObject_copy(copy, obj);
    const cutil_String *const copy_inner = cutil_GenericObject_get_data(copy);
    TEST_ASSERT_NOT_NULL(copy_inner);
    TEST_ASSERT_EQUAL_STRING("hello", copy_inner->str);
    TEST_ASSERT_NOT_EQUAL(inner->str, copy_inner->str);

    /* Cleanup */
    cutil_String_free(src_str);
    cutil_GenericObject_free(obj);
    cutil_GenericObject_free(copy);
}

static void
_should_returnTrue_when_genericDeepEqualsWithBothEmpty(void)
{
    /* Arrange */
    cutil_GenericObject *const a = cutil_GenericObject_alloc();
    cutil_GenericObject *const b = cutil_GenericObject_alloc();

    /* Act / Assert */
    TEST_ASSERT_TRUE(cutil_GenericObject_deep_equals_generic(a, b));

    /* Cleanup */
    cutil_GenericObject_free(a);
    cutil_GenericObject_free(b);
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

    RUN_TEST(_should_returnEmptyObject_when_alloc);
    RUN_TEST(_should_copyData_when_createWithValidTypeAndData);
    RUN_TEST(_should_initData_when_createWithNullData);
    RUN_TEST(_should_resetToEmpty_when_clear);
    RUN_TEST(_should_beIdempotent_when_clearTwice);
    RUN_TEST(_should_deepCopyData_when_copy);
    RUN_TEST(_should_clearDest_when_copyFromEmptySource);

    RUN_TEST(_should_returnType_when_getTypeOnCreatedObject);
    RUN_TEST(_should_returnData_when_getDataOnCreatedObject);
    RUN_TEST(_should_returnNull_when_getDataOnEmptyObject);

    RUN_TEST(_should_zeroFields_when_genericInit);
    RUN_TEST(_should_releaseData_when_genericClear);
    RUN_TEST(_should_deepCopy_when_genericCopy);
    RUN_TEST(_should_returnTrue_when_genericDeepEqualsWithSameTypeAndEqualData);
    RUN_TEST(_should_returnFalse_when_genericDeepEqualsWithDifferentTypes);
    RUN_TEST(_should_returnFalse_when_genericDeepEqualsWithDifferentData);
    RUN_TEST(_should_orderByTypeNameThenValue_when_genericCompare);
    RUN_TEST(_should_returnConsistentHash_when_genericHash);
    RUN_TEST(_should_formatCorrectly_when_genericToString);

    RUN_TEST(_should_haveCorrectNameAndSize_when_inspectTypeDescriptor);
    RUN_TEST(_should_haveAllCallbacks_when_inspectTypeDescriptor);

    RUN_TEST(_should_deepCopyCompositeInner_when_createWithStringType);

    RUN_TEST(_should_returnTrue_when_genericDeepEqualsWithBothEmpty);

    return UNITY_END();
}

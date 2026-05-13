#include "unity.h"
#include <cutil/core/util/void.h>

#include <cutil/core/util/macro.h>

static void
_should_returnCorrectElement_when_useVoidGetter(void)
{
    /* Arrange */
    size_t ELEMS[] = {1, 2, 4, 8, 16, 32, 64, 128};
    const size_t NUM_ELEMS = CUTIL_GET_NATIVE_ARRAY_SIZE(ELEMS);

    for (size_t i = 0; i < NUM_ELEMS; ++i) {
        /* Act */
        const void *const p_const
          = cutil_void_array_get_elem_const(sizeof *ELEMS, ELEMS, i);
        void *const p = cutil_void_array_get_elem(sizeof *ELEMS, ELEMS, i);

        /* Assert */
        TEST_ASSERT_EQUAL_PTR(&ELEMS[i], p_const);
        TEST_ASSERT_EQUAL_PTR(&ELEMS[i], p);
    }
}

static void
_should_swapContents_when_twoIntBuffersSwapped(void)
{
    /* Arrange */
    int a = 42, b = 99;
    const int orig_a = a, orig_b = b;

    /* Act */
    cutil_void_memswap(&a, &b, sizeof(int));

    /* Assert */
    TEST_ASSERT_EQUAL_INT(orig_b, a);
    TEST_ASSERT_EQUAL_INT(orig_a, b);
}

static void
_should_returnToOriginal_when_swappedTwice(void)
{
    /* Arrange */
    int a = 123, b = 456;
    const int orig_a = a, orig_b = b;

    /* Act */
    cutil_void_memswap(&a, &b, sizeof(int));
    cutil_void_memswap(&a, &b, sizeof(int));

    /* Assert */
    TEST_ASSERT_EQUAL_INT(orig_a, a);
    TEST_ASSERT_EQUAL_INT(orig_b, b);
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

    RUN_TEST(_should_returnCorrectElement_when_useVoidGetter);
    RUN_TEST(_should_swapContents_when_twoIntBuffersSwapped);
    RUN_TEST(_should_returnToOriginal_when_swappedTwice);

    return UNITY_END();
}

#include "unity.h"

#include <cutil/std/stdlib.h>
#include <cutil/util/macro.h>

static void
_should_returnNull_when_allocZeroBytes(void)
{
    /* Arrange */
    const size_t SIZES[] = {1, 2, 4, 8, 16, 32};
    const size_t NUM_SIZES = CUTIL_GET_NATIVE_ARRAY_SIZE(SIZES);
    void *const ptr = malloc(64);

    /* Act */
    /* Assert */
    TEST_ASSERT_NULL(cutil_malloc(0));
    TEST_ASSERT_NULL(cutil_calloc(0, 0));
    TEST_ASSERT_NULL(cutil_realloc(ptr, 0));
    for (size_t i = 0; i < NUM_SIZES; ++i) {
        TEST_ASSERT_NULL(cutil_calloc(SIZES[i], 0));
        TEST_ASSERT_NULL(cutil_calloc(0, SIZES[i]));
    }
}

static void
_should_returnValidPtr_when_allocNonzeroBytes(void)
{
    /* Arrange */
    const size_t SIZES[] = {1, 2, 4, 8, 16, 32};
    const size_t NUM_SIZES = CUTIL_GET_NATIVE_ARRAY_SIZE(SIZES);
    void *ptr = malloc(1);

    for (size_t i = 0; i < NUM_SIZES; ++i) {
        /* Act */
        void *const ptr_malloc = cutil_malloc(SIZES[i]);
        void *const ptr_calloc = cutil_calloc(1UL, SIZES[i]);
        void *const ptr_realloc = cutil_realloc(NULL, SIZES[i]);
        ptr = cutil_realloc(ptr, SIZES[i]);

        /* Assert */
        TEST_ASSERT_NOT_NULL(ptr_malloc);
        TEST_ASSERT_NOT_NULL(ptr_calloc);
        TEST_ASSERT_NOT_NULL(ptr_realloc);
        TEST_ASSERT_NOT_NULL(ptr);

        /* Cleanup */
        free(ptr_malloc);
        free(ptr_calloc);
        free(ptr_realloc);
    }
    free(ptr);
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

    RUN_TEST(_should_returnNull_when_allocZeroBytes);
    RUN_TEST(_should_returnValidPtr_when_allocNonzeroBytes);

    return UNITY_END();
}

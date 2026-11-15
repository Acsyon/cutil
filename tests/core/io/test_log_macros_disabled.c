/* Request POSIX.1-1993 for fileno() and read() */
#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 199309L
#endif /* _POSIX_C_SOURCE */
#include "unity.h"
#include <cutil/core/io/log.h>

#include <cutil/core/std/stdbool.h>
#include <cutil/core/std/stdio.h>
#include <cutil/core/std/stdlib.h>
#include <cutil/core/std/string.h>

static FILE *
sf_create_test_file(void)
{
    return tmpfile();
}

static void
sf_destroy_test_file(FILE *stream)
{
    if (stream == NULL) {
        return;
    }

    fclose(stream);
}

static char *
sf_read_file_contents(FILE *stream)
{
    fseek(stream, 0, SEEK_END);
    long length = ftell(stream);
    fseek(stream, 0, SEEK_SET);
    char *buffer = malloc(length + 1);
    TEST_ASSERT_EQUAL(length, fread(buffer, 1, length, stream));
    buffer[length] = '\0';
    return buffer;
}

static void
sf_setup_global_logger(FILE *stream)
{
    cutil_Logger *log = cutil_Logger_create(CUTIL_LOG_TRACE);
    cutil_Logger_add_handler_full(log, stream, CUTIL_LOG_TRACE, CUTIL_FALSE);
    cutil_set_global_logger(log);
}

static void
test_should_produceNoOutput_when_disabledLevelCalled(void)
{
    /* Arrange */
    FILE *const stream = sf_create_test_file();
    sf_setup_global_logger(stream);

    /* Act */
    CUTIL_LOG_TRACE("disabled-trace");
    CUTIL_LOG_DEBUG("disabled-debug");
    CUTIL_LOG_INFO("disabled-info");
    CUTIL_LOG_WARN("disabled-warn");
    CUTIL_LOG_ERROR("disabled-error");
    CUTIL_LOG_FATAL("disabled-fatal");

    /* Assert */
    fflush(stream);
    char *const content = sf_read_file_contents(stream);

#if defined(CUTIL_DISABLE_LOGGING)
    TEST_ASSERT_NULL(strstr(content, "disabled-trace"));
    TEST_ASSERT_NULL(strstr(content, "disabled-debug"));
    TEST_ASSERT_NULL(strstr(content, "disabled-info"));
    TEST_ASSERT_NULL(strstr(content, "disabled-warn"));
    TEST_ASSERT_NULL(strstr(content, "disabled-error"));
    TEST_ASSERT_NULL(strstr(content, "disabled-fatal"));
#else
    #if defined(CUTIL_DISABLE_LOG_TRACE)
    TEST_ASSERT_NULL(strstr(content, "disabled-trace"));
    #else
    TEST_ASSERT_NOT_NULL(strstr(content, "disabled-trace"));
    #endif
    #if defined(CUTIL_DISABLE_LOG_DEBUG)
    TEST_ASSERT_NULL(strstr(content, "disabled-debug"));
    #else
    TEST_ASSERT_NOT_NULL(strstr(content, "disabled-debug"));
    #endif
    #if defined(CUTIL_DISABLE_LOG_INFO)
    TEST_ASSERT_NULL(strstr(content, "disabled-info"));
    #else
    TEST_ASSERT_NOT_NULL(strstr(content, "disabled-info"));
    #endif
    #if defined(CUTIL_DISABLE_LOG_WARN)
    TEST_ASSERT_NULL(strstr(content, "disabled-warn"));
    #else
    TEST_ASSERT_NOT_NULL(strstr(content, "disabled-warn"));
    #endif
    #if defined(CUTIL_DISABLE_LOG_ERROR)
    TEST_ASSERT_NULL(strstr(content, "disabled-error"));
    #else
    TEST_ASSERT_NOT_NULL(strstr(content, "disabled-error"));
    #endif
    #if defined(CUTIL_DISABLE_LOG_FATAL)
    TEST_ASSERT_NULL(strstr(content, "disabled-fatal"));
    #else
    TEST_ASSERT_NOT_NULL(strstr(content, "disabled-fatal"));
    #endif
#endif

    /* Cleanup */
    free(content);
    cutil_Logger_free(cutil_set_global_logger(NULL));
    sf_destroy_test_file(stream);
}

static void
test_should_compileWithoutWarnings_when_usedInConditionals(void)
{
    int x = 1;
    if (x) {
        CUTIL_LOG_DEBUG("hygiene-conditional");
    } else {
        CUTIL_LOG_WARN("hygiene-else");
    }

    for (int i = 0; i < 1; ++i) {
        CUTIL_LOG_TRACE("hygiene-loop");
    }

    CUTIL_LOG_INFO("hygiene-standalone");

    TEST_PASS();
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
    RUN_TEST(test_should_produceNoOutput_when_disabledLevelCalled);
    RUN_TEST(test_should_compileWithoutWarnings_when_usedInConditionals);
    return UNITY_END();
}

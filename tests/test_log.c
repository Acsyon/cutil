#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <unity.h>

#include <cutil/log.h>
#include <cutil/std/stdio.h>

#define LEVEL_FOR_LOOP(VAR, BEGIN, END)                                        \
    for (enum cutil_LogLevel VAR = BEGIN; VAR <= END; ++VAR)

#define FOREACH_LEVEL(VAR) LEVEL_FOR_LOOP(VAR, CUTIL_LOG_TRACE, CUTIL_LOG_FATAL)

static FILE *
_create_test_file(void)
{
    return tmpfile();
}

static void
_destroy_test_file(FILE *stream)
{
    if (stream == NULL) {
        return;
    }

    fclose(stream);
}

static char *
_read_file_contents(FILE *stream)
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
_should_createLoggerCorrectly_when_useDefault(void)
{
    /* Arrange */
    /* Act */
    cutil_Logger *const default_log = cutil_Logger_create_default();

    /* Assert */
    TEST_ASSERT_NOT_NULL(default_log);
    TEST_ASSERT_EQUAL(CUTIL_LOG_WARN, cutil_Logger_get_level(default_log));

    /* Cleanup */
    cutil_Logger_free(default_log);
}

static void
_should_createLoggerCorrectly_when_useLevel(void)
{
    FOREACH_LEVEL(level)
    {
        /* Arrange */
        /* Act */
        cutil_Logger *const log = cutil_Logger_create(level);

        /* Assert */
        TEST_ASSERT_NOT_NULL(log);
        TEST_ASSERT_EQUAL(level, cutil_Logger_get_level(log));

        /* Cleanup */
        cutil_Logger_free(log);
    }
}

static void
_should_setLogLevel_when_haveValidLevel(void)
{
    /* Arrange */
    cutil_Logger *const log = cutil_Logger_create(CUTIL_LOG_INFO);

    FOREACH_LEVEL(newlvl)
    {
        /* Act */
        const enum cutil_LogLevel oldlvl = cutil_Logger_set_level(log, newlvl);

        /* Assert */
        TEST_ASSERT_EQUAL_INT(newlvl, cutil_Logger_get_level(log));
        if (newlvl > CUTIL_LOG_INFO) {
            TEST_ASSERT_EQUAL(newlvl - 1, oldlvl);
        }
    }

    /* Cleanup */
    cutil_Logger_free(log);
}

static void
_should_notSetLogLevel_when_haveInvalidLevel(void)
{
    /* Arrange */
    cutil_Logger *const log = cutil_Logger_create(CUTIL_LOG_INFO);
    const enum cutil_LogLevel current = cutil_Logger_get_level(log);
    const enum cutil_LogLevel invalid = CUTIL_LOG_FATAL + 1;

    /* Act */
    const enum cutil_LogLevel result = cutil_Logger_set_level(log, invalid);

    /* Assert */
    TEST_ASSERT_EQUAL(current, cutil_Logger_get_level(log));
    TEST_ASSERT_EQUAL(current, result);

    /* Cleanup */
    cutil_Logger_free(log);
}

static void
_should_outputCorrectly_when_haveMultipleHandlers(void)
{
    /* Arrange */
    FILE *const stream1 = _create_test_file();
    FILE *const stream2 = _create_test_file();
    cutil_Logger *const log = cutil_Logger_create(CUTIL_LOG_DEBUG);

    cutil_Logger_add_handler(log, stream1, CUTIL_LOG_DEBUG);
    cutil_Logger_add_handler_full(log, stream2, CUTIL_LOG_WARN, false);

    /* Act */
    cutil_Logger_debug(log, "Debug message");
    cutil_Logger_info(log, "Info message");
    cutil_Logger_warn(log, "Warning message");

    /* Assert  */
    fflush(stream1);
    char *content1 = _read_file_contents(stream1);
    TEST_ASSERT_NOT_NULL(strstr(content1, "Debug message"));
    TEST_ASSERT_NOT_NULL(strstr(content1, "Info message"));
    TEST_ASSERT_NOT_NULL(strstr(content1, "Warning message"));
    
    fflush(stream2);
    char *content2 = _read_file_contents(stream2);
    TEST_ASSERT_NULL(strstr(content2, "Debug message"));
    TEST_ASSERT_NULL(strstr(content2, "Info message"));
    TEST_ASSERT_NOT_NULL(strstr(content2, "Warning message"));
    
    /* Cleanup */
    free(content1);
    free(content2);
    _destroy_test_file(stream2);
    cutil_Logger_free(log);
}

static void
_should_formatCorrectly_when_callMessageFunctions(void)
{
    /* Arrange */
    FILE *const stream = _create_test_file();
    cutil_Logger *const log = cutil_Logger_create(CUTIL_LOG_TRACE);
    cutil_Logger_add_handler(log, stream, CUTIL_LOG_TRACE);

    /* Act */
    cutil_Logger_trace(log, "Trace %d", 1);
    cutil_Logger_debug(log, "Debug %s", "test");
    cutil_Logger_info(log, "Info %.1f", 3.14);
    cutil_Logger_warn(log, "Warning %c", 'X');
    cutil_Logger_error(log, "Error %p", log);
    cutil_Logger_fatal(log, "Fatal %ld", 123456L);

    /* Assert */
    fflush(stream);
    char *const content = _read_file_contents(stream);

    TEST_ASSERT_NOT_NULL(strstr(content, "[TRACE] Trace 1"));
    TEST_ASSERT_NOT_NULL(strstr(content, "[DEBUG] Debug test"));
    TEST_ASSERT_NOT_NULL(strstr(content, "[ INFO] Info 3.1"));
    TEST_ASSERT_NOT_NULL(strstr(content, "[ WARN] Warning X"));
    TEST_ASSERT_NOT_NULL(strstr(content, "[ERROR] Error "));
    TEST_ASSERT_NOT_NULL(strstr(content, "[FATAL] Fatal 123456"));

    /* Cleanup */
    free(content);
    cutil_Logger_free(log);
}

static void
_should_autoCloseStream_when_destroyLogger(void)
{
    /* Arrange */
    FILE *const stream = _create_test_file();
    cutil_Logger *const log = cutil_Logger_create(CUTIL_LOG_INFO);
    cutil_Logger_add_handler(log, stream, CUTIL_LOG_INFO);

    /* Act */
    cutil_Logger_free(log);

    /* Assert */
    TEST_ASSERT_EQUAL(EOF, fgetc(stream));
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

    RUN_TEST(_should_createLoggerCorrectly_when_useDefault);
    RUN_TEST(_should_createLoggerCorrectly_when_useLevel);
    RUN_TEST(_should_setLogLevel_when_haveValidLevel);
    RUN_TEST(_should_notSetLogLevel_when_haveInvalidLevel);
    RUN_TEST(_should_outputCorrectly_when_haveMultipleHandlers);
    RUN_TEST(_should_formatCorrectly_when_callMessageFunctions);
    RUN_TEST(_should_autoCloseStream_when_destroyLogger);

    return UNITY_END();
}

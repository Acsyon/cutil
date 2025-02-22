#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "unity.h"

#include <cutil/log.h>
#include <cutil/std/stdio.h>
#include <cutil/stringbuilder.h>

#define LEVEL_FOR_LOOP(VAR, BEGIN, END)                                        \
    for (enum cutil_LogLevel VAR = BEGIN; VAR <= END; ++VAR)

#define FOREACH_LEVEL(VAR) LEVEL_FOR_LOOP(VAR, CUTIL_LOG_TRACE, CUTIL_LOG_FATAL)

static FILE *
_create_tmpfile(void);

static void
_should_setLogStreamCorrectly(void)
{
    /* Arrange */
    FILE *const newstream = stdout;

    /* Act */
    FILE *const oldstream = cutil_log_set_stream(newstream);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(NULL, oldstream);
    TEST_ASSERT_EQUAL_PTR(newstream, cutil_log_get_stream());
}

static void
_should_setLogLevelCorrectly(void)
{
    /* Arrange */
    enum cutil_LogLevel oldlvl = cutil_log_get_level();

    FOREACH_LEVEL(newlvl)
    {
        /* Act */
        const enum cutil_LogLevel retlvl = cutil_log_set_level(newlvl);

        /* Assert */
        TEST_ASSERT_EQUAL_INT(newlvl, cutil_log_get_level());
        TEST_ASSERT_EQUAL_INT(oldlvl, retlvl);

        oldlvl = newlvl;
    }
}

static void
_should_returnCorrectPrefix_when_levelIsValid(void)
{
    FOREACH_LEVEL(lvl)
    {
        /* Arrange */
        const char *const assert_prefix = CUTIL_LOG_LEVEL_PREFIXES[lvl];

        /* Act */
        const char *const prefix = cutil_log_get_prefix(lvl);

        /* Assert */
        TEST_ASSERT_EQUAL_STRING(assert_prefix, prefix);
    }
}

static void
_should_returnEmptyPrefix_when_levelIsInvalid(void)
{
    const enum cutil_LogLevel LVLS[]
      = {-CUTIL_LOG_DEBUG,    -CUTIL_LOG_ERROR,     -CUTIL_LOG_FATAL,
         CUTIL_LOG_FATAL + 5, CUTIL_LOG_FATAL + 10, CUTIL_LOG_FATAL + 123456};
    const size_t num = (sizeof LVLS) / (sizeof *LVLS);

    for (size_t i = 0; i < num; ++i) {
        /* Arrange */
        const enum cutil_LogLevel lvl = LVLS[i];
        const char *const assert_prefix = "";

        /* Act */
        const char *const prefix = cutil_log_get_prefix(lvl);

        /* Assert */
        TEST_ASSERT_EQUAL_STRING(assert_prefix, prefix);
    }
}

static void
_should_outputApplicableMessages_when_levelIsSet(void)
{
    cutil_StringBuilder *const assert_sb = cutil_StringBuilder_create();
    FOREACH_LEVEL(lvl)
    {
        /* Arrange */
        FILE *const out = _create_tmpfile();
        cutil_log_set_stream(out);
        cutil_log_set_level(lvl);

        cutil_StringBuilder_clear(assert_sb);
        LEVEL_FOR_LOOP(tmplvl, lvl, CUTIL_LOG_FATAL)
        {
            const char *const prefix = cutil_log_get_prefix(tmplvl);
            cutil_StringBuilder_appendf(assert_sb, "[%s] \n", prefix);
        }

        /* Act */
        FOREACH_LEVEL(tmplvl)
        {
            cutil_log_message(tmplvl, "");
        }

        /* Assert */
        cutil_StringBuilder *const sb = cutil_StringBuilder_from_file(out);
        const char *const str = cutil_StringBuilder_get_string(sb);
        const char *const assert_str
          = cutil_StringBuilder_get_string(assert_sb);
        TEST_ASSERT_EQUAL_STRING(assert_str, str);

        /* Cleanup */
        cutil_StringBuilder_free(sb);
        fclose(out);
    }
    cutil_StringBuilder_free(assert_sb);
}

void
setUp(void)
{
    cutil_log_set_stream(NULL);
    cutil_log_set_level(CUTIL_LOG_TRACE);
}

void
tearDown(void)
{}

static FILE *
_create_tmpfile(void)
{
    FILE *const _tmpfile = tmpfile();
    if (_tmpfile == NULL) {
        fputs("Failed to create temporary output file.\n", stderr);
        TEST_ASSERT_TRUE(false);
    }
    return _tmpfile;
}

int
main(void)
{
    UNITY_BEGIN();
    RUN_TEST(_should_setLogStreamCorrectly);
    RUN_TEST(_should_setLogLevelCorrectly);
    RUN_TEST(_should_returnCorrectPrefix_when_levelIsValid);
    RUN_TEST(_should_returnEmptyPrefix_when_levelIsInvalid);
    RUN_TEST(_should_outputApplicableMessages_when_levelIsSet);
    return UNITY_END();
}

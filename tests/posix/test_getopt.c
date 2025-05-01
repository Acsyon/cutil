#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <unity.h>

#include <cutil/posix/getopt.h>

static char *
_dupstr(const char *str)
{
    if (str == NULL) {
        return NULL;
    }

    const size_t len = strlen(str) + 1;
    char *cpy = malloc(len);
    cpy = memcpy(cpy, str, len);

    return cpy;
}

#define MAKE_CASE_FLAG(CHAR, FLAG)                                             \
    case CHAR:                                                                 \
        FLAG = true;                                                           \
        break

#define MAKE_CASE_ARG(CHAR, VAR)                                               \
    case CHAR:                                                                 \
        VAR = _dupstr(cutil_optarg);                                           \
        break

static void
_should_setShortoptsCorrectly_when_haveNoArgument(void)
{
    /* Arrange */
    char *const argv[] = {"program", "-a", "-b", "-c", NULL};
    const int argc = ((sizeof argv) / (sizeof *argv)) - 1;
    const char *const optstring = "abcde";

    cutil_optind = 1; /* Reset for repeatability */

    bool a_flag = false;
    bool b_flag = false;
    bool c_flag = false;
    bool d_flag = false;
    bool e_flag = false;

    /* Act */
    int opt;
    while ((opt = cutil_getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            MAKE_CASE_FLAG('a', a_flag);
            MAKE_CASE_FLAG('b', b_flag);
            MAKE_CASE_FLAG('c', c_flag);
            MAKE_CASE_FLAG('d', d_flag);
            MAKE_CASE_FLAG('e', e_flag);
        default:
            break;
        }
    }

    /* Assert */
    TEST_ASSERT_TRUE(a_flag);
    TEST_ASSERT_TRUE(b_flag);
    TEST_ASSERT_TRUE(c_flag);
    TEST_ASSERT_FALSE(d_flag);
    TEST_ASSERT_FALSE(e_flag);
}

static void
_should_setShortoptsCorrectly_when_haveArguments(void)
{
    /* Arrange */
    char *const a_arg = "value";
    char *const b_arg = "another";
    char *const argv[] = {"program", "-a", a_arg, "-b", b_arg, NULL};
    const int argc = ((sizeof argv) / (sizeof *argv)) - 1;
    const char *const optstring = "a:b:c:d:e:";

    cutil_optind = 1; /* Reset for repeatability */

    char *a_str = NULL;
    char *b_str = NULL;
    char *c_str = NULL;
    char *d_str = NULL;
    char *e_str = NULL;

    /* Act */
    int opt;
    while ((opt = cutil_getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            MAKE_CASE_ARG('a', a_str);
            MAKE_CASE_ARG('b', b_str);
            MAKE_CASE_ARG('c', c_str);
            MAKE_CASE_ARG('d', d_str);
            MAKE_CASE_ARG('e', e_str);
        default:
            break;
        }
    }

    /* Assert */
    TEST_ASSERT_EQUAL_STRING(a_arg, a_str);
    TEST_ASSERT_EQUAL_STRING(b_arg, b_str);
    TEST_ASSERT_EQUAL_PTR(NULL, c_str);
    TEST_ASSERT_EQUAL_PTR(NULL, d_str);
    TEST_ASSERT_EQUAL_PTR(NULL, e_str);

    /* Cleanup */
    free(a_str);
    free(b_str);
    free(c_str);
    free(d_str);
    free(e_str);
}

static void
_should_ignoreShortopts_when_haveDoubleDash(void)
{
    /* Arrange */
    char *const argv[] = {"program", "--", "-a", "-b", "-c", NULL};
    const int argc = ((sizeof argv) / (sizeof *argv)) - 1;
    const char *const optstring = "abcde";

    cutil_optind = 1; /* Reset for repeatability */

    bool a_flag = false;
    bool b_flag = false;
    bool c_flag = false;
    bool d_flag = false;
    bool e_flag = false;

    /* Act */
    int opt;
    while ((opt = cutil_getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            MAKE_CASE_FLAG('a', a_flag);
            MAKE_CASE_FLAG('b', b_flag);
            MAKE_CASE_FLAG('c', c_flag);
            MAKE_CASE_FLAG('d', d_flag);
            MAKE_CASE_FLAG('e', e_flag);
        default:
            break;
        }
    }

    /* Assert */
    TEST_ASSERT_FALSE(a_flag);
    TEST_ASSERT_FALSE(b_flag);
    TEST_ASSERT_FALSE(c_flag);
    TEST_ASSERT_FALSE(d_flag);
    TEST_ASSERT_FALSE(e_flag);
}

static void
_should_detectInvalidShortopts_when_present(void)
{
    /* Arrange */
    char *const argv[] = {"program", "-x", "-y", "-z", NULL};
    const int argc = ((sizeof argv) / (sizeof *argv)) - 1;
    const char *const optstring = "abcde";

    cutil_optind = 1; /* Reset for repeatability */

    bool a_flag = false;
    bool b_flag = false;
    bool c_flag = false;
    bool d_flag = false;
    bool e_flag = false;

    /* Act */
    int ctr = 0;
    int opt;
    while ((opt = cutil_getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            MAKE_CASE_FLAG('a', a_flag);
            MAKE_CASE_FLAG('b', b_flag);
            MAKE_CASE_FLAG('c', c_flag);
            MAKE_CASE_FLAG('d', d_flag);
            MAKE_CASE_FLAG('e', e_flag);
        case '?':
            ++ctr;
        default:
            break;
        }
    }

    /* Assert */
    TEST_ASSERT_EQUAL_INT(3, ctr);
    TEST_ASSERT_FALSE(a_flag);
    TEST_ASSERT_FALSE(b_flag);
    TEST_ASSERT_FALSE(c_flag);
    TEST_ASSERT_FALSE(d_flag);
    TEST_ASSERT_FALSE(e_flag);
}

static void
_should_setLongoptsCorrectly_when_haveNoArgument(void)
{
    /* Arrange */
    char *const argv[] = {"program", "--alpha", "--beta", "--gamma", NULL};
    const int argc = ((sizeof argv) / (sizeof *argv)) - 1;
    const char *const shortopts = "abcde";
    const cutil_Option longopts[] = {
      {"alpha", CUTIL_OPTION_NO_ARGUMENT, 0, 'a'},
      {"beta", CUTIL_OPTION_NO_ARGUMENT, 0, 'b'},
      {"gamma", CUTIL_OPTION_NO_ARGUMENT, 0, 'c'},
      {0, 0, 0, 0},
    };

    cutil_optind = 1; /* Reset for repeatability */

    bool a_flag = false;
    bool b_flag = false;
    bool c_flag = false;
    bool d_flag = false;
    bool e_flag = false;

    /* Act */
    int opt;
    int optidx = 0;
    while ((opt = cutil_getopt_long(argc, argv, shortopts, longopts, &optidx))
           != -1)
    {
        switch (opt) {
            MAKE_CASE_FLAG('a', a_flag);
            MAKE_CASE_FLAG('b', b_flag);
            MAKE_CASE_FLAG('c', c_flag);
            MAKE_CASE_FLAG('d', d_flag);
            MAKE_CASE_FLAG('e', e_flag);
        default:
            break;
        }
    }

    /* Assert */
    TEST_ASSERT_TRUE(a_flag);
    TEST_ASSERT_TRUE(b_flag);
    TEST_ASSERT_TRUE(c_flag);
    TEST_ASSERT_FALSE(d_flag);
    TEST_ASSERT_FALSE(e_flag);
}

static void
_should_setLongoptsCorrectly_when_haveArguments(void)
{
    /* Arrange */
    char *const a_arg = "value";
    char *const b_arg = "another";
    char *const argv[] = {"program", "--alpha", a_arg, "--beta=another", NULL};
    const int argc = ((sizeof argv) / (sizeof *argv)) - 1;
    const char *const shortopts = "a:b:c:d:e:";
    const cutil_Option longopts[] = {
      {"alpha", CUTIL_OPTION_REQUIRED_ARGUMENT, 0, 'a'},
      {"beta", CUTIL_OPTION_REQUIRED_ARGUMENT, 0, 'b'},
      {0, 0, 0, 0},
    };

    cutil_optind = 1; /* Reset for repeatability */

    char *a_str = NULL;
    char *b_str = NULL;
    char *c_str = NULL;
    char *d_str = NULL;
    char *e_str = NULL;

    /* Act */
    int opt;
    int optidx = 0;
    while ((opt = cutil_getopt_long(argc, argv, shortopts, longopts, &optidx))
           != -1)
    {
        switch (opt) {
            MAKE_CASE_ARG('a', a_str);
            MAKE_CASE_ARG('b', b_str);
            MAKE_CASE_ARG('c', c_str);
            MAKE_CASE_ARG('d', d_str);
            MAKE_CASE_ARG('e', e_str);
        default:
            break;
        }
    }

    /* Assert */
    TEST_ASSERT_EQUAL_STRING(a_arg, a_str);
    TEST_ASSERT_EQUAL_STRING(b_arg, b_str);
    TEST_ASSERT_EQUAL_PTR(NULL, c_str);
    TEST_ASSERT_EQUAL_PTR(NULL, d_str);
    TEST_ASSERT_EQUAL_PTR(NULL, e_str);

    /* Cleanup */
    free(a_str);
    free(b_str);
    free(c_str);
    free(d_str);
    free(e_str);
}

static void
_should_ignoreLongopts_when_haveDoubleDash(void)
{
    /* Arrange */
    char *const argv[] = {"program", "--", "-a", "-b", "-c", NULL};
    const int argc = ((sizeof argv) / (sizeof *argv)) - 1;
    const char *const shortopts = "abcde";
    const cutil_Option longopts[] = {
      {"alpha", CUTIL_OPTION_NO_ARGUMENT, 0, 'a'},
      {"beta", CUTIL_OPTION_NO_ARGUMENT, 0, 'b'},
      {"gamma", CUTIL_OPTION_NO_ARGUMENT, 0, 'c'},
      {0, 0, 0, 0},
    };

    cutil_optind = 1; /* Reset for repeatability */

    bool a_flag = false;
    bool b_flag = false;
    bool c_flag = false;
    bool d_flag = false;
    bool e_flag = false;

    /* Act */
    int opt;
    int optidx = 0;
    while ((opt = cutil_getopt_long(argc, argv, shortopts, longopts, &optidx))
           != -1)
    {
        switch (opt) {
            MAKE_CASE_FLAG('a', a_flag);
            MAKE_CASE_FLAG('b', b_flag);
            MAKE_CASE_FLAG('c', c_flag);
            MAKE_CASE_FLAG('d', d_flag);
            MAKE_CASE_FLAG('e', e_flag);
        default:
            break;
        }
    }

    /* Assert */
    TEST_ASSERT_FALSE(a_flag);
    TEST_ASSERT_FALSE(b_flag);
    TEST_ASSERT_FALSE(c_flag);
    TEST_ASSERT_FALSE(d_flag);
    TEST_ASSERT_FALSE(e_flag);
}

static void
_should_detectInvalidLongopts_when_present(void)
{
    /* Arrange */

    /* Options have to be modifiable or else we'll get a SIGSEGV */
    char *const x_str = _dupstr("--xi=asdf");
    char *const y_str = _dupstr("--ypsilon");
    char *const z_str = _dupstr("--zeta");

    char *const argv[] = {"program", x_str, y_str, z_str, NULL};
    const int argc = ((sizeof argv) / (sizeof *argv)) - 1;
    const char *const shortopts = "abcde";
    const cutil_Option longopts[] = {
      {"alpha", CUTIL_OPTION_NO_ARGUMENT, 0, 'a'},
      {"beta", CUTIL_OPTION_NO_ARGUMENT, 0, 'b'},
      {"gamma", CUTIL_OPTION_NO_ARGUMENT, 0, 'c'},
      {0, 0, 0, 0},
    };

    cutil_optind = 1; /* Reset for repeatability */

    bool a_flag = false;
    bool b_flag = false;
    bool c_flag = false;
    bool d_flag = false;
    bool e_flag = false;

    /* Act */
    int ctr = 0;
    int opt;
    int optidx = 0;
    while ((opt = cutil_getopt_long(argc, argv, shortopts, longopts, &optidx))
           != -1)
    {
        switch (opt) {
            MAKE_CASE_FLAG('a', a_flag);
            MAKE_CASE_FLAG('b', b_flag);
            MAKE_CASE_FLAG('c', c_flag);
            MAKE_CASE_FLAG('d', d_flag);
            MAKE_CASE_FLAG('e', e_flag);
        case '?':
            ++ctr;
        default:
            break;
        }
    }

    /* Assert */
    TEST_ASSERT_EQUAL_INT(3, ctr);
    TEST_ASSERT_FALSE(a_flag);
    TEST_ASSERT_FALSE(b_flag);
    TEST_ASSERT_FALSE(c_flag);
    TEST_ASSERT_FALSE(d_flag);
    TEST_ASSERT_FALSE(e_flag);

    /* Cleanup */
    free(x_str);
    free(y_str);
    free(z_str);
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
    RUN_TEST(_should_setShortoptsCorrectly_when_haveNoArgument);
    RUN_TEST(_should_setShortoptsCorrectly_when_haveArguments);
    RUN_TEST(_should_ignoreShortopts_when_haveDoubleDash);
    RUN_TEST(_should_detectInvalidShortopts_when_present);
    RUN_TEST(_should_setLongoptsCorrectly_when_haveNoArgument);
    RUN_TEST(_should_setLongoptsCorrectly_when_haveArguments);
    RUN_TEST(_should_ignoreLongopts_when_haveDoubleDash);
    RUN_TEST(_should_detectInvalidLongopts_when_present);
    return UNITY_END();
}

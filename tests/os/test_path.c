#include "unity.h"

#include <cutil/os/path.h>

#include <cutil/std/stdio.h>
#include <cutil/std/stdlib.h>

#define PATH_BASE CUTIL_TEST_RESOURCE_DIR "/tmp"
#define DELIM CUTIL_PATH_DELIMITER

void
setUp(void)
{}

void
tearDown(void)
{}

static void
_should_returnFailure_when_pathIsNull(void)
{
    /* Act */
    const cutil_Status result = cutil_mkdirp(NULL, 0755);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, result);
}

static void
_should_createDirectory_when_pathDoesNotExist(void)
{
    /* Arrange */
    char path[256];
    snprintf(path, sizeof path, "%s%ctest_path_single", PATH_BASE, DELIM);

    /* Act */
    const cutil_Status result = cutil_mkdirp(path, 0755);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
    TEST_ASSERT_TRUE(cutil_isdir(path));

    /* Cleanup */
    cutil_rm(path, CUTIL_FALSE);
}

static void
_should_returnSuccess_when_directoryAlreadyExists(void)
{
    /* Arrange */
    char path[256];
    snprintf(path, sizeof path, "%s%ctest_path_idempotent", PATH_BASE, DELIM);
    cutil_mkdirp(path, 0755);

    /* Act */
    const cutil_Status result = cutil_mkdirp(path, 0755);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
    TEST_ASSERT_TRUE(cutil_isdir(path));

    /* Cleanup */
    cutil_rm(path, CUTIL_FALSE);
}

static void
_should_createNestedDirectories_when_intermediatesAbsent(void)
{
    /* Arrange */
    char parent[256];
    char path[256];
    snprintf(parent, sizeof parent, "%s%ctest_path_nested", PATH_BASE, DELIM);
    snprintf(path, sizeof path, "%s%ctest_path_nested/leaf", PATH_BASE, DELIM);

    /* Act */
    const cutil_Status result = cutil_mkdirp(path, 0755);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
    TEST_ASSERT_TRUE(cutil_isdir(parent));
    TEST_ASSERT_TRUE(cutil_isdir(path));

    /* Cleanup */
    cutil_rm(path, CUTIL_FALSE);
    cutil_rm(parent, CUTIL_FALSE);
}

static void
_should_createDirectory_when_parentExistsAndNotRecursive(void)
{
    /* Arrange */
    char path[256];
    snprintf(
      path, sizeof path, "%s%ctest_mkdir_nonrecursive", PATH_BASE, DELIM
    );
    cutil_mkdirp(PATH_BASE, 0755);

    /* Act */
    const cutil_Status result = cutil_mkdir(path, 0755, CUTIL_FALSE);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
    TEST_ASSERT_TRUE(cutil_isdir(path));

    /* Cleanup */
    cutil_rm(path, CUTIL_FALSE);
}

static void
_should_returnFailure_when_parentAbsentAndNotRecursive(void)
{
    /* Arrange */
    char path[256];
    snprintf(
      path, sizeof path, "%s%ctest_mkdir_noparent/leaf", PATH_BASE, DELIM
    );

    /* Act */
    const cutil_Status result = cutil_mkdir(path, 0755, CUTIL_FALSE);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, result);
    TEST_ASSERT_FALSE(cutil_isdir(path));
}

static void
_should_returnFailure_when_rmPathIsNull(void)
{
    /* Act */
    const cutil_Status result = cutil_rm(NULL, CUTIL_FALSE);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, result);
}

static void
_should_returnFailure_when_pathDoesNotExist(void)
{
    /* Arrange */
    char path[256];
    snprintf(path, sizeof path, "%s%ctest_rm_nonexistent", PATH_BASE, DELIM);

    /* Act */
    const cutil_Status result = cutil_rm(path, CUTIL_FALSE);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, result);
}

static void
_should_removeFile_when_pathIsRegularFile(void)
{
    /* Arrange */
    char path[256];
    snprintf(path, sizeof path, "%s%ctest_rm_file.txt", PATH_BASE, DELIM);
    cutil_mkdirp(PATH_BASE, 0755);
    FILE *const f = fopen(path, "w");
    fclose(f);

    /* Act */
    const cutil_Status result = cutil_rm(path, CUTIL_FALSE);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
    TEST_ASSERT_FALSE(cutil_isfile(path));
}

static void
_should_returnFailure_when_directoryNotEmptyAndNotRecursive(void)
{
    /* Arrange */
    char parent[256];
    char child[256];
    snprintf(parent, sizeof parent, "%s%ctest_rm_nonempty", PATH_BASE, DELIM);
    snprintf(
      child, sizeof child, "%s%ctest_rm_nonempty%csub", PATH_BASE, DELIM, DELIM
    );
    cutil_mkdirp(child, 0755);

    /* Act */
    const cutil_Status result = cutil_rm(parent, CUTIL_FALSE);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, result);
    TEST_ASSERT_TRUE(cutil_isdir(parent));

    /* Cleanup */
    cutil_rmr(parent);
}

static void
_should_removeDirectoryRecursively_when_directoryHasContents(void)
{
    /* Arrange */
    char parent[256];
    char child[256];
    snprintf(parent, sizeof parent, "%s%ctest_rmr_nested", PATH_BASE, DELIM);
    snprintf(
      child, sizeof child, "%s%ctest_rmr_nested%cleaf", PATH_BASE, DELIM, DELIM
    );
    cutil_mkdirp(child, 0755);

    /* Act */
    const cutil_Status result = cutil_rmr(parent);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
    TEST_ASSERT_FALSE(cutil_isdir(parent));
}

static void
_should_returnFalse_when_isdirPathDoesNotExist(void)
{
    /* Arrange */
    char path[256];
    snprintf(path, sizeof path, "%s%ctest_isdir_absent", PATH_BASE, DELIM);

    /* Act */ /* Assert */
    TEST_ASSERT_FALSE(cutil_isdir(path));
}

static void
_should_returnFalse_when_isdirPathIsFile(void)
{
    /* Arrange */
    char path[256];
    snprintf(path, sizeof path, "%s%ctest_isdir_file.txt", PATH_BASE, DELIM);
    cutil_mkdirp(PATH_BASE, 0755);
    FILE *const f = fopen(path, "w");
    fclose(f);

    /* Act */ /* Assert */
    TEST_ASSERT_FALSE(cutil_isdir(path));

    /* Cleanup */
    cutil_rm(path, CUTIL_FALSE);
}

static void
_should_returnFalse_when_isdirPathIsNull(void)
{
    /* Act */ /* Assert */
    TEST_ASSERT_FALSE(cutil_isdir(NULL));
}

static void
_should_returnTrue_when_pathIsRegularFile(void)
{
    /* Arrange */
    char path[256];
    snprintf(
      path, sizeof path, "%s%ctest_isfile_regular.txt", PATH_BASE, DELIM
    );
    cutil_mkdirp(PATH_BASE, 0755);
    FILE *const f = fopen(path, "w");
    fclose(f);

    /* Act */ /* Assert */
    TEST_ASSERT_TRUE(cutil_isfile(path));

    /* Cleanup */
    cutil_rm(path, CUTIL_FALSE);
}

static void
_should_returnFalse_when_isfilePathIsDirectory(void)
{
    /* Arrange */
    char path[256];
    snprintf(path, sizeof path, "%s%ctest_isfile_dir", PATH_BASE, DELIM);
    cutil_mkdirp(path, 0755);

    /* Act */ /* Assert */
    TEST_ASSERT_FALSE(cutil_isfile(path));

    /* Cleanup */
    cutil_rm(path, CUTIL_FALSE);
}

static void
_should_returnFalse_when_isfilePathDoesNotExist(void)
{
    /* Arrange */
    char path[256];
    snprintf(path, sizeof path, "%s%ctest_isfile_absent.txt", PATH_BASE, DELIM);

    /* Act */ /* Assert */
    TEST_ASSERT_FALSE(cutil_isfile(path));
}

static void
_should_returnFalse_when_isfilePathIsNull(void)
{
    /* Act */ /* Assert */
    TEST_ASSERT_FALSE(cutil_isfile(NULL));
}

int
main(void)
{
    UNITY_BEGIN();

    RUN_TEST(_should_returnFailure_when_pathIsNull);
    RUN_TEST(_should_createDirectory_when_pathDoesNotExist);
    RUN_TEST(_should_returnSuccess_when_directoryAlreadyExists);
    RUN_TEST(_should_createNestedDirectories_when_intermediatesAbsent);

    RUN_TEST(_should_createDirectory_when_parentExistsAndNotRecursive);
    RUN_TEST(_should_returnFailure_when_parentAbsentAndNotRecursive);

    RUN_TEST(_should_returnFailure_when_rmPathIsNull);
    RUN_TEST(_should_returnFailure_when_pathDoesNotExist);
    RUN_TEST(_should_removeFile_when_pathIsRegularFile);
    RUN_TEST(_should_returnFailure_when_directoryNotEmptyAndNotRecursive);
    RUN_TEST(_should_removeDirectoryRecursively_when_directoryHasContents);

    RUN_TEST(_should_returnFalse_when_isdirPathDoesNotExist);
    RUN_TEST(_should_returnFalse_when_isdirPathIsFile);
    RUN_TEST(_should_returnFalse_when_isdirPathIsNull);

    RUN_TEST(_should_returnTrue_when_pathIsRegularFile);
    RUN_TEST(_should_returnFalse_when_isfilePathIsDirectory);
    RUN_TEST(_should_returnFalse_when_isfilePathDoesNotExist);
    RUN_TEST(_should_returnFalse_when_isfilePathIsNull);

    TEST_ASSERT_TRUE(cutil_rmr(PATH_BASE) == CUTIL_STATUS_SUCCESS);

    return UNITY_END();
}

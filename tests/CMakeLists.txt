# Set additional compiler flags (UNIX)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS " -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/= ")
endif ()

# Treating warnings as errors would prevent compilation for MSVC
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    string(REPLACE " /WX" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
endif ()

if (NOT TARGET unity)
    # Include unity subdirectory
    add_subdirectory(../submodules/Unity unity EXCLUDE_FROM_ALL)

    # Set unity output directory to test directory
    set_target_properties(
        unity PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                         LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
endif ()

# Set correct cutil version
if (CUTIL_BUILD_SHARED_LIB)
    set(CUTIL_LIB "${CUTIL_LIBRARY}")
else ()
    set(CUTIL_LIB "${CUTIL_LIBRARY}-static")
endif ()

# Set output path for binaries
if (WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif ()

# Set test source files
set(TEST_SOURCES posix/test_getopt.c std/test_stdio.c test_log.c test_stringbuilder.c)

# Loop over all tests
foreach (TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME "${TEST_SOURCE}" NAME_WE)

    # Add tests and executables
    add_executable("${TEST_NAME}" "${TEST_SOURCE}")
    add_test(NAME "${TEST_NAME}" COMMAND "${TEST_NAME}")
    list(APPEND CUTIL_TEST_TARGETS "${TEST_NAME}")

    # Link and include Unity
    target_link_libraries("${TEST_NAME}" PRIVATE unity)

    # Link and include cutil
    target_link_libraries("${TEST_NAME}" PRIVATE "${CUTIL_LIB}")
endforeach ()

# Expose targets to set output directory
set(CUTIL_TEST_TARGETS "${CUTIL_TEST_TARGETS}" CACHE INTERNAL "List of cutil test targets" FORCE)

# "cutil-check" target that runs the tests
if (NOT TARGET cutil-check)
    add_custom_target(
        cutil-check
        COMMAND "${CMAKE_CTEST_COMMAND}" "--output-on-failure"
        DEPENDS "${CUTIL_TEST_TARGETS}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Running cutil tests"
    )
endif ()

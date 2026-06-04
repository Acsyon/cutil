# CXX is only needed for test_cpp.cpp targets; enable it narrowly here rather than in the
# root project (which is C-only by design).
enable_language(CXX)

# Derive test name: test_<dir_underscored>_<stem> e.g. data/generic/test_array.c ->
# test_data_generic_array
function (DERIVE_TEST_NAME TEST_NAME TEST_SOURCE)
    get_filename_component(TEST_DIR "${TEST_SOURCE}" DIRECTORY)
    get_filename_component(TEST_BASENAME "${TEST_SOURCE}" NAME_WE)
    string(REPLACE "/" "_" TEST_DIR_UNDERSCORED "${TEST_DIR}")
    string(REGEX REPLACE "^test_" "" TEST_STEM "${TEST_BASENAME}")
    set(${TEST_NAME} "test_${TEST_DIR_UNDERSCORED}_${TEST_STEM}" PARENT_SCOPE)
endfunction ()

# Create C test executables; appends created target names to the variable named by OUT_TARGETS.
function (CREATE_C_TESTS OUT_TARGETS SOURCE_FILES CUTIL_MODULES)
    set(_local_targets "${${OUT_TARGETS}}")
    # Loop over all C tests
    foreach (TEST_SOURCE IN LISTS ${SOURCE_FILES})
        derive_test_name(TEST_NAME ${TEST_SOURCE})

        # Add tests and executables
        add_executable("${TEST_NAME}" "${TEST_SOURCE}")
        add_test(NAME "${TEST_NAME}" COMMAND "${TEST_NAME}")
        list(APPEND _local_targets "${TEST_NAME}")

        # Add resources directory as a macro
        target_compile_definitions(
            "${TEST_NAME}" PRIVATE CUTIL_TEST_RESOURCE_DIR="${CMAKE_SOURCE_DIR}/tests/resources"
        )

        # Link and include Unity
        target_link_libraries("${TEST_NAME}" PRIVATE unity)

        # Link and include cutil modules
        foreach (CUTIL_MODULE IN LISTS ${CUTIL_MODULES})
            target_link_libraries("${TEST_NAME}" PRIVATE "${CUTIL_MODULE}")
        endforeach ()

        # Enable Unity double-comparison macros
        target_compile_definitions("${TEST_NAME}" PRIVATE UNITY_INCLUDE_DOUBLE)
    endforeach ()
    set(${OUT_TARGETS} ${_local_targets} PARENT_SCOPE)
endfunction ()

# Create C++ test executables; appends created target names to the variable named by OUT_TARGETS.
function (CREATE_CXX_TESTS OUT_TARGETS SOURCE_FILES CUTIL_MODULES)
    set(_local_targets "${${OUT_TARGETS}}")
    # Loop over all C++ tests
    foreach (TEST_SOURCE IN LISTS ${SOURCE_FILES})
        derive_test_name(TEST_NAME ${TEST_SOURCE})

        # Add tests and executables
        add_executable("${TEST_NAME}" "${TEST_SOURCE}")
        add_test(NAME "${TEST_NAME}" COMMAND "${TEST_NAME}")
        list(APPEND _local_targets "${TEST_NAME}")

        # Link and include cutil modules
        foreach (CUTIL_MODULE IN LISTS ${CUTIL_MODULES})
            target_link_libraries("${TEST_NAME}" PRIVATE "${CUTIL_MODULE}")
        endforeach ()
    endforeach ()
    set(${OUT_TARGETS} ${_local_targets} PARENT_SCOPE)
endfunction ()

# Set additional compiler flags (UNIX)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS " -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/= ")
endif ()

if (NOT TARGET unity)
    # Include unity subdirectory
    add_subdirectory(../submodules/Unity unity EXCLUDE_FROM_ALL)

    # Set unity output directory to test directory
    set_target_properties(
        unity PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                         LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )

    # Enable Unity double support (excluded by default)
    target_compile_definitions(unity PUBLIC UNITY_INCLUDE_DOUBLE)
endif ()

# Set correct cutil version
if (CUTIL_BUILD_SHARED_LIB)
    set(CUTIL_LIB_CORE "${CUTIL_LIBRARY_CORE}")
    set(CUTIL_LIB_DATA "${CUTIL_LIBRARY_DATA}")
    set(CUTIL_LIB_SERIAL "${CUTIL_LIBRARY_SERIAL}")
else ()
    set(CUTIL_LIB_CORE "${CUTIL_LIBRARY_CORE}-static")
    set(CUTIL_LIB_DATA "${CUTIL_LIBRARY_DATA}-static")
    set(CUTIL_LIB_SERIAL "${CUTIL_LIBRARY_SERIAL}-static")
endif ()

# Set C test source files
set(C_TEST_SOURCES_CORE
    core/io/test_log.c
    core/os/test_path.c
    core/posix/test_getopt.c
    core/std/test_stdio.c
    core/std/test_string.c
    core/string/test_builder.c
    core/string/test_type.c
    core/util/test_compare.c
    core/util/test_hash.c
    core/util/test_void.c
)
set(C_TEST_SOURCES_DATA
    data/generic/list/test_arraylist.c
    data/generic/map/test_hashmap.c
    data/generic/set/test_hashset.c
    data/generic/string/util/test_iterator.c
    data/generic/string/test_builder.c
    data/generic/string/test_type.c
    data/generic/test_array.c
    data/generic/test_iterator.c
    data/generic/test_list.c
    data/generic/test_map.c
    data/generic/test_object.c
    data/generic/test_set.c
    data/generic/test_type.c
    data/native/test_bitarray.c
)

find_package(libyaml REQUIRED)
find_package(cJSON REQUIRED)

set(C_TEST_SOURCES_SERIAL serial/test_json.c serial/test_yaml.c)

# Set module dependencies
set(MODULE_DEPENDENCIES_CORE ${CUTIL_LIB_CORE})
set(MODULE_DEPENDENCIES_DATA ${CUTIL_LIB_CORE} ${CUTIL_LIB_DATA})
set(MODULE_DEPENDENCIES_SERIAL ${CUTIL_LIB_CORE} ${CUTIL_LIB_SERIAL} cjson libyaml::libyaml)

create_c_tests(CUTIL_TEST_TARGETS C_TEST_SOURCES_CORE MODULE_DEPENDENCIES_CORE)
create_c_tests(CUTIL_TEST_TARGETS C_TEST_SOURCES_DATA MODULE_DEPENDENCIES_DATA)
create_c_tests(CUTIL_TEST_TARGETS C_TEST_SOURCES_SERIAL MODULE_DEPENDENCIES_SERIAL)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set C++ test source files
set(CXX_TEST_SOURCES_CORE core/test_cpp.cpp)
set(CXX_TEST_SOURCES_DATA data/test_cpp.cpp)
set(CXX_TEST_SOURCES_SERIAL serial/test_cpp.cpp)

create_cxx_tests(CUTIL_TEST_TARGETS CXX_TEST_SOURCES_CORE MODULE_DEPENDENCIES_CORE)
create_cxx_tests(CUTIL_TEST_TARGETS CXX_TEST_SOURCES_DATA MODULE_DEPENDENCIES_DATA)
create_cxx_tests(CUTIL_TEST_TARGETS CXX_TEST_SOURCES_SERIAL MODULE_DEPENDENCIES_SERIAL)

# Expose targets to set output directory
set(CUTIL_TEST_TARGETS "${CUTIL_TEST_TARGETS}" CACHE INTERNAL "List of cutil test targets" FORCE)

# "cutil-check" target that runs the tests
if (NOT TARGET cutil-check)
    add_custom_target(
        cutil-check
        COMMAND "${CMAKE_CTEST_COMMAND}" "--output-on-failure"
        DEPENDS "${CUTIL_TEST_TARGETS}"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Running cutil tests"
    )
endif ()

project("CUTIL_TESTS" LANGUAGES C CXX)

# Derive test name: test_<dir_underscored>_<stem> e.g. data/generic/test_array.c ->
# test_data_generic_array
function (DERIVE_TEST_NAME test_name test_source)
    get_filename_component(TEST_DIR "${test_source}" DIRECTORY)
    get_filename_component(TEST_BASENAME "${test_source}" NAME_WE)
    string(REPLACE "/" "_" TEST_DIR_UNDERSCORED "${TEST_DIR}")
    string(REGEX REPLACE "^test_" "" TEST_STEM "${TEST_BASENAME}")
    set(${test_name} "test_${TEST_DIR_UNDERSCORED}_${TEST_STEM}" PARENT_SCOPE)
endfunction ()

# Create C test executables
function (CREATE_C_TESTS source_files cutil_modules)
    # Loop over all C tests
    foreach (TEST_SOURCE IN LISTS ${source_files})
        derive_test_name(TEST_NAME ${TEST_SOURCE})

        # Add tests and executables
        add_executable("${TEST_NAME}" "${TEST_SOURCE}")
        add_test(NAME "${TEST_NAME}" COMMAND "${TEST_NAME}")
        list(APPEND CUTIL_TEST_TARGETS "${TEST_NAME}")

        # Add resources directory as a macro
        target_compile_definitions(
            "${TEST_NAME}" PRIVATE CUTIL_TEST_RESOURCE_DIR="${CMAKE_SOURCE_DIR}/tests/resources"
        )

        # Link and include Unity
        target_link_libraries("${TEST_NAME}" PRIVATE unity)

        # Link and include cutil modules
        foreach (CUTIL_MODULE IN LISTS ${cutil_modules})
            target_link_libraries("${TEST_NAME}" PRIVATE "${CUTIL_MODULE}")
        endforeach ()

        # Enable Unity double-comparison macros
        target_compile_definitions("${TEST_NAME}" PRIVATE UNITY_INCLUDE_DOUBLE)
    endforeach ()
endfunction ()

# Create C++ test executables
function (CREATE_CXX_TESTS source_files cutil_modules)
    # Loop over all C++ tests
    foreach (TEST_SOURCE IN LISTS ${source_files})
        derive_test_name(TEST_NAME ${TEST_SOURCE})

        # Add tests and executables
        add_executable("${TEST_NAME}" "${TEST_SOURCE}")
        add_test(NAME "${TEST_NAME}" COMMAND "${TEST_NAME}")
        list(APPEND CUTIL_TEST_TARGETS "${TEST_NAME}")

        # Link and include cutil modules
        foreach (CUTIL_MODULE IN LISTS ${cutil_modules})
            target_link_libraries("${TEST_NAME}" PRIVATE "${CUTIL_MODULE}")
        endforeach ()
    endforeach ()
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
else ()
    set(CUTIL_LIB_CORE "${CUTIL_LIBRARY_CORE}-static")
    set(CUTIL_LIB_DATA "${CUTIL_LIBRARY_DATA}-static")
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

# Set module dependencies
set(MODULE_DEPENDENCIES_CORE ${CUTIL_LIB_CORE})
set(MODULE_DEPENDENCIES_DATA ${CUTIL_LIB_CORE} ${CUTIL_LIB_DATA})

create_c_tests(C_TEST_SOURCES_CORE MODULE_DEPENDENCIES_CORE)
create_c_tests(C_TEST_SOURCES_DATA MODULE_DEPENDENCIES_DATA)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set C++ test source files
set(C_TEST_SOURCES_CORE core/test_cpp.cpp)
set(C_TEST_SOURCES_DATA data/test_cpp.cpp)

create_cxx_tests(C_TEST_SOURCES_CORE MODULE_DEPENDENCIES_CORE)
create_cxx_tests(C_TEST_SOURCES_DATA MODULE_DEPENDENCIES_DATA)

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

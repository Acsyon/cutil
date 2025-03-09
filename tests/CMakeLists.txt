# Set additional compiler flags (UNIX)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION
                                          VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS " -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/= ")
endif()

# Use FetchContent to download Unity if not present
include(FetchContent)
FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG master # You can specify a version if needed
)
FetchContent_MakeAvailable(Unity)

# Link and include Unity
link_directories(${unity_BINARY_DIR})
include_directories("${unity_SOURCE_DIR}/src")
link_libraries(unity)

# Link and include cutil
include_directories(${CUTIL_INCLUDE_DIR})
link_libraries(${CUTIL_LIBRARY})

# Enable testing with CTest
enable_testing()

add_executable(test_log test_log.c)
add_test(NAME test_log COMMAND test_log)

add_executable(test_stringbuilder test_stringbuilder.c)
add_test(NAME test_stringbuilder COMMAND test_stringbuilder)

add_executable(test_stdio std/test_stdio.c)
add_test(NAME test_stdio COMMAND test_stdio)

add_executable(test_getopt posix/test_getopt.c)
add_test(NAME test_getopt COMMAND test_getopt)

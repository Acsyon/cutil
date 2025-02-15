# Use FetchContent to download Unity if not present
include(FetchContent)
FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG master  # You can specify a version if needed
)
FetchContent_MakeAvailable(Unity)

# Link Unity and your source code
link_directories(${unity_BINARY_DIR})
include_directories("${unity_SOURCE_DIR}/src")
link_libraries(unity)

include_directories("../lib/include")
link_libraries(cutil)

# Set C standard and compiler flags
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS OFF)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wformat=1 -Wno-format-truncation -Werror")
endif ()

# Enable testing with CTest
enable_testing()

add_executable(test_stringbuilder test_stringbuilder.c)
add_test(NAME test_stringbuilder COMMAND test_stringbuilder)


# CMake version and project name
cmake_minimum_required(VERSION 3.5...3.31)
project(cutil LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS OFF)

# Set compiler flags (UNIX)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -fcx-limited-range")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -Wformat=1 -Wno-format-truncation")
endif()
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    string(APPEND CMAKE_C_FLAGS " -D_POSIX_C_SOURCE=199309L")
endif()
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    string(APPEND CMAKE_C_FLAGS " -march=native -Wall -Wextra -Wpedantic")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -DNDEBUG \
        -flto=auto \
        -fno-signed-zeros -fno-math-errno")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -O0 -g -Werror")
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO
           " ${CMAKE_C_FLAGS_RELEASE} -O2 -g")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -O3")
endif()

# Set variables for directory and library
set(CUTIL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/include")
set(CUTIL_LIBRARY ${PROJECT_NAME})

# Add the lib directory
add_subdirectory(lib)

# Add the tests directory
add_subdirectory(tests)

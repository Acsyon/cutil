# Set binary directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Set C standard and compiler flags
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS OFF)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
endif ()
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
    if (CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
            -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/src/= \
        ")
    endif ()
    set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O3 \
        -flto=auto \
        -fcx-limited-range -fno-signed-zeros -fno-math-errno \
    ")
    set(CMAKE_C_FLAGS_DEBUG "-g -Wformat=1 -Wno-format-truncation -Werror")
endif ()

# Set source files
include_directories("include/")
set(SOURCE_FILES
    src/stringbuilder.c
    src/std/stdio.c
)

# Compile
link_libraries(m)
add_library("${PROJECT_NAME}" SHARED ${SOURCE_FILES})


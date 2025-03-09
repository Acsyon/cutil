# Set additional compiler flags (UNIX)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION
                                          VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS
           " -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/src/= ")
endif()

# Set source files
include_directories("include/")
set(SOURCE_FILES src/log.c src/stringbuilder.c src/posix/getopt.c
                 src/std/stdio.c)

# Set output path for binary
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# Compile
link_libraries(m)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

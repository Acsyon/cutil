# Set additional compiler flags (UNIX)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS " -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/src/= ")
endif ()

# Set source files
set(SOURCE_FILES src/io/log.c src/posix/getopt.c src/std/stdio.c src/std/stdlib.c src/std/string.c
                 src/string/builder.c
)

# Compile shared library
if (CUTIL_BUILD_SHARED_LIB)
    add_library("${CUTIL_LIBRARY}" SHARED "${SOURCE_FILES}")

    if (NOT WIN32)
        target_link_libraries("${CUTIL_LIBRARY}" PRIVATE m)
    endif ()

    target_include_directories(
        "${CUTIL_LIBRARY}" PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                  "$<INSTALL_INTERFACE:include>"
    )

    install(
        TARGETS "${CUTIL_LIBRARY}"
        EXPORT "${CUTIL_LIBRARY}-targets"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        INCLUDES
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
endif ()

# Compile static library
if (CUTIL_BUILD_STATIC_LIB)
    add_library("${CUTIL_LIBRARY}-static" STATIC "${SOURCE_FILES}")
    set_target_properties(
        "${CUTIL_LIBRARY}-static" PROPERTIES OUTPUT_NAME "${CUTIL_LIBRARY}" PREFIX "lib"
    )

    if (NOT WIN32)
        target_link_libraries("${CUTIL_LIBRARY}-static" PRIVATE m)
    endif ()

    target_include_directories(
        "${CUTIL_LIBRARY}-static" PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                         "$<INSTALL_INTERFACE:include>"
    )

    install(
        TARGETS "${CUTIL_LIBRARY}-static"
        EXPORT "${CUTIL_LIBRARY}-targets"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        INCLUDES
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
endif ()

# Install headers
install(
    DIRECTORY include/
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING
    PATTERN "*.h"
)

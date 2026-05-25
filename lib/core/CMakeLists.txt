# Set additional compiler flags (UNIX)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS " -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/src/= ")
endif ()

# Set source files
set(SOURCE_FILES_CORE
    src/io/log.c
    src/os/path.c
    src/posix/getopt.c
    src/std/stdio.c
    src/std/stdlib.c
    src/std/string.c
    src/string/builder.c
    src/string/type.c
    src/util/compare.c
    src/util/hash.c
    src/util/void.c
    src/status.c
)

# Compile shared library
if (CUTIL_BUILD_SHARED_LIB)
    add_library("${CUTIL_LIBRARY_CORE}" SHARED "${SOURCE_FILES_CORE}")
    set_target_properties(
        "${CUTIL_LIBRARY_CORE}" PROPERTIES OUTPUT_NAME "${CUTIL_LIBRARY_CORE}"
                                           PREFIX "lib"
    )

    if (NOT WIN32)
        target_link_libraries("${CUTIL_LIBRARY_CORE}" PRIVATE m)
    endif ()

    target_include_directories(
        "${CUTIL_LIBRARY_CORE}"
        PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../lib/include>"
               "$<INSTALL_INTERFACE:include>"
    )

    install(
        TARGETS "${CUTIL_LIBRARY_CORE}"
        EXPORT "${CUTIL_LIBRARY}-targets"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        INCLUDES
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        NAMESPACE "${CUTIL_LIBRARY}::"
        COMPONENT "${CUTIL_COMPONENT_CORE}"
    )
endif ()

# Compile static library
if (CUTIL_BUILD_STATIC_LIB)
    add_library("${CUTIL_LIBRARY_CORE}-static" STATIC "${SOURCE_FILES_CORE}")
    set_target_properties(
        "${CUTIL_LIBRARY_CORE}-static"
        PROPERTIES OUTPUT_NAME "${CUTIL_LIBRARY}${CUTIL_LIBRARY_CORE}" PREFIX "lib"
    )

    if (NOT WIN32)
        target_link_libraries("${CUTIL_LIBRARY_CORE}-static" PRIVATE m)
    endif ()

    target_include_directories(
        "${CUTIL_LIBRARY_CORE}-static"
        PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../lib/include>"
               "$<INSTALL_INTERFACE:include>"
    )

    install(
        TARGETS "${CUTIL_LIBRARY_CORE}-static"
        EXPORT "${CUTIL_LIBRARY}-targets"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        INCLUDES
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        NAMESPACE "${CUTIL_LIBRARY}::"
        COMPONENT "${CUTIL_COMPONENT_CORE}"
    )
endif ()

# Install headers
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/include/cutil/${CUTIL_COMPONENT_CORE}"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING
    PATTERN "*.h"
)

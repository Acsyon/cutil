# Set additional compiler flags (UNIX)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS " -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/src/= ")
endif ()

find_package(libyaml REQUIRED)

# Set source files
set(SOURCE_FILES_SERIAL src/yaml.c)

# Compile shared library
if (CUTIL_BUILD_SHARED_LIB)
    add_library("${CUTIL_LIBRARY_SERIAL}" SHARED "${SOURCE_FILES_SERIAL}")
    set_target_properties(
        "${CUTIL_LIBRARY_SERIAL}" PROPERTIES OUTPUT_NAME "${CUTIL_LIBRARY_SERIAL}" PREFIX "lib"
    )

    if (NOT WIN32)
        target_link_libraries("${CUTIL_LIBRARY_SERIAL}" PRIVATE m)
    endif ()

    target_link_libraries("${CUTIL_LIBRARY_SERIAL}" PUBLIC "${CUTIL_LIBRARY_CORE}" libyaml::libyaml)

    target_include_directories(
        "${CUTIL_LIBRARY_SERIAL}"
        # The relative path is intentional: using ${CMAKE_SOURCE_DIR} would break when cutil is
        # consumed as a CMake subdirectory in another project.
        PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../lib/include>"
               "$<INSTALL_INTERFACE:include>"
    )

    install(
        TARGETS "${CUTIL_LIBRARY_SERIAL}"
        EXPORT "${CUTIL_EXPORT_SET}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        INCLUDES
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT "${CUTIL_COMPONENT_SERIAL}"
    )
endif ()

# Compile static library
if (CUTIL_BUILD_STATIC_LIB)
    add_library("${CUTIL_LIBRARY_SERIAL}-static" STATIC "${SOURCE_FILES_SERIAL}")
    set_target_properties(
        "${CUTIL_LIBRARY_SERIAL}-static" PROPERTIES OUTPUT_NAME "${CUTIL_LIBRARY_SERIAL}" PREFIX
                                                    "lib"
    )

    if (NOT WIN32)
        target_link_libraries("${CUTIL_LIBRARY_SERIAL}-static" PRIVATE m)
    endif ()

    target_link_libraries("${CUTIL_LIBRARY_SERIAL}-static" PUBLIC "${CUTIL_LIBRARY_CORE}")

    target_include_directories(
        "${CUTIL_LIBRARY_SERIAL}-static"
        # The relative path is intentional: using ${CMAKE_SOURCE_DIR} would break when cutil is
        # consumed as a CMake subdirectory in another project.
        PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../lib/include>"
               "$<INSTALL_INTERFACE:include>"
    )

    install(
        TARGETS "${CUTIL_LIBRARY_SERIAL}-static"
        EXPORT "${CUTIL_EXPORT_SET}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        INCLUDES
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT "${CUTIL_COMPONENT_SERIAL}"
    )
endif ()

# Install headers
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/include/cutil/${CUTIL_COMPONENT_SERIAL}"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING
    PATTERN "*.h"
)

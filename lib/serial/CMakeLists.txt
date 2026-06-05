# Set additional compiler flags (UNIX)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS " -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/src/= ")
endif ()

if (CUTIL_SERIAL_LINK_STATIC)
    # Set BUILD_SHARED_AND_STATIC_LIBS to ensure both targets are available
    set(BUILD_SHARED_AND_STATIC_LIBS ON CACHE BOOL "" FORCE)

    # Set CMAKE_POLICY_VERSION_MINIMUM to ensure that both targets build
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

    # Ensure cJSON is built
    if (NOT TARGET cjson AND NOT TARGET cjson-static)
        # Prevent cJSON tests from being added (save and restore BUILD_TESTING)
        set(_SAVED_BUILD_TESTING "${BUILD_TESTING}")
        set(BUILD_TESTING OFF)
        add_subdirectory(../../submodules/cJSON cJSON EXCLUDE_FROM_ALL)
        if (DEFINED _SAVED_BUILD_TESTING)
            set(BUILD_TESTING "${_SAVED_BUILD_TESTING}")
        else ()
            unset(BUILD_TESTING)
        endif ()
        unset(_SAVED_BUILD_TESTING)
        # Set output directories to match cutil convention
        if (TARGET cjson)
            set_target_properties(
                cjson PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CUTIL_OUTPUT_DIR}"
                                 ARCHIVE_OUTPUT_DIRECTORY "${CUTIL_OUTPUT_DIR}"
            )
            # Suppress -Werror for cjson to handle portability warnings
            target_compile_options(cjson PRIVATE -Wno-error)
        endif ()
        if (TARGET cjson-static)
            set_target_properties(
                cjson-static PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CUTIL_OUTPUT_DIR}"
                                        ARCHIVE_OUTPUT_DIRECTORY "${CUTIL_OUTPUT_DIR}"
            )
            # Suppress -Werror for cjson-static to handle portability warnings
            target_compile_options(cjson-static PRIVATE -Wno-error)
        endif ()
    endif ()

    # Ensure libyaml is built
    if (NOT TARGET yaml)
        add_subdirectory(../../submodules/libyaml libyaml EXCLUDE_FROM_ALL)
        # Set output directories to match cutil convention
        if (TARGET yaml)
            set_target_properties(
                yaml PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CUTIL_OUTPUT_DIR}"
                                ARCHIVE_OUTPUT_DIRECTORY "${CUTIL_OUTPUT_DIR}"
            )
            # Suppress -Werror for libyaml to handle portability warnings
            target_compile_options(yaml PRIVATE -Wno-error)
            # Set POSIX version to make libyaml compile
            target_compile_options(yaml PRIVATE -D_POSIX_C_SOURCE=200809L)
        endif ()
    endif ()
else ()
    find_package(libyaml REQUIRED)
    find_package(cJSON REQUIRED)
endif ()

# Set source files
set(SOURCE_FILES_SERIAL src/json.c src/yaml.c)

# Compile shared library
if (CUTIL_BUILD_SHARED_LIB)
    add_library("${CUTIL_LIBRARY_SERIAL}" SHARED "${SOURCE_FILES_SERIAL}")
    set_target_properties(
        "${CUTIL_LIBRARY_SERIAL}" PROPERTIES OUTPUT_NAME "${CUTIL_LIBRARY_SERIAL}" PREFIX "lib"
    )

    if (NOT WIN32)
        target_link_libraries("${CUTIL_LIBRARY_SERIAL}" PRIVATE m)
    endif ()

    if (CUTIL_SERIAL_LINK_STATIC)
        target_link_libraries(
            "${CUTIL_LIBRARY_SERIAL}" PUBLIC "${CUTIL_LIBRARY_CORE}" PRIVATE cjson-static yaml
        )
    else ()
        target_link_libraries(
            "${CUTIL_LIBRARY_SERIAL}" PUBLIC "${CUTIL_LIBRARY_CORE}" cjson libyaml::libyaml
        )
    endif ()

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

    if (CUTIL_SERIAL_LINK_STATIC)
        target_link_libraries(
            "${CUTIL_LIBRARY_SERIAL}-static" PUBLIC "${CUTIL_LIBRARY_CORE}" PRIVATE cjson-static
                                                                                    yaml
        )
    else ()
        target_link_libraries("${CUTIL_LIBRARY_SERIAL}-static" PUBLIC "${CUTIL_LIBRARY_CORE}")
    endif ()

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

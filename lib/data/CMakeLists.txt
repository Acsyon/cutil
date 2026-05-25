# Set additional compiler flags (UNIX)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
    string(APPEND CMAKE_C_FLAGS " -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/src/= ")
endif ()

# Set source files
set(SOURCE_FILES_DATA
    src/generic/list/arraylist.c
    src/generic/map/hashmap.c
    src/generic/set/hashset.c
    src/generic/string/util/iterator.c
    src/generic/string/builder.c
    src/generic/string/type.c
    src/generic/array.c
    src/generic/list.c
    src/generic/iterator.c
    src/generic/map.c
    src/generic/object.c
    src/generic/set.c
    src/generic/type.c
    src/native/bitarray.c
)

# Compile shared library
if (CUTIL_BUILD_SHARED_LIB)
    add_library("${CUTIL_LIBRARY_DATA}" SHARED "${SOURCE_FILES_DATA}")
    set_target_properties(
        "${CUTIL_LIBRARY_DATA}" PROPERTIES OUTPUT_NAME "${CUTIL_LIBRARY}${CUTIL_LIBRARY_DATA}"
                                           PREFIX "lib"
    )

    if (NOT WIN32)
        target_link_libraries("${CUTIL_LIBRARY_DATA}" PRIVATE m)
    endif ()

    target_link_libraries("${CUTIL_LIBRARY_DATA}" PUBLIC "${CUTIL_LIBRARY_CORE}")

    target_include_directories(
        "${CUTIL_LIBRARY_DATA}" PUBLIC "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/include>"
                                       "$<INSTALL_INTERFACE:include>"
    )

    install(
        TARGETS "${CUTIL_LIBRARY_DATA}"
        EXPORT "${CUTIL_LIBRARY}-targets"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        INCLUDES
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        NAMESPACE "${CUTIL_LIBRARY}::"
        COMPONENT "${CUTIL_COMPONENT_DATA}"
    )
endif ()

# Compile static library
if (CUTIL_BUILD_STATIC_LIB)
    add_library("${CUTIL_LIBRARY_DATA}-static" STATIC "${SOURCE_FILES_DATA}")
    set_target_properties(
        "${CUTIL_LIBRARY_DATA}-static"
        PROPERTIES OUTPUT_NAME "${CUTIL_LIBRARY}${CUTIL_LIBRARY_DATA}" PREFIX "lib"
    )

    if (NOT WIN32)
        target_link_libraries("${CUTIL_LIBRARY_DATA}-static" PRIVATE m)
    endif ()

    target_link_libraries("${CUTIL_LIBRARY_DATA}-static" PUBLIC "${CUTIL_LIBRARY_CORE}")

    target_include_directories(
        "${CUTIL_LIBRARY_DATA}-static" PUBLIC "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/include>"
                                              "$<INSTALL_INTERFACE:include>"
    )

    install(
        TARGETS "${CUTIL_LIBRARY_DATA}-static"
        EXPORT "${CUTIL_LIBRARY}-targets"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        INCLUDES
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        NAMESPACE "${CUTIL_LIBRARY}::"
        COMPONENT "${CUTIL_COMPONENT_DATA}"
    )
endif ()

# Install headers
install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/lib/include/cutil/${CUTIL_COMPONENT_DATA}"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING
    PATTERN "*.h"
)

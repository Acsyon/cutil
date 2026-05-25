# CMake version and project name
cmake_minimum_required(VERSION 3.24...4.3.2)
set(CUTIL_LIBRARY "cutil")
# Export set name is derived from the library name; all install(TARGETS EXPORT ...) and
# install(EXPORT ...) blocks use this variable to ensure the name stays in sync.
set(CUTIL_EXPORT_SET "${CUTIL_LIBRARY}-targets")
project("${CUTIL_LIBRARY}" LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS OFF)

# Set compiler flags (UNIX)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -fcx-limited-range")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -Wformat=1")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -Wno-format-truncation")
endif ()

if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    string(APPEND CMAKE_C_FLAGS " -D_POSIX_C_SOURCE=199309L")
endif ()

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    string(APPEND CMAKE_C_FLAGS " -march=native")
    string(APPEND CMAKE_C_FLAGS " -Wall")
    string(APPEND CMAKE_C_FLAGS " -Wextra")
    string(APPEND CMAKE_C_FLAGS " -Wpedantic")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -DNDEBUG")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -flto=auto")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -fno-signed-zeros")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -fno-math-errno")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -O0")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -g")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -Werror")
    # RelWithDebInfo intentionally uses -O2, not -O3: CMAKE_C_FLAGS_RELEASE is expanded at this
    # point (before -O3 is appended below), so RelWithDebInfo inherits all Release flags except -O3.
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " ${CMAKE_C_FLAGS_RELEASE}")
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " -O2")
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " -g")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -O3")
endif ()

# Set output path for binaries. CMAKE_BINARY_DIR is read-only after project(); use CUTIL_OUTPUT_DIR
# for the artefact path.
set(CUTIL_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CUTIL_OUTPUT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CUTIL_OUTPUT_DIR}")

# Options
option(CUTIL_BUILD_SHARED_AND_STATIC_LIBS "Build both shared and static libraries" ON)
option(ENABLE_TESTS "Build and enable unit tests" OFF)

# Utility variables

# When CUTIL_BUILD_SHARED_AND_STATIC_LIBS is ON (default), both flags are always set regardless of
# BUILD_SHARED_LIBS; the OR-conditions below are unreachable while the combined option is ON. They
# serve as a fallback when that option is explicitly disabled.
if (CUTIL_BUILD_SHARED_AND_STATIC_LIBS OR BUILD_SHARED_LIBS)
    set(CUTIL_BUILD_SHARED_LIB TRUE)
endif ()
if (CUTIL_BUILD_SHARED_AND_STATIC_LIBS OR NOT BUILD_SHARED_LIBS)
    set(CUTIL_BUILD_STATIC_LIB TRUE)
endif ()

# Include GNUInstallDirs BEFORE setting CMAKE_INSTALL_INCLUDEDIR
include(GNUInstallDirs)

# Set correct include directory (append to system include dir)
set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/${CUTIL_LIBRARY}")

# Add core library
set(CUTIL_COMPONENT_CORE "core")
set(CUTIL_LIBRARY_CORE "${CUTIL_LIBRARY}${CUTIL_COMPONENT_CORE}")
add_subdirectory(lib/core)

# Add data library
set(CUTIL_COMPONENT_DATA data)
set(CUTIL_LIBRARY_DATA "${CUTIL_LIBRARY}${CUTIL_COMPONENT_DATA}")
add_subdirectory(lib/data)

# Include CTest support; also invokes enable_testing() internally.
include(CTest)

if (ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

# Configure the uninstaller
configure_file(cmake/cmake_uninstall.cmake.in cmake_uninstall.cmake @ONLY)

# Add the 'uninstall' target
add_custom_target(
    uninstall COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    COMMENT "Uninstalling..."
)

# Generate config file for find_package()
include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/${CUTIL_LIBRARY}Config.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/${CUTIL_LIBRARY}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CUTIL_LIBRARY}"
)

# Install the generated wrapper config file
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${CUTIL_LIBRARY}Config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CUTIL_LIBRARY}"
)

# Export targets to <CUTIL_EXPORT_SET>.cmake (referenced via include() from cutilConfig.cmake.in)
install(
    EXPORT "${CUTIL_EXPORT_SET}"
    FILE "${CUTIL_EXPORT_SET}.cmake"
    NAMESPACE "${CUTIL_LIBRARY}::"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CUTIL_LIBRARY}"
)

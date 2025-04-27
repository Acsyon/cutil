# CMake version and project name
cmake_minimum_required(VERSION 3.5...3.31)
set(CUTIL_LIBRARY "cutil")
project("{CUTIL_LIBRARY}" LANGUAGES C)

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
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " ${CMAKE_C_FLAGS_RELEASE}")
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " -O2")
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " -g")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -O3")
endif ()

# Set compiler flags (MSVC)
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    string(APPEND CMAKE_C_FLAGS " /W4")    
    string(APPEND CMAKE_C_FLAGS " /WX")
    string(APPEND CMAKE_C_FLAGS_RELEASE " /O2")
    string(APPEND CMAKE_C_FLAGS_RELEASE " /DNDEBUG")
    string(APPEND CMAKE_C_FLAGS_DEBUG " /Od")
    string(APPEND CMAKE_C_FLAGS_DEBUG " /Zi")
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " ${CMAKE_C_FLAGS_RELEASE}")
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " /Zi")
endif()

# Set output path for binaries
set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Options
option(CUTIL_BUILD_SHARED_AND_STATIC_LIBS "Build both shared and static libraries" ON)

# Utility variables
if (CUTIL_BUILD_SHARED_AND_STATIC_LIBS OR BUILD_SHARED_LIBS)
    set(CUTIL_BUILD_SHARED_LIB TRUE)
endif ()
if (CUTIL_BUILD_SHARED_AND_STATIC_LIBS OR NOT BUILD_SHARED_LIBS)
    set(CUTIL_BUILD_STATIC_LIB TRUE)
endif ()

# Add the lib directory
add_subdirectory(lib)

# Add the tests directory
include(CTest)

# Configure the uninstaller
configure_file(cmake/cmake_uninstall.cmake.in cmake_uninstall.cmake @ONLY)

# Add the 'uninstall' target
add_custom_target(
    uninstall COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    COMMENT "Uninstalling..."
)

# Include GNUInstallDirs for proper install destinations
include(GNUInstallDirs)

# Generate and install export configuration
install(
    EXPORT "${CUTIL_LIBRARY}-targets"
    FILE "${CUTIL_LIBRARY}Config.cmake"
    NAMESPACE "${CUTIL_LIBRARY}::"
    DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/${CUTIL_LIBRARY}"
)

# Generate config file for find_package()
include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/${CUTIL_LIBRARY}Config.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/${CUTIL_LIBRARY}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CUTIL_LIBRARY}"
)

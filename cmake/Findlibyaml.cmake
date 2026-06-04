# Findlibyaml.cmake
#
# Locates the libyaml library.
#
# Primary strategy: pkg-config with package name 'yaml-0.1'.
# Fallback: find_path(yaml.h) + find_library(yaml).
#
# Result variables:
#   libyaml_FOUND        - True if the library was found
#   libyaml_INCLUDE_DIR  - Directory containing yaml.h
#   libyaml_LIBRARY      - Full path to the libyaml library
#
# Imported target:
#   libyaml::libyaml

include(FindPackageHandleStandardArgs)
find_package(PkgConfig QUIET)

if (PkgConfig_FOUND)
    pkg_check_modules(_libyaml QUIET yaml-0.1)
endif ()

find_path(libyaml_INCLUDE_DIR
    NAMES yaml.h
    HINTS ${_libyaml_INCLUDE_DIRS}
)

find_library(libyaml_LIBRARY
    NAMES yaml
    HINTS ${_libyaml_LIBRARY_DIRS}
)

find_package_handle_standard_args(libyaml
    REQUIRED_VARS libyaml_LIBRARY libyaml_INCLUDE_DIR
    VERSION_VAR _libyaml_VERSION
)

if (libyaml_FOUND AND NOT TARGET libyaml::libyaml)
    add_library(libyaml::libyaml UNKNOWN IMPORTED)
    set_target_properties(libyaml::libyaml PROPERTIES
        IMPORTED_LOCATION             "${libyaml_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${libyaml_INCLUDE_DIR}"
    )
endif ()

mark_as_advanced(libyaml_INCLUDE_DIR libyaml_LIBRARY)

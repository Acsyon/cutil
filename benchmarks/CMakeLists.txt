# Benchmark suite for cutil
# Enabled via -DENABLE_BENCHMARKS=TRUE

cmake_minimum_required(VERSION 3.5...3.31)
project(CUTIL_BENCHMARKS LANGUAGES C CXX)

# C++ standard for STL benchmarks
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

# Determine cutil link target (mirrors tests/CMakeLists.txt convention)
if(CUTIL_BUILD_SHARED_LIB)
    set(CUTIL_LIB "${CUTIL_LIBRARY}")
else()
    set(CUTIL_LIB "${CUTIL_LIBRARY}-static")
endif()

# --------------------------------------------------------------------------
# cutil C benchmarks
# --------------------------------------------------------------------------

set(CUTIL_BENCH_SOURCES
    cutil/bench_hashmap.c
    cutil/bench_hashset.c
    cutil/bench_arraylist.c
)

set(ALL_BENCH_TARGETS "")

foreach(SRC ${CUTIL_BENCH_SOURCES})
    get_filename_component(STEM "${SRC}" NAME_WE)
    set(BENCH_NAME "${STEM}")
    add_executable("${BENCH_NAME}" "${SRC}")
    target_include_directories("${BENCH_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries("${BENCH_NAME}" PRIVATE "${CUTIL_LIB}")
    list(APPEND ALL_BENCH_TARGETS "${BENCH_NAME}")
endforeach()

# --------------------------------------------------------------------------
# STL C++ benchmarks
# --------------------------------------------------------------------------

set(STL_BENCH_SOURCES
    stl/bench_unordered_map.cpp
    stl/bench_unordered_set.cpp
    stl/bench_vector.cpp
)

foreach(SRC ${STL_BENCH_SOURCES})
    get_filename_component(STEM "${SRC}" NAME_WE)
    set(BENCH_NAME "${STEM}")
    add_executable("${BENCH_NAME}" "${SRC}")
    target_include_directories("${BENCH_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    # STL benchmarks do not link cutil, but they do include bench_harness.h
    list(APPEND ALL_BENCH_TARGETS "${BENCH_NAME}")
endforeach()

# --------------------------------------------------------------------------
# Aggregate build target
# --------------------------------------------------------------------------

add_custom_target(cutil-bench
    DEPENDS ${ALL_BENCH_TARGETS}
    COMMENT "Building all benchmark executables"
)

# --------------------------------------------------------------------------
# Run target — execute all benchmarks and collect CSV output
# --------------------------------------------------------------------------

# Output directory for CSV files (created at configure time)
set(BENCH_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/results")
file(MAKE_DIRECTORY "${BENCH_OUTPUT_DIR}")

# Build the COMMAND list: one command per executable
set(BENCH_RUN_COMMANDS "")
foreach(TARGET_NAME ${ALL_BENCH_TARGETS})
    list(APPEND BENCH_RUN_COMMANDS
        COMMAND "$<TARGET_FILE:${TARGET_NAME}>" > "${BENCH_OUTPUT_DIR}/${TARGET_NAME}.csv"
    )
endforeach()

add_custom_target(cutil-bench-run
    ${BENCH_RUN_COMMANDS}
    DEPENDS cutil-bench
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Running all benchmarks and writing CSV to ${BENCH_OUTPUT_DIR}/"
)

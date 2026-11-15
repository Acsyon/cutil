# cutil

Welcome to my C99 utility library! This project serves multiple purposes: It is a learning experience for writing reusable and unit-tested C code, an effort to achieve cross-platform compatibility and a centralized repository for commonly used code that I have rewritten multiple times across various projects.

## Goals

- **Learn to build a proper C library**: Focus on modularity, maintainability, and reusability.
- **Achieve cross-platform compatibility**: Currently tested on Linux, with the aim of supporting more in the future.
- **Implement unit testing in C**: Each feature of the library should be unit-tested.
- **Centralize reusable code**: Avoid redundant implementations by consolidating common utility functions.

## Features

The library is organized by domain / module, each providing a focused set of utilities:

- **Core** – Foundational utilities organized as a dedicated module:
  - Standard library wrappers for `stdio`, `stdlib`, `string`, `stddef`, `inttypes`, and `stdbool`
  - String builder and string type with iterator support
  - Hash functions, comparison functions, macro utilities, and a `void` utility
  - I/O utilities, e.g., simple logging system
  - OS-specific utilities, e.g., directory creation and deletion
  - POSIX utilities, e.g., command-line argument parser inspired by POSIX `getopt` and GNU `getopt_long`
- **Data** – Generic (type-erased) collections / data structures with iterator support:
  - ArrayList, HashSet, HashMap (via vtable-based abstract interfaces: List, Set, Map, Array)
  - Iterator interface for uniform traversal
  - Generic type descriptors for type-safe operations on `void *` elements
  - Native BitArray for compact bit storage

A comprehensive test suite (based on the Unity framework) covers all modules.

## Source Organization

Sources and headers follow a domain-based layout:

```
lib/
├── include/cutil/       # Public API headers (consumed as #include <cutil/...>)
│   ├── core/            # Core module public headers
│   │   ├── debug/       # Debug macros and null-check utilities
│   │   ├── io/          # Logging system
│   │   ├── os/          # OS-specific utilities
│   │   ├── posix/       # POSIX utilities (getopt)
│   │   ├── std/         # stdio, stdlib, stddef, stdbool, inttypes, string wrappers
│   │   ├── string/      # String builder and string type
│   │   └── util/        # Hash, compare, macro, and void utilities
│   └── data/            # Data structures module public headers
│       ├── generic/     # Generic (type-erased) collections
│       │   ├── list/    # ArrayList
│       │   ├── map/     # HashMap
│       │   ├── set/     # HashSet
│       │   └── string/  # String collections
│       │       └── util/ # Iterator
│       └── native/      # Native data structures (BitArray)
├── core/src/            # Core module implementation sources
│   ├── io/              # Logging implementation
│   ├── os/              # OS utilities implementation
│   ├── posix/           # POSIX utilities implementation
│   ├── std/             # Standard library wrappers implementation
│   ├── string/          # String implementation
│   └── util/            # Utility implementation
└── data/src/            # Data structures module implementation sources
    ├── generic/         # Generic collections implementation
    │   ├── list/        # ArrayList
    │   ├── map/         # HashMap
    │   ├── set/         # HashSet
    │   └── string/      # String collections
    │       └── util/    # Iterator
    └── native/          # Native data structures implementation
```

Each domain / module can be linked and included independently even though `data` depends on `core`.

## Getting Started

### Prerequisites

- A C99-compatible compiler (e.g., GCC, Clang)
- CMake (≥ 3.24)

### Building the Library

```sh
mkdir -p build && cd build
cmake [-DCMAKE_BUILD_TYPE=<TYPE>] ..
cmake --build .
```

Build types: **Release** (optimized), **Debug** (symbols, `-Werror`), **RelWithDebInfo** (optimized + symbols).

Both shared (`libcutil.so`) and static (`libcutil.a`) libraries are built by default. Compiled artefacts are placed in `bin/`. To build only one variant, set `-DCUTIL_BUILD_SHARED_AND_STATIC_LIBS=OFF`.

### Running Unit Tests

Initialize the Unity test framework submodule (first time only):

```sh
git submodule update --init --remote submodules/Unity
```

Configure with tests enabled, build, and run:

```sh
cd build
cmake -DENABLE_TESTS=TRUE [-DCMAKE_BUILD_TYPE=<TYPE>] ..
cmake --build .
ctest --output-on-failure
```

### Compile-Time Logging Configuration

#### When to log

Use the `CUTIL_LOG_*()` macros in all library implementation files and public headers. Never call `cutil_log_*()` functions directly in library code — macros are the canonical interface.

| Level | When to use |
|-------|-------------|
| TRACE | Extremely detailed execution flow; rarely used |
| DEBUG | Performance events: capacity growth, iterator creation, rehash triggers, fallback selections |
| INFO  | General operational messages (reserved; not currently used) |
| WARN  | Invalid parameters, boundary violations, type mismatches, non-critical lookup failures |
| ERROR | I/O failures, allocation failures, validation errors |
| FATAL | Unrecoverable errors (reserved; not currently used) |

#### Macro usage patterns

```c
CUTIL_LOG_DEBUG("ArrayList: capacity grow %zu to %zu", old_cap, new_cap);
CUTIL_LOG_WARN("Key type is not valid");
CUTIL_LOG_ERROR("Could not create path '%s'", path);
```

Always use the `CUTIL_LOG_*()` macros (not direct `cutil_log_*()` functions) for all logging in library implementation files and public headers.

#### CMake configuration

Pass `CUTIL_COMPILE_MIN_LOG_LEVEL` at configure time to suppress logging below a chosen level:

```
cmake -DCUTIL_COMPILE_MIN_LOG_LEVEL=WARN ..
```

| Value  | Disabled levels (compiled out) |
|--------|-------------------------------|
| TRACE  | (none — all logging compiled in, default) |
| DEBUG  | TRACE |
| INFO   | TRACE, DEBUG |
| WARN   | TRACE, DEBUG, INFO |
| ERROR  | TRACE, DEBUG, INFO, WARN |
| FATAL  | TRACE, DEBUG, INFO, WARN, ERROR |

The option affects both `cutilcore` and `cutildata` targets (shared and static variants).

#### Performance note

Logging disabled via `CUTIL_COMPILE_MIN_LOG_LEVEL` has zero runtime overhead — calls are eliminated entirely by the C preprocessor before compilation.

## Roadmap

- Expand data structure offerings and utility functions
- Improve cross-platform support (Windows, macOS)
- Enhance test coverage with additional unit tests

## License

This project is licensed under the MIT License.

## Contact
For any questions or discussions, feel free to reach to me.


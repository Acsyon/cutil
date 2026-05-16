# cutil

Welcome to my C99 utility library! This project serves multiple purposes: It is a learning experience for writing reusable and unit-tested C code, an effort to achieve cross-platform compatibility and a centralized repository for commonly used code that I have rewritten multiple times across various projects.

## Goals

- **Learn to build a proper C library**: Focus on modularity, maintainability, and reusability.
- **Achieve cross-platform compatibility**: Currently tested on Linux, with the aim of supporting more in the future.
- **Implement unit testing in C**: Each feature of the library should be unit-tested.
- **Centralize reusable code**: Avoid redundant implementations by consolidating common utility functions.

## Features

The library is organized by domain, each providing a focused set of utilities:

- **Data structures** – Generic (type-erased) collections with iterator support:
  - ArrayList, HashSet, HashMap (via vtable-based abstract interfaces: List, Set, Map, Array)
  - Iterator interface for uniform traversal
  - Generic type descriptors for type-safe operations on `void *` elements
  - Native BitArray for compact bit storage
- **String** – String builder for incremental string construction; string type with iterator support
- **I/O** – Input and output utilities, e.g., simple logging system
- **OS** – OS-specific utilites, e.g., directory creation and deletion
- **POSIX** – POSIX utilities, e.g., command-line argument parser inspired by POSIX `getopt` and GNU `getopt_long`
- **Standard library** – Portable wrappers for, e.g., `stdio`, `stdlib` and `string`
- **Utilities** – Hash functions, comparison functions, macro utilities

A comprehensive test suite (Unity framework) covers all modules.

## Source Organization

Sources and headers follow a domain-based layout:

```
lib/
├── include/cutil/       # Public API headers (consumed as #include <cutil/...>)
│   ├── data/            # Generic collections, native data structures
│   ├── io/              # Input and output utilities, e.g., logging system
│   ├── os/              # OS-specific utlities
│   ├── posix/           # POSIX utilities, e.g., getopt
│   ├── std/             # Standard library wrappers
│   ├── string/          # String builder, string type
│   └── util/            # Utilities: hashing, comparisons, macros
└── src/                 # Implementation sources (mirrors header layout)
```

## Getting Started

### Prerequisites

- A C99-compatible compiler (e.g., GCC, Clang)
- CMake (≥ 3.5)

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

## Roadmap

- Expand data structure offerings and utility functions
- Improve cross-platform support (Windows, macOS)
- Enhance test coverage with additional unit tests

## License

This project is licensed under the MIT License.

## Contact
For any questions or discussions, feel free to reach to me.


# cutil

Welcome to my C99 utility library! This project serves multiple purposes: It is a learning experience for writing reusable and unit-tested C code, an effort to achieve cross-platform compatibility and a centralized repository for commonly used code that I have rewritten multiple times across various projects.

## Goals

- **Learn to build a proper C library**: Focus on modularity, maintainability, and reusability.
- **Achieve cross-platform compatibility**: Currently tested on Linux, with the aim of supporting more in the future.
- **Implement unit testing in C**: Each feature of the library should be unit-tested.
- **Centralize reusable code**: Avoid redundant implementations by consolidating common utility functions.

## Features

- A collection of commonly used utility functions for C99.

  - A string builder for convenient string handling
  - A parser for command-line arguments heavily inspired by POSIX `getopt` and GNU `getopt_long`
  - A simple logging system 

- A test suite to ensure correctness and stability.


## Getting Started

### Prerequisites

- A C99-compatible compiler (e.g., GCC, Clang, MSVC)
- This project uses CMake as its build system, so make sure to have it installed.

### Building the Library

This project uses CMake for building. Run the following commands:

```sh
cd build
cmake [-DCMAKE_BUILD_TYPE=<TYPE>] ..
cmake --build .
```

Here, `TYPE` specifies the build type. Possible options are

-   **Release**: Enables the highest (sensible) compiler optimizations to improve perfomance (recommended).

-   **Debug**: Disables compiler optimizations and includes debug symbols into the executable.

-   **RelWithDebInfo** (release with debug info): Enable many compiler optimizations, but also include debug symbols.

### Running Unit Tests

If you want to run the unit tests, you have to load the git submodule for the "Unity" unit-test framework:

```sh
git submodule update --init --remote submodules/Unity
```

Afterwards, CMake has to be set up with the appropriate variable. The tests will be built automatically if no target is specified. They can be run using CTest. All of this can be achieved by running the following commands:

```sh
cd build
cmake -DENABLE_TESTS=TRUE [-DCMAKE_BUILD_TYPE=<TYPE>] ..
cmake --build .
ctest
```

## Roadmap
- Expand functionality with more utilities.
- Improve cross-platform support.
- Enhance test coverage with additional unit tests.

## License

This project is licensed under the MIT License.

## Contact
For any questions or discussions, feel free to reach to me.


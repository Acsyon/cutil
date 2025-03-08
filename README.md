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

### Building the Library

This project uses CMake for building. Run the following commands:

```
cd build
cmake [-DCMAKE_BUILD_TYPE=Release] ..
make
```

### Running Unit Tests

The tests have to be enabled using a CMake variable:
```
cd build
cmake -DENABLE_TESTS=true ..
make
```

Thereafter, they can be run using CTest:
```
cd build/tests
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


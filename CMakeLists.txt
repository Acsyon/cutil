# CMake version (all should work) and project name
cmake_minimum_required(VERSION 3.5...3.19)
project(cutil LANGUAGES C)

# Add the lib directory
add_subdirectory(lib)

# Add the tests directory
add_subdirectory(tests)


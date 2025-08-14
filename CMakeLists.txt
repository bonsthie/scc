cmake_minimum_required(VERSION 3.13)

project(scc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(lib)
add_subdirectory(tools)

set(SCC_TARGET "LLVM" CACHE STRING "Semicolon-separated list of formats to support")

set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# add_subdirectory(unittests)

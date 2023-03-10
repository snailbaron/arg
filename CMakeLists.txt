cmake_minimum_required(VERSION 3.0)

project(arg)

set(ARG_BUILD_TESTS TRUE CACHE BOOL "Build tests for arg library")
set(ARG_BUILD_EXAMPLES TRUE CACHE BOOL "Build examples for arg library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
    add_compile_options(/W4 /WX)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_library(arg INTERFACE)
target_include_directories(arg INTERFACE "${PROJECT_SOURCE_DIR}/include")
set_target_properties (arg PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

if(ARG_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(ARG_BUILD_TESTS)
    add_subdirectory(deps)
    enable_testing()
    add_subdirectory(tests)
endif()

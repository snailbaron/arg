cmake_minimum_required(VERSION 3.0)

project(arg)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(arg INTERFACE)
target_include_directories(arg INTERFACE "${PROJECT_SOURCE_DIR}/include")

set(ARG_BUILD_TESTS FALSE CACHE BOOL "Build tests for arg library")

if(ARG_BUILD_TESTS)
    add_subdirectory(deps)
    enable_testing()
    add_subdirectory(tests)
endif()

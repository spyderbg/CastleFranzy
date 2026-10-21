# Compatibility entry point; all platforms share the root x64 CMake build.
cmake_minimum_required(VERSION 3.24)
project(CastleFrenzyWindows LANGUAGES C CXX)
if(NOT WIN32)
    message(FATAL_ERROR "Use the root CMakeLists.txt for Linux.")
endif()
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../.." "${CMAKE_CURRENT_BINARY_DIR}/castle")

# Use a real YAML parser for both flow and block collections, quoting and comments.
include(FetchContent)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_INSTALL OFF CACHE BOOL "" FORCE)
set(YAML_CPP_FORMAT_SOURCE OFF CACHE BOOL "" FORCE)
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(yaml_cpp
    URL https://codeload.github.com/jbeder/yaml-cpp/tar.gz/refs/tags/0.8.0
    URL_HASH SHA256=fbe74bbdcee21d656715688706da3c8becfd946d92cd44705cc6098bb23b3a16
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(yaml_cpp)
add_library(SpriteYaml STATIC "${CMAKE_CURRENT_SOURCE_DIR}/Src/Sprites/SpriteYaml.cpp")
target_include_directories(SpriteYaml PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Src/Sprites")
target_link_libraries(SpriteYaml PRIVATE yaml-cpp::yaml-cpp)

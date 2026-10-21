cmake_minimum_required(VERSION 3.20)

# The BRES resources contain 32-bit pointers and structure layouts.
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    include(cmake/linux32.cmake)
endif()
project(FortressDefender LANGUAGES C CXX)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "Use Projects/Win32 for Windows or Projects/IPhone for iOS.")
endif()
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "The bundled binary assets require a 32-bit build. Install g++-multilib.")
endif()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CASTLE_ASSET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Build/bin" CACHE PATH "Game resource directory")

find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(X11 REQUIRED)
find_library(XXF86VM_LIBRARY Xxf86vm REQUIRED)

set(IRR_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Lib/Irrlicht")
set(IRR_SRC "${IRR_ROOT}/source/Irrlicht")
add_executable(PvrDecode EXCLUDE_FROM_ALL tools/linux/pvr_decode.cpp "${IRR_SRC}/PVRT/PVRTDecompress.cpp")
target_include_directories(PvrDecode PRIVATE "${IRR_ROOT}/include" "${IRR_SRC}")
set_target_properties(PvrDecode PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
file(GLOB ENGINE_SOURCES CONFIGURE_DEPENDS "${IRR_SRC}/*.cpp")
file(GLOB ZLIB_SOURCES CONFIGURE_DEPENDS "${IRR_SRC}/zlib/*.c")
file(GLOB PNG_SOURCES CONFIGURE_DEPENDS "${IRR_SRC}/libpng/png*.c")
# Compile the codec, using this fork's allocator instead of platform backends.
file(GLOB JPEG_SOURCES CONFIGURE_DEPENDS "${IRR_SRC}/jpeglib/j*.c")
list(FILTER JPEG_SOURCES EXCLUDE REGEX "/(jmem(ansi|dos|mac|name|nobs)|jpegtran)\\.c$")
add_library(Irrlicht STATIC ${ENGINE_SOURCES} ${ZLIB_SOURCES} ${PNG_SOURCES} ${JPEG_SOURCES}
    "${IRR_ROOT}/tools/ResFile/src/resFile.cpp" "${IRR_SRC}/PVRT/PVRTDecompress.cpp")
target_include_directories(Irrlicht PUBLIC
    "${IRR_ROOT}/include" "${IRR_ROOT}/tools/ColladaConverter/include"
    "${IRR_ROOT}/tools/ResFile/include" "${IRR_SRC}"
    PRIVATE "${IRR_SRC}/zlib" "${IRR_SRC}/libpng" "${IRR_SRC}/jpeglib"
    "${IRR_SRC}/particlesystem/include" ${X11_INCLUDE_DIR})
target_compile_definitions(Irrlicht PUBLIC _IRR_STATIC_LIB_ PRIVATE
    PNG_NO_MMX_CODE PNG_NO_MNG_FEATURES PNG_THREAD_UNSAFE_OK)
target_link_libraries(Irrlicht PUBLIC OpenGL::GL OpenGL::GLU ${X11_LIBRARIES} ${XXF86VM_LIBRARY} ${CMAKE_DL_LIBS})

add_executable(BdaeCheck EXCLUDE_FROM_ALL tools/linux/bdae_check.cpp)
target_link_libraries(BdaeCheck PRIVATE Irrlicht)
set_target_properties(BdaeCheck PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

file(GLOB SOUND_SOURCES CONFIGURE_DEPENDS "Lib/SoundLib/src/*.cpp")
add_library(SoundLib STATIC ${SOUND_SOURCES})
target_include_directories(SoundLib PUBLIC "Lib/SoundLib/include" ${OPENAL_INCLUDE_DIR})
target_link_libraries(SoundLib PUBLIC Irrlicht ${OPENAL_LIBRARY})

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "Src/*.cpp")
list(FILTER GAME_SOURCES EXCLUDE REGEX "/PlatformSpecifics/(Win32|IPhone)/|AccelerometerWin32\\.cpp$|MediaPlayerWin32\\.cpp$")
add_executable(FortressDefender ${GAME_SOURCES}
    Src/PlatformSpecifics/Win32/CImageLoaderPVR.cpp
    Src/PlatformSpecifics/Win32/PVRTDecompress.cpp)
target_include_directories(FortressDefender PRIVATE Src)
target_compile_definitions(FortressDefender PRIVATE OS_LINUX)
target_link_libraries(FortressDefender PRIVATE SoundLib)
set_target_properties(FortressDefender PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
# The default asset location works regardless of the process working directory.
target_compile_definitions(FortressDefender PRIVATE CASTLE_ASSET_DIR="${CASTLE_ASSET_DIR}")
if(EXISTS "${CASTLE_DEPS}/usr/lib/i386-linux-gnu/dri")
    target_compile_definitions(FortressDefender PRIVATE
        CASTLE_LOCAL_DRI_DIR="${CASTLE_DEPS}/usr/lib/i386-linux-gnu/dri"
        CASTLE_LOCAL_LIBRARY_DIR="${CASTLE_DEPS}/usr/lib/i386-linux-gnu:${CASTLE_DEPS}/usr/lib/i386-linux-gnu/pulseaudio:${CASTLE_DEPS}/usr/lib32")
endif()

add_custom_target(run
    COMMAND "$<TARGET_FILE:FortressDefender>"
    DEPENDS FortressDefender WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Build"
    USES_TERMINAL)

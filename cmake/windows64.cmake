# Build the audio dependency with the same x64 compiler as the game. The legacy
# OpenAL32.dll and emulator DLLs in Build/ are x86 and must never be copied here.
include(FetchContent)
set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_CONFIG OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_HRTF_DATA OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF CACHE BOOL "" FORCE)
set(ALSOFT_EAX OFF CACHE BOOL "" FORCE)
set(ALSOFT_BUILD_IMPORT_LIB OFF CACHE BOOL "" FORCE)
FetchContent_Declare(openal_soft
    URL https://codeload.github.com/kcat/openal-soft/tar.gz/refs/tags/1.24.3
    URL_HASH SHA256=7e1fecdeb45e7f78722b776c5cf30bd33934b961d7fd2a11e0494e064cc631ce
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(openal_soft)
set(OPENAL_INCLUDE_DIR "${openal_soft_SOURCE_DIR}/include")
set(OPENAL_LIBRARY OpenAL)

# Also support native/cross MinGW builds without relying on compiler DLLs in PATH.
if(MINGW)
    foreach(runtime libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll)
        execute_process(COMMAND "${CMAKE_CXX_COMPILER}" "-print-file-name=${runtime}"
            OUTPUT_VARIABLE runtime_path OUTPUT_STRIP_TRAILING_WHITESPACE
            COMMAND_ERROR_IS_FATAL ANY)
        if(NOT EXISTS "${runtime_path}")
            message(FATAL_ERROR "Could not find the x64 MinGW runtime ${runtime}")
        endif()
        list(APPEND CASTLE_MINGW_RUNTIME_DLLS "${runtime_path}")
    endforeach()
endif()

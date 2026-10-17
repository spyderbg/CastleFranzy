
#cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

#project(SoundLib)


set(PROJECT_NAME SoundLib)

#include(../../../Projects/Win32/CMake/Utils.cmake)

################################################################################
# Source groups
################################################################################
set(Headers
    "../../../Src/stdafx.h"
    "../include/AdpcmDecoder.h"
    "../include/AdpcmState.h"
    "../include/AudioDefs.h"
    "../include/BaseSoundManager.h"
    "../include/CSound.h"
    "../include/CSoundData.h"
    "../include/CSoundDecoderADPCM.h"
    "../include/CSoundDecoderMPC.h"
    "../include/CSoundDecoderMSADPCM.h"
    "../include/CSoundDecoderMSWAV.h"
    "../include/CSoundDecoderPCM.h"
    "../include/ISoundDecoder.h"
    "../include/ISoundFile.h"
    "../include/MSHeaders.h"
    "../include/SourcesPool.h"
    "../include/StaticSound.h"
    "../include/StreamingSound.h"
)
source_group("Headers" FILES ${Headers})

set(Sources
    "../src/AdpcmDecoder.cpp"
    "../src/AdpcmState.cpp"
    "../src/BaseSoundManager.cpp"
    "../src/CSound.cpp"
    "../src/CSoundData.cpp"
    "../src/CSoundDecoderADPCM.cpp"
    "../src/CSoundDecoderMPC.cpp"
    "../src/CSoundDecoderMSADPCM.cpp"
    "../src/CSoundDecoderMSWAV.cpp"
    "../src/CSoundDecoderPCM.cpp"
    "../src/SourcesPool.cpp"
    "../src/StaticSound.cpp"
    "../src/StreamingSound.cpp"
)
source_group("Sources" FILES ${Sources})

set(ALL_FILES
    ${Headers}
    ${Sources}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_COMMONDEBUG                       "${CMAKE_CURRENT_SOURCE_DIR}/../build/Win32/Release/"
    OUTPUT_DIRECTORY_COMMONRELEASE                     "${CMAKE_CURRENT_SOURCE_DIR}/../build/Win32/Release/"
    OUTPUT_DIRECTORY_COMMONLITEDEBUG                   "${CMAKE_CURRENT_SOURCE_DIR}/../build/Win32/Debug/"
    OUTPUT_DIRECTORY_COMMONLITERELEASE                 "${CMAKE_CURRENT_SOURCE_DIR}/../build/Win32/Release/"
    OUTPUT_DIRECTORY_DEBUGIPHONEEMULATION              "${CMAKE_CURRENT_SOURCE_DIR}/../build/Win32/Debug/"
    OUTPUT_DIRECTORY_DEBUGMULTITHREADEDIRRLICHTEDITION "${CMAKE_CURRENT_SOURCE_DIR}/../build/Win32/Debug/"
    OUTPUT_DIRECTORY_DEBUGMULTITHREADED                "${CMAKE_CURRENT_SOURCE_DIR}/../build/Win32/Debug/"
    OUTPUT_DIRECTORY_DEBUGSINGLETHREADED               "${CMAKE_CURRENT_SOURCE_DIR}/../build/Win32/Debug/"
    OUTPUT_DIRECTORY_DEBUG                             "${CMAKE_CURRENT_SOURCE_DIR}/../build/Win32/Debug/"
    OUTPUT_DIRECTORY_RELEASEIPHONEEMULATION            "${CMAKE_CURRENT_SOURCE_DIR}/../build/Win32/Release/"
    OUTPUT_DIRECTORY_RELEASEMULTITHREADED              "${CMAKE_CURRENT_SOURCE_DIR}/../build/Win32/Release/"
    OUTPUT_DIRECTORY_RELEASESINGLETHREADED             "${CMAKE_CURRENT_SOURCE_DIR}/../build/Win32/Release/"
    OUTPUT_DIRECTORY_RELEASE                           "${CMAKE_CURRENT_SOURCE_DIR}/../build/Win32/Release/"
)
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
    $<$<CONFIG:CommonDebug>:
        MultiThreaded
    >
    $<$<CONFIG:CommonRelease>:
        MultiThreaded
    >
    $<$<CONFIG:CommonLiteDebug>:
        MultiThreadedDebug
    >
    $<$<CONFIG:CommonLiteRelease>:
        MultiThreaded
    >
    $<$<CONFIG:DebugiPhoneEmulation>:
        MultiThreadedDebug
    >
    $<$<CONFIG:DebugMultithreadedIrrLichtEdition>:
        MultiThreadedDebug
    >
    $<$<CONFIG:DebugMultithreaded>:
        MultiThreadedDebug
    >
    $<$<CONFIG:DebugSinglethreaded>:
        MultiThreadedDebug
    >
    $<$<CONFIG:Debug>:
        MultiThreadedDebug
    >
    $<$<CONFIG:ReleaseiPhoneEmulation>:
        MultiThreaded
    >
    $<$<CONFIG:ReleaseMultithreaded>:
        MultiThreaded
    >
    $<$<CONFIG:ReleaseSinglethreaded>:
        MultiThreaded
    >
    $<$<CONFIG:Release>:
        MultiThreaded
    >
    $<$<NOT:$<OR:$<CONFIG:CommonDebug>,$<CONFIG:CommonRelease>,$<CONFIG:CommonLiteDebug>,$<CONFIG:CommonLiteRelease>,$<CONFIG:DebugiPhoneEmulation>,$<CONFIG:DebugMultithreadedIrrLichtEdition>,$<CONFIG:DebugMultithreaded>,$<CONFIG:DebugSinglethreaded>,$<CONFIG:Debug>,$<CONFIG:ReleaseiPhoneEmulation>,$<CONFIG:ReleaseMultithreaded>,$<CONFIG:ReleaseSinglethreaded>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
)
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../Src;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../LibOpenAL/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Irrlicht/include"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:CommonDebug>:"
        "NDEBUG"
    ">"
    "$<$<CONFIG:CommonRelease>:"
        "NDEBUG"
    ">"
    "$<$<CONFIG:CommonLiteDebug>:"
        "_DEBUG"
    ">"
    "$<$<CONFIG:CommonLiteRelease>:"
        "NDEBUG"
    ">"
    "$<$<CONFIG:DebugiPhoneEmulation>:"
        "_DEBUG"
    ">"
    "$<$<CONFIG:DebugMultithreadedIrrLichtEdition>:"
        "_DEBUG"
    ">"
    "$<$<CONFIG:DebugMultithreaded>:"
        "_DEBUG"
    ">"
    "$<$<CONFIG:DebugSinglethreaded>:"
        "_DEBUG"
    ">"
    "$<$<CONFIG:Debug>:"
        "_DEBUG"
    ">"
    "$<$<CONFIG:ReleaseiPhoneEmulation>:"
        "NDEBUG"
    ">"
    "$<$<CONFIG:ReleaseMultithreaded>:"
        "NDEBUG"
    ">"
    "$<$<CONFIG:ReleaseSinglethreaded>:"
        "NDEBUG"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG"
    ">"
    "WIN32;"
    "_WINDOWS"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:CommonDebug>:
            /Zi
        >
        $<$<CONFIG:CommonRelease>:
            /Zi
        >
        $<$<CONFIG:CommonLiteDebug>:
            /RTC1;
            /ZI
        >
        $<$<CONFIG:CommonLiteRelease>:
            /Zi
        >
        $<$<CONFIG:DebugiPhoneEmulation>:
            /RTC1;
            /ZI
        >
        $<$<CONFIG:DebugMultithreadedIrrLichtEdition>:
            /RTC1;
            /ZI
        >
        $<$<CONFIG:DebugMultithreaded>:
            /RTC1;
            /ZI
        >
        $<$<CONFIG:DebugSinglethreaded>:
            /RTC1;
            /ZI
        >
        $<$<CONFIG:Debug>:
            /RTC1;
            /ZI;
            /MP
        >
        $<$<CONFIG:ReleaseiPhoneEmulation>:
            /Zi
        >
        $<$<CONFIG:ReleaseMultithreaded>:
            /Zi
        >
        $<$<CONFIG:ReleaseSinglethreaded>:
            /Zi
        >
        $<$<CONFIG:Release>:
            /Zi
        >
        /Od;
        /W3;
        /MP;
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
endif()

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "openAL32"
)
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../LibOpenAL/libs/Win32"
)


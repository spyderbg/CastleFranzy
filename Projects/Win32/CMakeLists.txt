cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(FortressDefender C CXX)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

#if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32"))
#    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
#endif()

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "CommonDebug"
    "CommonLiteDebug"
    "CommonLiteRelease"
    "CommonRelease"
    "Debug"
    "DebugiPhoneEmulation"
    "DebugMultithreaded"
    "DebugMultithreadedIrrLichtEdition"
    "DebugSinglethreaded"
    "Release"
    "ReleaseiPhoneEmulation"
    "ReleaseMultithreaded"
    "ReleaseSinglethreaded"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_C_FLAGS "")
    set(CMAKE_C_FLAGS_COMMONDEBUG "")
    set(CMAKE_C_FLAGS_COMMONLITEDEBUG "")
    set(CMAKE_C_FLAGS_COMMONLITERELEASE "")
    set(CMAKE_C_FLAGS_COMMONRELEASE "")
    set(CMAKE_C_FLAGS_DEBUG "")
    set(CMAKE_C_FLAGS_DEBUGIPHONEEMULATION "")
    set(CMAKE_C_FLAGS_DEBUGMULTITHREADED "")
    set(CMAKE_C_FLAGS_DEBUGMULTITHREADEDIRRLICHTEDITION "")
    set(CMAKE_C_FLAGS_DEBUGSINGLETHREADED "")
    set(CMAKE_C_FLAGS_RELEASE "")
    set(CMAKE_C_FLAGS_RELEASEIPHONEEMULATION "")
    set(CMAKE_C_FLAGS_RELEASEMULTITHREADED "")
    set(CMAKE_C_FLAGS_RELEASESINGLETHREADED "")
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_COMMONDEBUG "")
    set(CMAKE_CXX_FLAGS_COMMONLITEDEBUG "")
    set(CMAKE_CXX_FLAGS_COMMONLITERELEASE "")
    set(CMAKE_CXX_FLAGS_COMMONRELEASE "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_DEBUGIPHONEEMULATION "")
    set(CMAKE_CXX_FLAGS_DEBUGMULTITHREADED "")
    set(CMAKE_CXX_FLAGS_DEBUGMULTITHREADEDIRRLICHTEDITION "")
    set(CMAKE_CXX_FLAGS_DEBUGSINGLETHREADED "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
    set(CMAKE_CXX_FLAGS_RELEASEIPHONEEMULATION "")
    set(CMAKE_CXX_FLAGS_RELEASEMULTITHREADED "")
    set(CMAKE_CXX_FLAGS_RELEASESINGLETHREADED "")
endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_COMMONDEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_COMMONDEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_COMMONDEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_COMMONDEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_COMMONLITEDEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_COMMONLITEDEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_COMMONLITEDEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_COMMONLITEDEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_COMMONLITERELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_COMMONLITERELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_COMMONLITERELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_COMMONLITERELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_COMMONRELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_COMMONRELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_COMMONRELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_COMMONRELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUGIPHONEEMULATION "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUGIPHONEEMULATION "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUGIPHONEEMULATION "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUGIPHONEEMULATION "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUGMULTITHREADED "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUGMULTITHREADED "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUGMULTITHREADED "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUGMULTITHREADED "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUGMULTITHREADEDIRRLICHTEDITION "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUGMULTITHREADEDIRRLICHTEDITION "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUGMULTITHREADEDIRRLICHTEDITION "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUGMULTITHREADEDIRRLICHTEDITION "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUGSINGLETHREADED "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUGSINGLETHREADED "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUGSINGLETHREADED "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUGSINGLETHREADED "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASEIPHONEEMULATION "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASEIPHONEEMULATION "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASEIPHONEEMULATION "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASEIPHONEEMULATION "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASEMULTITHREADED "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASEMULTITHREADED "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASEMULTITHREADED "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASEMULTITHREADED "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASESINGLETHREADED "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASESINGLETHREADED "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASESINGLETHREADED "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASESINGLETHREADED "${CMAKE_STATIC_LINKER_FLAGS}")
endif()

################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################
add_subdirectory(../../Lib/Irrlicht/source/Irrlicht ${CMAKE_BINARY_DIR}/Irrlicht)
add_subdirectory(../../Lib/SoundLib/prj ${CMAKE_BINARY_DIR}/SoundLib)



























set(PROJECT_NAME FortressDefender)

################################################################################
# Conon build info
################################################################################
#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup()


################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "../../Src/config_version.h"
    "../../Src/GX_Device.h"
    "../../Src/stdafx.cpp"
)
source_group("" FILES ${no_group_source_files})

set(Autogenerated
    "../../Src/Autogenerated/Fonts.h"
    "../../Src/Autogenerated/FortressDefender_ID.hpp"
    "../../Src/Autogenerated/FortressDefender_ID_string.hpp"
    "../../Src/Autogenerated/Ingame.h"
    "../../Src/Autogenerated/Menu.h"
    "../../Src/Autogenerated/sounds.h"
    "../../Src/Autogenerated/version.h"
)
source_group("Autogenerated" FILES ${Autogenerated})

set(Common
    "../../Src/Common/EventManager.cpp"
    "../../Src/Common/EventManager.h"
    "../../Src/Common/Gestures.cpp"
    "../../Src/Common/Gestures.h"
    "../../Src/Common/GX_Math.cpp"
    "../../Src/Common/GX_Math.h"
    "../../Src/Common/Singleton.h"
    "../../Src/Common/StateMachine.cpp"
    "../../Src/Common/StateMachine.h"
    "../../Src/Common/StringTable.cpp"
    "../../Src/Common/StringTable.h"
)
source_group("Common" FILES ${Common})

set(Core
    "../../Src/Core/Application.cpp"
    "../../Src/Core/Application.h"
    "../../Src/Core/irrlicht.h"
    "../../Src/Core/Logs.cpp"
    "../../Src/Core/Logs.h"
)
source_group("Core" FILES ${Core})

set(Editor
    "../../Src/Editor/EditorUI.cpp"
    "../../Src/Editor/EditorUI.h"
    "../../Src/Editor/GS_Editor.cpp"
    "../../Src/Editor/GS_Editor.h"
)
source_group("Editor" FILES ${Editor})

set(Entities
    "../../Src/Entities/IDrawable.h"
    "../../Src/Entities/IEvent.h"
    "../../Src/Entities/IEventReceiver.h"
    "../../Src/Entities/IFileStream.h"
    "../../Src/Entities/IMenu.cpp"
    "../../Src/Entities/IMenu.h"
    "../../Src/Entities/ISleepable.h"
    "../../Src/Entities/IStateBase.h"
    "../../Src/Entities/IStream.h"
    "../../Src/Entities/IUpdatable.h"
)
source_group("Entities" FILES ${Entities})

set(Events
    "../../Src/Events/EvAcceleration.h"
    "../../Src/Events/Events.h"
    "../../Src/Events/EvKeyboard.h"
    "../../Src/Events/EvMouse.h"
    "../../Src/Events/EvVirtualButton.h"
)
source_group("Events" FILES ${Events})

set(Fonts
    "../../Src/Fonts/Font.cpp"
    "../../Src/Fonts/Font.h"
    "../../Src/Fonts/FontManager.cpp"
    "../../Src/Fonts/FontManager.h"
)
source_group("Fonts" FILES ${Fonts})

set(Game
    "../../Src/Game/Achievements.cpp"
    "../../Src/Game/Achievements.h"
    "../../Src/Game/Cheats.cpp"
    "../../Src/Game/Cheats.h"
    "../../Src/Game/ColladaControl.cpp"
    "../../Src/Game/ColladaControl.h"
    "../../Src/Game/LevelInfo.cpp"
    "../../Src/Game/LevelInfo.h"
    "../../Src/Game/Misc.cpp"
    "../../Src/Game/Misc.h"
    "../../Src/Game/Playfield.cpp"
    "../../Src/Game/Playfield.h"
    "../../Src/Game/Value.cpp"
    "../../Src/Game/Value.h"
)
source_group("Game" FILES ${Game})

set(GameStates
    "../../Src/GameStates/GS_Cheat.cpp"
    "../../Src/GameStates/GS_Cheat.h"
    "../../Src/GameStates/GS_Menu.cpp"
    "../../Src/GameStates/GS_Menu.h"
    "../../Src/GameStates/GS_Shop.cpp"
    "../../Src/GameStates/GS_Shop.h"
    "../../Src/GameStates/GS_Splash.cpp"
    "../../Src/GameStates/GS_Splash.h"
    "../../Src/GameStates/GS_SwitchToExternal.cpp"
    "../../Src/GameStates/GS_SwitchToExternal.h"
    "../../Src/GameStates/GSLevel.cpp"
    "../../Src/GameStates/GSLevel.h"
)
source_group("GameStates" FILES ${GameStates})

set(Game__Config
    "../../Src/Game/Config/GameConfig.cpp"
    "../../Src/Game/Config/GameConfig.h"
    "../../Src/Game/Config/LevelStats.cpp"
    "../../Src/Game/Config/LevelStats.h"
    "../../Src/Game/Config/Scores.cpp"
    "../../Src/Game/Config/Scores.h"
    "../../Src/Game/Config/Upgrades.cpp"
    "../../Src/Game/Config/Upgrades.h"
    "../../Src/Game/Config/UserProfile.cpp"
    "../../Src/Game/Config/UserProfile.h"
)
source_group("Game\\Config" FILES ${Game__Config})

set(Game__HUD
    "../../Src/Game/HUD/HUD.cpp"
    "../../Src/Game/HUD/HUD.h"
)
source_group("Game\\HUD" FILES ${Game__HUD})

set(Game__Objects
    "../../Src/Game/Objects/GameCamera.cpp"
    "../../Src/Game/Objects/GameCamera.h"
    "../../Src/Game/Objects/GameObject.cpp"
    "../../Src/Game/Objects/GameObject.h"
    "../../Src/Game/Objects/Projectile.cpp"
    "../../Src/Game/Objects/Projectile.h"
    "../../Src/Game/Objects/Unit.cpp"
    "../../Src/Game/Objects/Unit.h"
)
source_group("Game\\Objects" FILES ${Game__Objects})

set(Game__Objects__UnitParts
    "../../Src/Game/Objects/UnitParts/Bomb.cpp"
    "../../Src/Game/Objects/UnitParts/Bomb.h"
    "../../Src/Game/Objects/UnitParts/DamageSource.h"
    "../../Src/Game/Objects/UnitParts/ShamanAura.cpp"
    "../../Src/Game/Objects/UnitParts/ShamanAura.h"
    "../../Src/Game/Objects/UnitParts/UnitKind.cpp"
    "../../Src/Game/Objects/UnitParts/UnitKind.h"
)
source_group("Game\\Objects\\UnitParts" FILES ${Game__Objects__UnitParts})

set(Game__Objects__Weapons
    "../../Src/Game/Objects/Weapons/CastleGate.cpp"
    "../../Src/Game/Objects/Weapons/CastleGate.h"
    "../../Src/Game/Objects/Weapons/Fireball.cpp"
    "../../Src/Game/Objects/Weapons/Fireball.h"
    "../../Src/Game/Objects/Weapons/Griffin.cpp"
    "../../Src/Game/Objects/Weapons/Griffin.h"
    "../../Src/Game/Objects/Weapons/Hydra.cpp"
    "../../Src/Game/Objects/Weapons/Hydra.h"
    "../../Src/Game/Objects/Weapons/Iceball.cpp"
    "../../Src/Game/Objects/Weapons/Iceball.h"
    "../../Src/Game/Objects/Weapons/Spells.cpp"
    "../../Src/Game/Objects/Weapons/Spells.h"
    "../../Src/Game/Objects/Weapons/WeaponObject.cpp"
    "../../Src/Game/Objects/Weapons/WeaponObject.h"
)
source_group("Game\\Objects\\Weapons" FILES ${Game__Objects__Weapons})

set(Game__PlayfieldParts
    "../../Src/Game/PlayfieldParts/Challenge.cpp"
    "../../Src/Game/PlayfieldParts/Challenge.h"
)
source_group("Game\\PlayfieldParts" FILES ${Game__PlayfieldParts})

set(IO__Accelerometer
    "../../Src/IO/Accelerometer/AccelerometerBase.cpp"
    "../../Src/IO/Accelerometer/AccelerometerBase.h"
    "../../Src/IO/Accelerometer/AccelerometerWin32.cpp"
    "../../Src/IO/Accelerometer/AccelerometerWin32.h"
)
source_group("IO\\Accelerometer" FILES ${IO__Accelerometer})

set(IO__Audio
    "../../Src/IO/Audio/SoundFileManager.cpp"
    "../../Src/IO/Audio/SoundFileManager.h"
    "../../Src/IO/Audio/SoundManager.cpp"
    "../../Src/IO/Audio/SoundManager.h"
)
source_group("IO\\Audio" FILES ${IO__Audio})

set(IO__TouchScreen
    "../../Src/IO/TouchScreen/TouchPad.cpp"
    "../../Src/IO/TouchScreen/TouchPad.h"
)
source_group("IO\\TouchScreen" FILES ${IO__TouchScreen})

set(Menus
    "../../Src/Menus/Menu_About.cpp"
    "../../Src/Menus/Menu_About.h"
    "../../Src/Menus/Menu_Achievements.cpp"
    "../../Src/Menus/Menu_Achievements.h"
    "../../Src/Menus/Menu_Campaign.cpp"
    "../../Src/Menus/Menu_Campaign.h"
    "../../Src/Menus/Menu_Confirmation.cpp"
    "../../Src/Menus/Menu_Confirmation.h"
    "../../Src/Menus/Menu_Difficulty.cpp"
    "../../Src/Menus/Menu_Difficulty.h"
    "../../Src/Menus/Menu_Engage.cpp"
    "../../Src/Menus/Menu_Engage.h"
    "../../Src/Menus/Menu_GameSettings.cpp"
    "../../Src/Menus/Menu_GameSettings.h"
    "../../Src/Menus/Menu_Help.cpp"
    "../../Src/Menus/Menu_Help.h"
    "../../Src/Menus/Menu_HighScore.cpp"
    "../../Src/Menus/Menu_HighScore.h"
    "../../Src/Menus/Menu_HighScoresLevelList.cpp"
    "../../Src/Menus/Menu_HighScoresLevelList.h"
    "../../Src/Menus/Menu_Info.cpp"
    "../../Src/Menus/Menu_Info.h"
    "../../Src/Menus/Menu_Ingame.cpp"
    "../../Src/Menus/Menu_Ingame.h"
    "../../Src/Menus/Menu_Ingame_Help.cpp"
    "../../Src/Menus/Menu_Ingame_Help.h"
    "../../Src/Menus/Menu_LanguageSelection.cpp"
    "../../Src/Menus/Menu_LanguageSelection.h"
    "../../Src/Menus/Menu_Loading.cpp"
    "../../Src/Menus/Menu_Loading.h"
    "../../Src/Menus/Menu_Login.cpp"
    "../../Src/Menus/Menu_Login.h"
    "../../Src/Menus/Menu_NewGame.cpp"
    "../../Src/Menus/Menu_NewGame.h"
    "../../Src/Menus/Menu_Options.cpp"
    "../../Src/Menus/Menu_Options.h"
    "../../Src/Menus/Menu_Score.cpp"
    "../../Src/Menus/Menu_Score.h"
    "../../Src/Menus/Menu_ScoreInputType.h"
    "../../Src/Menus/Menu_Shop.cpp"
    "../../Src/Menus/Menu_Shop.h"
    "../../Src/Menus/Menu_SoundOptions.cpp"
    "../../Src/Menus/Menu_SoundOptions.h"
    "../../Src/Menus/MenuMain.cpp"
    "../../Src/Menus/MenuMain.h"
)
source_group("Menus" FILES ${Menus})

set(Network
    "../../Src/Network/XPlayerManager.cpp"
    "../../Src/Network/xplayermanager.h"
)
source_group("Network" FILES ${Network})

set(PlatformSpecifics__IPhone
    "../../Src/PlatformSpecifics/IPhone/Device.h"
    "../IPhone/main.mm"
)
source_group("PlatformSpecifics\\IPhone" FILES ${PlatformSpecifics__IPhone})

set(PlatformSpecifics__IPhone__Classes
    "../IPhone/Classes/EAGLView.h"
    "../IPhone/Classes/EAGLView.mm"
    "../IPhone/Classes/iCDAppDelegate.h"
    "../IPhone/Classes/iCDAppDelegate.mm"
    "../IPhone/Classes/TextInput.h"
    "../IPhone/Classes/TextInput.mm"
    "../IPhone/Classes/WTextInput.h"
)
source_group("PlatformSpecifics\\IPhone\\Classes" FILES ${PlatformSpecifics__IPhone__Classes})

set(PlatformSpecifics__Win32
    "../../Src/PlatformSpecifics/Win32/CImageLoaderPVR.cpp"
    "../../Src/PlatformSpecifics/Win32/CImageLoaderPVR.h"
    "../../Src/PlatformSpecifics/Win32/Device.h"
    "../../Src/PlatformSpecifics/Win32/main.cpp"
    "../../Src/PlatformSpecifics/Win32/PVRTDecompress.cpp"
    "../../Src/PlatformSpecifics/Win32/PVRTDecompress.h"
    "../../Src/PlatformSpecifics/Win32/PVRTGlobal.h"
    "../../Src/PlatformSpecifics/Win32/PVRTTexture.h"
#    "../../Src/PlatformSpecifics/Win32/PVRTTextureAPI.h"
)
source_group("PlatformSpecifics\\Win32" FILES ${PlatformSpecifics__Win32})

set(Sprites
    "../../Src/Sprites/AnimationInstance.cpp"
    "../../Src/Sprites/AnimationInstance.h"
    "../../Src/Sprites/ASprite.cpp"
    "../../Src/Sprites/ASprite.h"
    "../../Src/Sprites/ASpriteInstance.cpp"
    "../../Src/Sprites/ASpriteInstance.h"
)
source_group("Sprites" FILES ${Sprites})

set(Sprites__Particle
    "../../Src/Particle/ParticlePreset.cpp"
    "../../Src/Particle/ParticlePreset.h"
    "../../Src/Particle/ParticleSystem.cpp"
    "../../Src/Particle/ParticleSystem.h"
    "../../Src/Particle/ParticleSystemManager.cpp"
    "../../Src/Particle/ParticleSystemManager.h"
)
source_group("Sprites\\Particle" FILES ${Sprites__Particle})

set(Sprites__Particle__ParticleSystem
    "../../Src/Particle/ParticleSystem/CFpsParticleAttractionAffector.cpp"
    "../../Src/Particle/ParticleSystem/CFpsParticleAttractionAffector.h"
    "../../Src/Particle/ParticleSystem/CFpsParticleBoxEmitter.cpp"
    "../../Src/Particle/ParticleSystem/CFpsParticleBoxEmitter.h"
    "../../Src/Particle/ParticleSystem/CFpsParticleFadeOutAffector.cpp"
    "../../Src/Particle/ParticleSystem/CFpsParticleFadeOutAffector.h"
    "../../Src/Particle/ParticleSystem/CFpsParticleGravityAffector.cpp"
    "../../Src/Particle/ParticleSystem/CFpsParticleGravityAffector.h"
    "../../Src/Particle/ParticleSystem/CFpsParticleRotationAffector.cpp"
    "../../Src/Particle/ParticleSystem/CFpsParticleRotationAffector.h"
    "../../Src/Particle/ParticleSystem/CFpsParticleSizeAffector.cpp"
    "../../Src/Particle/ParticleSystem/CFpsParticleSizeAffector.h"
    "../../Src/Particle/ParticleSystem/CFpsParticleSpinAffector.cpp"
    "../../Src/Particle/ParticleSystem/CFpsParticleSpinAffector.h"
    "../../Src/Particle/ParticleSystem/CFpsParticleSystemSceneNode.cpp"
    "../../Src/Particle/ParticleSystem/CFpsParticleSystemSceneNode.h"
    "../../Src/Particle/ParticleSystem/IFpsParticleAffector.h"
    "../../Src/Particle/ParticleSystem/IFpsParticleAttractionAffector.h"
    "../../Src/Particle/ParticleSystem/IFpsParticleBoxEmitter.h"
    "../../Src/Particle/ParticleSystem/IFpsParticleEmitter.h"
    "../../Src/Particle/ParticleSystem/IFpsParticleFadeOutAffector.h"
    "../../Src/Particle/ParticleSystem/IFpsParticleGravityAffector.h"
    "../../Src/Particle/ParticleSystem/IFpsParticleRotationAffector.h"
    "../../Src/Particle/ParticleSystem/IFpsParticleSizeAffector.h"
    "../../Src/Particle/ParticleSystem/IFpsParticleSpinAffector.h"
    "../../Src/Particle/ParticleSystem/IFpsParticleSystemSceneNode.h"
    "../../Src/Particle/ParticleSystem/SFpsParticle.h"
)
source_group("Sprites\\Particle\\ParticleSystem" FILES ${Sprites__Particle__ParticleSystem})

set(Utils
    "../../Src/Utils/Float.cpp"
    "../../Src/Utils/Float.h"
    "../../Src/Utils/Point2D.h"
    "../../Src/Utils/profiler.cpp"
    "../../Src/Utils/profiler.h"
    "../../Src/Utils/StringUtils.cpp"
    "../../Src/Utils/StringUtils.h"
)
source_group("Utils" FILES ${Utils})

set(VisualControls
    "../../Src/VisualControls/IVisualControl.cpp"
    "../../Src/VisualControls/IVisualControl.h"
    "../../Src/VisualControls/VisualControlButton.cpp"
    "../../Src/VisualControls/VisualControlButton.h"
    "../../Src/VisualControls/VisualControlCommon.h"
    "../../Src/VisualControls/VisualControlContainer.cpp"
    "../../Src/VisualControls/VisualControlContainer.h"
    "../../Src/VisualControls/VisualControlEditBox.cpp"
    "../../Src/VisualControls/VisualControlEditBox.h"
    "../../Src/VisualControls/VisualControlMesh.cpp"
    "../../Src/VisualControls/VisualControlMesh.h"
    "../../Src/VisualControls/VisualControlPane.cpp"
    "../../Src/VisualControls/VisualControlPane.h"
    "../../Src/VisualControls/VisualControlSlider.cpp"
    "../../Src/VisualControls/VisualControlSlider.h"
    "../../Src/VisualControls/VisualControlSprite.cpp"
    "../../Src/VisualControls/VisualControlSprite.h"
    "../../Src/VisualControls/VisualControlSwitch.cpp"
    "../../Src/VisualControls/VisualControlSwitch.h"
    "../../Src/VisualControls/VisualControlText.cpp"
    "../../Src/VisualControls/VisualControlText.h"
)
source_group("VisualControls" FILES ${VisualControls})

set(ALL_FILES
    ${no_group_source_files}
    ${Autogenerated}
    ${Common}
    ${Core}
    ${Editor}
    ${Entities}
    ${Events}
    ${Fonts}
    ${Game}
    ${GameStates}
    ${Game__Config}
    ${Game__HUD}
    ${Game__Objects}
    ${Game__Objects__UnitParts}
    ${Game__Objects__Weapons}
    ${Game__PlayfieldParts}
    ${IO__Accelerometer}
    ${IO__Audio}
    ${IO__TouchScreen}
    ${Menus}
    ${Network}
#    ${PlatformSpecifics__IPhone}
#    ${PlatformSpecifics__IPhone__Classes}
    ${PlatformSpecifics__Win32}
    ${Sprites}
    ${Sprites__Particle}
    ${Sprites__Particle__ParticleSystem}
    ${Utils}
    ${VisualControls}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

add_precompiled_header(${PROJECT_NAME} "stdafx.h" "../../Src/stdafx.cpp")

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE FortressDefender)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_COMMONDEBUG                       "${CMAKE_CURRENT_SOURCE_DIR}/../../Build/"
    OUTPUT_DIRECTORY_COMMONRELEASE                     "${CMAKE_CURRENT_SOURCE_DIR}/../../Build/"
    OUTPUT_DIRECTORY_COMMONLITEDEBUG                   "${CMAKE_CURRENT_SOURCE_DIR}/../../Build/"
    OUTPUT_DIRECTORY_COMMONLITERELEASE                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Build/"
    OUTPUT_DIRECTORY_DEBUGIPHONEEMULATION              "${CMAKE_CURRENT_SOURCE_DIR}/../../Build/"
    OUTPUT_DIRECTORY_DEBUGMULTITHREADEDIRRLICHTEDITION "${CMAKE_CURRENT_SOURCE_DIR}/../../Build/"
    OUTPUT_DIRECTORY_DEBUGMULTITHREADED                "${CMAKE_CURRENT_SOURCE_DIR}/../../Build/"
    OUTPUT_DIRECTORY_DEBUGSINGLETHREADED               "${CMAKE_CURRENT_SOURCE_DIR}/../../Build/"
    OUTPUT_DIRECTORY_DEBUG                             "${CMAKE_CURRENT_SOURCE_DIR}/../../Build/"
    OUTPUT_DIRECTORY_RELEASEIPHONEEMULATION            "${CMAKE_CURRENT_SOURCE_DIR}/../../Build/"
    OUTPUT_DIRECTORY_RELEASEMULTITHREADED              "${CMAKE_CURRENT_SOURCE_DIR}/../../Build/"
    OUTPUT_DIRECTORY_RELEASESINGLETHREADED             "${CMAKE_CURRENT_SOURCE_DIR}/../../Build/"
    OUTPUT_DIRECTORY_RELEASE                           "${CMAKE_CURRENT_SOURCE_DIR}/../../Build/"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_COMMONDEBUG                       "TRUE"
    INTERPROCEDURAL_OPTIMIZATION_COMMONRELEASE                     "TRUE"
    INTERPROCEDURAL_OPTIMIZATION_COMMONLITERELEASE                 "TRUE"
    INTERPROCEDURAL_OPTIMIZATION_RELEASEIPHONEEMULATION            "TRUE"
    INTERPROCEDURAL_OPTIMIZATION_RELEASEMULTITHREADED              "TRUE"
    INTERPROCEDURAL_OPTIMIZATION_RELEASESINGLETHREADED             "TRUE"
    INTERPROCEDURAL_OPTIMIZATION_RELEASE                           "TRUE"
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
    "$<$<CONFIG:CommonLiteDebug>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/LibOpenAL/include"
    ">"
    "$<$<CONFIG:DebugiPhoneEmulation>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/LibOpenAL/include"
    ">"
    "$<$<CONFIG:DebugMultithreadedIrrLichtEdition>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/LibOpenAL/include"
    ">"
    "$<$<CONFIG:DebugMultithreaded>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/LibOpenAL/include"
    ">"
    "$<$<CONFIG:DebugSinglethreaded>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/LibOpenAL/include"
    ">"
    "$<$<CONFIG:Debug>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/LibOpenAL/include"
    ">"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Src;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/tools/ColladaConverter/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/tools/ResFile/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/source/Irrlicht/PowerVR/OGLES/Include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/source/Irrlicht/PowerVR/OGLES/Windows/Include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/SoundLib/include"
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
    "_WINDOWS;"
    "_IRR_STATIC_LIB_;"
    "_IRR_IPHONE_EMULATION_;"
    "OS_WIN32;"
    "BUILD_OGLES;"
    "_WIN32;"
    "UNICODE;"
    "_UNICODE"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:CommonDebug>:
            /O2;
            /Oi;
            /Zi
        >
        $<$<CONFIG:CommonRelease>:
            /O2;
            /Oi;
            /Zi
        >
        $<$<CONFIG:CommonLiteDebug>:
            /Od;
            /RTC1;
            /ZI
        >
        $<$<CONFIG:CommonLiteRelease>:
            /O2;
            /Oi;
            /Zi
        >
        $<$<CONFIG:DebugiPhoneEmulation>:
            /Od;
            /RTC1;
            /ZI
        >
        $<$<CONFIG:DebugMultithreadedIrrLichtEdition>:
            /Od;
            /RTC1;
            /ZI
        >
        $<$<CONFIG:DebugMultithreaded>:
            /Od;
            /RTC1;
            /ZI
        >
        $<$<CONFIG:DebugSinglethreaded>:
            /Od;
            /RTC1;
            /ZI
        >
        $<$<CONFIG:Debug>:
            /Od;
            /RTC1;
            /ZI;
            /MP
        >
        $<$<CONFIG:ReleaseiPhoneEmulation>:
            /O2;
            /Oi;
            /Zi
        >
        $<$<CONFIG:ReleaseMultithreaded>:
            /O2;
            /Oi;
            /Zi
        >
        $<$<CONFIG:ReleaseSinglethreaded>:
            /O2;
            /Oi;
            /Zi
        >
        $<$<CONFIG:Release>:
            /O2;
            /Oi;
            /Zi
        >
        /W1;
        /MP;
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:CommonDebug>:
            /OPT:REF;
            /OPT:ICF
        >
        $<$<CONFIG:CommonRelease>:
            /OPT:REF;
            /OPT:ICF
        >
        $<$<CONFIG:CommonLiteDebug>:
            /INCREMENTAL
        >
        $<$<CONFIG:CommonLiteRelease>:
            /OPT:REF;
            /OPT:ICF
        >
        $<$<CONFIG:DebugiPhoneEmulation>:
            /INCREMENTAL
        >
        $<$<CONFIG:DebugMultithreadedIrrLichtEdition>:
            /INCREMENTAL
        >
        $<$<CONFIG:DebugMultithreaded>:
            /INCREMENTAL
        >
        $<$<CONFIG:DebugSinglethreaded>:
            /INCREMENTAL
        >
        $<$<CONFIG:Debug>:
            /INCREMENTAL
        >
        $<$<CONFIG:ReleaseiPhoneEmulation>:
            /OPT:REF;
            /OPT:ICF
        >
        $<$<CONFIG:ReleaseMultithreaded>:
            /OPT:REF;
            /OPT:ICF
        >
        $<$<CONFIG:ReleaseSinglethreaded>:
            /OPT:REF;
            /OPT:ICF
        >
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF
        >
        /DEBUG;
        /MACHINE:X86;
        /SUBSYSTEM:CONSOLE
    )
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    Irrlicht
    SoundLib
)

# Link with other targets.
target_link_libraries(${PROJECT_NAME} PRIVATE
    Irrlicht
    SoundLib
)

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "$<$<CONFIG:CommonDebug>:"
        "Irrlicht"
    ">"
    "$<$<CONFIG:CommonRelease>:"
        "Irrlicht"
    ">"
    "$<$<CONFIG:CommonLiteRelease>:"
        "Irrlicht"
    ">"
    "$<$<CONFIG:ReleaseiPhoneEmulation>:"
        "Irrlicht"
    ">"
    "$<$<CONFIG:ReleaseMultithreaded>:"
        "Irrlicht"
    ">"
    "$<$<CONFIG:ReleaseSinglethreaded>:"
        "Irrlicht"
    ">"
    "$<$<CONFIG:Release>:"
        "Irrlicht"
    ">"
    "SoundLib"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:CommonDebug>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/source/Irrlicht/Release Multithreaded/;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/SoundLib/build/Win32/Release/"
    ">"
    "$<$<CONFIG:CommonRelease>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/source/Irrlicht/Release Multithreaded/;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/SoundLib/build/Win32/Release/"
    ">"
    "$<$<CONFIG:CommonLiteDebug>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/source/Irrlicht/Debug Multithreaded/;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/SoundLib/build/Win32/Debug/"
    ">"
    "$<$<CONFIG:CommonLiteRelease>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/source/Irrlicht/Release Multithreaded/;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/SoundLib/build/Win32/Release/"
    ">"
    "$<$<CONFIG:DebugiPhoneEmulation>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/source/Irrlicht/Debug Multithreaded/;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/SoundLib/build/Win32/Debug/"
    ">"
    "$<$<CONFIG:DebugMultithreadedIrrLichtEdition>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/source/Irrlicht/Debug Multithreaded/;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/SoundLib/build/Win32/Debug/"
    ">"
    "$<$<CONFIG:DebugMultithreaded>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/source/Irrlicht/Debug Multithreaded/;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/SoundLib/build/Win32/Debug/"
    ">"
    "$<$<CONFIG:DebugSinglethreaded>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/source/Irrlicht/Debug Multithreaded/;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/SoundLib/build/Win32/Debug/"
    ">"
    "$<$<CONFIG:Debug>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/source/Irrlicht/Debug Multithreaded/;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/SoundLib/build/Win32/Debug/"
    ">"
    "$<$<CONFIG:ReleaseiPhoneEmulation>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/source/Irrlicht/Release Multithreaded/;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/SoundLib/build/Win32/Release/"
    ">"
    "$<$<CONFIG:ReleaseMultithreaded>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/source/Irrlicht/Release Multithreaded/;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/SoundLib/build/Win32/Release/"
    ">"
    "$<$<CONFIG:ReleaseSinglethreaded>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/source/Irrlicht/Release Multithreaded/;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/SoundLib/build/Win32/Release/"
    ">"
    "$<$<CONFIG:Release>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/Irrlicht/source/Irrlicht/Release Multithreaded/;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/SoundLib/build/Win32/Release/"
    ">"
)


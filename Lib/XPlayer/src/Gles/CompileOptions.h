#ifndef _COMPILEOPTIONS_H_
#define _COMPILEOPTIONS_H_


#if _DEBUG || DEBUG
	#define USE_ASSERT	1
#else
	#define USE_ASSERT	0
#endif


#if _DEBUG || DEBUG
	#define USE_ASSERT2	1
#else
	#define USE_ASSERT2	0
#endif


#define LIMIT_FPS_VALUE			10
#define LIMIT_FPS_FRAME_MS		100
#define DISABLE_VIBRATE			1


#if defined(_WINDOWS) && !defined( WIPI )
// {
    #ifdef WIN_DEBUG
    	#define USE_PACK_FILE_SYSTEM	0
    	#define MEMORY_MONITOR_CHANGES  0
    #else
    	#define USE_PACK_FILE_SYSTEM	1	//always 1 in release version
    	#define MEMORY_MONITOR_CHANGES  0
    #endif

	#define CHECK_MEMORY_LEAKS		1
	#define USE_FILENAME_AS_RESID	1 //for now, only valid in windows
	#define USE_DEBUG_FIXED			1
	#define LOG_LOADING_STATUS		0
	#define LOG_PAINT_STATUS		0
	#define LOG_PERFORMANCE_STATUS	0
	#define LIMIT_FPS				1
	#define USE_NEW_DOG_AI			1
	#define USE_BSP_TREE			0
// }
#elif defined( WIPI )
// {
    #ifdef _DEBUG
    	#define USE_PACK_FILE_SYSTEM	1
    #else
    	#define USE_PACK_FILE_SYSTEM	1	//always 1 in release version
    #endif

	#define CHECK_MEMORY_LEAKS		0
	#define MEMORY_MONITOR_CHANGES  0
	#define USE_FILENAME_AS_RESID	1 //for now, only valid in windows
	#define USE_DEBUG_FIXED			0
	#define LOG_LOADING_STATUS		0
	#define LOG_PAINT_STATUS		0
	#define LOG_PERFORMANCE_STATUS	0
	#define LIMIT_FPS				1
	#define USE_NEW_DOG_AI			1
	#define USE_BSP_TREE			0
// }
#else
// (
	#define USE_PACK_FILE_SYSTEM	1
	#define CHECK_MEMORY_LEAKS		0
	#define MEMORY_MONITOR_CHANGES  0
	#define USE_FILENAME_AS_RESID	1 //tempory changed by ZhangBaiBo
	#define USE_DEBUG_FIXED			0
	#define LOG_LOADING_STATUS		1
	#define LOG_PAINT_STATUS		0
	#define LOG_PERFORMANCE_STATUS	0
	#define LIMIT_FPS				1
	#define USE_NEW_DOG_AI			1
	#define USE_BSP_TREE			0
// }
#endif


#define DEBUG_CHARACTER  0
#define LOG_MSG          0

//#define NGAGE_ACHIEVEMENT_ENABLE    1
#define WCHAR_T_STRINGS      		1
#define SEMITRANSPARENCY_HUD_ENABLE 1
//#define BACKLIGHT_CONTROL_ENABLE    1
#define BATTERY_STATE_ENABLE        1
#define ACTION_MAPPING_ENABLED      0
//#define GAME_ORIENTATION_ENABLED    0

//#ifdef __WINSCW__
//    #define NGAGE_ARENA_ENABLE		0
//#else
//    #define NGAGE_ARENA_ENABLE		1
//#endif

#define DEBUG_MESSAGE_ENABLE 		0
//#define KEY_CONFIG_ENABLE			1

//**************************************
//	This file must be created manually. 
//	An empty file is a valid one. 
//	Use it to override the define 
//	values for your personal version
//#include "gll_override.h"
//**************************************


#endif // _COMPILEOPTIONS_H_

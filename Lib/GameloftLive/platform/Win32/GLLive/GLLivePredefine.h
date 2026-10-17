#ifndef _PREDEFINE_H_
#define _PREDEFINE_H_

#include "System/Device.h"

#define PLATFORM				PLATFORM_WIN32
#define SCREEN_SIZE				SCREEN_320_480
#define TOUCH_ENABLE			1
#define MENU_ENABLE				1
#define ORIENTATION_ENABLE		0
#define RGB_SPLASH				1
#define TOUCH_MODE				TOUCH_MULTIPLY
#define GRAPHIC_OPENGL			1
#define IPHONE					1
#define PLAYGROUND_CLIP_ENABLE	0
#define SOUND_ENABLE			0
#define NEW_SOUND				1
#define MATRIX_USE_16_ELEMENTS	1
#define USE_NORMAL				0
#define USE_LINEAR_TEXTURE		1	
#define HAS_CLICK_EFFECT		1
#define SAVE_LOAD_ENABLE        1
#define CHEAT_ENABLE			1
#define DISPLAY_JP_PLAYER_NAME  1
#define USE_THROW_IN_CAMERA		1
#define WIFI_ENABLE
#define XPLAYER					1

//#define SCREEN_SIZE		SCREEN_176_208
//#define SCREEN_SIZE		SCREEN_320_320

//#define MATRIX_USE_16_ELEMENTS		(1)

#define USE_VERY_EASY 1
#define SOUND_MODE SOUND_WAVE_ONLY
//#define MENU_ENABLE
#define PATH_SEP	"\\"

#define KEYPAD_MODE KEYPAD_NORMAL_MODE

#define USE_HANDSET_SHOOT_WAY

#define FPS					20

#if FPS == 15
#define FPS_RATIO				2
#else
#define FPS_RATIO				1
#endif

#define ANIMATION_TIME_PER_FRAME (1024*FPS_RATIO)
#define ANIMATION_PLAYEDTIMES	 (1*FPS_RATIO)

#include "System/GLLive_Memory.h"

#ifdef ASSERT
#undef ASSERT
#endif

#ifdef _ASSERT
#undef _ASSERT
#endif

#ifndef ASSERT

extern int GLLiveShowMessageBox(const char* message, const char* code, const char* file, const long line);

#define _ASSERT(c, f, l)	{											\
									if(!(c))								\
									{										\
									if(GLLiveShowMessageBox("assert failed!", #c, f, l))\
										{									\
											__asm int 3\
										}									\
									}										\
								}											\

#define ASSERT(x) _ASSERT(x, __FILE__, __LINE__)
#endif


#define DYNAMIC_CAST(t, v) ((t)(v))
#define STATIC_CAST(t, v) ((t)(v))
#define REINTERPRET_CAST(t, v) ((t)(v))
#define CONST_CAST(t, v) ((t)(v))

typedef long long	s64;
typedef int			s32;
typedef short		s16;
//typedef char		s8;

typedef unsigned long long	u64;
typedef unsigned int		u32;
typedef unsigned short		u16;
typedef unsigned char		u8;
typedef u16 			AECHAR;
typedef u32			RGBVAL;
typedef u32			uint32;
typedef u16 			uint16;
typedef uint16			AEEFont;
typedef int			bool32;

#if(PLATFORM==PLATFORM_MAC_OS_X)&&(!defined(WIN32))
typedef signed char		BOOL;
#else
typedef int BOOL;
#endif


typedef signed short		fx16;
typedef signed int		fx32;


typedef unsigned long		DWORD;
typedef unsigned char		byte;

#pragma warning(disable:4311)
#pragma warning(disable:4291)
#pragma warning(disable:4312)
#pragma warning(disable:4996)

extern void Trace(char* format, ...);

#define TRACE Trace
#define debug_out OutputDebugStringA


#define MEMCPY memcpy
#define MEMSET memset
#define STRLEN strlen
#define WSTRLEN(x)		wcslen((wchar_t*)x)

#define  STREXPAND(a, b, c, d) 


#include "Gles/gll.h"
#include "System/FMath.h"

#endif

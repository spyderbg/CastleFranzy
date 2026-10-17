/*
 *  Predefine.h
 *  iRF09
 *
 *  Created by Meng Yu on 08-5-20.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __IRF09_PREDEFINE_H__
#define __IRF09_PREDEFINE_H__

#include "Device.h"

#define MATRIX_USE_16_ELEMENTS		(1)

#define PLATFORM			PLATFORM_MAC_OS_X
#define TOUCH_ENABLE		1
#define RGB_SPLASH			1
#define SCREEN_SIZE			SCREEN_320_480
#define SOUND_MODE			0
#define KEYPAD_MODE			KEYPAD_NORMAL_MODE
#define FPS					25
#define FPS_RATIO			1
#define ANIMATION_TIME_PER_FRAME (1024*FPS_RATIO)
#define ANIMATION_PLAYEDTIMES	 (1*FPS_RATIO)
#define TOUCH_MODE			TOUCH_MULTIPLY
#define PATH_SEP			"/"
#define GRAPHIC_OPENGL		1
#define MENU_ENABLE			1
#define IPHONE				1
#define USE_SOUNDS			1
#define SOUND_ENABLE		1
#define NEW_SOUND			1
#define USE_NORMAL			0
#define USE_LINEAR_TEXTURE	1
#define HAS_CLICK_EFFECT	1
#define SAVE_LOAD_ENABLE        2
#define CHEAT_ENABLE			1
#define DISPLAY_JP_PLAYER_NAME  1

#define USE_HANDSET_SHOOT_WAY
#define USE_VERY_EASY 		1
#define USE_THROW_IN_CAMERA		1

#define WIFI_ENABLE

#include "Memory.h"

#ifdef ASSERT
#undef ASSERT
#endif

#ifdef _ASSERT
#undef _ASSERT
#endif

#ifndef ASSERT

extern int ShowMessageBox(const char* message, const char* code, const char* file, const long line);

#define _ASSERT(c, f, l)
//#define _ASSERT(c, f, l)	{					\
//	if(!(c))							\
//	{								\
//		if(ShowMessageBox("assert failed!", #c, f, l))		\
//		{							\
//			__asm int 3					\
//		}							\
//	}								\
//}									\

#define ASSERT(x) _ASSERT(x, __FILE__, __LINE__)
#endif

#define DYNAMIC_CAST(t, v) ((t)(v))
#define STATIC_CAST(t, v) ((t)(v))
#define REINTERPRET_CAST(t, v) ((t)(v))
#define CONST_CAST(t, v) ((t)(v))

#if(PLATFORM==PLATFORM_MAC_OS_X)&&(!defined(_WINDOWS))
typedef signed char		BOOL;
#else
typedef int BOOL;
#endif
typedef unsigned char		u8;
typedef unsigned short		u16;
typedef short			s16;
typedef signed short		fx16;
typedef signed int		fx32;
typedef unsigned int		u32;
typedef int			s32;
typedef unsigned long		DWORD;
typedef unsigned char		byte;
typedef unsigned long long	u64;
typedef unsigned int		uint32;

typedef u16					AECHAR;
typedef u32					RGBVAL;
typedef u16					uint16;
typedef uint16				AEEFont;

typedef unsigned int		UINT;
#define S_OK			(0L)
#define E_FAIL			(-1)
#define FAILED(e)		((e) < 0)
typedef long			HRESULT;

#pragma warning(disable:4311)
#pragma warning(disable:4291)
#pragma warning(disable:4312)
#pragma warning(disable:4996)

extern void Trace(char* format, ...);

//#define TRACE Trace
#define TRACE
#define debug_out 


#define MEMCPY memcpy
#define MEMSET memset
#define STRLEN strlen
#define WSTRLEN(x)		wcslen((wchar_t*)x)

#define  STREXPAND(a, b, c, d) 

#include <stdio.h>
//#include "GLES/gl.h"
#include <Math.h>

#endif __IRF09_PREDEFINE_H__

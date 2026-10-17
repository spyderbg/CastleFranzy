///////////////////////////////////////////////////////////////////////
//
//	Device Specific Defines
//
///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Core/Logs.h"

///////////////////////////////////////////////////////////////////////
/// version RELEASE, REGION and FINAL are set in prefix header file 
///////////////////////////////////////////////////////////////////////

#if !defined VERSION_RELEASE
	#define VERSION_RELEASE	(0)
#endif

#if !defined VERSION_FINAL
	#define VERSION_FINAL	(0)
#endif

#if !defined VERSION_REGION
	#define VERSION_REGION	(0)
#endif


///////////////////////////////////////////////////////////////////////
/// Platform specific defines
///////////////////////////////////////////////////////////////////////

#include "config_version.h"

#ifdef OS_WIN32

	#include "PlatformSpecifics/Win32/Device.h"

#endif

#ifdef OS_LINUX
	#include "PlatformSpecifics/Linux/Device.h"
#endif

#ifdef OS_IPHONE

	#include "PlatformSpecifics/IPhone/Device.h"

	#define DEVICE_TYPE_IPHONE				0
	#define DEVICE_TYPE_ITOUCH				1
	
	#define STATUS_BAR_ORIENTATION_PORTRAIT (1 << 0)
	#define STATUS_BAR_ORIENTATION_IPODTOUCH (1 << 1)
	#define STATUS_BAR_ORIENTATION_IPHONE (1 << 2)
	#define STATUS_BAR_ORIENTATION_DEFAULT STATUS_BAR_ORIENTATION_IPHONE
	
#endif


///////////////////////////////////////////////////////////////////////
/// FPS
///////////////////////////////////////////////////////////////////////

#define LIMIT_FPS

#define FPS_LIMIT				(60)	// compensate the time spent in system timers
#define FRAME_DELAY				(1000 / FPS_LIMIT) /* in ms */


///////////////////////////////////////////////////////////////////////
/// Window size
///////////////////////////////////////////////////////////////////////

#define OS_SCREEN_W		(480)
#define OS_SCREEN_H		(320)

#define GAME_SCREEN_W		OS_SCREEN_W
#define GAME_SCREEN_H		OS_SCREEN_H

#define GAME_SCREEN_W2		(GAME_SCREEN_W>>1)
#define GAME_SCREEN_H2		(GAME_SCREEN_H>>1)

#define GX_SCREEN_W_OFFSET		(0)
#define GX_SCREEN_W				(OS_SCREEN_W - (2 * GX_SCREEN_W_OFFSET))
#define GX_SCREEN_H_OFFSET		(16)
#define GX_SCREEN_H				(OS_SCREEN_H - (2 * GX_SCREEN_H_OFFSET))

#define RX_SCREEN_W_OFFSET		(0)
#define RX_SCREEN_W				(OS_SCREEN_W - (2 * RX_SCREEN_W_OFFSET))
#define RX_SCREEN_H_OFFSET		(0)
#define RX_SCREEN_H				(OS_SCREEN_H - (2 * RX_SCREEN_H_OFFSET))


///////////////////////////////////////////////////////////////////////
// Defines / Typedefs
///////////////////////////////////////////////////////////////////////

#ifndef NULL
	#define NULL 0
#endif

#ifndef null
	#define null 0
#endif

#ifndef pure
	#define pure =0
#endif

#ifndef FALSE
	#define FALSE false
#endif

#ifndef TRUE
	#define TRUE true
#endif


///////////////////////////////////////////////////////////////////////
/// Basic types
///////////////////////////////////////////////////////////////////////

typedef irr::u8			byte;


///////////////////////////////////////////////////////////////////////
/// Asserts
///////////////////////////////////////////////////////////////////////

//array sizeof, get number of element in a static array
#define asizeof(x)			(sizeof(x) / sizeof(x[0]))
#define ASIZEOF(x)			(sizeof(x) / sizeof(x[0]))

struct StructAssert
{
	StructAssert* member;
};

#if defined( DEBUG ) || defined( _DEBUG )			
	#define GX_ASSERT(x) if(!(x)) { \
		DBG_OUT("assert %s failed(%d)%s\n", #x, __LINE__, __FILE__);  \
		{ \
			StructAssert* __assert__=0;\
			__assert__->member = 0;\
		}\
	}
#else
	#define GX_ASSERT( cond )	
#endif

//anim macro
#ifndef ANIM
	#define ANIM(prefix, sufix) ANIM_##prefix##_ANIM_##sufix
#endif


///////////////////////////////////////////////////////////////////////
/// Memory
///////////////////////////////////////////////////////////////////////

#define NEW							new
#define STATIC_NEW					new
#define MM_DELETE					delete
#define MM_DELETE_ARRAY				delete[]
#define SAFE_DEL(p)					{ if (p) { MM_DELETE (p); p = NULL; } }
#define SAFE_DEL_ARRAY(p)			{ if (p) { MM_DELETE_ARRAY (p); p = NULL; } }
#define SAFE_DEL_ARRAY_OBJ(p, n)	{ if (p) { for (s32 __i = 0; __i < n; __i++) SAFE_DEL((p)[__i]); MM_DELETE_ARRAY (p); p = NULL; } }
#define SAFE_DEL_ARRAY_ARRAY(p, n)	{ if (p) { for (s32 __i = 0; __i < n; __i++) SAFE_DEL_ARRAY((p)[__i]); MM_DELETE_ARRAY (p); p = NULL; } }
#define SAFE_DEL_TYPE(p,t)			{ if (p) { SAFE_DEL(p); p = NULL; } }
#define SAFE_DEL_ARRAY_TYPE(p,t)	{ if (p) { SAFE_DEL_ARRAY(p); p = NULL; } } 


///////////////////////////////////////////////////////////////////////
/// Dynamic casts
///////////////////////////////////////////////////////////////////////

#ifdef ENABLE_RTTI
	#define DYNAMIC_CAST(t, v) dynamic_cast<t>(v)
	#define STATIC_CAST(t, v) static_cast<t>(v)
	#define REINTERPRET_CAST(t, v) reinterpret_cast<t>(v)
	#define CONST_CAST(t, v) const_cast<t>(v)
#else
	#define DYNAMIC_CAST(t, v) ((t)(v))
	#define STATIC_CAST(t, v) ((t)(v))
	#define REINTERPRET_CAST(t, v) ((t)(v))
	#define CONST_CAST(t, v) ((t)(v))
#endif


///////////////////////////////////////////////////////////////////////
/// s64
///////////////////////////////////////////////////////////////////////

class s64
{
	int high_bits;
	unsigned int low_bits;
public:
	explicit s64( int int_val )
	{
		GX_ASSERT(int_val >= 0);
		low_bits = int_val;
		high_bits = 0;
	}
	s64( int high, unsigned int low )
	{
		high_bits = high;
		low_bits = low;
	}
	s64()
	{
		high_bits = 0;
		low_bits = 0;
	}

	// <-- higher [63, 0] lower -->
	bool TestBit( int which_bit )
	{
		GX_ASSERT(which_bit >= 0 && which_bit <= 63);
		if ( which_bit <= 31 )
		{
			return ( (low_bits >> which_bit) & 1 ) != 0;
		}

		which_bit -= 32;
		return ( (high_bits >> which_bit) & 1 ) != 0;
	}

	const s64& operator = ( int int_val )
	{
		GX_ASSERT(int_val >= 0);
		low_bits = int_val;
		high_bits = 0;
		return *this;
	}

//		operator bool()
//		{
//			return !(high_bits == 0 && low_bits == 0);
//		}
//
//		s64 operator << ( int bits )
//		{
//		}
};


// Integer and float data types, maths macros
#include "Common/GX_Math.h"

///////////////////////////////////////////////////////////////////////
/// Global defines
///////////////////////////////////////////////////////////////////////

#define ARRAY_SIZE(array)	(sizeof(array)/sizeof(array[0]))
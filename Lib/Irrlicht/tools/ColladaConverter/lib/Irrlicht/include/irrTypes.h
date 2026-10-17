// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_TYPES_H_INCLUDED__
#define __IRR_TYPES_H_INCLUDED__

#include "IrrCompileConfig.h"

#define IRR_ENUM_FLAG(name) \
	static name operator|(name a, name b) \
	{ \
		return (name)((int)a | (int)b); \
	} \
	static name operator&(name a, name b) \
	{ \
		return (name)((int)a & (int)b); \
	} \
	static name operator~(name a) \
	{ \
		return (name)(~(int)a); \
	}\
	static name& operator|=(name& a, name b) \
	{ \
		a = a | b; \
		return a; \
	} \
	static name& operator&=(name& a, name b) \
	{ \
		a = a & b; \
		return a; \
	} \

namespace irr {
enum MemHint 
{
	MEMHINT_NONE		= 0x00000000,

	MEMHINT_ZERO		= 0x00000001, //memset 0 after allocation
	MEMHINT_STATIC		= 0x00000002, //for 'allocated once at the beginning and is never going to be released' allocation
	MEMHINT_ALLOCBACK	= 0x00000004, //as opposed to bottom
	MEMHINT_BESTFIT		= 0x00000008, //as opposed to first fit

	MEMHINT_FAST		= 0x00000100, //platform specific, for wii, allocate from mem1 (instead of mem2)
	MEMHINT_TEXTURE		= 0x00000200, //platform specific
	
	MEMHINT_ALIGN4		= 0x00001000,
	MEMHINT_ALIGN8		= 0x00002000,
	MEMHINT_ALIGN16		= 0x00003000,
	MEMHINT_ALIGN32		= 0x00004000,
	MEMHINT_ALIGN64		= 0x00005000,
	MEMHINT_ALIGN128	= 0x00006000,
	MEMHINT_ALIGN256	= 0x00007000,
	MEMHINT_ALIGN512	= 0x00008000,
	MEMHINT_ALIGN1024	= 0x0000a000,

	MEMHINT_PAD4		= 0x10000000,
	MEMHINT_PAD8		= 0x20000000,
	MEMHINT_PAD16		= 0x30000000,
	MEMHINT_PAD32		= 0x40000000,
	MEMHINT_PAD64		= 0x50000000,
	MEMHINT_PAD128		= 0x60000000,
	MEMHINT_PAD256		= 0x70000000,
	MEMHINT_PAD512		= 0x80000000,
	MEMHINT_PAD1024		= 0xa0000000,

	//flags ends here
	MEMHINT_ALIGN_FLAG	= 0x0000f000,
	MEMHINT_ALIGN_SHIFT	= 12,

	MEMHINT_PAD_FLAG	= 0xf0000000,
	MEMHINT_PAD_SHIFT	= 28,

	MEMHINT_DEFAULT_ALIGNMENT	= 4,
	MEMHINT_DEFAULT_PADDING		= 4,
};
}

void* operator new(size_t size, irr::MemHint hint);
void* operator new[](size_t size, irr::MemHint hint);
void operator delete(void* p, irr::MemHint hint);
void operator delete[](void* p, irr::MemHint hint);

void* operator new(size_t size, irr::MemHint hint, int block, const char* filename, int line);
void* operator new[](size_t size, irr::MemHint hint, int block, const char* filename, int line);
void operator delete(void* p, irr::MemHint hint, int block, const char* filename, int line);
void operator delete[](void* p, irr::MemHint hint, int block, const char* filename, int line);

namespace irr
{


//! 8 bit unsigned variable.
/** This is a typedef for unsigned char, it ensures portability of the engine. */
//#ifdef _MSC_VER
//typedef unsigned __int8		u8;
//#else
typedef unsigned char		u8;
//#endif

//! 8 bit signed variable.
/** This is a typedef for signed char, it ensures portability of the engine. */
//#ifdef _MSC_VER
//typedef __int8			s8;
//#else
typedef signed char		s8;
//#endif

//! 8 bit character variable.
/** This is a typedef for char, it ensures portability of the engine. */
typedef char			c8;



//! 16 bit unsigned variable.
/** This is a typedef for unsigned short, it ensures portability of the engine. */
//#ifdef _MSC_VER
//typedef unsigned __int16	u16;
//#else
typedef unsigned short		u16;
//#endif

//! 16 bit signed variable.
/** This is a typedef for signed short, it ensures portability of the engine. */
//#ifdef _MSC_VER
//typedef __int16			s16;
//#else
typedef signed short		s16;
//#endif



//! 32 bit unsigned variable.
/** This is a typedef for unsigned int, it ensures portability of the engine. */
//#ifdef _MSC_VER
//typedef unsigned __int32	u32;
//#else
typedef unsigned int		u32;
//#endif

//! 32 bit signed variable.
/** This is a typedef for signed int, it ensures portability of the engine. */
//#ifdef _MSC_VER
//typedef __int32			s32;
//#else
typedef signed int		s32;
//#endif



// 64 bit signed variable.
// This is a typedef for __int64, it ensures portability of the engine.
// This type is currently not used by the engine and not supported by compilers
// other than Microsoft Compilers, so it is outcommented.
//typedef __int64				s64;



//! 32 bit floating point variable.
/** This is a typedef for float, it ensures portability of the engine. */
typedef float				f32;

//! 64 bit floating point variable.
/** This is a typedef for double, it ensures portability of the engine. */
typedef double				f64;


} // end namespace irr

IRR_ENUM_FLAG(irr::MemHint)


#include <wchar.h>
#ifdef _IRR_WINDOWS_API_
//! Defines for s{w,n}printf because these methods do not match the ISO C
//! standard on Windows platforms, but it does on all others.
//! These should be int snprintf(char *str, size_t size, const char *format, ...);
//! and int swprintf(wchar_t *wcs, size_t maxlen, const wchar_t *format, ...);
#if defined(_MSC_VER) && _MSC_VER > 1310 && !defined (_WIN32_WCE)
#define swprintf swprintf_s
#define snprintf sprintf_s
#else
#define swprintf _snwprintf
#define snprintf _snprintf
#endif

// define the wchar_t type if not already built in.
#ifdef _MSC_VER
#ifndef _WCHAR_T_DEFINED
//! A 16 bit wide character type.
/**
	Defines the wchar_t-type.
	In VS6, its not possible to tell
	the standard compiler to treat wchar_t as a built-in type, and
	sometimes we just don't want to include the huge stdlib.h or wchar.h,
	so we'll use this.
*/
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif // wchar is not defined
#endif // microsoft compiler
#endif // _IRR_WINDOWS_API_

//! define a break macro for debugging.
#if defined(_DEBUG)
#if defined(_IRR_WINDOWS_API_) && defined(_MSC_VER) && !defined (_WIN32_WCE)
  #if defined(_WIN64) // using portable common solution for x64 configuration
  #include <crtdbg.h>
  #define _IRR_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_CrtDbgBreak();}
  #else
  #define _IRR_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_asm int 3}
  #endif
#else
#include "assert.h"
#define _IRR_DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
#endif
#else
#define _IRR_DEBUG_BREAK_IF( _CONDITION_ )
#endif

//! Defines a small statement to work around a microsoft compiler bug.
/** The microsoft compiler 7.0 - 7.1 has a bug:
When you call unmanaged code that returns a bool type value of false from managed code,
the return value may appear as true. See
http://support.microsoft.com/default.aspx?kbid=823071 for details.
Compiler version defines: VC6.0 : 1200, VC7.0 : 1300, VC7.1 : 1310, VC8.0 : 1400*/
#if defined(_IRR_WINDOWS_API_) && defined(_MSC_VER) && (_MSC_VER > 1299) && (_MSC_VER < 1400)
#define _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX __asm mov eax,100
#else
#define _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX
#endif // _IRR_MANAGED_MARSHALLING_BUGFIX


// memory debugging
#if defined(_DEBUG) && defined(IRRLICHT_EXPORTS) && defined(_MSC_VER) && \
	(_MSC_VER > 1299) && !defined(_IRR_DONT_DO_MEMORY_DEBUGGING_HERE) && !defined(_WIN32_WCE)

	#define CRTDBG_MAP_ALLOC
	#define _CRTDBG_MAP_ALLOC
	
	//#define DEBUG_CLIENTBLOCK(hint) irrnew (hint, _CLIENT_BLOCK, __FILE__, __LINE__)
	
	#include <stdlib.h>
	#include <crtdbg.h>

	#define irrnew			new (irr::MEMHINT_NONE, _CLIENT_BLOCK, __FILE__, __LINE__)
	#define irrnewh(hint)	new (hint, _CLIENT_BLOCK, __FILE__, __LINE__)

#else
	#define irrnew			new (irr::MEMHINT_NONE)
	#define irrnewh(hint)	new (hint)
#endif

// disable truncated debug information warning in visual studio 6 by default
#if defined(_MSC_VER) && (_MSC_VER < 1300 )
#pragma warning( disable: 4786)
#endif // _MSC


//! ignore VC8 warning deprecated
/** The microsoft compiler */
#if defined(_IRR_WINDOWS_API_) && defined(_MSC_VER) && (_MSC_VER >= 1400)
	//#pragma warning( disable: 4996)
	//#define _CRT_SECURE_NO_DEPRECATE 1
	//#define _CRT_NONSTDC_NO_DEPRECATE 1
#endif


//! creates four CC codes used in Irrlicht for simple ids
/** some compilers can create those by directly writing the
code like 'code', but some generate warnings so we use this macro here */
#define MAKE_IRR_ID(c0, c1, c2, c3) \
		((u32)(u8)(c0) | ((u32)(u8)(c1) << 8) | \
		((u32)(u8)(c2) << 16) | ((u32)(u8)(c3) << 24 ))


#endif // __IRR_TYPES_H_INCLUDED__



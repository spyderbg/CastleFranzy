/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/
/*
	Based on the FS Import classes:
	Copyright (C) 2005-2006 Feeling Software Inc
	Copyright (C) 2005-2006 Autodesk Media Entertainment
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FUtils.h
	Includes the common utilities classes and macros.
*/

/** @defgroup FUtils Utility Classes. */

// [January 2007] PLUG_CRT and the CRT memory debugger is not used directly, anymore, at Feeling Software.
// Instead, we rely on the Visual Leak Detector: http://www.codeproject.com/tools/visualleakdetector.asp

#ifndef _F_UTILS_H_
#define _F_UTILS_H_

// CRT
#ifdef _WIN32
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1 // MSVS 2005 support.
#endif // _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable:4702)
#endif // _WIN32

#define NOMINMAX /**< Rid us of the default versions of MINMAX. */
#ifdef max
#undef max
#endif // max
#ifdef min
#undef min
#endif // min

#ifndef FCOLLADA_NOMINMAX
/** Retrieves the largest of two values. */
template<class T>
inline T& max(T& x, T& y) { return (x > y) ? x : y; }
template<class T>
inline const T& max(const T& x, const T& y) { return (x > y) ? x : y; } /**< See above. */

/** Retrieves the smallest of two values. */
template <class T>
inline T& min(T& x, T& y) { return (x < y) ? x : y; }
template <class T>
inline const T& min(const T& x,const T& y) { return (x < y) ? x : y; } /**< See above. */
#endif // FCOLLADA_NOMINMAX

#ifdef _WIN32
#pragma warning(default:4702)
#endif

// Conversion macros
#define UNUSED(a) /**< Removes a piece of code during the pre-process. This macro is useful for these pesky unused variable warnings. */
#ifdef _DEBUG
#define UNUSED_NDEBUG(a) a
#else
#define UNUSED_NDEBUG(a) /**< Removes a piece of debug code during the pre-process. This macro is useful for these pesky unused variable warnings. */
#endif // _DEBUG

// Pre-include the platform-specific macros and definitions
#ifndef _FU_PLATFORMS_H_
#include "FUtils/Platforms.h"
#endif // _FU_PLATFORMS_H_
#ifndef _FU_ASSERT_H_
#include "FUtils/FUAssert.h"
#endif // _FU_ASSERT_H_

// FMath
#define HAS_VECTORTYPES /**< Used by FCollada, implies that we are including all the common dynamically-sized arrays. */
#ifndef _F_MATH_H_
#include "FMath/FMath.h"
#endif // _F_MATH_H_

// LibXML
#ifndef NO_LIBXML
#define HAS_LIBXML /**< Used by FCollada, implies that we are including LibXML functions in the library interface. */
#if !defined(FCOLLADA_DLL) || defined(FCOLLADA_INTERNAL)
#define LIBXML_STATIC /**< Used by LibXML, implies that we are statically-linking the LibXML. */
#endif // !FCOLLADA_DLL || !FCOLLADA_INTERNAL
#ifndef __XML_TREE_H__
#include <libxml/tree.h>
#endif // __XML_TREE_H__
#else // NO_LIBXML
typedef struct _xmlNode xmlNode;
#endif // NO_LIBXML

// SAFE_DELETE Macro set.
#define SAFE_DELETE(ptr) if ((ptr) != NULL) { delete (ptr); (ptr) = NULL; } /**< This macro safely deletes a pointer and sets the given pointer to NULL. */
#define SAFE_DELETE_ARRAY(ptr) if (ptr != NULL) { delete [] ptr; ptr = NULL; } /**< This macro safely deletes an heap array and sets the given pointer to NULL. */
#define SAFE_FREE(ptr) if (ptr != NULL) { free(ptr); ptr = NULL; } /**< This macro safely frees a memory block and sets the given pointer to NULL. */
#define SAFE_RELEASE(ptr) { if ((ptr) != NULL) { (ptr)->Release(); (ptr) = NULL; } } /**< This macro safely releases an interface and sets the given pointer to NULL. */
#define CLEAR_POINTER_VECTOR(a) { size_t l = (a).size(); for (size_t i = 0; i < l; ++i) SAFE_DELETE((a)[i]); (a).clear(); } /**< This macro deletes all the object pointers contained within a vector and clears it. */
#define CLEAR_POINTER_MAP(m,iterType) { for ( iterType __mapIter = (m).begin(); __mapIter != (m).end(); ++__mapIter ){ SAFE_DELETE( __mapIter->second ); } (m).clear(); } /**< This macro deletes all the object pointers contained within the map's values and clears it. */
#define CLEAR_ARRAY_VECTOR(a) { size_t l = (a).size(); for (size_t i = 0; i < l; ++i) SAFE_DELETE_ARRAY((a)[i]); (a).clear(); } /**< This macro deletes all the object array pointers contained within a vector and clears it. */
#define CLEAR_POINTER_STD_PAIR_CONT(cont, a){ for (cont::iterator it = (a).begin(); it != (a).end(); ++it) SAFE_DELETE((*it).second); (a).clear(); } /**< This macro deletes all the object pointers contained within any std container and clears it. */
#define CLEAR_POINTER_STD_CONT(cont, a){ for (cont::iterator it = (a).begin(); it != (a).end(); ++it) SAFE_DELETE((*it)); (a).clear(); } /**< This macro deletes all the object pointers contained within any std container and clears it. */

#define FILL_VECTOR( vector, value ){ for ( int vectorIndex = 0; vectorIndex < (vector).size(); ++vectorIndex ){ (vector)[vectorIndex] = value; } }

#define ARRAY_SIZE( array )( sizeof( array ) / sizeof( *array ) )
// for array declared as []
#define FILL_ARRAY( array, value ){ for ( int __arrayIdx = 0; __arrayIdx < ARRAY_SIZE( array ); __arrayIdx++ ){ array[__arrayIdx] = value; } }
#define ZERO_ARRAY( array ) FILL_PTR_ARRAY( array, 0 )
// for array declared as pointer
#define FILL_PTR_ARRAY( array, size, value ){ for ( int __arrayIdx = 0; __arrayIdx < size; __arrayIdx++ ){ array[__arrayIdx] = value; } }
#define ZERO_PTR_ARRAY( array, size ) FILL_PTR_ARRAY( array, size, 0 )

#define MAX_FILL_ARRAY( array, size, value ){ for ( int __arrayIdx = 0; __arrayIdx < size; __arrayIdx++ ){ array[__arrayIdx] = value; } }
#define MAX_ZERO_ARRAY( array, size ) MAX_FILL_ARRAY( array, size, 0 )

#define MAX_NEW( p ) MAX_new( p )
#define MAX_NEW_ARRAY( p ) MAX_new_array( p )
#define MAX_DELETE_ARRAY( p ) MAX_delete_array( p )
#define MAX_SAFE_DELETE_ARRAY( p ){ if ( p ){ MAX_DELETE_ARRAY( p ); p = NULL; } }

#if MAX_VERSION_MAJOR >= 9
#define MAX_MALLOC( size ) MAX_malloc( size )
#else
#define MAX_MALLOC( size ) malloc( size )
#endif // MAX_VERSION_MAJOR

// flag methods
#define SET_FLAG(dest, flag)		dest |= (flag)
#define RESET_FLAG(dest, flag)		dest &= (~(flag))
#define TEST_FLAG(dest, flag)		(((dest) & (flag)) != 0)
#define TEST_ALLFLAG(dest, flag)	(((dest) & (flag)) == flag)
#define RESETALL_FLAG(dest)			dest = 0

// ticks, frames, secs conversion methods
#define TicksToFrames( ticks )( (ticks) / GetTicksPerFrame() )
#define FramesToTicks( frames )( (frames) * GetTicksPerFrame() )
#define SecsToFrames( secs )( (secs) * GetFrameRate() )
#define FramesToSecs( frames )( (frames) / GetFrameRate() )
#define SecsToTicks( secs )( (secs) * TIME_TICKSPERSEC )
#define TicksToSecs( ticks )( (ticks) / TIME_TICKSPERSEC )

#define GET_VALUE_SHIFTED_AND_MASKED( value, shift, mask )( ( value >> shift ) & mask )

// error enums
#define IC_OK (0)
#define IC_ERROR (-1)

// max9/max8 macros
#if MAX_VERSION_MAJOR >= 9
#define NEW_MAX_STRING( charArray )( new String( charArray ) )
#else
#define NEW_MAX_STRING( charArray )( new String( ( TCHAR * )( charArray ) ) )
#endif

#if MAX_VERSION_MAJOR >= 9
#define CLONE_REF_TARGET_HANDLE RefTargetHandle Clone(RemapDir &remap = DefaultRemapDir())
#else
#define CLONE_REF_TARGET_HANDLE RefTargetHandle Clone(RemapDir &remap = NoRemap())
#endif

#if MAX_VERSION_MAJOR < 9
#define MCHAR char
#endif // MAX_VERSION_MAJOR >= 9

//---------------------------------------------------------------------------
// In / Out helper
//---------------------------------------------------------------------------
#define BReadByte( buffer, bytePtr ){ *bytePtr = *(char*)buffer; ((char*)buffer) += sizeof( char ); }
#define BReadShort( buffer, shortPtr ){ *shortPtr = *(short*)buffer; ((char*)buffer) += sizeof( short ); }
#define BReadInt( buffer, intPtr ){ *intPtr = *(int*)buffer; ((char*)buffer) += sizeof( int ); }

#define BWriteByte( buffer, _byte  ){ *(char *)buffer = _byte; ((char*)buffer) += sizeof( char ); }
#define BWriteShort( buffer, _short ){ *(short *)buffer = _short; ((char*)buffer) += sizeof( short ); }
#define BWriteInt( buffer, _int   ){ *(int *)buffer = _int; ((char*)buffer) += sizeof( int ); }

// ui macros
#define DLG_PROCS_NAME( name ) name##DlgProcS
#define DLG_PROC_NAME( name ) name##DlgProc

#define DLG_PROC_DECL( name ) \
	INT_PTR DLG_PROC_NAME( name )(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); \
	static INT_PTR CALLBACK DLG_PROCS_NAME( name )(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#define DLG_PROC_IMPL( _class, name ) \
	INT_PTR CALLBACK _class##::DLG_PROCS_NAME( name )(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) \
{ \
	_class* exp = UIHelpers::GetExpPointer<_class>( hWnd, message, wParam, lParam ); \
	return exp ? exp->DLG_PROC_NAME( name )(hWnd, message, wParam, lParam) : FALSE; \
} \
\
INT_PTR _class##::DLG_PROC_NAME( name )(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )

// Base RTTI object and object container class
#ifndef _FU_OBJECT_TYPE_H_
#include "FUtils/FUObjectType.h"
#endif //_FU_OBJECT_TYPE_H_
#ifndef _FU_OBJECT_H_
#include "FUtils/FUObject.h"
#endif // _FU_OBJECT_H_
#ifndef _FU_TRACKER_H_
#include "FUtils/FUTracker.h"
#endif // _FU_TRACKER_H_

// More complex utility classes
#ifndef _FU_STRING_H_
#include "FUtils/FUString.h"
#endif // _FU_STRING_H_
#ifndef _FU_CRC32_H_
#include "FUtils/FUCrc32.h"
#endif // _FU_CRC32_H_
#ifndef _FU_DEBUG_H_
#include "FUtils/FUDebug.h"
#endif // _FU_DEBUG_H_
#ifndef _FU_ERROR_H_
#include "FUtils/FUError.h"
#endif // _FU_ERROR_H_

#ifndef RETAIL
#define ENABLE_TEST /**< Used by FCollada, enables the compilation and gives access to the unit tests. Disabled in the retail configurations. */
#endif

#endif // _F_UTILS_H_

#ifndef _GLT_DEVICE_H_
#define _GLT_DEVICE_H_
//#include "platform/xGLLiveMemory.h"

#include "GLLiveStdafx.h"
//#include "gles/egltypes.h"

#ifndef NULL
	#define NULL 0
#endif
#if defined(_WIN32_WCE)
#include <string.h>

typedef __int64				GLfixed64;
typedef unsigned __int64		u64;
typedef __int64				s64;
#define GLT_ASSERT( exp )	

#elif defined(WIN32) && !defined(WIPI)

	#include <string.h>

//	#define GLT_MEMCPY				memcpy
//	#define GLT_MEMSET				memset

	typedef __int64					GLfixed64;
	typedef unsigned __int64		u64;
	typedef __int64					s64;

#if defined _DEBUG
	#define GLT_ASSERT( exp )		{ if(!(exp)) {_asm int 3 } }
#else
	#define GLT_ASSERT( exp )	
#endif

//	#define	GLT_NEW					new
//	#define GLT_DELETE				delete
//	#define GLT_DELETE_ARRAY		delete[]
//
//	#include <malloc.h>
//	#define GLT_MALLOC				malloc
//	#define GLT_FREE				free

#elif BREW

	#include "AEEStdLib.h"			// Std Functions

//	#define GLT_MEMCPY				MEMCPY
//	#define GLT_MEMSET				MEMSET

	typedef long long				GLfixed64;
	typedef unsigned long long		u64;

	#define GLT_ASSERT( exp )		

//	#define	GLT_NEW					new
//	#define GLT_DELETE				delete
//	#define GLT_DELETE_ARRAY		delete[]

//#if defined(ALLOC_NO_ZMEM)
//	#define GLT_MALLOC(size)		MALLOC((size) | ALLOC_NO_ZMEM)
//#else
//	#define GLT_MALLOC				MALLOC
//#endif
//
//	#define GLT_FREE				FREE

#elif defined(WIPI)
#include "WIPIHeader.h"
#include "memory/MemoryAllocation.h"
//		typedef unsigned char	u8;
//		typedef signed char		s8;
//		typedef unsigned short	u16;
//		typedef signed short	s16;
//		typedef unsigned int	u32;
//		typedef signed int		s32;

//		#define GLT_MEMCPY				memcpy
//		#define GLT_MEMSET				memset

		typedef M_Int64					GLfixed64;
		//typedef M_Uint64				u64;
		typedef M_Uint64				u64;

		#define GLT_ASSERT( exp1 )

//		#define	GLT_NEW					STATIC_NEW
//		#define GLT_DELETE				MM_DELETE
//		#define GLT_DELETE_ARRAY		DELETE_ARRAY
//
//		#define GLT_MALLOC				MM_MALLOC//glt_malloc_wipi(size)
//
//		#define GLT_FREE				MM_FREE//glt_free_wipi(ptr)
#elif defined(__SYMBIAN32__)
	//#include "AEEStdLib.h"			// Std Functions

//	#define GLT_MEMCPY				MEMCPY
//	#define GLT_MEMSET				MEMSET

	typedef long long				GLfixed64;
	typedef unsigned long long		u64;

	#define GLT_ASSERT( exp )		

//	#define	GLT_NEW					new
//	#define GLT_DELETE				delete
//	#define GLT_DELETE_ARRAY		delete[]

//#if defined(ALLOC_NO_ZMEM)
//	#define GLT_MALLOC(size)		malloc((size) | ALLOC_NO_ZMEM)
//#else
//	#define GLT_MALLOC				malloc
//#endif
//
//	#define GLT_FREE				free

#elif (PLATFORM == PLATFORM_MAC_OS_X) && (!defined(WIN32))

typedef long long			GLfixed64;
typedef unsigned long long		u64;
#define GLT_ASSERT( exp )

#elif (PLATFORM == PLATFORM_KDDI)
typedef long long			GLfixed64;
typedef unsigned long long		u64;
#define GLT_ASSERT(exp)
#else

#error not implemented

#endif

#endif


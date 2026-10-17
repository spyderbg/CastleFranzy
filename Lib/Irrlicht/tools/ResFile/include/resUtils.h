#ifndef __RESUTILS_H__
#define __RESUTILS_H__

#define DECL_RES_WRITER(type) void Write(Chunk &chk, const type &data)

//! define a break macro for debugging.
#if defined(_DEBUG)
	#if defined(WIN32) && defined(_MSC_VER) && !defined (_WIN32_WCE)
		#if defined(_WIN64) // using portable common solution for x64 configuration
			#include <crtdbg.h>
			#define _RES_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_CrtDbgBreak();}
		#else
			#define _RES_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_asm int 3}
		#endif
	#else
		#include <cassert>
		#define _RES_DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
	#endif
#else
	#define _RES_DEBUG_BREAK_IF( _CONDITION_ )
#endif

#endif
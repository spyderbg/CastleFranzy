#ifndef __GLL_H_INCLUDED__
#define __GLL_H_INCLUDED__

extern int GLLiveShowMessageBox(const char* message, const char* code, const char* file, const long line);

#define GLL_VERSION "0.0.0"

#define TOSTRING(x) #x
#define STRING(x) 	TOSTRING(x)

//#define CGL_OVERRIDE_CONFIG cgl_custom_config.h

#include "Compileoptions.h"

#if PLATFORM == PLATFORM_WIN32

//#if defined(_WINDOWS) && !defined(WIPI)
	#define GLL_BREAKPOINT { _asm int 3 }
//#else
//	#define GLL_BREAKPOINT { System::Print("breakpoint failed: %d:%s", __LINE__, System::ExtractFilename(__FILE__)); }
//#endif

#if USE_ASSERT
	#ifdef WIPI
		#define GLL_ASSERT(cond)		//if(!(cond)) { if(System::GLLiveShowMessageBox(#cond, "assert failed", __FILE__, __LINE__)) GLL_BREAKPOINT; }
	#else
		#define GLL_ASSERT(cond)		if(!(cond)) { if(GLLiveShowMessageBox(#cond, "assert failed", __FILE__, __LINE__)) GLL_BREAKPOINT; }
	#endif
#else
	#define GLL_ASSERT(cond)			//if(!(cond)) { GLL_BREAKPOINT }
#endif


#if USE_ASSERT2
	#define GLL_ASSERT2(cond, msg)	if(!(cond)) { if(System::GLLiveShowMessageBox(#cond, msg, __FILE__, __LINE__)) GLL_BREAKPOINT; }
#else
	#define GLL_ASSERT2(cond, msg)		//if(!(cond)) { System::Println(msg); GLL_BREAKPOINT }
#endif

#ifndef NULL
  #define NULL  0
#endif



#if _DEBUG || DEBUG

	#define REMINDER( msg )				message( __FILE__ "(" STRING(__LINE__) ") : REMINDER " msg )
	#define REMINDERTO(to, msg )		message( __FILE__ "(" STRING(__LINE__) ") : REMINDER [TO "#to"] " msg )
	#define HARDCODED(v)				message( __FILE__ "(" STRING(__LINE__) ") : HARDCODED '"#v"'"  )
	#define MUST_NOT_COMMIT				message( __FILE__ "(" STRING(__LINE__) ") : *************************** MUST NOT COMMIT THIS ***********************")
	#define REMOVE_THIS_SHIT			message( __FILE__ "(" STRING(__LINE__) ") : REMOVE THIS SHIT : TEMP/HARDCODED/HACKED OR JUST PLAIN WRONG STUFF HERE.")

	#define PRINTSTACKTRACE(e)	

	#define DBG_TRY				try
	#define DBG_CATCH(excep)	catch(excep)

#else

	#define REMINDER( msg )
	#define REMINDERTO(to, msg )
	#define HARDCODED(v)
	#define MUST_NOT_CHECKIN
	#define REMOVE_THIS_SHIT

	#define PRINTSTACKTRACE(e)	

	#define DBG_TRY					
	#define DBG_CATCH(excep)	if(false)

#endif

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;

typedef signed char		s8;
typedef signed short	s16;
typedef signed int		s32;

#if defined(_WINDOWS) && !defined(WIPI)
	typedef __int64 s64;
	typedef __int64 int64;
#elif defined(WIPI)
	#include"MCtypes.h"
	typedef M_Int64		s64;
	typedef M_Int64		int64;
#elif defined(__SYMBIAN32__)
	typedef __int64 s64;
	typedef __int64 int64;
#else
	typedef signed long long s64;
	typedef signed long long int64;
#endif

typedef int resid;

#define DECLARE_LOADABLE(type) \
static void Load##type(void* ptr, BufferStream* stream) \
{\
	((Resource<type>*)ptr)->Load(stream);\
}\

//#include "GLLivePredefine.h"


//#include "gll/resourcemanager.h"
#elif PLATFORM == PLATFORM_NGAGE

#define GLL_ASSERT(cond) void(0)

#elif  PLATFORM == PLATFORM_SYMBIAN

#define GLL_ASSERT(cond) void(0)

#elif  PLATFORM == PLATFORM_WINDOWS_MOBILE

#define GLL_ASSERT(cond) void(0)

#elif  PLATFORM == PLATFORM_MAC_OS_X

#define GLL_ASSERT(cond) void(0)

#elif PLATFORM == PLATFORM_KDDI

#define GLL_ASSERT(cond)

#endif

#endif

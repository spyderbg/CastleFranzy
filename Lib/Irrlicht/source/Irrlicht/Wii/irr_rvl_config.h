// make sure _DEBUG is defined if NDEBUG is not defined


// defines for RVL target
#define RVL
#define RVL_SDK 1
#define TRK_INTEGRATION
#define RVL_OS 1
#define EPPC
#define GEKKO
#define	HOLLYWOOD_REV	1
#define	BROADWAY_REV	1
#define	IOP_REV	1
#define __STDC_WANT_LIB_EXT1__ 0
#define _MSL_C_HAS_CPP_EXTENSIONS 1
#define _MSL_C99 1
//#define _MSL_NO_THROW_SPECS 1
/*
 * Enabling C++ extensions if compiling as C++
 * sw on 09102002
 */

#define _WII 1
#define _IRR_STATIC_LIB_ 1

#ifdef __cplusplus
#pragma cpp_extensions on
#endif

#include <ansi_prefix.PPCEABI.Gamecube.h>


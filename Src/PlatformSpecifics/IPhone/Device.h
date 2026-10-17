#ifndef __DEFS_H__
#define __DEFS_H__


////////////////////////////////////////////////////////////////////////////////////////////////////


#define EMPTY_BODY
#define EMPTY_BODY_RET(x)


typedef unsigned long long	oword;
typedef unsigned long long	dqword;
typedef unsigned long long	uint64;
typedef long long			int64;

typedef double				TimeStamp;
typedef unsigned long long	StreamSize;
typedef short				Coord;

typedef wchar_t TChar;



////////////////////////////////////////////////////////////////////////////////////////////////////
// Common includes...


#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <new>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <QuartzCore/QuartzCore.h>

//#include <Foundation/NSException.h>


// Device-specific defines
//#include "Common/DeviceInfo.h"

// Game version defines
//#include "Common/config_version.h"

// Integer and float data types, maths macros
//#include "Common/GX_Math.h"

#include "IO/TouchScreen/TouchPad.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
// Resources...

#ifdef OS_IPHONE
	#define RES_PATH "/FortressDefender.app/"
	#define	DOC_PATH "/Documents/"
#else
	#define RES_PATH ""
	#define	DOC_PATH ""
#endif

#define MAX_RESOURCE_FILE 1
#define DEFAULT_RESOURCE_FILE    RES_PATH"res.bar"

//char KFile_Save[]				= DOC_PATH"data.sav";
//char KFile_MiscTextures[]		= RES_PATH"texturesmisc.bar";
//char KFile_Sprites[]			= RES_PATH"sprites.bar";
//char KFile_SpritesTex[]			= RES_PATH"sprites_tex.bar";
//char KFile_Sounds[]				= RES_PATH"sounds.bar";
//char KLogFile[]					= RES_PATH"log.txt";
//char KJPFontMap[]               = RES_PATH"JPFont.dat";
//char KKRFontMap[]               = RES_PATH"KRFont.dat";




////////////////////////////////////////////////////////////////////////////////////////////////////
// Maths, logics and strings macros...

	#ifndef TRUE
	#define TRUE	(true)
	#endif
	#ifndef FALSE
	#define FALSE	(false)
	#endif

	#define TCHAR TChar

	#define BOOL	bool
	#define BYTE	unsigned char

	#define _WIN32_WINNT 0x0500
	#define MAX_INT			0x7FFFFFFF
	#define MAX_PTR			32768
	
	#define STRCHR(dest, src)				strchr(dest, src)
	#define STRNCPY(dest, src, n)			strncpy(dest, src, n)
	#define STRLEN(string)					strlen(string)


////////////////////////////////////////////////////////////////////////////////////////////////////
// Range checking...

// ToDo
#ifdef GX_CHECK_MATH
	// ToDo
#else
	#define CHECK_SHORT_LIMIT(x)
	#define CHECK_LIMIT(x)
    #define CHK_PRECISION(fResult, nResult, fTolerancePercentage)
	#define CHK_ADD(x, y)
	#define	CHK_MUL(x, y)
#endif // GX_CHECK_MATH

#define CHK_ADD3(x1,x2,x3)					{ CHK_ADD(x1,x2); CHK_ADD((x1)+(x2),x3); }
#define CHK_MUL2_ADD3(x1,x2,y1,y2,z1,z2)	{ CHK_MUL(x1,x2); CHK_MUL(y1,y2); CHK_MUL(z1,z2); CHK_ADD3((x1)*(x2),(y1)*(y2),(z1)*(z2)); }

////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLAMP_VAL(val, min, max)              { if ((val) < (min)){val = min;} else if ((val) > (max)) {val = max;} }


#endif //__DEFS_H__
///////////////////////////////////////////////////////////////////////
//
//	Device Specific Defines
//
///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include <time.h>
#include <windows.h>
#include <process.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <GL/gl.h>
//#include <gl/GLAux.h>
#include <GL/glu.h>

///////////////////////////////////////////////////////////////////////

#define EMPTY_BODY			{}
#define EMPTY_BODY_RET(x)	{return (x);}


typedef unsigned __int64	oword;
typedef unsigned __int64	dqword;
typedef unsigned __int64	uint64;
typedef __int64				int64;

typedef double				TimeStamp;
typedef unsigned __int64	StreamSize;
typedef short				Coord;

typedef wchar_t TChar;

///////////////////////////////////////////////////////////////////////
// Maths, logics and strings macros...

#ifndef NULL
	#define	NULL	0L
#endif

#define STRCHR(dest, src)				strchr(dest, src)
#define STRNCPY(dest, src, n)			strncpy(dest, src, n)
#define STRLEN(string)					lstrlenA(string)

///////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////

#define CLAMP_VAL(val, min, max)              { if ((val) < (min)){val = min;} else if ((val) > (max)) {val = max;} }

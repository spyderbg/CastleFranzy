#pragma once

#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <GL/gl.h>
#include <GL/glu.h>

typedef uint64_t oword;
typedef uint64_t dqword;
typedef uint64_t uint64;
typedef int64_t int64;
typedef double TimeStamp;
typedef uint64_t StreamSize;
typedef short Coord;
typedef wchar_t TChar;
typedef TChar TCHAR;

#define EMPTY_BODY {}
#define EMPTY_BODY_RET(x) { return (x); }
#define STRCHR(dest, src) strchr(dest, src)
#define STRNCPY(dest, src, n) strncpy(dest, src, n)
#define STRLEN(string) strlen(string)
#define CHECK_SHORT_LIMIT(x)
#define CHECK_LIMIT(x)
#define CHK_PRECISION(fResult, nResult, fTolerancePercentage)
#define CHK_ADD(x, y)
#define CHK_MUL(x, y)
#define CHK_ADD3(x1,x2,x3)
#define CHK_MUL2_ADD3(x1,x2,y1,y2,z1,z2)
#define CLAMP_VAL(val, min, max) { if ((val) < (min)) val = min; else if ((val) > (max)) val = max; }

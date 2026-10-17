#ifndef _GLT_MATH_FIXED_H_
#define _GLT_MATH_FIXED_H_

#if (PLATFORM==PLATFORM_MAC_OS_X) && (!defined(WIN32))
#include <OpenGLES/ES1/gl.h>
#elif IPHONE > 0
#include "../tools/rasteroid3.1/include/gles/gl.h"
#else
#include "gles/gl.h"
#endif

#include "gles/glt_device.h"

#define GLT_FIXED_SHIFT		16
#define GLT_FIXED_ONE		(1<<GLT_FIXED_SHIFT)

#define GLT_FIXED_TO_INT(a)     ((a) >> GLT_FIXED_SHIFT)
#define GLT_INT_TO_FIXED(a)		((a) << GLT_FIXED_SHIFT)
#define GLT_FLOAT_TO_FIXED(a)	((GLfixed)((a) * (GLT_FIXED_ONE)))

#define GLT_ANGLE_SHIFT		12							// fixed point 4 bit degres angles
#define GLT_ANGLE_2PI		(1<<GLT_ANGLE_SHIFT)		// OpengGL uses degres
#define GLT_ANGLE_PI		(1<<(GLT_ANGLE_SHIFT-1))		// OpengGL uses degres

#define GLT_ANGLE_MASK		(GLT_ANGLE_2PI-1)

#define GLT_PI_FLOAT		3.14159265358979323846f

extern const GLfixed		GLT_TSIN[GLT_ANGLE_2PI/4+1];		// sinus table

#define GLT_INVERSE_INT_MAX	1024
extern const GLfixed		GLT_INVERSE_INT[GLT_INVERSE_INT_MAX];	// small integer inverse table

// Inverse small fixed point integer  (between [1-511])
inline GLfixed	GLTInverseSmallIntegerNumberFixed( GLfixed val )
{
	GLT_ASSERT( (val>>GLT_FIXED_SHIFT) < GLT_INVERSE_INT_MAX );
	GLT_ASSERT( val >= 0 );
	return GLT_INVERSE_INT[ val >> GLT_FIXED_SHIFT ];
}

// Inverse small integer (between [1-511]), result is fixed point
inline GLfixed	GLTInverseSmallIntegerNumber( GLfixed val )
{
	GLT_ASSERT( val < GLT_INVERSE_INT_MAX );
	GLT_ASSERT( val >= 0 );
	return GLT_INVERSE_INT[ val ];
}


/*
// Multiply 2 fixed point number
inline GLfixed	GLTMul( GLfixed a, GLfixed b )
{
	// TBD check that the compiler uses the ARM SMULL opcode, otherwise code it in ASM !
	return (GLfixed)(((GLfixed64)a * (GLfixed64)b) >> GLT_FIXED_SHIFT);
}
*/

// Multiply 2 fixed point number
#ifdef WIN_DEBUG
inline GLfixed GLTMul( GLfixed a, GLfixed b )
{
	GLfixed64 ccc = ((((GLfixed64)(a)) * ((GLfixed64)(b))) >> GLT_FIXED_SHIFT);

    #ifdef WIN_DEBUG
	    GLfixed c = (GLfixed) ccc;
	    if( (GLfixed64)(c) != ccc )	//c overflow
	    {
		    GLT_ASSERT(!"GLT Multiply Overflow.");
	    }
    #endif

	return c;
}
#else
#define GLTMul( a, b ) ((GLfixed)((((GLfixed64)((GLfixed)a)) * ((GLfixed64)((GLfixed)b))) >> GLT_FIXED_SHIFT))
#endif




// Multiply 2 fixed point number and shift the result by GLT_FIXED_SHIFT (usefull for multiplication by a big number)
// used for multiplication by the inverse of a small number to optimize a division
inline GLfixed	GLTMulShift( GLfixed a, GLfixed b )	
{
	// TBD check that the compiler uses the ARM SMULL opcode, otherwise code it in ASM !
	return (GLfixed)(((GLfixed64)a * (GLfixed64)b) >> (2*GLT_FIXED_SHIFT));
}

// Interpolate between 2 fixed point numbers a & b, using a fixed point factor
inline GLfixed	GLTInterpolate( GLfixed a, GLfixed b, GLfixed alpha )
{
	return (a + GLTMul( (b - a), alpha ));
}

// Interpolate between 2 ARGB u8 a & b, using a fixed point factor (slow, only for clipping !)
inline GLfixed	GLTInterpolateARGB8888( u32 a, u32 b, GLfixed alpha )
{
	u32			res;

	const s32	colaa = a >> 24;
	const s32	colba = b >> 24;
	
	res = (colaa + (((colba - colaa) * alpha) >> GLT_FIXED_SHIFT)) << 24;

	const s32	colar = (a & 0xFF0000) >> 16;
	const s32	colbr = (b & 0xFF0000) >> 16;
	
	res |= (colar + (((colbr - colar) * alpha) >> GLT_FIXED_SHIFT)) << 16;

	const s32	colag = (a & 0xFF00) >> 8;
	const s32	colbg = (b & 0xFF00) >> 8;

	res |= (colag + (((colbg - colag) * alpha) >> GLT_FIXED_SHIFT)) << 8;

	const s32	colab = (a & 0xFF);
	const s32	colbb = (b & 0xFF);

	res |= (colab + (((colbb - colab) * alpha) >> GLT_FIXED_SHIFT));

	return (res);
}

// Divide 2 fixed point numbers
inline GLfixed GLTDiv( GLfixed a, GLfixed b )
{
	return (GLfixed)(((((GLfixed64)a) << 32)  / (GLfixed64)b) >> GLT_FIXED_SHIFT);
}

// Inverse a fixed point number
inline GLfixed GLTInv( GLfixed a )		{ return GLTDiv( GLT_FIXED_ONE, a ); }

// square root of a fixed point number
GLfixed GLTSqrt( GLfixed a );

inline GLfixed GLTAbs( GLfixed a )				{ return a > 0 ? a : -a; }
inline GLfixed GLTMax( GLfixed a, GLfixed b )	{ return a > b ? a : b; }
inline GLfixed GLTMin( GLfixed a, GLfixed b )	{ return a < b ? a : b; }


// Cos / Sin use Fixed Point Angles (2PI == GLT_ANGLE_2PI)

inline GLfixed GLTSin( GLfixed a )		
{ 
	a = a & GLT_ANGLE_MASK;

	if (a <= (GLT_ANGLE_2PI/4))
	{
		return GLT_TSIN[ a ];
	}
	else if (a <= (GLT_ANGLE_2PI/2))
	{
		return GLT_TSIN[(GLT_ANGLE_2PI/2) - a];
	}
	else if (a <= (GLT_ANGLE_2PI*3/4))
	{
		return -GLT_TSIN[a - (GLT_ANGLE_2PI/2)];
	}
	else
	{
		return -GLT_TSIN[GLT_ANGLE_2PI -1 - a];
	}
}

inline GLfixed GLTCos( GLfixed a )		
{ 
	return GLTSin( a + (GLT_ANGLE_2PI/4) );
}

inline bool GLTSameSign ( GLfixed a, GLfixed b )
{
	return ((a ^ b) > 0);
}

// Convert DEGRES to FIXED POINT ANGLES
inline GLfixed GLTDeg2FixedAngle( GLfixed angle )	{ return GLTMul(angle, (GLfixed)((GLT_ANGLE_2PI << GLT_FIXED_SHIFT) / 360) ) >> GLT_FIXED_SHIFT;	}



#endif

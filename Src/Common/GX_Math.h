// GX_Math.h
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _GX_MATH_H_
#define _GX_MATH_H_


#ifdef MIN
	#undef MIN
#endif

#ifdef MAX
	#undef MAX
#endif

#ifdef ABS
	#undef ABS
#endif

template <class T>
inline T max(T a, T b) { return a > b ? a : b; }
template <class T>
inline T min(T a, T b) { return a < b ? a : b; }

#define MIN(x,y)		min(x, y)
#define MAX(x,y)		max(x, y)
#define ABS(x)			(((x) < 0) ? -(x) : (x))
#define Abs(x)			ABS(x)
#define SGN(x)			(((x) > 0) ? 1 : -1)
#define SQR(x)			((x) * (x))
#define SQRT(x)			FSqrt(x)

////////////////////////////////////////////////////////////////////////////////////////////////////

#define MIN_INT			0x80000000
#define MIN_SHORT		0x8000

#define MAX_INT			0x7FFFFFFF
#define MAX_UINT		0xFFFFFFFF
#define MAX_SHORT		0x7FFF
#define MAX_USHORT		0xFFFF

////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLAMP(col, v0, v1)		{ if (col < v0) col = v0; else if (col > v1) col = v1; }
#define CLAMPED(col, v0, v1)	( (col) < (v0) ? (v0) : (col) > (v1) ? (v1) : (col) )

#define CLAMP_ZERO(col)		{ if (col < 0) col = 0; }
#define CLAMP_U8(col)		{ if (col > 255) col = 255; }

#define R(col)				((col) & COLOR_MASK_R)
#define CLAMP_R(col)		{ if (col > COLOR_MASK_R) col = COLOR_MASK_R; }

#define G(col)				((col) & COLOR_MASK_G)
#define CLAMP_G(col)		{ if (col > COLOR_MASK_G) col = COLOR_MASK_G; }

#define B(col)				((col) & COLOR_MASK_B)
#define CLAMP_B(col)		{ if (col > COLOR_MASK_B) col = COLOR_MASK_B; }

#define TRANSPARENT_LONG	0
#define IS_OPAQUE(x)		(x)	// returns true if x is an opaque color

#define CLAMP_LIGHT(x)      { if ((x) < 0) (x) = 0; }

#define CLAMP_DS_NORMAL(col) { if (col < -4096) {col = -4096;} else if (col > 4095) {col = 4095;}}

////////////////////////////////////////////////////////////////////////////////////////////////////

#define DownShift16(x)		(((x) + 0x7FFF) >> 16)
#define DownShift8(x)		(((x) + 0x007F) >>  8)

////////////////////////////////////////////////////////////////////////////////////////////////////
// ANGLES
// radians: FIXED POINT S7.12, 4096 = 2*PI

#define GX_ANGLE_SHIFT		12
#define GX_ANGLE_2PI		(1<<GX_ANGLE_SHIFT)		// 2*PI [radians]
#define GX_ANGLE_PI			(GX_ANGLE_2PI/2)		// PI [radians]
#define GX_ANGLE_MASK		(GX_ANGLE_2PI-1)		// angle mask use to access sin/cos table

#define GX_FLOAT_PI			3.14159265358979323846f				// floating point PI value
#define GX_DOUBLE_PI		3.1415926535897932384626433832795	// double floating point PI value

#define GX_DEG_2_ANGLE(x)	(((x) << GX_ANGLE_SHIFT) / 360)
#define GX_ANGLE_2_DEG(a)	(((a) * 360) >> GX_ANGLE_SHIFT)

#define GX_ANGLE_360		(GX_ANGLE_2PI)
#define GX_ANGLE_270		(GX_ANGLE_2PI*3/4)
#define GX_ANGLE_180		(GX_ANGLE_2PI/2)
#define GX_ANGLE_90			(GX_ANGLE_2PI/4)
#define GX_ANGLE_45			(GX_ANGLE_2PI/8)

////////////////////////////////////////////////////////////////////////////////////////////////////
// Sinus, Cosinus...
// input: ANGLE
// result: FIXED POINT S7.12, 4096 = 1.0

#define DIST_SHIFT					4
#define COS_SIN_SHIFT				12						// shifted 2^n value for sinus/cosinus result
#define COS_SIN_ONE					(1 << COS_SIN_SHIFT)	// sinus/cosinus max value (==1)
#define HALF_PRECISION				(COS_SIN_ONE/2)			// 0.5
#define MATH_ANGLE_SHIFT(d)			((d) >> COS_SIN_SHIFT)

#define GX_ROT_SHIFT	COS_SIN_SHIFT

#define HI_QUAT_PRECISION	15
#define HI_QUAT_ONE			(1 << HI_QUAT_PRECISION)
#define HI_QUAT_ONE_2		(HI_QUAT_ONE * HI_QUAT_ONE)
#define HI_QUAT_SHIFT		(HI_QUAT_PRECISION * 2 - COS_SIN_SHIFT)

#define MED_QUAT_PRECISION	14
#define MED_QUAT_ONE		(1 << MED_QUAT_PRECISION)
#define MED_QUAT_ONE_2		(MED_QUAT_ONE * MED_QUAT_ONE)
#define MED_QUAT_SHIFT		(MED_QUAT_PRECISION * 2 - COS_SIN_SHIFT)

#define LOW_QUAT_PRECISION	7
#define LOW_QUAT_ONE		(1 << LOW_QUAT_PRECISION)
#define LOW_QUAT_ONE_2		(LOW_QUAT_ONE * LOW_QUAT_ONE)
#define LOW_QUAT_SHIFT		(LOW_QUAT_PRECISION * 2 - COS_SIN_SHIFT)

extern const irr::s16 TSIN[GX_ANGLE_2PI]; // sinus table

irr::s16 Arcsin(irr::s32 a);
inline irr::s16 Sinus(irr::s32 a)		{ return TSIN[a & GX_ANGLE_MASK]; }
inline irr::s16 Cosinus(irr::s32 a)	{ return TSIN[(a + GX_ANGLE_90) & GX_ANGLE_MASK]; }

#define COSIN(angle) Cosinus(angle)
#define SINUS(angle) Sinus(angle)

#define COS(angle) Cosinus(angle)
#define SIN(angle) Sinus(angle)


/*
inline irr::s16 Sinus(irr::s32 a)
{
	double da = ((double)a  * (2.0 * GX_DOUBLE_PI)) / (double)GX_ANGLE_2PI;
	double s = sin(da) * (double)COS_SIN_ONE;
	int ss = (s < 0) ? (int)(s - 0.5) : (int)(s + 0.5);
//	assert(ss == TSIN[a & GX_ANGLE_MASK]);
	return ss;
}
inline irr::s16 Cosinus(irr::s32 a)
{
	double da = ((double)a  * (2.0 * GX_DOUBLE_PI)) / (double)GX_ANGLE_2PI;
	double c = cos(da) * (double)COS_SIN_ONE;
	int cc = (c < 0) ? (int)(c - 0.5) : (int)(c + 0.5);
//	assert(cc == TSIN[(a + (GX_ANGLE_2PI/4)) & GX_ANGLE_MASK]);
	return cc;
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////
// Position coordinates
// FIXED POINT S28.4, 64 = 1 unit

#define GX_POS_SHIFT	(4)
#define GX_POS_ONE		(1 << GX_POS_SHIFT)		// == 1 unit

#define NORMAL_SHIFT	14						//normal vectors precision
#define NORMAL_ONE		(1 << NORMAL_SHIFT)

////////////////////////////////////////////////////////////////////////////////////////////////////
// Multiply/Divide by 100... suitable for number less than 20bits
//#define	GX_MULTILPY_100(v)		((v << 6) + (v << 5) + (v << 2))
//#define	GX_DIVIDE_100(v)		(((v << 9) + (v << 7) + (v << 4)) >> 16)

//inline irr::s32 GX_MULTILPY_100(irr::s32 v)
//{
//	return ((v << 6) + (v << 5) + (v << 2));
//};

#define	GX_MULTILPY_100(v)		((v) * 100)

inline irr::s32 GX_DIVIDE_100(irr::s32 v)
{
	if (v >= 0)
		return ((v * 10486) >> 20);

	v = -v;
	return -((v * 10486) >> 20);

	//if (v >= 0)
	//	return (((v << 9) + (v << 7) + (v << 4)) >> 16);

	//v = -v;
	//return -(((v << 9) + (v << 7) + (v << 4)) >> 16);
};

// a, b : 256 == 1.0
inline irr::s32 FN_LINEAR(irr::s32 x, irr::s32 a, irr::s32 b)
{
	return (((a * x) + b) >> 8);
};
inline irr::s32 FN_LINEAR2(irr::s32 x, irr::s32 a, irr::s32 b, irr::s32 c)
{
	return (((a * x * x) + (b * x) + c) >> 8);
};
////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool IsPointInsideRect(int x, int y, int rx, int ry, int rw, int rh)
{
	return ( x >= rx && x < (rx+rw) && y >= ry && y < (ry+rh) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Fast square root with integers

irr::u32	FSqrt (irr::u32 n);
irr::u32 FSqrt4(irr::u32 n);		// sqrt(n)*16
irr::u32 FSqrt8(irr::u32 n);		// sqrt(n)*256

#define FSqrtI	FSqrt

//irr::u32 FSqrtI(irr::u32 n);		// with interpolation
//irr::u32 FSqrtI4(irr::u32 n);		// sqrt(n)*16
//irr::u32 FSqrtI8(irr::u32 n);		// sqrt(n)*256

irr::u32 FSqrtRec(irr::u32 n);	// recurrently

////////////////////////////////////////////////////////////////////////////////////////////////////
// ATAN

irr::s32 Atan2i(irr::s32 x, irr::s32 y);
irr::s32 AngleDiff(irr::s32 dx, irr::s32 dy, irr::s32 ref);
irr::s32 GetYOrient(const irr::s16* src, const irr::s16* dest);
irr::s32 GetXOrient(const irr::s16* src, const irr::s16* dest);

//irr::s32 GetFovFromXAngle(irr::s32 in_nXAngle);
//irr::s32 GetFovFromYAngle(irr::s32 in_nYAngle);

void GetQuaternionMed(irr::s16* quat, irr::s32 rx, irr::s32 ry, irr::s32 rz);

bool IsPointInside(irr::s32 x0, irr::s32 y0, irr::s32 x1, irr::s32 y1, irr::s32 x2, irr::s32 y2, irr::s32 x, irr::s32 y);
////////////////////////////////////////////////////////////////////////////////////////////////////


#endif // _GX_MATH_H_

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
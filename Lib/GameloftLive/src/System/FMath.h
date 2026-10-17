// Copyright (C) 2005 Gameloft. All rights reserved.
// GAMELOFT PROPRIETARY/CONFIDENTIAL.
//
// Author: Qiu Wei Min

#ifndef __FMATH_H__
#define __FMATH_H__

#include "GLLivePredefine.h"
/// @ingroup Global
/// @brief fixed point number with 12 bits fraction part and 20 bit integer part
typedef int F12;

namespace gllive
{
	enum {
		F12_SHIFT = 12,

		F12_ONE = 1 << F12_SHIFT,
		F12_MASK = F12_ONE - 1
	};

	inline F12 IntToF12(int i) {return i << F12_SHIFT;}
	inline int F12ToInt(F12 f) {return f >> F12_SHIFT;}

	/// @brief Utility class for numerical calculations.
	class CFMath
	{
	public:
		typedef struct	{
			int x, y;
		}Point2D;

		CFMath();
		virtual ~CFMath();

		/// @brief Calculates the approximate sine.
		/// @param[in]	angle	The angle in 4096ths of a circle (2PI radian = F12_ONE). Can be set to any signed integer.
		/// @return Returns the sine of the specified angle in F12
		static F12 Sin(F12 angle);

		/// @brief Calculates the approximate cosine.
		/// @param[in]	angle	The angle in 4096ths of a circle (2PI radian = F12_ONE). Can be set to any signed integer.
		/// @return Returns the cosine of the specified angle in F12
		static F12 Cos(F12 angle) {return Sin(angle + F12_ONE / 4);}
	#if PLATFORM == PLATFORM_KDDI

		static int searchInUpArray(TriValue value, const long *array, int len);
		static AngDegree atan(TriValue value);
		static AngDegree Atan2(PhType y, PhType x);
		static AngDegree Asin(TriValue value);

	#endif
		/// @brief Rotate a 2D point by a specified value.
		/// @param[out]	dst	The result of the rotation. Can be the same variable as src.
		/// @param[in]	src	The 2D point to be rotated.
		/// @param[in]	pivot	The pivot point of the rotation.
		/// @param[in]	angle	The angle in 4096ths of a circle (2PI radian = F12_ONE). Can be set to any signed integer.
		static void Rotate(Point2D &dst, const Point2D &src, const Point2D &pivot, F12 angle);

		int FastDiv(int Numerator, int Denominator);
		
		static void FloorDivMod(int Numerator, int Denominator, int &Floor, int &Mod);
		
		/// @brief Generates the next pseudorandom number.
		///
		/// This is a linear congruential pseudorandom number generator, as the one in java.util.Random.next(int bits).
		/// @param[in]	bits	random bits.
		/// @return the next pseudorandom value from this random number generator's sequence.
		unsigned int NextRandom(int bits);

		/// @brief Sets the seed of this random number generator using a single long seed.
		///
		/// The general contract of setSeed is that it alters the state of this random number
		/// generator object so as to be in exactly the same state as if it had just been created
		/// with the argument seed as a seed.
		/// @param[in]	seed	the initial seed.
		void SetRandomSeed(u64 seed);

	protected:
		static const unsigned short m_sinTable[];
		unsigned int m_recipTable[F12_ONE + 1];
		u64 m_randomSeed;
	};


	class Math
	{
	public:
	#if PLATFORM != PLATFORM_KDDI
		inline static int abs(int param)
		{
			return param>=0?param:-param;
		}
	#else
		inline static int myabs(int param)
		{
			return param>=0?param:-param;
		}
	#endif

		static int Random(int nMax);
		static int Random(int nMin,int nMax);
	#if PLATFORM == PLATFORM_KDDI
		static int _rand(int n);
		static int _rand(int min,int max);
		static int rand_int();
		static int next_int();
		static int next_int(int value);
		static double mysqrt(double value);
		static void Qsort(void* s, const int cnt, const int s_size, s32 _PtFuncCompare(const void*, const void*));
	#endif
	};
}

#endif




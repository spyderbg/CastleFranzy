/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/
/*
	Based on the 3dsMax COLLADA Tools:
	Copyright (C) 2005-2006 Autodesk Media Entertainment
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _CONVERSION_FUNCTIONS_
#define _CONVERSION_FUNCTIONS_

#ifndef __JAGTYPES__
#include "maxtypes.h"
#endif //__JAGTYPES__

#ifndef _FCD_ANIMATION_CURVE_H_
#include "FCDocument/FCDAnimationCurve.h"
#endif // _FCD_ANIMATION_CURVE_H_

//
// The purpose of this file is to provide a common interface for data
// being converted from COLLADA to MAX and back again.
// Common conversion functions should be defined in this class
//

// Add more as they are used?
//typedef float (*FConversionFunction)(float v);
//typedef FMVector3 (*V3ConversionFunction)(float FMVector3);
//typedef FMVector4 (*V4ConversionFunction)(float FMVector4);

// Conversion functions.
namespace FSConvert
{
	static FCDConversionScaleFunctor MaxTickToSecs(1.0f / TIME_TICKSPERSEC);
	static FCDConversionScaleFunctor MaxTickToFrames(1.0f / GetTicksPerFrame());
	static FCDConversionScaleFunctor MaxSecToTicks(TIME_TICKSPERSEC);
	static FCDConversionScaleFunctor DegToRad(FMath::DegToRad(1.0f));
	static FCDConversionScaleFunctor RadToDeg(FMath::RadToDeg(1.0f));
	static FCDConversionScaleFunctor ToPercent(100.0f);
	static FCDConversionScaleFunctor FromPercent(0.01f);
	static FCDConversionScaleFunctor PerFrameToPerSec(GetFrameRate());
	static FCDConversionScaleFunctor ToMaxGlossiness(0.01f);

	class FCDConversionDiffFunctor : public FCDConversionFunctor
	{
	private:
		float diff;
	public:
		FCDConversionDiffFunctor(float _diff) { diff = _diff; }
		virtual ~FCDConversionDiffFunctor() {}
		virtual float operator() (float v, float UNUSED(t) = 0) { return diff - v; }
	};


	class FCDConversionTruncateFunctor : public FCDConversionFunctor
	{
	public:
		FCDConversionTruncateFunctor() {}
		virtual ~FCDConversionTruncateFunctor() {}
		virtual float operator() (float v, float UNUSED(t) = 0) { return ( float )( int )v; }
	};

	class FCDConversionMaxToMayaUV : public FCDConversionFunctor
	{
	private:
		IParamBlock *m_pBlock;
		int m_paramId;
		float m_mirrorScale;
		bool m_bInvertCoord;

	public:
		/** Constructor. 
			@param factor This should be the UV scale/tile.
			@param isMirrored This should be non-zero if the U/V coordinates are mirrored */
		FCDConversionMaxToMayaUV( IParamBlock *pBlock, int paramId, uint32 isMirrored, bool bInvertCoord = false )
			: m_pBlock( pBlock )
			, m_paramId( paramId ) 
			, m_bInvertCoord( bInvertCoord )
			, m_mirrorScale( isMirrored ? 2 : 1 )
		{}

		virtual ~FCDConversionMaxToMayaUV() {} /**< Destructor. */

		/** Changes the input U/V offset from Max terms to Maya (COLLADA) values.  
			@param the U/V offset in Max terms. 
			@return the U/V offset in Maya terms. */
		virtual float operator() (float v, float t = 0)
		{
			int ticks = ( int )FSConvert::MaxSecToTicks( t );
			float scaleFactor = m_pBlock->GetFloat( m_paramId, ticks );
			float result = m_mirrorScale * (((1.0f + ( m_bInvertCoord ? scaleFactor : -scaleFactor )) / 2) - (v * scaleFactor));
			return m_bInvertCoord ? 1.f - result : result; 
		}
	};

	class FCDConversionMayaToMaxUV : public FCDConversionFunctor
	{
	private:
		float scaleFactor;
		float mirrorScale;

	public:
		 /** Constructor. 
			@param factor This should be the UV scale/tile.
			@param isMirrored This should be non-zero if the U/V coordinates are mirrored*/
		FCDConversionMayaToMaxUV(float factor, uint32 isMirrored) { scaleFactor = factor; mirrorScale = (isMirrored) ? 0.5f : 1; }
		virtual ~FCDConversionMayaToMaxUV() {} /**< Destructor. */

		/** Changes the input U/V offset from Maya (COLLADA) compatable to Max.  
			@param the U/V offset in Maya terms. 
			@return the U/V offset in Max terms. */
		virtual float operator() (float v, float UNUSED(t) = 0) { return (((1.0f - scaleFactor) / 2) - (v * mirrorScale)) / scaleFactor; }
	};

	// Float equivalency
	#define TOLERANCE 0.001f
	inline bool Equals(double f1, double f2) { return f1 - f2 < TOLERANCE && f1 - f2 > -TOLERANCE; }
	inline bool Equals(float f1, float f2) { return f1 - f2 < TOLERANCE && f1 - f2 > -TOLERANCE; }
	inline bool Equals(double f1, double f2, double tolerance) { return f1 - f2 < tolerance && f1 - f2 > -1.0 * tolerance; }

	static FCDConversionDiffFunctor ToMaxOpacity(1.0f);
	static FCDConversionOffsetFunctor AddTwo(2.0f);
	static FCDConversionTruncateFunctor Truncate;

	// Make sure the two eulers dont flip.
	extern void PatchEuler(float* pval, float* val);
};

#endif

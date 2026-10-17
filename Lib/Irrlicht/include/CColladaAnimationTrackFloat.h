#ifndef __CCOLLADAANIMATIONTRACKFLOAT_H__
#define __CCOLLADAANIMATIONTRACKFLOAT_H__

#include "CColladaAnimationTrack.h"

namespace irr
{
namespace collada
{
namespace animation_track
{

class CFloatEx
	: public collada::CAnimationTrackEx
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
// static - used in CWeight and CWeightEx
///////////////////////////////////////////////////////////////////////////////////////////////////
	static inline int getValueSizeEx()
	{
		return sizeof(float);
	}

	

	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
	{
		float *pInputs = (float *)pInputsArray;
		float &output = *(float *)pOutputPtr;
		output = 0;
		for(int i = 0; i < iSize; i++)
		{
			output += pInputs[i] * pWeightArray[i];
		}
	}

	

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr)
	{
		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
		float &output = *(float*)pOutputPtr;

		float &k0 = vWeight[iKey0];
		float &k1 = vWeight[iKey1];
		output = core::lerp(k0, k1, ratio);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr)
	{
		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
		float &output = *(float*)pOutputPtr;

		output = vWeight[iKey0];			
	}

	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr, 
		float blendOutWeight)
	{
		float &output = *(float *)pOutputPtr;
		float input = 0;
		getBlendedValueEx(pInputsArray, pWeightArray, iSize, &input);
		output = output * (1.0f - blendOutWeight) + input * blendOutWeight;
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr, 
		float blendOutWeight)
	{
		float &output = *(float *)pOutputPtr;
		float input = 0;
		getKeyBasedValueEx(animation, iKey0, iKey1, ratio, &input);
		output = output * (1.0f - blendOutWeight) + input * blendOutWeight;
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr, 
		float blendOutWeight)
	{
		float &output = *(float *)pOutputPtr;
		float input = 0;
		getKeyBasedValueEx(animation, iKey0, &input);
		output = output * (1.0f - blendOutWeight) + input * blendOutWeight;
	}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

public:
	virtual void applyValue(void *pDataPtr, void *pOutputPtr) const
	{
		memcpy(pOutputPtr, pDataPtr, getValueSize());
	}

	virtual int getValueSize() const
	{
		return getValueSizeEx();
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		getKeyBasedValueEx(animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr) const
	{
		getKeyBasedValueEx(animation, iKey0, pOutputPtr);
	}

	static const CFloatEx s_Instance;
};

class CFloat 
	: public CAnimationTrack
{
public:
	CFloat(collada::SAnimation &animation)
	: CAnimationTrack(animation)
	{
		
	}

	virtual int getValueSize() const
	{
		return CFloatEx::getValueSizeEx();
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		CFloatEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		CFloatEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		CFloatEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
	}

	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
	{
		return &CFloatEx::s_Instance;
	}

protected:

};

}; // namespace irr
}; // namespace collada
}; // namespace animation_track

#endif // __CCOLLADAANIMATIONTRACKFLOAT_H__
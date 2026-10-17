#ifndef __CCOLLADAANIMATIONTRACKWEIGHT_H__
#define __CCOLLADAANIMATIONTRACKWEIGHT_H__

#include "CColladaAnimationTrack.h"

namespace irr
{
namespace collada
{
namespace animation_track
{

class CWeightEx
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

	static inline int getWeightIndexEx(const collada::SAnimation &animation)
	{
		return animation.channels[0].reserved0;
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

	virtual void applyBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void applyKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		getKeyBasedValueEx(animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void applyKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr) const
	{
		getKeyBasedValueEx(animation, iKey0, pOutputPtr);
	}

	static const CWeightEx s_Instance;
};

class CWeight 
	: public CAnimationTrack
{
public:
	CWeight(collada::SAnimation &animation)
	: CAnimationTrack(animation)
	{
		
	}

	virtual int getValueSize() const
	{
		return CWeightEx::getValueSizeEx();
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		CWeightEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		CWeightEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		CWeightEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
	}

	virtual void applyBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		CWeightEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void applyKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		CWeightEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void applyKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		CWeightEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
	}

	int getWeightIndex()
	{
		return CWeightEx::getWeightIndexEx(m_Animation);
	}

	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
	{
		return &CWeightEx::s_Instance;
	}
};


}; // namespace irr
}; // namespace collada
}; // namespace animation_track

#endif // __CCOLLADAANIMATIONTRACKWEIGHT_H__
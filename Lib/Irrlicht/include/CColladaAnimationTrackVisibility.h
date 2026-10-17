#ifndef __CCOLLADAANIMATIONTRACKVISIBILITY_H__
#define __CCOLLADAANIMATIONTRACKVISIBILITY_H__

#include "CColladaAnimationTrack.h"
#include "ISceneNode.h"

namespace irr
{
namespace collada
{
namespace animation_track
{

class CVisibilityEx
	: public collada::CAnimationTrackEx
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
// static - used in CWeight and CWeightEx
///////////////////////////////////////////////////////////////////////////////////////////////////
	static inline int getValueSizeEx()
	{
		return sizeof(unsigned int);
	}

	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
	{
		unsigned int *pInputs = (unsigned int *)pInputsArray;
		unsigned int &output = *(unsigned int *)pOutputPtr;
		output = 1;
		for(int i = 0; i < iSize; i++)
		{
			if((pWeightArray[i] != 0) && (pInputs[i] == 0))
			{
				output = 0;
				break;
			}
		}
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr)
	{
		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
		unsigned int &output = *(unsigned int*)pOutputPtr;

		//unsigned int &k0 = (vWeight[iKey0 >> 5] >> (iKey0 & 31)) & 0x1;
		unsigned int k0 = (vWeight[iKey0] >= 1.0f);
		output = k0;
	}
	

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr)
	{
		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
		unsigned int &output = *(unsigned int*)pOutputPtr;

		//unsigned int &k0 = (vWeight[iKey0 >> 5] >> (iKey0 & 31)) & 0x1;
		unsigned int k0 = (vWeight[iKey0] >= 1.0f);
		output = k0;		
	}

	static inline void applyBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
	{
		unsigned int *pInputs = (unsigned int *)pInputsArray;
		applyValueEx(pOutputPtr, true);
		for(int i = 0; i < iSize; i++)
		{
			if((pWeightArray[i] != 0) && (pInputs[i] == 0))
			{
				applyValueEx(pOutputPtr, false);
				break;
			}
		}
	}

	static inline void applyKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr)
	{
		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
		applyValueEx(pOutputPtr, vWeight[iKey0] >= 1.0f);
	}
	

	static inline void applyKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr)
	{
		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
		applyValueEx(pOutputPtr, vWeight[iKey0] >= 1.0f);
	}

	static inline void applyValueEx(void *pOutputPtr, bool outputValue)
	{
		scene::ISceneNode &output = *(scene::ISceneNode*)pOutputPtr;
		output.setVisible(outputValue);
	}

	static inline void retrieveValueEx(void *pTarget, void * pValue)
	{
		*((bool*)pValue) = ((scene::ISceneNode*)pTarget)->getVisibility() != 0;
	}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

public:
	virtual void retrieveValue(void *pTarget, void * pValue) const
	{
		retrieveValueEx(pTarget, pValue);
	}

	virtual void applyValue(void *pDataPtr, void *pOutputPtr) const
	{
		applyValueEx(pOutputPtr, *(bool *)pDataPtr);
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
		applyBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void applyKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		applyKeyBasedValueEx(animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void applyKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr) const
	{
		applyKeyBasedValueEx(animation, iKey0, pOutputPtr);
	}

	static const CVisibilityEx s_Instance;
};

class CVisibility 
	: public CAnimationTrack
{
public:
	CVisibility(collada::SAnimation &animation)
	: CAnimationTrack(animation)
	{
		
	}

	virtual int getValueSize() const
	{
		return CVisibilityEx::getValueSizeEx();
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		CVisibilityEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		CVisibilityEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		CVisibilityEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
	}

	virtual void applyBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		CVisibilityEx::applyBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void applyKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		CVisibilityEx::applyKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void applyKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		CVisibilityEx::applyKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
	}

	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
	{
		return &CVisibilityEx::s_Instance;
	}

protected:

};

}; // namespace irr
}; // namespace collada
}; // namespace animation_track

#endif //__CCOLLADAANIMATIONTRACKVISIBILITY_H__
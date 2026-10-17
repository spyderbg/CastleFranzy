#ifndef __CCOLLADAANIMATIONTRACKVECTOR3D_H__
#define __CCOLLADAANIMATIONTRACKVECTOR3D_H__

#include "CColladaAnimationTrack.h"

namespace irr
{
namespace collada
{
namespace animation_track
{

class CVector3dEx
	: public collada::CAnimationTrackEx
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
// static - used in CQuaternion and CQuaternionEx
///////////////////////////////////////////////////////////////////////////////////////////////////
	static inline int getValueSizeEx()
	{
		return sizeof(core::vector3df);
	}

	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
	{
		core::vector3df* pVectors = (core::vector3df*) pInputsArray;
		core::vector3df &output = *(core::vector3df*)pOutputPtr;
		
		if(iSize > 2)
		{
			core::vector3df result = pVectors[0];
			float resultWeight = pWeightArray[0];
			for(int i = 1; i < iSize; i++)
			{
				core::vector3df &newPos = pVectors[i];

				resultWeight += pWeightArray[i];
				float ratio = pWeightArray[i] / resultWeight;
				result = core::lerp(result, newPos, ratio);
			}

			output = result;
		} else if(iSize == 2)
		{
			core::vector3df &k0 = pVectors[0];
			core::vector3df &k1 = pVectors[1];

			float ratio = pWeightArray[1] / (pWeightArray[0] + pWeightArray[1]);
			output = core::lerp(k0, k1, ratio);
		}
		else if(iSize == 1)
		{
			core::vector3df &k0 = pVectors[0];
			output = k0;
		}
		else
		{
			_IRR_DEBUG_BREAK_IF("Not implemented");
		}
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr)
	{
		SVectorTemplate<core::vector3df> &vSrt = *(SVectorTemplate<core::vector3df>*)&animation.getOutput();
		core::vector3df &output = *(core::vector3df*)pOutputPtr;

		core::vector3df &k0 = vSrt[iKey0];
		core::vector3df &k1 = vSrt[iKey1];
		output = core::lerp(k0, k1, ratio);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr)
	{
		SVectorTemplate<core::vector3df> &vSrt = *(SVectorTemplate<core::vector3df>*)&animation.getOutput();
		core::vector3df &output = *(core::vector3df*)pOutputPtr;

		output = vSrt[iKey0];	
	}

	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr, 
		float blendOutWeight)
	{
		core::vector3df &output = *(core::vector3df *)pOutputPtr;
		core::vector3df input;
		getBlendedValueEx(pInputsArray, pWeightArray, iSize, &input);
		output = output * (1.0f - blendOutWeight) + input * blendOutWeight;
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr, 
		float blendOutWeight)
	{
		core::vector3df &output = *(core::vector3df *)pOutputPtr;
		core::vector3df input;
		getKeyBasedValueEx(animation, iKey0, iKey1, ratio, &input);
		output = output * (1.0f - blendOutWeight) + input * blendOutWeight;
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr, 
		float blendOutWeight)
	{
		core::vector3df &output = *(core::vector3df *)pOutputPtr;
		core::vector3df input;
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

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr, 
		float blendOutWeight) const
	{
		getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr, blendOutWeight);
	}

	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr, 
		float blendOutWeight) const
	{
		getKeyBasedValueEx(animation, iKey0, iKey1, ratio, pOutputPtr, blendOutWeight);
	}

	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr, 
		float blendOutWeight) const
	{
		getKeyBasedValueEx(animation, iKey0, pOutputPtr, blendOutWeight);
	}

	static const CVector3dEx s_Instance;
protected:

};

class CVector3d
	: public CAnimationTrack
{
public:
	CVector3d(collada::SAnimation &animation)
	: CAnimationTrack(animation)
	{
		
	}

	virtual int getValueSize() const
	{
		return CVector3dEx::getValueSizeEx();
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		CVector3dEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		CVector3dEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		CVector3dEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
	}

	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
	{
		return &CVector3dEx::s_Instance;
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr, 
		float blendOutWeight) const
	{
		CVector3dEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr, blendOutWeight);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr, 
		float blendOutWeight) const
	{
		CVector3dEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr, blendOutWeight);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr, 
		float blendOutWeight) const
	{
		CVector3dEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr, blendOutWeight);
	}

};

}; // namespace irr
}; // namespace collada
}; // namespace animation_track

#endif // __CCOLLADAANIMATIONTRACKVECTOR3D_H__
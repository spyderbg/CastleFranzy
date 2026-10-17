#ifndef __CCOLLADAANIMATIONTRACKQUATERNION_H__
#define __CCOLLADAANIMATIONTRACKQUATERNION_H__

#include "CColladaAnimationTrack.h"

namespace irr
{
namespace collada
{
namespace animation_track
{

class CQuaternionEx 
	: public collada::CAnimationTrackEx
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
// static - used in CQuaternion and CQuaternionEx
///////////////////////////////////////////////////////////////////////////////////////////////////
	static inline int getValueSizeEx()
	{
		return sizeof(core::quaternion);
	}

	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
	{
		core::quaternion* pQuats = (core::quaternion*) pInputsArray;
		core::quaternion &output = *(core::quaternion*)pOutputPtr;

		if(iSize > 2)
		{
			core::quaternion result;
			float resultWeight;
			int i = 0;
			for(; i < iSize; i++)
			{
				if (pWeightArray[i] != 0) 
				{
					result = pQuats[i];
					resultWeight = pWeightArray[i];
					break;
				}
			}

			if(resultWeight == 1)
			{
				output = result;
				return;
			}

			for(; i < iSize; i++)
			{
				if (pWeightArray[i] != 0) 
				{
					resultWeight += pWeightArray[i];
					float ratio = pWeightArray[i] / resultWeight;
					result.slerp(result, pQuats[i], ratio);
				}
				
			}
			output = result;
		}
		else if(iSize == 2)
		{
			core::quaternion &k0 = pQuats[0];
			core::quaternion &k1 = pQuats[1];

			if(pWeightArray[1] == 0)
			{
				output = k0;
			}
			else if(pWeightArray[1] == 1)
			{
				output = k1;
			}
			else
			{
				float ratio = pWeightArray[1] / (pWeightArray[0] + pWeightArray[1]);
				output.slerp(k0, k1, ratio);
			}
		}
		else if(iSize == 1)
		{
			output = pQuats[0];
		}
		else
		{
			_IRR_DEBUG_BREAK_IF("Not implemented");
		}
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr)
	{
		SVectorTemplate<core::quaternion> &vSrt = *(SVectorTemplate<core::quaternion>*)&animation.getOutput();
		core::quaternion &output = *(core::quaternion*)pOutputPtr;

		core::quaternion &k0 = vSrt[iKey0];
		core::quaternion &k1 = vSrt[iKey1];
		output.slerp(k0, k1, ratio);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr)
	{
		SVectorTemplate<core::quaternion> &vSrt = *(SVectorTemplate<core::quaternion>*)&animation.getOutput();
		core::quaternion &output = *(core::quaternion*)pOutputPtr;

		output = vSrt[iKey0];	
	}

	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr, 
		float blendOutWeight)
	{
		core::quaternion &output = *(core::quaternion *)pOutputPtr;
		core::quaternion input;
		getBlendedValueEx(pInputsArray, pWeightArray, iSize, &input);
		output.slerp(output, input, blendOutWeight);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr, 
		float blendOutWeight)
	{
		core::quaternion &output = *(core::quaternion *)pOutputPtr;
		core::quaternion input;
		getKeyBasedValueEx(animation, iKey0, iKey1, ratio, &input);
		output.slerp(output, input, blendOutWeight);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr, 
		float blendOutWeight)
	{
		core::quaternion &output = *(core::quaternion *)pOutputPtr;
		core::quaternion input;
		getKeyBasedValueEx(animation, iKey0, &input);
		output.slerp(output, input, blendOutWeight);
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

	static const CQuaternionEx s_Instance;
protected:

};

class CQuaternion 
	: public CAnimationTrack
{
public:
	CQuaternion(collada::SAnimation &animation)
	: CAnimationTrack(animation)
	{
		
	}

	virtual int getValueSize() const
	{
		return CQuaternionEx::getValueSizeEx();
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		CQuaternionEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		CQuaternionEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		CQuaternionEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
	}

	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
	{
		return &CQuaternionEx::s_Instance;
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr, 
		float blendOutWeight) const
	{
		CQuaternionEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr, blendOutWeight);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr, 
		float blendOutWeight) const
	{
		CQuaternionEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr, blendOutWeight);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr, 
		float blendOutWeight) const
	{
		CQuaternionEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr, blendOutWeight);
	}
};




class CQuaternionAngleEx 
	: public collada::CAnimationTrackEx
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
// static - used in CQuaternion and CQuaternionEx
///////////////////////////////////////////////////////////////////////////////////////////////////
	static inline int getValueSizeEx()
	{
		return sizeof(core::quaternion);
	}

	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
	{
		CQuaternionEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr)
	{
		res::vector<float> &pAngles = *(res::vector<float>*)&animation.getOutput();
		core::quaternion &output = *(core::quaternion*)pOutputPtr;

		float &k0 = pAngles[iKey0];
		float &k1 = pAngles[iKey1];
		float outAngle = core::lerp(k0, k1, ratio);

		float angle;
		core::vector3df axe;
		output.toAngleAxis(angle, axe);
		output.fromAngleAxis(outAngle, axe);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr)
	{
		res::vector<float> &pAngles = *(res::vector<float>*)&animation.getOutput();
		core::quaternion &output = *(core::quaternion*)pOutputPtr;

		float outAngle = pAngles[iKey0];

		float angle;
		core::vector3df axe;
		output.toAngleAxis(angle, axe);
		output.fromAngleAxis(outAngle, axe);
	}

	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr, 
		float blendOutWeight)
	{
		core::quaternion &output = *(core::quaternion *)pOutputPtr;
		core::quaternion input;
		getBlendedValueEx(pInputsArray, pWeightArray, iSize, &input);
		output.slerp(output, input, blendOutWeight);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr, 
		float blendOutWeight)
	{
		core::quaternion &output = *(core::quaternion *)pOutputPtr;
		core::quaternion input;
		getKeyBasedValueEx(animation, iKey0, iKey1, ratio, &input);
		output.slerp(output, input, blendOutWeight);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr, 
		float blendOutWeight)
	{
		core::quaternion &output = *(core::quaternion *)pOutputPtr;
		core::quaternion input;
		getKeyBasedValueEx(animation, iKey0, &input);
		output.slerp(output, input, blendOutWeight);
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

	static const CQuaternionAngleEx s_Instance;
protected:

};

class CQuaternionAngle
	: public CAnimationTrack
{
public:
	CQuaternionAngle(collada::SAnimation &animation)
	: CAnimationTrack(animation)
	{
		
	}

	virtual int getValueSize() const
	{
		return CQuaternionAngleEx::getValueSizeEx();
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		CQuaternionAngleEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		CQuaternionAngleEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		CQuaternionAngleEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
	}

	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
	{
		return &CQuaternionAngleEx::s_Instance;
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr, 
		float blendOutWeight) const
	{
		CQuaternionAngleEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr, blendOutWeight);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr, 
		float blendOutWeight) const
	{
		CQuaternionAngleEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr, blendOutWeight);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr, 
		float blendOutWeight) const
	{
		CQuaternionAngleEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr, blendOutWeight);
	}

};

}; // namespace irr
}; // namespace collada
}; // namespace animation_track

#endif // __CCOLLADAANIMATIONTRACKQUATERNION_H__
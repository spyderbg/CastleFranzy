#ifndef __CCOLLADAANIMATIONTRACKPOSITION_H__
#define __CCOLLADAANIMATIONTRACKPOSITION_H__

#include "CColladaAnimationTrack.h"

namespace irr
{
namespace collada
{
namespace animation_track
{

class CPositionEx
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
				if (pWeightArray[i] != 0) {
					resultWeight += pWeightArray[i];
					float ratio = pWeightArray[i] / resultWeight;
					result = core::lerp(result, newPos, ratio);
				}
			}

			output = result;
		}
		else if(iSize == 2)
		{
			core::vector3df &k0 = pVectors[0];
			core::vector3df &k1 = pVectors[1];

			if (pWeightArray[0] == 0) 
			{
				output = k1;
			}
			else if(pWeightArray[1] == 0) 
			{
				output = k0;
			}
			else
			{
				float ratio = pWeightArray[1] / (pWeightArray[0] + pWeightArray[1]);
				output = core::lerp(k0, k1, ratio);
			}

			
		}
		else if(iSize == 1)
		{
			core::vector3df &k0 = pVectors[0];
			output = k0;
		}
		else
		{
		//	output = pVectors[0] * pWeightArray[0];
		//	for(int i = 1; i < iSize; i++)
		//	{
		//		output += pVectors[i] * pWeightArray[i];
		//	}
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

	static const CPositionEx s_Instance;
protected:

};

class CPosition
	: public CAnimationTrack
{
public:
	CPosition(collada::SAnimation &animation)
	: CAnimationTrack(animation)
	{
		
	}

	virtual int getValueSize() const
	{
		return CPositionEx::getValueSizeEx();
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		CPositionEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		CPositionEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		CPositionEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
	}

	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
	{
		return &CPositionEx::s_Instance;
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr, 
		float blendOutWeight) const
	{
		CPositionEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr, blendOutWeight);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr, 
		float blendOutWeight) const
	{
		CPositionEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr, blendOutWeight);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr, 
		float blendOutWeight) const
	{
		CPositionEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr, blendOutWeight);
	}

};



class CPositionXEx
	: public CPositionEx
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
// static - used in CPositionX and CPositionXEx
///////////////////////////////////////////////////////////////////////////////////////////////////
	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr)
	{
		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
		core::vector3df &output = *(core::vector3df*)pOutputPtr;

		float &k0 = vWeight[iKey0];
		float &k1 = vWeight[iKey1];
		output.X = core::lerp(k0, k1, ratio);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr)
	{
		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
		core::vector3df &output = *(core::vector3df*)pOutputPtr;

		output.X = vWeight[iKey0];	
	}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
public:
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

	static const CPositionXEx s_Instance;
protected:

};

class CPositionYEx
	: public CPositionEx
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
// static - used in CQuaternion and CQuaternionEx
///////////////////////////////////////////////////////////////////////////////////////////////////
	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr)
	{
		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
		core::vector3df &output = *(core::vector3df*)pOutputPtr;

		float &k0 = vWeight[iKey0];
		float &k1 = vWeight[iKey1];
		output.Y = core::lerp(k0, k1, ratio);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr)
	{
		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
		core::vector3df &output = *(core::vector3df*)pOutputPtr;

		output.Y = vWeight[iKey0];	
	}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
public:
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

	static const CPositionYEx s_Instance;
protected:

};

class CPositionZEx
	: public CPositionEx
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
// static - used in CQuaternion and CQuaternionEx
///////////////////////////////////////////////////////////////////////////////////////////////////
	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr)
	{
		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
		core::vector3df &output = *(core::vector3df*)pOutputPtr;

		float &k0 = vWeight[iKey0];
		float &k1 = vWeight[iKey1];
		output.Z = core::lerp(k0, k1, ratio);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr)
	{
		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
		core::vector3df &output = *(core::vector3df*)pOutputPtr;

		output.Z = vWeight[iKey0];	
	}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
public:
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

	static const CPositionZEx s_Instance;
protected:

};

class CPositionX
	: public CPosition
{
public:
	CPositionX(collada::SAnimation &animation)
	: CPosition(animation)
	{
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		CPositionXEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		CPositionXEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
	}

	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
	{
		return &CPositionXEx::s_Instance;
	}

};

class CPositionY
	: public CPosition
{
public:
	CPositionY(collada::SAnimation &animation)
	: CPosition(animation)
	{
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		CPositionYEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		CPositionYEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
	}

	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
	{
		return &CPositionYEx::s_Instance;
	}

};


class CPositionZ
	: public CPosition
{
public:
	CPositionZ(collada::SAnimation &animation)
	: CPosition(animation)
	{
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		CPositionZEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		CPositionZEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
	}

	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
	{
		return &CPositionZEx::s_Instance;
	}

};

}; // namespace irr
}; // namespace collada
}; // namespace animation_track

#endif // __CCOLLADAANIMATIONTRACKVECTOR3D_H__
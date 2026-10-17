#ifndef __CCOLLADAANIMATIONTRACKOFFSETU_H__
#define __CCOLLADAANIMATIONTRACKOFFSETU_H__

#include "CColladaAnimationTrack.h"
#include "SMaterial.h"

namespace irr
{
namespace collada
{
namespace animation_track
{

class CTextureTransformEx
	: public collada::CAnimationTrackEx
{
public:
	struct SData
	{
		SData()
			: OffsetU(0), OffsetV(0)
			, Rotate(0)
			, ScaleU(1), ScaleV(1) {}

		SData & operator * (float factor)
		{
			OffsetU *= factor;
			OffsetV *= factor;
			Rotate *= factor;
			ScaleU *= factor;
			ScaleV *= factor;
			return *this;
		}

		SData & operator += (const SData &other)
		{
			OffsetU += other.OffsetU;
			OffsetV += other.OffsetV;
			Rotate += other.Rotate;
			ScaleU += other.ScaleU;
			ScaleV += other.ScaleV;
			return *this;
		}

		float OffsetU, OffsetV;
		float Rotate;
		float ScaleU, ScaleV;
	};
///////////////////////////////////////////////////////////////////////////////////////////////////
// static - used in CQuaternion and CQuaternionEx
///////////////////////////////////////////////////////////////////////////////////////////////////

	static inline int getValueSizeEx()
	{
		return sizeof(SData);
	}

	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
	{
		SData *pInputs = (SData *)pInputsArray;
		SData outputValue;
		SData &output = *(SData*)pOutputPtr;
		for(int i = 0; i < iSize; i++)
		{
			outputValue += pInputs[i] * pWeightArray[i];
		}
		output = outputValue;
	}

	static inline void applyBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
	{
		SData *pInputs = (SData *)pInputsArray;
		SData outputValue;
		for(int i = 0; i < iSize; i++)
		{
			outputValue += pInputs[i] * pWeightArray[i];
		}

		applyValueEx(pOutputPtr, outputValue);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iTrack, int iKey0, int iKey1, float ratio, void *pOutputPtr)
	{
		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput(iTrack);
		float &output = *(float*)pOutputPtr;

		float &k0 = vWeight[iKey0];
		float &k1 = vWeight[iKey1];
		output = core::lerp(k0, k1, ratio);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iTrack, int iKey0, void *pOutputPtr)
	{
		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput(iTrack);
		float &output = *(float*)pOutputPtr;

		float &k0 = vWeight[iKey0];
		output = k0;
	}

	static inline void getValueEx(const collada::SAnimation &animation,
		s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate)
	{
		SData &data  = *(SData*)pOutputPtr;
		data = *(SData*)animation.pAnimationExtra;
		for(int iTrack = 0; iTrack < animation.channels.size(); iTrack++)
		{
			float outputValue;
			const res::vector<int> &input = animation.getKeyTime(iTrack);
			s32 frameNo = 0;															
			bInterpolate &= animation.findKeyFrameNo(iTrack, timeMs, frameNo);					
			if(bInterpolate && animation.getInterpolationType(iTrack) != SSampler::IT_STEP)	
			{																			
				s32 previousKeyTime = input[frameNo];									
				s32 nextKeyTime = input[frameNo + 1];									
				s32 deltaTime = nextKeyTime - previousKeyTime;							
				f32 ratio = (f32)(timeMs - previousKeyTime) / (f32)deltaTime;			
				ratio = core::clamp<f32>(ratio, 0, 1);		
				getKeyBasedValueEx(animation, iTrack, frameNo, frameNo + 1, ratio, &outputValue);												
			}																			
			else																		
			{																			
				getKeyBasedValueEx(animation, iTrack, frameNo, &outputValue);												
			}

			switch(animation.channels[iTrack].type & SChannel::CT_EFFECT_TRANSFORM_MASK)
			{
			case SChannel::CT_EFFECT_TRANSFORM_OFFSET_U & SChannel::CT_EFFECT_TRANSFORM_MASK:
				data.OffsetU = outputValue;
				break;
			case SChannel::CT_EFFECT_TRANSFORM_OFFSET_V & SChannel::CT_EFFECT_TRANSFORM_MASK:
				data.OffsetV = outputValue;
				break;
			case SChannel::CT_EFFECT_TRANSFORM_SCALE_U & SChannel::CT_EFFECT_TRANSFORM_MASK:
				data.ScaleU = outputValue;
				break;
			case SChannel::CT_EFFECT_TRANSFORM_SCALE_V & SChannel::CT_EFFECT_TRANSFORM_MASK:
				data.ScaleV = outputValue;
				break;
			case SChannel::CT_EFFECT_TRANSFORM_ROTATE & SChannel::CT_EFFECT_TRANSFORM_MASK:
				data.Rotate = outputValue;
				break;
			}
		}
	}

	static inline void  applyValueEx(const collada::SAnimation &animation,
		s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate)
	{
		SData data;
		getValueEx(animation, timeMs, &data, hintKeyLookUp, bInterpolate);

		applyValueEx(pOutputPtr, data);
	}

	static inline void applyValueEx(void *pOutputPtr, SData &outputValue)
	{
		video::SMaterial &output = *(video::SMaterial*)pOutputPtr;
		core::matrix4 transform;
		transform.buildTextureTransform(outputValue.Rotate / 180.0f * 3.141596f, core::vector2df(0.5, 0.5),
			 core::vector2df(outputValue.OffsetU, outputValue.OffsetV), 
			 core::vector2df(outputValue.ScaleU, outputValue.ScaleV));
		output.setTextureMatrix(0, transform);
	}

	static inline void retrieveValueEx(void *pTarget, void * pValue)
	{
		_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	}

public:
	virtual void retrieveValue(void *pTarget, void * pValue) const
	{
		retrieveValueEx(pTarget, pValue);
	}

	virtual void applyValue(void *pDataPtr, void *pOutputPtr) const
	{
		applyValueEx(pOutputPtr, *(SData *)pDataPtr);
	}

	virtual int getValueSize() const
	{
		return getValueSizeEx();
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void applyBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		applyBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
	}

	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr) const
	{
		_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
	}

	virtual void getValue(const SAnimation &animation, s32 timeMs, 
		void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate) const
	{
		getValueEx(animation, timeMs, pOutputPtr, hintKeyLookUp, bInterpolate);
	}

	static const CTextureTransformEx s_Instance;
protected:

};

class CTextureTransform
	: public CAnimationTrack
{
public:
	CTextureTransform(collada::SAnimation &animation)
	: CAnimationTrack(animation)
	{
		
	}

	virtual void getValue(s32 timeMs, void *pOutputPtr, bool bInterpolate) const
	{
		_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	}

	virtual void getValue(s32 timeMs, void *pOutputPtr, float blendOutWeight, bool bInterpolate) const
	{
		_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	}

	virtual void getValue(s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate) const
	{
		_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	}

	virtual void getValue(s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, float blendOutWeight, bool bInterpolate) const
	{
		_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	}


	virtual void applyKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	}

	virtual void applyKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	}
	
	virtual void  applyValue(s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate) const
	{
		CTextureTransformEx::applyValueEx(m_Animation, timeMs, pOutputPtr, hintKeyLookUp, bInterpolate);
	}

	virtual int getValueSize() const
	{
		return CTextureTransformEx::getValueSizeEx();
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		CTextureTransformEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	}

	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
	{
		return &CTextureTransformEx::s_Instance;
	}

};

//class COffsetUEx
//	: public collada::CAnimationTrackEx
//{
//public:
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// static - used in CQuaternion and CQuaternionEx
/////////////////////////////////////////////////////////////////////////////////////////////////////
//	static inline int getValueSizeEx()
//	{
//		return sizeof(float);
//	}
//
//	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
//	{
//		float *pInputs = (float *)pInputsArray;
//		float outputValue = 0;
//		float &output = *(float*)pOutputPtr;
//		for(int i = 0; i < iSize; i++)
//		{
//			outputValue += pInputs[i] * pWeightArray[i];
//		}
//		output = outputValue;
//	}
//
//	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, int iKey1, float ratio, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		float &output = *(float*)pOutputPtr;
//
//		float &k0 = vWeight[iKey0];
//		float &k1 = vWeight[iKey1];
//		float outputValue = core::lerp(k0, k1, ratio);
//		output = outputValue;
//	}
//
//	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		float &output = *(float*)pOutputPtr;
//
//		float &k0 = vWeight[iKey0];
//		output = k0;	
//	}
//
//	static inline void applyBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
//	{
//		float *pInputs = (float *)pInputsArray;
//		float outputValue = 0;
//		for(int i = 0; i < iSize; i++)
//		{
//			outputValue += pInputs[i] * pWeightArray[i];
//		}
//
//		applyValueEx(pOutputPtr, outputValue);
//	}
//
//	static inline void applyKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, int iKey1, float ratio, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//
//		float &k0 = vWeight[iKey0];
//		float &k1 = vWeight[iKey1];
//		float outputValue = core::lerp(k0, k1, ratio);
//
//		applyValueEx(pOutputPtr, outputValue);
//	}
//
//	static inline void applyKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		float &k0 = vWeight[iKey0];
//		applyValueEx(pOutputPtr, k0);
//	}
//
//	static inline void applyValueEx(void *pOutputPtr, float outputValue)
//	{
//		video::SMaterial &output = *(video::SMaterial*)pOutputPtr;
//		core::vector2df offset = output.getTextureOffset(0);
//		offset.X = outputValue;
//		output.setTextureOffset(0, offset);
//	}
//
//	static inline void retrieveValueEx(void *pTarget, void * pValue)
//	{
//		video::SMaterial &target = *(video::SMaterial*)pTarget;
//		core::vector2df offset = target.getTextureOffset(0);
//		*((float*)pValue) = offset.X;
//	}
//
//	///////////////////////////////////////////////////////////////////////////////////////////////////
//	///////////////////////////////////////////////////////////////////////////////////////////////////
//public:
//	virtual void retrieveValue(void *pTarget, void * pValue) const
//	{
//		retrieveValueEx(pTarget, pValue);
//	}
//
//	virtual void applyValue(void *pDataPtr, void *pOutputPtr) const
//	{
//		applyValueEx(pOutputPtr, *(float *)pDataPtr);
//	}
//
//	virtual int getValueSize() const
//	{
//		return getValueSizeEx();
//	}
//
//	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
//	{
//		getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
//	}
//
//	virtual void applyBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
//	{
//		applyBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
//		int iKey0, int iKey1, float ratio, void *pOutputPtr) const
//	{
//		getKeyBasedValueEx(animation, iKey0, iKey1, ratio, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
//		int iKey0, void *pOutputPtr) const
//	{
//		getKeyBasedValueEx(animation, iKey0, pOutputPtr);
//	}
//
//	static const COffsetUEx s_Instance;
//protected:
//
//};
//
//class COffsetU 
//	: public CAnimationTrack
//{
//public:
//	COffsetU(collada::SAnimation &animation)
//	: CAnimationTrack(animation)
//	{
//		
//	}
//
//	virtual void applyKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
//	{
//		COffsetUEx::applyKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
//	}
//
//	virtual void applyKeyBasedValue(int iKey0, void *pOutputPtr) const
//	{
//		COffsetUEx::applyKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
//	}
//
//	virtual int getValueSize() const
//	{
//		return COffsetUEx::getValueSizeEx();
//	}
//
//	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
//	{
//		COffsetUEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
//	{
//		COffsetUEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
//	{
//		COffsetUEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
//	}
//
//	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
//	{
//		return &COffsetUEx::s_Instance;
//	}
//
//};
//
//class COffsetVEx
//	: public collada::CAnimationTrackEx
//{
//public:
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// static - used in CQuaternion and CQuaternionEx
/////////////////////////////////////////////////////////////////////////////////////////////////////
//	static inline int getValueSizeEx()
//	{
//		return sizeof(float);
//	}
//
//	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
//	{
//		float *pInputs = (float *)pInputsArray;
//		float outputValue = 0;
//		float &output = *(float*)pOutputPtr;
//		for(int i = 0; i < iSize; i++)
//		{
//			outputValue += pInputs[i] * pWeightArray[i];
//		}
//		output = outputValue;
//	}
//
//	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, int iKey1, float ratio, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		float &output = *(float*)pOutputPtr;
//
//		float &k0 = vWeight[iKey0];
//		float &k1 = vWeight[iKey1];
//		float outputValue = core::lerp(k0, k1, ratio);
//		output = outputValue;
//	}
//
//	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		float &output = *(float*)pOutputPtr;
//
//		float &k0 = vWeight[iKey0];
//		output = k0;	
//	}
//
//	static inline void applyBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
//	{
//		float *pInputs = (float *)pInputsArray;
//		float outputValue = 0;
//		for(int i = 0; i < iSize; i++)
//		{
//			outputValue += pInputs[i] * pWeightArray[i];
//		}
//
//		applyValueEx(pOutputPtr, outputValue);
//	}
//
//	static inline void applyKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, int iKey1, float ratio, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//
//		float &k0 = vWeight[iKey0];
//		float &k1 = vWeight[iKey1];
//		float outputValue = core::lerp(k0, k1, ratio);
//		applyValueEx(pOutputPtr, outputValue);	
//	}
//
//	static inline void applyKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		float &k0 = vWeight[iKey0];
//		applyValueEx(pOutputPtr, k0);	
//	}
//
//	static inline void applyValueEx(void *pOutputPtr, float outputValue)
//	{
//		video::SMaterial &output = *(video::SMaterial*)pOutputPtr;
//		core::vector2df offset = output.getTextureOffset(0);
//		offset.Y = outputValue;
//		output.setTextureOffset(0, offset);
//	}
//
//	static inline void retrieveValueEx(void *pTarget, void * pValue)
//	{
//		video::SMaterial &target = *(video::SMaterial*)pTarget;
//		core::vector2df offset = target.getTextureOffset(0);
//		*((float*)pValue) = offset.Y;
//	}
//	///////////////////////////////////////////////////////////////////////////////////////////////////
//	///////////////////////////////////////////////////////////////////////////////////////////////////
//public:
//	virtual void retrieveValue(void *pTarget, void * pValue) const
//	{
//		retrieveValueEx(pTarget, pValue);
//	}
//
//	virtual void applyValue(void *pDataPtr, void *pOutputPtr) const
//	{
//		applyValueEx(pOutputPtr, *(float *)pDataPtr);
//	}
//
//	virtual int getValueSize() const
//	{
//		return getValueSizeEx();
//	}
//
//	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
//	{
//		getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
//	}
//
//	virtual void applyBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
//	{
//		applyBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
//		int iKey0, int iKey1, float ratio, void *pOutputPtr) const
//	{
//		getKeyBasedValueEx(animation, iKey0, iKey1, ratio, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
//		int iKey0, void *pOutputPtr) const
//	{
//		getKeyBasedValueEx(animation, iKey0, pOutputPtr);
//	}
//
//	static const COffsetVEx s_Instance;
//protected:
//
//};
//
//class COffsetV 
//	: public CAnimationTrack
//{
//public:
//	COffsetV(collada::SAnimation &animation)
//	: CAnimationTrack(animation)
//	{
//		
//	}
//
//	virtual void applyKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
//	{
//		COffsetVEx::applyKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
//	}
//
//	virtual void applyKeyBasedValue(int iKey0, void *pOutputPtr) const
//	{
//		COffsetVEx::applyKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
//	}
//
//	virtual int getValueSize() const
//	{
//		return COffsetVEx::getValueSizeEx();
//	}
//
//	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
//	{
//		COffsetVEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
//	{
//		COffsetVEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
//	{
//		COffsetVEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
//	}
//
//	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
//	{
//		return &COffsetVEx::s_Instance;
//	}
//
//};
//
//class CRotateUVEx
//	: public collada::CAnimationTrackEx
//{
//public:
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// static - used in CQuaternion and CQuaternionEx
/////////////////////////////////////////////////////////////////////////////////////////////////////
//	static inline int getValueSizeEx()
//	{
//		return sizeof(float);
//	}
//
//	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
//	{
//		float *pInputs = (float *)pInputsArray;
//		float outputValue = 0;
//		float &output = *(float*)pOutputPtr;
//		for(int i = 0; i < iSize; i++)
//		{
//			outputValue += pInputs[i] * pWeightArray[i];
//		}
//		output = outputValue;
//	}
//
//	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, int iKey1, float ratio, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		float &output = *(float*)pOutputPtr;
//
//		float &k0 = vWeight[iKey0];
//		float &k1 = vWeight[iKey1];
//		float outputValue = core::lerp(k0, k1, ratio);
//		output = outputValue;
//	}
//
//	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		float &output = *(float*)pOutputPtr;
//
//		float &k0 = vWeight[iKey0];
//		output = k0;	
//	}
//
//	static inline void applyBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
//	{
//		float *pInputs = (float *)pInputsArray;
//		float outputValue = 0;
//		for(int i = 0; i < iSize; i++)
//		{
//			outputValue += pInputs[i] * pWeightArray[i];
//		}
//
//		core::matrix4 &output = *(core::matrix4*)pOutputPtr;
//		applyValueEx(&output, outputValue);
//	}
//
//	static inline void applyKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, int iKey1, float ratio, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		core::matrix4 &output = *(core::matrix4*)pOutputPtr;
//
//		float &k0 = vWeight[iKey0];
//		float &k1 = vWeight[iKey1];
//		float outputValue = core::lerp(k0, k1, ratio);
//		applyValueEx(&output, outputValue);
//	}
//
//	static inline void applyKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		core::matrix4 &output = *(core::matrix4*)pOutputPtr;
//
//		float &k0 = vWeight[iKey0];
//		applyValueEx(&output, k0);
//	}
//
//	static inline void applyValueEx(void *pOutputPtr, float outputValue)
//	{
//		video::SMaterial &output = *(video::SMaterial*)pOutputPtr;
//		output.setTextureRotation(0, outputValue);
//		/*core::matrix4 &output = *(core::matrix4*)pOutputPtr;
//		outputValue = (outputValue / 180.0f) * 3.141596f;
//		float cv = cos(outputValue);
//		float sv = sin(outputValue);
//		output(0, 0) = cv;
//		output(1, 1) = cv;
//		output(0, 1) = -sv;
//		output(1, 0) = sv;
//		output(2, 0) = 0.5f * ( 1 - cv - sv );
//		output(2, 1) = 0.5f * ( 1 - cv + sv );*/
//	}
//
//	static inline void retrieveValueEx(void *pTarget, void * pValue)
//	{
//		video::SMaterial &target = *(video::SMaterial*)pTarget;
//		*((float*)pValue) = target.getTextureRotation(0);
//	}
//	///////////////////////////////////////////////////////////////////////////////////////////////////
//	///////////////////////////////////////////////////////////////////////////////////////////////////
//public:
//	virtual void retrieveValue(void *pTarget, void * pValue) const
//	{
//		retrieveValueEx(pTarget, pValue);
//	}
//
//	virtual void applyValue(void *pDataPtr, void *pOutputPtr) const
//	{
//		applyValueEx(pOutputPtr, *(float *)pDataPtr);
//	}
//
//	virtual int getValueSize() const
//	{
//		return getValueSizeEx();
//	}
//
//	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
//	{
//		getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
//	}
//
//	virtual void applyBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
//	{
//		applyBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
//		int iKey0, int iKey1, float ratio, void *pOutputPtr) const
//	{
//		getKeyBasedValueEx(animation, iKey0, iKey1, ratio, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
//		int iKey0, void *pOutputPtr) const
//	{
//		getKeyBasedValueEx(animation, iKey0, pOutputPtr);
//	}
//
//	static const CRotateUVEx s_Instance;
//protected:
//
//};
//
//class CRotateUV 
//	: public CAnimationTrack
//{
//public:
//	CRotateUV(collada::SAnimation &animation)
//	: CAnimationTrack(animation)
//	{
//		
//	}
//
//	virtual void applyKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
//	{
//		CRotateUVEx::applyKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
//	}
//
//	virtual void applyKeyBasedValue(int iKey0, void *pOutputPtr) const
//	{
//		CRotateUVEx::applyKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
//	}
//
//	virtual int getValueSize() const
//	{
//		return CRotateUVEx::getValueSizeEx();
//	}
//
//	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
//	{
//		CRotateUVEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
//	{
//		CRotateUVEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
//	{
//		CRotateUVEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
//	}
//
//	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
//	{
//		return &CRotateUVEx::s_Instance;
//	}
//
//};
//
//class CScaleUEx
//	: public collada::CAnimationTrackEx
//{
//public:
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// static - used in CQuaternion and CQuaternionEx
/////////////////////////////////////////////////////////////////////////////////////////////////////
//	static inline int getValueSizeEx()
//	{
//		return sizeof(float);
//	}
//
//	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
//	{
//		float *pInputs = (float *)pInputsArray;
//		float outputValue = 0;
//		float &output = *(float*)pOutputPtr;
//		for(int i = 0; i < iSize; i++)
//		{
//			outputValue += pInputs[i] * pWeightArray[i];
//		}
//		output = outputValue;
//	}
//
//	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, int iKey1, float ratio, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		float &output = *(float*)pOutputPtr;
//
//		float &k0 = vWeight[iKey0];
//		float &k1 = vWeight[iKey1];
//		float outputValue = core::lerp(k0, k1, ratio);
//		output = outputValue;
//	}
//
//	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		float &output = *(float*)pOutputPtr;
//
//		float &k0 = vWeight[iKey0];
//		output = k0;	
//	}
//
//	static inline void applyBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
//	{
//		float *pInputs = (float *)pInputsArray;
//		float outputValue = 0;
//		for(int i = 0; i < iSize; i++)
//		{
//			outputValue += pInputs[i] * pWeightArray[i];
//		}
//
//		applyValueEx(pOutputPtr, outputValue);
//	}
//
//	static inline void applyKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, int iKey1, float ratio, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//
//		float &k0 = vWeight[iKey0];
//		float &k1 = vWeight[iKey1];
//		float outputValue = core::lerp(k0, k1, ratio);
//
//		applyValueEx(pOutputPtr, outputValue);
//	}
//
//	static inline void applyKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		float &k0 = vWeight[iKey0];
//		applyValueEx(pOutputPtr, k0);
//	}
//
//	static inline void applyValueEx(void *pOutputPtr, float outputValue)
//	{
//		video::SMaterial &output = *(video::SMaterial*)pOutputPtr;
//		core::vector2df offset = output.getTextureScale(0);
//		offset.X = outputValue;
//		output.setTextureScale(0, offset);
//	}
//
//	static inline void retrieveValueEx(void *pTarget, void * pValue)
//	{
//		video::SMaterial &target = *(video::SMaterial*)pTarget;
//		core::vector2df offset = target.getTextureScale(0);
//		*((float*)pValue) = offset.X;
//	}
//
//	///////////////////////////////////////////////////////////////////////////////////////////////////
//	///////////////////////////////////////////////////////////////////////////////////////////////////
//public:
//	virtual void retrieveValue(void *pTarget, void * pValue) const
//	{
//		retrieveValueEx(pTarget, pValue);
//	}
//
//	virtual void applyValue(void *pDataPtr, void *pOutputPtr) const
//	{
//		applyValueEx(pOutputPtr, *(float *)pDataPtr);
//	}
//
//	virtual int getValueSize() const
//	{
//		return getValueSizeEx();
//	}
//
//	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
//	{
//		getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
//	}
//
//	virtual void applyBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
//	{
//		applyBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
//		int iKey0, int iKey1, float ratio, void *pOutputPtr) const
//	{
//		getKeyBasedValueEx(animation, iKey0, iKey1, ratio, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
//		int iKey0, void *pOutputPtr) const
//	{
//		getKeyBasedValueEx(animation, iKey0, pOutputPtr);
//	}
//
//	static const CScaleUEx s_Instance;
//protected:
//
//};
//
//class CScaleU 
//	: public CAnimationTrack
//{
//public:
//	CScaleU(collada::SAnimation &animation)
//	: CAnimationTrack(animation)
//	{
//		
//	}
//
//	virtual void applyKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
//	{
//		CScaleUEx::applyKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
//	}
//
//	virtual void applyKeyBasedValue(int iKey0, void *pOutputPtr) const
//	{
//		CScaleUEx::applyKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
//	}
//
//	virtual int getValueSize() const
//	{
//		return CScaleUEx::getValueSizeEx();
//	}
//
//	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
//	{
//		CScaleUEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
//	{
//		CScaleUEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
//	{
//		CScaleUEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
//	}
//
//	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
//	{
//		return &CScaleUEx::s_Instance;
//	}
//
//};
//
//class CScaleVEx
//	: public collada::CAnimationTrackEx
//{
//public:
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// static - used in CQuaternion and CQuaternionEx
/////////////////////////////////////////////////////////////////////////////////////////////////////
//	static inline int getValueSizeEx()
//	{
//		return sizeof(float);
//	}
//
//	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
//	{
//		float *pInputs = (float *)pInputsArray;
//		float outputValue = 0;
//		float &output = *(float*)pOutputPtr;
//		for(int i = 0; i < iSize; i++)
//		{
//			outputValue += pInputs[i] * pWeightArray[i];
//		}
//		output = outputValue;
//	}
//
//	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, int iKey1, float ratio, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		float &output = *(float*)pOutputPtr;
//
//		float &k0 = vWeight[iKey0];
//		float &k1 = vWeight[iKey1];
//		float outputValue = core::lerp(k0, k1, ratio);
//		output = outputValue;
//	}
//
//	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		float &output = *(float*)pOutputPtr;
//
//		float &k0 = vWeight[iKey0];
//		output = k0;	
//	}
//
//	static inline void applyBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
//	{
//		float *pInputs = (float *)pInputsArray;
//		float outputValue = 0;
//		for(int i = 0; i < iSize; i++)
//		{
//			outputValue += pInputs[i] * pWeightArray[i];
//		}
//
//		applyValueEx(pOutputPtr, outputValue);
//	}
//
//	static inline void applyKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, int iKey1, float ratio, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//
//		float &k0 = vWeight[iKey0];
//		float &k1 = vWeight[iKey1];
//		float outputValue = core::lerp(k0, k1, ratio);
//		applyValueEx(pOutputPtr, outputValue);	
//	}
//
//	static inline void applyKeyBasedValueEx(const collada::SAnimation &animation, 
//		int iKey0, void *pOutputPtr)
//	{
//		SVectorTemplate<float> &vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
//		float &k0 = vWeight[iKey0];
//		applyValueEx(pOutputPtr, k0);	
//	}
//
//	static inline void applyValueEx(void *pOutputPtr, float outputValue)
//	{
//		video::SMaterial &output = *(video::SMaterial*)pOutputPtr;
//		core::vector2df offset = output.getTextureScale(0);
//		offset.Y = outputValue;
//		output.setTextureScale(0, offset);
//	}
//
//	static inline void retrieveValueEx(void *pTarget, void * pValue)
//	{
//		video::SMaterial &target = *(video::SMaterial*)pTarget;
//		core::vector2df offset = target.getTextureScale(0);
//		*((float*)pValue) = offset.Y;
//	}
//	///////////////////////////////////////////////////////////////////////////////////////////////////
//	///////////////////////////////////////////////////////////////////////////////////////////////////
//public:
//	virtual void retrieveValue(void *pTarget, void * pValue) const
//	{
//		retrieveValueEx(pTarget, pValue);
//	}
//
//	virtual void applyValue(void *pDataPtr, void *pOutputPtr) const
//	{
//		applyValueEx(pOutputPtr, *(float *)pDataPtr);
//	}
//
//	virtual int getValueSize() const
//	{
//		return getValueSizeEx();
//	}
//
//	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
//	{
//		getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
//	}
//
//	virtual void applyBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
//	{
//		applyBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
//		int iKey0, int iKey1, float ratio, void *pOutputPtr) const
//	{
//		getKeyBasedValueEx(animation, iKey0, iKey1, ratio, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
//		int iKey0, void *pOutputPtr) const
//	{
//		getKeyBasedValueEx(animation, iKey0, pOutputPtr);
//	}
//
//	static const CScaleVEx s_Instance;
//protected:
//
//};
//
//class CScaleV 
//	: public CAnimationTrack
//{
//public:
//	CScaleV(collada::SAnimation &animation)
//	: CAnimationTrack(animation)
//	{
//		
//	}
//
//	virtual void applyKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
//	{
//		CScaleVEx::applyKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
//	}
//
//	virtual void applyKeyBasedValue(int iKey0, void *pOutputPtr) const
//	{
//		CScaleVEx::applyKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
//	}
//
//	virtual int getValueSize() const
//	{
//		return CScaleVEx::getValueSizeEx();
//	}
//
//	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
//	{
//		CScaleVEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
//	{
//		CScaleVEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
//	}
//
//	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
//	{
//		CScaleVEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
//	}
//
//	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
//	{
//		return &CScaleVEx::s_Instance;
//	}
//
//};

}; // namespace irr
}; // namespace collada
}; // namespace animation_track

#endif // __CCOLLADAANIMATIONTRACKWEIGHT_H__

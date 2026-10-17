//-*-c++-*-
#ifndef __CCOLLADAANIMATIONTRACKTRANSPARENCY_H__
#define __CCOLLADAANIMATIONTRACKTRANSPARENCY_H__

#include "CColladaAnimationTrack.h"
#include "SMaterial.h"

namespace irr
{
namespace collada
{
namespace animation_track
{

class CTransparencyEx
	: public collada::CAnimationTrackEx
{
public:

///////////////////////////////////////////////////////////////////////////////////////////////////
// static - used in CWeight and CWeightEx
///////////////////////////////////////////////////////////////////////////////////////////////////
	static inline int getValueSizeEx()
	{
		return sizeof(video::SMaterial);
	}

	static inline void getBlendedValueEx(void* inputsArray,
										 float* weightArray,
										 int size,
										 void* outputPtr)
	{
		_IRR_DEBUG_BREAK_IF("Not implemented");
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation& animation, 
										  int key0,
										  int key1,
										  float ratio,
										  void* outputPtr)
	{
		SVectorTemplate<float>& vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
		video::SMaterial& output = *(video::SMaterial*)outputPtr;

		float& k0 = vWeight[key0];
		float& k1 = vWeight[key1];
		unsigned char value = (unsigned char)(core::lerp(k0, k1, ratio) * 255);
		output.setDiffuseAlpha(value);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation& animation, 
										  int key0,
										  void* outputPtr)
	{
		SVectorTemplate<float>& vWeight = *(SVectorTemplate<float>*)&animation.getOutput();
		video::SMaterial& output = *(video::SMaterial*)outputPtr;

		output.setDiffuseAlpha(u32(vWeight[key0] * 255));
	}

	static inline void getBlendedValueEx(void* inputsArray,
										 float* weightArray,
										 int size,
										 void* outputPtr, 
										 float blendOutWeight)
	{
		_IRR_DEBUG_BREAK_IF("Not implemented");
		/*float& output = *(float*)outputPtr;
		float input = 0;
		getBlendedValueEx(inputsArray, weightArray, size, &input);
		output = output * (1.0f - blendOutWeight) + input * blendOutWeight;*/
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation& animation, 
										  int key0,
										  int key1,
										  float ratio,
										  void* outputPtr, 
										  float blendOutWeight)
	{
		_IRR_DEBUG_BREAK_IF("Not implemented");
		/*float& output = *(float*)outputPtr;
		float input = 0;
		getKeyBasedValueEx(animation, key0, key1, ratio, &input);
		output = output * (1.0f - blendOutWeight) + input * blendOutWeight;*/
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation& animation, 
										  int key0,
										  void* outputPtr, 
										  float blendOutWeight)
	{
		_IRR_DEBUG_BREAK_IF("Not implemented");
		/*float& output = *(float*)outputPtr;
		float input = 0;
		getKeyBasedValueEx(animation, key0, &input);
		output = output * (1.0f - blendOutWeight) + input * blendOutWeight;*/
	}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

public:

	virtual void applyValue(void* dataPtr, void* outputPtr) const;

	virtual int getValueSize() const;
	virtual void getBlendedValue(void* inputsArray,
								 float* weightArray,
								 int size,
								 void* outputPtr) const;
	virtual void getKeyBasedValue(const collada::SAnimation& animation, 
								  int key0,
								  int key1,
								  float ratio,
								  void* outputPtr) const;
	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
								  int key0,
								  void* outputPtr) const;

	static const CTransparencyEx Instance;
};

class CTransparency 
	: public CAnimationTrack
{
public:

	CTransparency(collada::SAnimation& animation)
		: CAnimationTrack(animation)
	{
		
	}

	virtual int getValueSize() const;
	virtual void getBlendedValue(void* inputsArray,
								 float* weightArray,
								 int size,
								 void* outputPtr) const;
	virtual void getKeyBasedValue(int key0,
								  int key1,
								  float ratio,
								  void* outputPtr) const;
	virtual void getKeyBasedValue(int key0, void* outputPtr) const;
	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const;

};

} // namespace animation_track
} // namespace collada
} // namespace irr

#endif // __CCOLLADAANIMATIONTRACKFLOAT_H__

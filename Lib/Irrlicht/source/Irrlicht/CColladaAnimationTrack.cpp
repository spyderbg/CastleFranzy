#include "stdafx.h"
#include "CColladaAnimationTrack.h"

namespace irr
{
namespace collada
{
#define MANIP_VALUE_BODY(animation, interpolation_call, noInterpolation_call)		\
		const res::vector<int> &input = animation.getKeyTime();						\
		s32 frameNo = 0;															\
		bInterpolate &= animation.findKeyFrameNo(0, timeMs, frameNo);				\
		if(bInterpolate && animation.getInterpolationType() != SSampler::IT_STEP)	\
		{																			\
			s32 previousKeyTime = input[frameNo];									\
			s32 nextKeyTime = input[frameNo + 1];									\
			s32 deltaTime = nextKeyTime - previousKeyTime;							\
			f32 ratio = (f32)(timeMs - previousKeyTime) / (f32)deltaTime;			\
			ratio = core::clamp<f32>(ratio, 0, 1);									\
			interpolation_call;														\
		}																			\
		else																		\
		{																			\
			noInterpolation_call;													\
		}

void
CAnimationTrackEx::getValue(const SAnimation &animation, s32 timeMs, void *pOutputPtr, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		animation,
		getKeyBasedValue(animation, frameNo, frameNo + 1, ratio, pOutputPtr),
		getKeyBasedValue(animation, frameNo, pOutputPtr));
}

void
CAnimationTrackEx::getValue(const SAnimation &animation, s32 timeMs, void *pOutputPtr, float blendOutWeight, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		animation,
		getKeyBasedValue(animation, frameNo, frameNo + 1, ratio, pOutputPtr, blendOutWeight),
		getKeyBasedValue(animation, frameNo, pOutputPtr, blendOutWeight));
}

//! Retrieve the animation value and use an hint to accelerate the lookup in the track
void 
CAnimationTrackEx::getValue(const SAnimation &animation, s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		animation,
		getKeyBasedValue(animation, frameNo, frameNo + 1, ratio, pOutputPtr),
		getKeyBasedValue(animation, frameNo, pOutputPtr));
	hintKeyLookUp = frameNo;
}

//! Retrieve the animation value and use an hint to accelerate the lookup in the track
void 
CAnimationTrackEx::getValue(const SAnimation &animation, s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp
							, float blendOutWeight, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		animation,
		getKeyBasedValue(animation, frameNo, frameNo + 1, ratio, pOutputPtr, blendOutWeight),
		getKeyBasedValue(animation, frameNo, pOutputPtr, blendOutWeight));
	hintKeyLookUp = frameNo;
}

void
CAnimationTrackEx::applyValue(const SAnimation &animation, s32 timeMs, void *pOutputPtr, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		animation,
		applyKeyBasedValue(animation, frameNo, frameNo + 1, ratio, pOutputPtr),
		applyKeyBasedValue(animation, frameNo, pOutputPtr));
}

void
CAnimationTrackEx::applyValue(const SAnimation &animation, s32 timeMs, void *pOutputPtr, float blendOutWeight, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		animation,
		applyKeyBasedValue(animation, frameNo, frameNo + 1, ratio, pOutputPtr, blendOutWeight),
		applyKeyBasedValue(animation, frameNo, pOutputPtr, blendOutWeight));
}

//! Retrieve the animation value and use an hint to accelerate the lookup in the track
void 
CAnimationTrackEx::applyValue(const SAnimation &animation, s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		animation,
		applyKeyBasedValue(animation, frameNo, frameNo + 1, ratio, pOutputPtr),
		applyKeyBasedValue(animation, frameNo, pOutputPtr));
	hintKeyLookUp = frameNo;
}

//! Retrieve the animation value and use an hint to accelerate the lookup in the track
void 
CAnimationTrackEx::applyValue(const SAnimation &animation, s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp
							, float blendOutWeight, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		animation,
		applyKeyBasedValue(animation, frameNo, frameNo + 1, ratio, pOutputPtr, blendOutWeight),
		applyKeyBasedValue(animation, frameNo, pOutputPtr, blendOutWeight));
	hintKeyLookUp = frameNo;
}

CAnimationTrack::CAnimationTrack(collada::SAnimation &animation)
: m_Animation(animation)
{
}

//! Retrieve the animation value
void 
CAnimationTrack::getValue(s32 timeMs, void *pOutputPtr, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		m_Animation,
		getKeyBasedValue(frameNo, frameNo + 1, ratio, pOutputPtr),
		getKeyBasedValue(frameNo, pOutputPtr));
}

//! Retrieve the animation value
void 
CAnimationTrack::getValue(s32 timeMs, void *pOutputPtr, float blendOutWeight, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		m_Animation,
		getKeyBasedValue(frameNo, frameNo + 1, ratio, pOutputPtr, blendOutWeight),
		getKeyBasedValue(frameNo, pOutputPtr, blendOutWeight));
}

//! Retrieve the animation value and use an hint to accelerate the lookup in the track
void 
CAnimationTrack::getValue(s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		m_Animation,
		getKeyBasedValue(frameNo, frameNo + 1, ratio, pOutputPtr),
		getKeyBasedValue(frameNo, pOutputPtr));
	hintKeyLookUp = frameNo;
}

//! Retrieve the animation value and use an hint to accelerate the lookup in the track
void 
CAnimationTrack::getValue(s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, float blendOutWeight, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		m_Animation,
		getKeyBasedValue(frameNo, frameNo + 1, ratio, pOutputPtr, blendOutWeight),
		getKeyBasedValue(frameNo, pOutputPtr, blendOutWeight));
	hintKeyLookUp = frameNo;
}

//! Retrieve the animation value
void 
CAnimationTrack::applyValue(s32 timeMs, void *pOutputPtr, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		m_Animation,
		applyKeyBasedValue(frameNo, frameNo + 1, ratio, pOutputPtr),
		applyKeyBasedValue(frameNo, pOutputPtr));
}

//! Retrieve the animation value
void 
CAnimationTrack::applyValue(s32 timeMs, void *pOutputPtr, float blendOutWeight, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		m_Animation,
		applyKeyBasedValue(frameNo, frameNo + 1, ratio, pOutputPtr, blendOutWeight),
		applyKeyBasedValue(frameNo, pOutputPtr, blendOutWeight));
}

//! Retrieve the animation value and use an hint to accelerate the lookup in the track
void 
CAnimationTrack::applyValue(s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		m_Animation,
		applyKeyBasedValue(frameNo, frameNo + 1, ratio, pOutputPtr),
		applyKeyBasedValue(frameNo, pOutputPtr));
	hintKeyLookUp = frameNo;
}

//! Retrieve the animation value and use an hint to accelerate the lookup in the track
void 
CAnimationTrack::applyValue(s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, float blendOutWeight, bool bInterpolate) const
{
	MANIP_VALUE_BODY(
		m_Animation,
		applyKeyBasedValue(frameNo, frameNo + 1, ratio, pOutputPtr, blendOutWeight),
		applyKeyBasedValue(frameNo, pOutputPtr, blendOutWeight));
	hintKeyLookUp = frameNo;
}

};
};

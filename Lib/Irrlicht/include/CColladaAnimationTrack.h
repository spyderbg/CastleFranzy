#ifndef __CCOLLADAANIMATIONTRACK_H__
#define __CCOLLADAANIMATIONTRACK_H__

#include "IAnimationTrack.h"
#include "SDatabaseCollada.h"
#include "IReferenceCounted.h"

namespace irr
{
namespace collada
{

class CAnimationTrackEx;

class IAnimationTrackEx
{
public:
	virtual int getValueSize() const = 0;

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const = 0;

	// Default implementation, apply on the void *
	virtual void applyBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		getBlendedValue(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr) const = 0;

	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr) const = 0;

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr,
		float blendOutWeight) const {
			_IRR_DEBUG_BREAK_IF("Not implemented")
	};

	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr,
		float blendOutWeight) const {
			_IRR_DEBUG_BREAK_IF("Not implemented")
	};

	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr,
		float blendOutWeight) const {
			_IRR_DEBUG_BREAK_IF("Not implemented")
	};

	virtual void applyKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr) const {
			getKeyBasedValue(animation, iKey0, iKey1, ratio, pOutputPtr);
	};

	virtual void applyKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr) const {
		getKeyBasedValue(animation, iKey0, pOutputPtr);
	};

	virtual void applyBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr,
		float blendOutWeight) const {
			_IRR_DEBUG_BREAK_IF("Not implemented")
	};

	virtual void applyKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr,
		float blendOutWeight) const {
			_IRR_DEBUG_BREAK_IF("Not implemented")
	};

	virtual void applyKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr,
		float blendOutWeight) const {
			_IRR_DEBUG_BREAK_IF("Not implemented")
	};

	//! Retrieve the animation value
	virtual void getValue(const collada::SAnimation &m_Animation, 
		s32 timeMs, void *pOutputPtr, bool bInterpolate) const = 0;

	virtual void applyValue(const collada::SAnimation &m_Animation,
		s32 timeMs, void *pOutputPtr, bool bInterpolate) const
	{
		getValue(m_Animation, timeMs, pOutputPtr, bInterpolate);
	}

	//! Retrieve the animation value and use an hint to accelerate the lookup in the track
	virtual void getValue(const collada::SAnimation &m_Animation, 
		s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate) const = 0;

	virtual void applyValue(const collada::SAnimation &m_Animation,
		s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate) const
	{
		getValue(m_Animation, timeMs, pOutputPtr, hintKeyLookUp, bInterpolate);
	}

	virtual void applyValue(void *pDataPtr, void *pOutputPtr) const = 0;
};

class IAnimationTrack
{
public:
	virtual int getValueSize() const = 0;

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const = 0;

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr,
		float blendOutWeight) const = 0;

	virtual void getKeyBasedValue(collada::SAnimation &m_Animation, int iKey0, int iKey1, float ratio, void *pOutputPtr) const = 0;

	virtual void getKeyBasedValue(collada::SAnimation &m_Animation, int iKey0, void *pOutputPtr) const = 0;	

	virtual void getKeyBasedValue(collada::SAnimation &m_Animation, int iKey0, int iKey1, float ratio, void *pOutputPtr,
		float blendOutWeight) const = 0;

	virtual void getKeyBasedValue(collada::SAnimation &m_Animation, int iKey0, void *pOutputPtr,
		float blendOutWeight) const = 0;
	
	//! Retrieve the animation value
	virtual void getValue(collada::SAnimation &m_Animation, s32 timeMs, void *pOutputPtr, bool bInterpolate) const;

	//! Retrieve the animation value and use an hint to accelerate the lookup in the track
	virtual void getValue(collada::SAnimation &m_Animation, s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate) const;

	virtual const CAnimationTrackEx* getAnimationTrackEx() const = 0;
};

class CAnimationTrackEx
	: public IAnimationTrackEx
{
public:
	//! Retrieve the animation value
	virtual void getValue(const collada::SAnimation &animation, 
		s32 timeMs, void *pOutputPtr, bool bInterpolate) const;

	//! Retrieve the animation value and use an hint to accelerate the lookup in the track
	virtual void getValue(const collada::SAnimation &animation, 
		s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate) const;

	//! Retrieve the animation value blended with the outout value
	virtual void getValue(const collada::SAnimation &animation, 
		s32 timeMs, void *pOutputPtr,
		float blendOutWeight, bool bInterpolate) const;

	//! Retrieve the animation value blended with the outout value 
	//  and use an hint to accelerate the lookup in the track
	virtual void getValue(const collada::SAnimation &animation, 
		s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp,
		float blendOutWeight, bool bInterpolate) const;

	//! Retrieve the animation value
	virtual void applyValue(const collada::SAnimation &animation, 
		s32 timeMs, void *pOutputPtr, bool bInterpolate) const;

	//! Retrieve the animation value and use an hint to accelerate the lookup in the track
	virtual void applyValue(const collada::SAnimation &animation, 
		s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate) const;

	//! Retrieve the animation value blended with the outout value
	virtual void applyValue(const collada::SAnimation &animation, 
		s32 timeMs, void *pOutputPtr,
		float blendOutWeight, bool bInterpolate) const;

	//! Retrieve the animation value blended with the outout value 
	//  and use an hint to accelerate the lookup in the track
	virtual void applyValue(const collada::SAnimation &animation, 
		s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp,
		float blendOutWeight, bool bInterpolate) const;

	virtual void applyValue(void *pDataPtr, void *pOutputPtr) const = 0;

	virtual void retrieveValue(void *pTarget, void * pValue) const
	{
		memcpy(pValue, pTarget, getValueSize());
	}
};

class CAnimationTrack
	: public virtual IReferenceCounted
	//: public scene::IAnimationTrack
{
protected:
	collada::SAnimation &m_Animation;
	
	const res::vector<int>	&getKeyTime() const { return m_Animation.samplers[0].pInput->iData; }

	const res::vector<float> &getOutput() const { return m_Animation.samplers[0].pOutput->fData;}

public:
	CAnimationTrack(collada::SAnimation &animation);

	const char *getBindURI() { return m_Animation.channels[0].target; }

	SChannel::Type getType() { return m_Animation.channels[0].type; }

public:
	float getStart() const { return (float)(getKeyTime())[0]; };
	float getEnd() const { return (float)(getKeyTime())[getKeyTime().size() - 1]; };
	float getLength() const { return getEnd() - getStart();  };
	const SChannel & getChannel() { return m_Animation.channels[0]; };

	virtual int getValueSize() const = 0;

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const = 0;

	// Default implementation, apply on the void *
	virtual void applyBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
	{
		getBlendedValue(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const = 0;

	virtual void applyKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		getKeyBasedValue(iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const = 0;

	virtual void applyKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		getKeyBasedValue(iKey0, pOutputPtr);
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr,
		float blendOutWeight) const {
			_IRR_DEBUG_BREAK_IF("Not implemented")
	};

	// Default implementation, apply on the void *
	virtual void applyBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr,
		float blendOutWeight)
	{
		getBlendedValue(pInputsArray, pWeightArray, iSize, pOutputPtr, blendOutWeight);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr,
		float blendOutWeight) const {
			_IRR_DEBUG_BREAK_IF("Not implemented")
	};

	virtual void applyKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr,
		float blendOutWeight) const {
			_IRR_DEBUG_BREAK_IF("Not implemented")
	};


	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr,
		float blendOutWeight) const {
			_IRR_DEBUG_BREAK_IF("Not implemented")
	};

	virtual void applyKeyBasedValue(int iKey0, void *pOutputPtr,
		float blendOutWeight) const {
			_IRR_DEBUG_BREAK_IF("Not implemented")
	};

	virtual const CAnimationTrackEx* getAnimationTrackEx() const = 0;

public: // IAnimationTrack virtual pure function
	//! Retrieve the animation value
	virtual void getValue(s32 timeMs, void *pOutputPtr, bool bInterpolate) const;

	//virtual void applyValue(void *pDataPtr, void *pOutputPtr) const = 0;

	virtual void applyValue(s32 timeMs, void *pOutputPtr, bool bInterpolate) const;

	//! Retrieve the animation value and use an hint to accelerate the lookup in the track
	virtual void getValue(s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate) const;

	virtual void applyValue(s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp, bool bInterpolate) const;

	//! Retrieve the animation value blended with the output value
	virtual void getValue(s32 timeMs, void *pOutputPtr,
		float blendOutWeight, bool bInterpolate) const;

	virtual void applyValue(s32 timeMs, void *pOutputPtr, float blendOutWeight, bool bInterpolate) const;

	//! Retrieve the animation value blended with the output value 
	//  and use an hint to accelerate the lookup in the track
	virtual void getValue(s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp,
		float blendOutWeight, bool bInterpolate) const;

	virtual void applyValue(s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp,
		float blendOutWeight, bool bInterpolate) const;

};

}; // namespace scene
}; // namespace irr

#endif

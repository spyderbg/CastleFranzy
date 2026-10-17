#ifndef __CANIMATIONTRACK_H_INCLUDED__
#define __CANIMATIONTRACK_H_INCLUDED__

#include "IAnimationTrack.h"
#include "quaternion.h"
#include "irrArray.h"

namespace irr
{
namespace scene
{

class CAnimationTrack
	: public IAnimationTrack
{
public:
	//! Retrieve the animation value
	virtual void GetValue(s32 timeMs, void *pOutputPtr) const = 0;

	//! Retrieve the animation value and use an hint to accelerate the lookup in the track
	virtual void GetValue(s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp) const = 0;

	//! Set input from a millisecond array
	virtual void SetInput(const core::array<s32> &input) 
	{ 
		m_input = input; 
		ComputeLength();
	}

	//! Set input from a secondes array
	virtual void SetInput(const core::array<f32> &input) 
	{ 
		m_input.reallocate(input.size());
		m_input.set_used(input.size());
		for(u32 i = 0; i < input.size(); i++)
		{
			m_input[i] = (s32)(input[i] * 1000.0f); 
		}
		ComputeLength();
	}

protected:

	void ComputeLength()
	{
		if(m_input.size())
		{
			m_Length = m_input.getLast();
		}
		else
		{
			m_Length = 0;
		}
	}

	//! Key ID, commonly time but could be anything
	core::array<s32> m_input;

	//! Do a binary lookup
	/** \return : Return true if the value is in between the 2 frame
				  or false if the value is exactly the prev frame
	*/
	bool findKeyFrameNo(s32 timeMs, s32 &outFrame) const
	{
		s32 m;
		/** Left intentionnaly fixed to 1 instead of 0.
			Right value at the end of the search is always at Left - 1.
			When the timeMs is before the first key frame time, the right value is = to left(1) - 1.
		*/
		s32 left = 1; 
		s32 right = m_input.size() - 1;
		do
		{
			m = (left+right)>>1;

			if (timeMs < m_input[m])
				right = m - 1;
			else
				left = m + 1;

		} while(left<=right);

		outFrame = right;
		return !((timeMs == m_input[outFrame]) || (outFrame == m_input.size() - 1));
	}

	//! Do a linear lookup based on the hint
	bool findKeyFrameNo(s32 timeMs, s32 &outFrame, s32 hint) const
	{
		s32 max = m_input.size() - 1;
		s32 m = core::clamp<s32>(hint, 0, max);
		
		while((m <= max) && (timeMs < m_input[m] || m_input[m+1] < timeMs))
		{
			if (timeMs < m_input[m])
				m--;
			else
				m++;
		} 

		outFrame = m;
		return !((timeMs == m_input[outFrame]) || (outFrame == max));
	}
};


template<typename T, IAnimationTrack::Type debugType>
class CLinearInterpolationTrack
	: public CAnimationTrack
{
public:
	//! Retrieve the animation value
	virtual void GetValue(s32 inputTime, void *pOutputPtr) const
	{
		s32 frameNo = 0;
		bool bInterpolate = findKeyFrameNo(inputTime, frameNo);
		if(bInterpolate)
		{
			s32 previousKeyTime = m_input[frameNo];
			s32 nextKeyTime = m_input[frameNo + 1];
			s32 deltaTime = nextKeyTime - previousKeyTime;
			f32 ratio = (f32)(inputTime - previousKeyTime) / (f32)deltaTime;
			ratio = core::clamp<f32>(ratio, 0, 1);
			(*(T*)pOutputPtr) = lerp(m_output[frameNo], m_output[frameNo + 1], ratio);
		}
		else
		{
			(*(T*)pOutputPtr) = m_output[frameNo];
		}
	};

	//! Retrieve the animation value and use an hint to accelerate the lookup in the track
	virtual void GetValue(s32 inputTime, void *pOutputPtr, s32 &hintKeyLookUp) const
	{
		s32 frameNo = 0;
		bool bInterpolate = findKeyFrameNo(inputTime, frameNo, hintKeyLookUp);
		if(bInterpolate)
		{
			s32 previousKeyTime = m_input[frameNo];
			s32 nextKeyTime = m_input[frameNo + 1];
			s32 deltaTime = nextKeyTime - previousKeyTime;
			f32 ratio = (inputTime - previousKeyTime) / (f32)deltaTime;
			(*(T*)pOutputPtr) = lerp(m_output[frameNo], m_output[frameNo + 1], ratio);
		}
		else
		{
			(*(T*)pOutputPtr) = m_output[frameNo];
		}
		hintKeyLookUp = frameNo;
	};

#ifdef _DEBUG
	//! Retrieve value type
	virtual IAnimationTrack::Type GetValueType() const { return debugType; };
#endif

	//! Set input from a secondes array
	virtual void SetOutput(const core::array<T> &output) 
	{ 
		m_output = output;
	}

protected:
	core::array<T> m_output;
};

struct SPositionDirection
{
	core::vector3df		pos;
	core::quaternion	dir;
	core::vector3df		scale;
};

inline SPositionDirection lerp(const SPositionDirection &p1, const SPositionDirection &p2, f32 ratio)
{
	SPositionDirection temp;
	temp.pos = core::lerp(p1.pos, p2.pos, ratio);
	temp.dir.slerp(p1.dir, p2.dir, ratio);
	temp.scale = core::lerp(p1.scale, p2.scale, ratio);
	return temp;
}

inline f32 lerp(const f32 &p1, const f32 &p2, f32 ratio)
{
	return core::lerp(p1, p2, ratio);
}

inline core::matrix4 lerp(const core::matrix4 &p1, const core::matrix4 &p2, f32 ratio)
{
	return core::lerp(p1, p2, ratio);
}

typedef CLinearInterpolationTrack<f32, IAnimationTrack::FLOAT> CFloatLerpAnimationTrack;
//typedef CLinearInterpolationTrack<core::matrix4, IAnimationTrack::MATRIX> CMatrixLerpAnimationTrack;
//typedef CLinearInterpolationTrack<SPositionDirection, IAnimationTrack::MATRIX> CPosDirLerpAnimationTrack;

class CMatrixLerpAnimationTrack 
	: public CLinearInterpolationTrack<core::matrix4, IAnimationTrack::MATRIX>
{
public:
	//! Set output from an array
	virtual void SetOutput(const core::array<f32> &output) 
	{ 
		int count = output.size() >> 4;
		m_output.reallocate(count);
		m_output.set_used(count);
		for(s32 i = 0; i < count; i++)
		{
			memcpy(m_output[i].pointer(), &output[i << 4], 16 * sizeof(f32));
			m_output[i] = m_output[i].getTransposed();
		}
	}
};

class CPosDirLerpAnimationTrack
	: public CLinearInterpolationTrack<SPositionDirection, IAnimationTrack::MATRIX>
{
public:
	//! Set output from an array
	virtual void SetOutput(const core::array<f32> &output) 
	{ 
		int count = output.size() >> 4;
		m_output.reallocate(count);
		m_output.set_used(count);
		for(s32 i = 0; i < count; i++)
		{
			core::matrix4 mat;
			memcpy(mat.pointer(), &output[i << 4], 16 * sizeof(f32));
			mat = mat.getTransposed();
			m_output[i].pos = mat.getTranslation();
			m_output[i].dir = mat;
			m_output[i].scale = mat.getScale();
		}
	}
};

};
};

#endif
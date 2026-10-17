#include "stdafx.h"
#include "CColladaAnimationTrackImageFileList.h"

namespace irr
{
namespace collada
{
namespace animation_track
{

CImageFileList::CImageFileList(collada::SAnimation &animation)
: CAnimationTrack(animation)
{
	m_iLoopLengthMs = animation.sources[0].iData[animation.sources[0].iData.size() - 1];
}


int 
CImageFileList::getValueSize() const
{
	return sizeof(float);
}

void 
CImageFileList::getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
{
	_IRR_DEBUG_BREAK_IF("Not blendable");
}

//! Retrieve the animation value
void 
CImageFileList::getValue(s32 timeMs, void *pOutputPtr) const
{
	const res::vector<int> &m_input = getKeyTime();
	timeMs %= m_iLoopLengthMs;

	s32 frameNo = 0;
	bool bInterpolate = m_Animation.findKeyFrameNo(0, timeMs, frameNo);
	getKeyBasedValue(frameNo, pOutputPtr);
}

//! Retrieve the animation value and use an hint to accelerate the lookup in the track
void 
CImageFileList::getValue(s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp) const
{
	const res::vector<int> &m_input = getKeyTime();
	timeMs %= m_iLoopLengthMs;

	s32 frameNo = 0;
	bool bInterpolate = m_Animation.findKeyFrameNo(0, timeMs, frameNo);
	getKeyBasedValue(frameNo, pOutputPtr);
}

void 
CImageFileList::getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
{
	SVectorTemplate<video::ITexture *> &vTexture = *(SVectorTemplate<video::ITexture *>*)&getOutput();
	video::ITexture *&output = *(video::ITexture **)pOutputPtr;

	output = vTexture[iKey0];
}

void 
CImageFileList::getKeyBasedValue(int iKey0, void *pOutputPtr) const
{
	SVectorTemplate<video::ITexture *> &vTexture = *(SVectorTemplate<video::ITexture *>*)&getOutput();
	video::ITexture *&output = *(video::ITexture **)pOutputPtr;

	output = vTexture[iKey0];
}

};
};
};
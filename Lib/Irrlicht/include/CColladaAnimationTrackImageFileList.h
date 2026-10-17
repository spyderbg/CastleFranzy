#ifndef __CCOLLADAANIMATIONTRACKIMAGEFILELIST_H__
#define __CCOLLADAANIMATIONTRACKIMAGEFILELIST_H__

#include "CColladaAnimationTrack.h"

namespace irr
{
namespace collada
{
namespace animation_track
{

class CImageFileList
	: public CAnimationTrack
{
public:
	CImageFileList(collada::SAnimation &animation);

	virtual int getValueSize() const;

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const;

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const;

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const;

	//! Retrieve the animation value
	virtual void getValue(s32 timeMs, void *pOutputPtr) const;

	//! Retrieve the animation value and use an hint to accelerate the lookup in the track
	virtual void getValue(s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp) const;

	virtual  const collada::CAnimationTrackEx *getAnimationTrackEx(void) const
	{
		_IRR_DEBUG_BREAK_IF("Not implemented");
		return 0;
	}

protected:
	int m_iLoopLengthMs;
};

}; // namespace animation_track
}; // namespace collada
}; // namespace irr

#endif // __CCOLLADAANIMATIONTRACKIMAGEFILELIST_H__
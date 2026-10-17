#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaSceneNodeAnimator.h"

namespace irr
{
namespace collada
{

const CAnimationTrackEx* 
CSceneNodeAnimator::getAnimationTrackEx(int track)
{
	return ChannelAnimations[track].Track->getAnimationTrackEx();
}

const SChannel&
CSceneNodeAnimator::getAnimationTrack(int track)
{
	return ChannelAnimations[track].Track->getChannel();
}

void 
CSceneNodeAnimator::setTarget(int channel, void* target)
{
	ChannelAnimations[channel].Target = target;
}

const char*
CSceneNodeAnimator::getBindURI(int i)
{
	return ChannelAnimations[i].Track->getBindURI();
}

int 
CSceneNodeAnimator::getTargetCount()
{
	return ChannelAnimations.size();
}

int 
CSceneNodeAnimator::getTargetSize(int i)
{
	return ChannelAnimations[i].Track->getValueSize();
};

int 
CSceneNodeAnimator::getTargetsSize()
{
	int targetSize = 0;

	for (u32 i = 0, sz = ChannelAnimations.size(); i < sz; ++i)
	{
		targetSize += ChannelAnimations[i].Track->getValueSize();
	}

	return targetSize;
};


CSceneNodeAnimator::CSceneNodeAnimator(const CColladaDatabase& database, SLibraryAnimationClips& animationClips) 
	: IObject(database)
	, AnimationClips(animationClips)
	, BlendOutWeight(1.0f)
	, IsSampled(database.isSampled())
{
	CTimelineController* timeCtrl = irrnew CTimelineController();

	if (animationClips.animationClips.size())
	{
		timeCtrl->setAnimationClips(&AnimationClips);
	}
	else
	{
		timeCtrl->setRange(getStart(), getEnd());
	}

	setTimelineCtrl(timeCtrl);
	timeCtrl->drop();
}
 
CSceneNodeAnimator::~CSceneNodeAnimator()
{
	removeAnimationTracks();
}

void 
CSceneNodeAnimator::updateTime(u32 timeMs)
{
	if (ChannelAnimations.size() == 0 && EventsManager == 0)
	{
		return;
	}

	ISceneNodeAnimator::updateTime(timeMs);
}

void
CSceneNodeAnimator::applyAnimationValues(u32 timeMs)
{
	if (ChannelAnimations.size() == 0 && EventsManager == 0)
	{
		return;
	}

	ISceneNodeAnimator::updateTime(timeMs);

	scene::ITimelineController* timeCtrl = getTimelineCtrl();
	s32 before = 0;
	s32 relativeTime = 0;

	// Remap the time base on the controller
	if (timeCtrl)
	{
		relativeTime = timeCtrl->getCtrlTime();
	}
	else
	{
		relativeTime = timeMs % Length;
	}

	bool bInterpolate = getInterpolationMode() != ISceneNodeAnimator::EIT_STEP;
		
	if(BlendOutWeight == 1.0f)
	{
		for(u32 i = 0; i < ChannelAnimations.size(); i++)
		{
			void *pTarget = ChannelAnimations[i].Target;
			if(pTarget)
			{
				ChannelAnimations[i].Track->applyValue(
					relativeTime, 
					pTarget, 
					IsSampled ? ChannelAnimations[0].Hint : ChannelAnimations[i].Hint, 
					bInterpolate);
			}
		}
	}
	else
	{
		for(u32 i = 0; i < ChannelAnimations.size(); i++)
		{
			void *pTarget = ChannelAnimations[i].Target;
			if(pTarget)
			{
				ChannelAnimations[i].Track->applyValue(
					relativeTime, 
					pTarget, 
					IsSampled ? ChannelAnimations[0].Hint : ChannelAnimations[i].Hint,
					BlendOutWeight, 
					bInterpolate);
			}
		}
	}
}

void
CSceneNodeAnimator::computeAnimationValues(u32 timeMs)
{
	if (ChannelAnimations.size() == 0 && EventsManager == 0)
	{
		return;
	}

	ISceneNodeAnimator::updateTime(timeMs);

	scene::ITimelineController* timeCtrl = getTimelineCtrl();
	s32 before = 0;
	s32 relativeTime = 0;

	// Remap the time base on the controller
	if (timeCtrl)
	{
		relativeTime = timeCtrl->getCtrlTime();
	}
	else
	{
		relativeTime = timeMs % Length;
	}

	bool interpolate = getInterpolationMode() != ISceneNodeAnimator::EIT_STEP;
		
	if (BlendOutWeight == 1.0f)
	{
		for (u32 i = 0, sz = ChannelAnimations.size(); i < sz; ++i)
		{
			void* target = ChannelAnimations[i].Target;
			if (target)
			{
				ChannelAnimations[i].Track->getValue(
					relativeTime, 
					target, 
					IsSampled ? ChannelAnimations[0].Hint : ChannelAnimations[i].Hint, 
					interpolate);
			}
		}
	}
	else
	{
		for (u32 i = 0, sz = ChannelAnimations.size(); i < sz; ++i)
		{
			void* target = ChannelAnimations[i].Target;
			if (target)
			{
				ChannelAnimations[i].Track->getValue(
					relativeTime, 
					target, 
					IsSampled ? ChannelAnimations[0].Hint : ChannelAnimations[i].Hint,
					BlendOutWeight, 
					interpolate);
			}
		}
	}
}

//! animates a scene node
void 
CSceneNodeAnimator::animateNode(scene::ISceneNode* node, u32 timeMs)
{
	applyAnimationValues(timeMs);
}

//! Writes attributes of the scene node animator.
void 
CSceneNodeAnimator::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
}

//! Reads attributes of the scene node animator.
void 
CSceneNodeAnimator::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
}

//! Returns type of the scene node animator
//virtual ESCENE_NODE_ANIMATOR_TYPE getType() const { return ESNAT_TEXTURE; }

//! Creates a clone of this animator.
/** Please note that you will have to drop
(IReferenceCounted::drop()) the returned pointer after calling
this. */
scene::ISceneNodeAnimator* 
CSceneNodeAnimator::createClone()
{
	_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	return 0;
}

//! Add a new channel animator to the current set.
void 
CSceneNodeAnimator::addAnimationTrack(CAnimationTrack* animator)
{
	animator->grab();
	SBinding toBind;
	toBind.Track = animator;
	toBind.Target = 0;

	ChannelAnimations.push_back(toBind);

	if (ChannelAnimations.size() > 1)
	{
		Start = ((f32)Start > animator->getStart() ? (s32)animator->getStart() : Start);
		End = ((f32)End > animator->getEnd() ? (s32)animator->getEnd() : End);
		Length = ((f32)Length > animator->getLength() ? Length : (s32)animator->getLength());
	}
	else
	{
		Start = (s32)animator->getStart();
		End = (s32)animator->getEnd();
		Length = (s32)animator->getLength();
	}

	getTimelineCtrl()->setRange(getStart(), getEnd());
}

void 
CSceneNodeAnimator::removeAnimationTrack(CAnimationTrack* animator)
{
	for (u32 i = 0, sz = ChannelAnimations.size(); i < sz; ++i)
	{
		if (ChannelAnimations[i].Track == animator)
		{
			ChannelAnimations[i].Track->drop();
			ChannelAnimations.erase(i);
			return;
		}
	}
}

void 
CSceneNodeAnimator::removeAnimationTracks()
{
	for (u32 i = 0, sz = ChannelAnimations.size(); i < sz; ++i)
	{
		ChannelAnimations[i].Track->drop();
	}
}

} // namespace collada
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

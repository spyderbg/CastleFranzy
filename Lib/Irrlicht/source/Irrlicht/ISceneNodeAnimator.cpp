#include "stdafx.h"
#include "ISceneNodeAnimator.h"

namespace irr
{
namespace scene
{

ISceneNodeAnimator::~ISceneNodeAnimator() 
{
	if (TimelineCtrl)
		TimelineCtrl->drop();
}

void ISceneNodeAnimator::updateTime(u32 timeMs)
{
}

bool ISceneNodeAnimator::isEventReceiverEnabled() const
{
	return false;
}

bool ISceneNodeAnimator::OnEvent(const SEvent& event)
{
	return false;
}

ESCENE_NODE_ANIMATOR_TYPE ISceneNodeAnimator::getType() const
{
	return ESNAT_UNKNOWN;
}

void ISceneNodeAnimator::onBind(ISceneNode* node)
{
}

void ISceneNodeAnimator::onUnbind(ISceneNode* node)
{
}

void ISceneNodeAnimator::setAnimationClip(int idx)
{
	TimelineCtrl->setClip(idx);
}

void ISceneNodeAnimator::setAnimationClip(const char *animId)
{
	TimelineCtrl->setClip(animId);
}

int ISceneNodeAnimator::getAnimationClipIndex(const char *animId)
{
	return TimelineCtrl->getClipIndex(animId);
}

void ISceneNodeAnimator::setTimelineCtrl(ITimelineController *timeCtrl)
{
	if (TimelineCtrl)
	{
		TimelineCtrl->drop();
	}
	TimelineCtrl = timeCtrl;
	if (TimelineCtrl)
	{
		TimelineCtrl->grab();
	}
}

const ITimelineController* ISceneNodeAnimator::getTimelineCtrl() const
{
	return TimelineCtrl;
}

ITimelineController* ISceneNodeAnimator::getTimelineCtrl()
{
	return TimelineCtrl;
}

int ISceneNodeAnimator::getLength() const
{
	return 0;
}

void ISceneNodeAnimator::computeAnimationValues(u32 timeMs)
{
}

void ISceneNodeAnimator::applyAnimationValues(u32 timeMs)
{
}

} // end namespace scene
} // end namespace irr

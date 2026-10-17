#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaSceneNodeAnimatorSet.h"

namespace irr
{
namespace collada
{

int CAnimationSet::addAnimation(const SAnimation* animation)
{
	const SChannel* channel = &animation->channels[0];
	
	for (u32 i = 0, sz = Channels.size(); i < sz; ++i)
	{
		if ((Channels[i]->type == channel->type)
			&& (Channels[i]->reserved0 == channel->reserved0) /*Morphing Index*/
			&& (Channels[i]->target == channel->target))
		{
			return i;
		}
	}

	
	const collada::CAnimationTrackEx* animationTrackEx = CColladaDatabase::getAnimationTrackEx(animation);
	if(animationTrackEx == 0)
		return -1;

	Channels.push_back(channel);
	AnimatorEx.push_back(animationTrackEx);
	return Channels.size() - 1;
}

int
CAnimationSet::remAnimation(const SAnimation* Animation)
{
	return 0;
}

SChannel&
CAnimationSet::getChannel(u32 index)
{
	return *const_cast<SChannel*>(Channels[index]);
}

const SChannel&
CAnimationSet::getChannel(u32 index) const
{
	return *Channels[index];
}

const SChannel&
CAnimationSet::getAnimationTrack(int track) const
{
	return getChannel(track);
}

const CAnimationTrackEx*
CAnimationSet::getAnimationTrackEx(int track)
{
	return AnimatorEx[track];
}

int 
CAnimationSet::getAnimationCount() const
{
	return AnimationSet.size();
}

int 
CAnimationSet::getAnimatedTrackCount() const
{
	return AnimatedTrackCount;
}

int 
CAnimationSet::getAnimationLength(int i) const
{
	if(AnimationSet[i].getAnimationCount())
	{
		return AnimationSet[i].getAnimation(0)->getLength();
	}
	else
	{
		return 0;
	}
}

int 
CAnimationSet::getAnimationStart(int i) const
{
	if(AnimationSet[i].getAnimationCount())
	{
		return AnimationSet[i].getAnimation(0)->getStart();
	}
	else
	{
		return 0;
	}
}

int 
CAnimationSet::getAnimationEnd(int i) const
{
	if(AnimationSet[i].getAnimationCount())
	{
		return AnimationSet[i].getAnimation(0)->getEnd();
	}
	else
	{
		return 0;
	}
}

int 
CAnimationSet::addAnimationLibrary(const CColladaDatabase& database)
{
	AnimationSet.push_back(database);
	return AnimationSet.size() - 1;
}

const CColladaDatabase&
CAnimationSet::getDatabase(int i)
{
	return AnimationSet[i];
}

const CAnimationSet::SBinding&
CAnimationSet::getBinding(int i) const
{
	return Binding[i];
}

void 
CAnimationSet::remAnimationLibrary(CColladaDatabase& database)
{
	//AnimationSet.remove(database);
}

void 
CAnimationSet::remAnimationLibrary(int index)
{
}

void 
CAnimationSet::compile()
{
	for (u32 i = 0; i < AnimationSet.size(); ++i)
	{
		const CColladaDatabase* database = &AnimationSet[i];
		for (int j = 0; j < database->getAnimationCount(); ++j)
		{
			const SAnimation* animation = database->getAnimation(j);
			addAnimation(animation);
		}
	}
	
	for (u32 i = 0, k = 0; i < AnimationSet.size(); ++i)
	{
		void* target;
		const CColladaDatabase* database = &AnimationSet[i];
		for (u32 j = 0; j < Channels.size(); ++j)
		{
			// getAnimatorIdx
			const SAnimation* animation = database->getBlendableAnimation(Channels[j]);
			if (animation == 0)
			{
				bool found = database->getDefaultValue(Channels[j], &target);
				if (!found && MismatchBehavior == EMB_TRIM_ANIMATION)
				{
#ifdef _DEBUG
					char temp[256];
					sprintf(temp, "Channel %s not found in %s\n", (const char*)Channels[j]->target, (database->getVisualScene(0) == 0) ? "novisualscene" : (const char*)database->getVisualScene(0)->id);
					os::Printer::log(temp);
#endif
					Channels.erase(j);
					AnimatorEx.erase(j);
					--j;
				}
			}
		}
	}

	AnimatedTrackCount = Channels.size();

	int bindingCount = AnimatedTrackCount * AnimationSet.size();
	Binding.reallocate(bindingCount);
	Binding.set_used(bindingCount);

	for (u32 i = 0, k = 0; i < AnimationSet.size(); ++i)
	{
		const CColladaDatabase* database = &AnimationSet[i];
		for (u32 j = 0; j < Channels.size(); ++j, ++k)
		{
			// getAnimatorIdx
			const SAnimation* animation = database->getBlendableAnimation(Channels[j]);
			bool found = database->getDefaultValue(Channels[j], &Binding[k].DefaultValue);
			if (animation == 0)
			{
				Binding[k].Type = SBinding::EBT_DEFAULT_VALUE;
				if (!found)
				{
					Binding[k].DefaultValue = 0;
				}
				//_IRR_DEBUG_BREAK_IF(!found);
			}
			else
			{
				Binding[k].Type = SBinding::EBT_ANIMATION_VALUE;
				Binding[k].Animation = animation;
			}
		}
	}
}

//class CSceneNodeAnimatorSet

int 
CSceneNodeAnimatorSet::getAnimationLength(int i)
{
	return AnimationSet->getAnimationLength(i);
}

int 
CSceneNodeAnimatorSet::getAnimationStart(int i)
{
	return AnimationSet->getAnimationStart(i);
}

int 
CSceneNodeAnimatorSet::getAnimationEnd(int i)
{
	return AnimationSet->getAnimationEnd(i);
}

CSceneNodeAnimatorSet::CSceneNodeAnimatorSet(CAnimationSet* animationSet)
	: AnimationStart(0)
	, AnimationSet(animationSet)
{
	AnimationSet->grab();
	int animatedTrackCount = AnimationSet->getAnimatedTrackCount();

	Targets.reallocate(animatedTrackCount);
	Targets.set_used(animatedTrackCount);

	Hints.reallocate(animatedTrackCount);
	Hints.set_used(animatedTrackCount);

	CTimelineController* timeCtrl = irrnew CTimelineController;
	setTimelineCtrl(timeCtrl);
	setCurrentAnimation(0);

	timeCtrl->drop();
}

CSceneNodeAnimatorSet::~CSceneNodeAnimatorSet()
{
	AnimationSet->drop();
}

void 
CSceneNodeAnimatorSet::setCurrentAnimation(int i)
{
	Length = getAnimationLength(i);
	AnimationStart = i * AnimationSet->AnimatedTrackCount;
	CurrentAnim = i;
	SLibraryAnimationClips* animationClips = AnimationSet->getDatabase(i).getAnimationClipLibrary();
	if (getTimelineCtrl())
	{
		if (animationClips->getClipCount())
		{
			static_cast<CTimelineController*>(getTimelineCtrl())->setAnimationClips(animationClips);
		}
		else
		{
			static_cast<CTimelineController*>(getTimelineCtrl())->setAnimationClips(0);
			static_cast<CTimelineController*>(getTimelineCtrl())->setRange(getAnimationStart(i), getAnimationEnd(i));
		}

		setEventsTrack(AnimationSet->getDatabase(i).getEventsTrack());
	}
}

int
CSceneNodeAnimatorSet::getCurrentAnimation() const
{
	return CurrentAnim;
}

const char *
CSceneNodeAnimatorSet::getAnimationVisualSceneID(int i) const
{
	return AnimationSet->getDatabase(i).getVisualScene(0)->id;
}

int 
CSceneNodeAnimatorSet::getAnimationCount() const
{
	return AnimationSet->getAnimationCount();
}

const SChannel&
CSceneNodeAnimatorSet::getAnimationTrack(int track)
{
	return AnimationSet->getAnimationTrack(track);
}

const CAnimationTrackEx*
CSceneNodeAnimatorSet::getAnimationTrackEx(int track)
{
	return AnimationSet->getAnimationTrackEx(track);
}

const char*
CSceneNodeAnimatorSet::getBindURI(int i)
{
	return AnimationSet->getChannel(i).target;
}

int 
CSceneNodeAnimatorSet::getTargetCount()
{
	return AnimationSet->AnimatedTrackCount;
}

int 
CSceneNodeAnimatorSet::getTargetSize(int i)
{
	return AnimationSet->AnimatorEx[i]->getValueSize();
}

int 
CSceneNodeAnimatorSet::getTargetsSize()
{
	int targetSize = 0;

	for (u32 i = 0, cnt = AnimationSet->AnimatedTrackCount; i < cnt; ++i)
	{
		targetSize += AnimationSet->AnimatorEx[i]->getValueSize();
	}

	return targetSize;
}

void 
CSceneNodeAnimatorSet::setTarget(int channel, void* target)
{
	Targets[channel] = target;
}

void 
CSceneNodeAnimatorSet::computeAnimationValues(u32 timeMs)
{
	if (AnimationSet->AnimatedTrackCount == 0 && EventsManager == 0)
	{
		return;
	}

	ISceneNodeAnimator::updateTime(timeMs);

	scene::ITimelineController* timeCtrl = getTimelineCtrl();
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
	bool isSampled = AnimationSet->getDatabase(CurrentAnim).isSampled();
	
	for (u32 i = 0, cnt = AnimationSet->AnimatedTrackCount; i < cnt; ++i)
	{
		void* target = Targets[i];
		if (target)
		{
			CAnimationSet::SBinding& animationBinding = AnimationSet->Binding[i + AnimationStart];
			if (animationBinding.DefaultValue)
			{
				memcpy(target,
					   animationBinding.DefaultValue, 
					   AnimationSet->AnimatorEx[i]->getValueSize());
			}

			if (animationBinding.Type == CAnimationSet::SBinding::EBT_ANIMATION_VALUE)
			{
				const SAnimation* animation = animationBinding.Animation;

				static_cast<CAnimationTrackEx*>(animation->pAnimator)->getValue(
					*animation,
					relativeTime, 
					target, 
					isSampled ? Hints[0] : Hints[i],
					interpolate
				);
			}
		}
	}
}

void 
CSceneNodeAnimatorSet::applyAnimationValues(u32 timeMs)
{
	if(AnimationSet->AnimatedTrackCount == 0 && EventsManager == 0)
	{
		return;
	}

	ISceneNodeAnimator::updateTime(timeMs);

	scene::ITimelineController* timeCtrl = getTimelineCtrl();
	s32 relativeTime = 0;

	// Remap the time base on the controller
	if(timeCtrl)
	{
		relativeTime = timeCtrl->getCtrlTime();
	}
	else
	{
		relativeTime = timeMs % Length;
	}

	bool interpolate = getInterpolationMode() != ISceneNodeAnimator::EIT_STEP;
	bool isSampled = AnimationSet->getDatabase(CurrentAnim).isSampled();
	
	for (u32 i = 0, cnt = AnimationSet->AnimatedTrackCount; i < cnt; ++i)
	{
		void* target = Targets[i];
		if (target)
		{
			CAnimationSet::SBinding& animationBinding = AnimationSet->Binding[i + AnimationStart];
			if(animationBinding.DefaultValue)
			{
				AnimationSet->AnimatorEx[i]->applyValue(animationBinding.DefaultValue, target);
			}

			if(animationBinding.Type == CAnimationSet::SBinding::EBT_ANIMATION_VALUE)
			{
				const SAnimation* animation = animationBinding.Animation;

				((CAnimationTrackEx*)animation->pAnimator)->applyValue(
					*animation,
					relativeTime, 
					target, 
					isSampled ? Hints[0] : Hints[i], 
					interpolate
				);
			}
		}
	}
}

//! animates a scene node
void 
CSceneNodeAnimatorSet::animateNode(scene::ISceneNode* node, u32 timeMs)
{
	applyAnimationValues(timeMs);
}

} // namespace collada
} // namespace irr

#endif //_IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

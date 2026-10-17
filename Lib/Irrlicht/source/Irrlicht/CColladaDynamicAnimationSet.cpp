#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaDynamicAnimationSet.h"

namespace irr
{
namespace collada
{

int
CDynamicAnimationSet::addAnimation(const SAnimation* animation)
{
	const SChannel* channel = &animation->channels[0];

	for (u32 i = 0; i < ChannelsCopy.size(); ++i)
	{
		if ((ChannelsCopy[i].type == channel->type)
			&& (ChannelsCopy[i].reserved0 == channel->reserved0) /*Morphing Index*/
			&& (ChannelsCopy[i].target == channel->target))
		{
			return i;
		}
	}

	ChannelsCopy.push_back(animation->channels[0]);
	AnimatorEx.push_back(CColladaDatabase::getAnimationTrackEx(animation));
	//NeedsCompilation = true;
	return ChannelsCopy.size() - 1;
}

int
CDynamicAnimationSet::remAnimation(const SAnimation* animation)
{
	const SChannel* channel = &animation->channels[0];

	for (u32 i = 0; i < ChannelsCopy.size(); ++i)
	{
		if ((ChannelsCopy[i].type == channel->type)
			&& (ChannelsCopy[i].reserved0 == channel->reserved0) /*Morphing Index*/
			&& (ChannelsCopy[i].target == channel->target))
		{
			ChannelsCopy.erase(i);
			AnimatorEx.erase(i);
			//NeedsCompilation = true;
			return i;
		}
	}
	return -1;
}

SChannel&
CDynamicAnimationSet::getChannel(u32 index)
{
	return ChannelsCopy[index];
}

const SChannel&
CDynamicAnimationSet::getChannel(u32 index) const
{
	return ChannelsCopy[index];
}

int
CDynamicAnimationSet::getDatabaseIndex(CColladaDatabase &database)
{
	for (u32 i = 0; i < AnimationSet.size(); ++i)
	{
		if (AnimationSet[i] == database)
			return i;
	}
	return -1;
}

void
CDynamicAnimationSet::clearTracks()
{
	//Use set_used(0) instead?
	//ChannelsCopy.clear();
	//AnimatorEx.clear();
	//Binding.clear();
	ChannelsCopy.set_used(0);
	AnimatorEx.set_used(0);
	Binding.set_used(0);
	AnimatedTrackCount = 0;
	NeedsCompilation = true;
}

void
CDynamicAnimationSet::clearSet()
{
	//Clear here because we want the destructor to be called
	AnimationSet.clear();
	NeedsCompilation = true;
}

int
CDynamicAnimationSet::addAnimationLibrary(const CColladaDatabase& database)
{
	int i = CAnimationSet::addAnimationLibrary(database);
	NeedsCompilation = true;
	return i;
}

void
CDynamicAnimationSet::remAnimationLibrary(CColladaDatabase& database)
{
	remAnimationLibrary(getDatabaseIndex(database));
}

void
CDynamicAnimationSet::remAnimationLibrary(u32 index)
{
	if (index >= AnimationSet.size())
		return;
	AnimationSet.erase(index);
	NeedsCompilation = true;
}

void
CDynamicAnimationSet::compile()
{
	if (!NeedsCompilation)
		return;
	clearTracks();

	for (u32 i = 0; i < AnimationSet.size(); ++i)
	{
		const CColladaDatabase* database = &AnimationSet[i];
		for (int j = 0; j < database->getAnimationCount(); ++j)
		{
			const SAnimation* animation = database->getAnimation(j);
			addAnimation(animation);
		}
	}
	
	for(u32 i = 0, k = 0; i < AnimationSet.size(); ++i)
	{
		void* target;
		const CColladaDatabase* database = &AnimationSet[i];
		for (u32 j = 0; j < ChannelsCopy.size(); ++j)
		{
			// getAnimatorIdx
			const SAnimation* animation = database->getBlendableAnimation(&ChannelsCopy[j]);
			if (animation == 0)
			{
				bool found = database->getDefaultValue(&ChannelsCopy[j], &target);
				if (!found)
				{
#ifdef _DEBUG
					char temp[256];
					sprintf(temp, "Channel %s not found in %s\n",
							(const char*)ChannelsCopy[j].target,
							(const char*)database->getVisualScene(0)->id);
					os::Printer::log(temp);
#endif
					ChannelsCopy.erase(j);
					AnimatorEx.erase(j);
					--j;
				}
			}
		}
	}

	AnimatedTrackCount = ChannelsCopy.size();

	int bindingCount = AnimatedTrackCount * AnimationSet.size();
	Binding.reallocate(bindingCount);
	Binding.set_used(bindingCount);

	for (u32 i = 0, k = 0; i < AnimationSet.size(); ++i)
	{
		const CColladaDatabase* database = &AnimationSet[i];
		for (u32 j = 0; j < AnimatedTrackCount; ++j, ++k)
		{
			// getAnimatorIdx
			const SAnimation* animation = database->getBlendableAnimation(&ChannelsCopy[j]);
			bool found = database->getDefaultValue(&ChannelsCopy[j], &Binding[k].DefaultValue);
			if (animation == 0)
			{
				Binding[k].Type = SBinding::EBT_DEFAULT_VALUE;
				Binding[k].Animation = 0;
				if (!found)
				{
					Binding[k].DefaultValue = 0;
				}
				//_IRR_DEBUG_BREAK_IF(!bFound);
			}
			else
			{
				Binding[k].Type = SBinding::EBT_ANIMATION_VALUE;
				Binding[k].Animation = animation;
			}
			if(!found) {
				Binding[k].DefaultValue = 0;
			}
		}
	}

	NeedsCompilation = false;
}

void
CDynamicAnimationSet::addAnimationLibraryBindings(const CColladaDatabase& database)
{
	AnimationSet.push_back(database);
	u32 k = Binding.size();
	int newBindingSize = k + AnimatedTrackCount;
	Binding.reallocate(newBindingSize);
	Binding.set_used(newBindingSize);
	for (u32 j = 0; j < AnimatedTrackCount; ++j, ++k)
	{
		const SAnimation* animation = database.getBlendableAnimation(&ChannelsCopy[j]);
		bool found = database.getDefaultValue(&ChannelsCopy[j], &Binding[k].DefaultValue);
		if (animation == 0)
		{
			Binding[k].Type = SBinding::EBT_DEFAULT_VALUE;
			Binding[k].Animation = 0;
			if (!found)
			{
				Binding[k].DefaultValue = 0;
			}
		}
		else
		{
			Binding[k].Type = SBinding::EBT_ANIMATION_VALUE;
			Binding[k].Animation = animation;
		}
		if(!found)	{
			Binding[k].DefaultValue = 0;
		}
	}
}

void
CDynamicAnimationSet::remAnimationLibraryBindings(CColladaDatabase& database)
{
	remAnimationLibraryBindings(getDatabaseIndex(database));
}

void
CDynamicAnimationSet::remAnimationLibraryBindings(u32 index)
{
	if (index >= AnimationSet.size())
	{
		return;
	}
	Binding.erase(index * AnimatedTrackCount, AnimatedTrackCount);
	AnimationSet.erase(index);
}

void
CDynamicAnimationSet::overwriteAnimationLibraryBindings(const CColladaDatabase& replace, CColladaDatabase& find)
{
	overwriteAnimationLibraryBindings(replace, getDatabaseIndex(find)); 
}

void
CDynamicAnimationSet::overwriteAnimationLibraryBindings(const CColladaDatabase& database, u32 index)
{
	if (index >= AnimationSet.size())
	{
		return;
	}
	CColladaDatabase oldDb = AnimationSet[index];
	AnimationSet[index] = database;
	for (u32 j = 0, k = index * AnimatedTrackCount; j < AnimatedTrackCount; ++j, ++k)
	{
		const SAnimation* animation = database.getBlendableAnimation(&ChannelsCopy[j]);
		bool found = database.getDefaultValue(&ChannelsCopy[j], &Binding[k].DefaultValue);
		if (animation == 0)
		{
			Binding[k].Type = SBinding::EBT_DEFAULT_VALUE;
			Binding[k].Animation = 0;
			if (!found)
			{
				Binding[k].DefaultValue = 0;
			}
		}
		else
		{
			Binding[k].Type = SBinding::EBT_ANIMATION_VALUE;
			Binding[k].Animation = animation;
		}
		if(!found)	{
			Binding[k].DefaultValue = 0;
		}
	}
}

};
};

#endif //#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

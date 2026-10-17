//-*-c++-*-
#ifndef __CCOLLADASCENENODEANIMATORBLENDER_H__
#define __CCOLLADASCENENODEANIMATORBLENDER_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaSceneNodeAnimator.h"
#include "irrArray.h"

namespace irr
{
namespace collada
{

class CSceneNodeAnimatorBlender
	: public ISceneNodeAnimator
{
protected:

	bool								NeedCompilation;
	core::array<ISceneNodeAnimator*>	Animators;
	core::array<float>					Weights;
	core::array<unsigned char>			DefaultBuffer;
	core::array<void*>					BlendingBuffers;
	core::array<void*>					Targets;

	virtual const SChannel& getAnimationTrack(int track);
	virtual const CAnimationTrackEx* getAnimationTrackEx(int track);
	virtual void setTarget(int ChannelID, void* Ptr);
	virtual const char* getBindURI(int i);
	
public:

	~CSceneNodeAnimatorBlender();

	virtual int getTargetCount();
	virtual int getTargetSize(int i);
	virtual int getTargetsSize();
	virtual int getLength() const;

	virtual void compile(core::array<unsigned char>* workingBuffer = 0);

	float getWeight(int index) const
	{
		return Weights[index];
	}

	void setWeight(int index, float weight)
	{
		Weights[index] = weight;
	}

	int getWeightIndex(ISceneNodeAnimator* animator)
	{
		if (NeedCompilation)
		{
			return -1; // Blending list is invalid.
		}

		for (u32 i = 0, cnt = Animators.size(); i < cnt; ++i)
		{
			if (Animators[i] == animator)
			{
				return i;
			}
		}

		return -2; // Not inside the blending list.
	}
	
	void normalizeWeights()
	{
		float total = 0;
		for (u32 i = 0, cnt = Weights.size(); i < cnt; ++i)
		{
			total += Weights[i];
		}

		if (total)
		{
			for (u32 j = 0, cnt = Weights.size(); j < cnt; ++j)
			{
				Weights[j] /= total;
			}
		}
		else
		{
			if (Weights.size() > 0)
			{
				Weights[0] = 1.0;
			}
		}
	}

	virtual void applyAnimationValues(u32 timeMs);

	virtual void computeAnimationValues(u32 timeMs);

	//! animates a scene node
	virtual void animateNode(scene::ISceneNode* node, u32 timeMs);

	int getAnimatorCount() const
	{
		return Animators.size();
	}

	//! Add a new channel animator to the current set.
	void addAnimator(ISceneNodeAnimator* animator)
	{
		NeedCompilation = true;
		animator->grab();
		Animators.push_back(animator);
	}

	////! Allow Animator to unlink with the node
	//virtual void onUnbind(scene::ISceneNode* node)
	//{
	//	
	//}
};

} // namespace collada
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif // __CCOLLADASCENENODEANIMATORBLENDER_H__

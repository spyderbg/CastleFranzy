#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaSceneNodeAnimatorBlender.h"

namespace irr
{
namespace collada
{

CSceneNodeAnimatorBlender::~CSceneNodeAnimatorBlender()
{
	for (u32 i = 0; i < Animators.size(); ++i)
	{
		Animators[i]->drop();
	}
}

const SChannel&
CSceneNodeAnimatorBlender::getAnimationTrack(int track)
{
	return Animators[0]->getAnimationTrack(track);
}

const CAnimationTrackEx*
CSceneNodeAnimatorBlender::getAnimationTrackEx(int track)
{
	return Animators[0]->getAnimationTrackEx(track);
}

void
CSceneNodeAnimatorBlender::setTarget(int channelID, void* ptr)
{
	Targets[channelID] = ptr;
}

const char*
CSceneNodeAnimatorBlender::getBindURI(int i)
{
	return Animators[0]->getBindURI(i);
}

int
CSceneNodeAnimatorBlender::getTargetCount()
{
	return Animators[0]->getTargetCount();
}

int
CSceneNodeAnimatorBlender::getTargetSize(int i)
{
	return Animators[0]->getTargetSize(i);
}

int
CSceneNodeAnimatorBlender::getTargetsSize()
{
	// Potential problem here the first animator may not be the bigger one
	return Animators[0]->getTargetsSize();
}

int
CSceneNodeAnimatorBlender::getLength() const
{
	return Animators[0]->getLength();
}

void
CSceneNodeAnimatorBlender::compile(core::array<unsigned char>* workingBuffer)
{
	u32 size = getTargetsSize();
	u32 animators = Animators.size();
	u32 workingBufferSize = size * animators;
	int targetCount = getTargetCount();
		
	//Prepare buffers
	if (!workingBuffer)
	{
		workingBuffer = &DefaultBuffer;
		workingBuffer->reallocate(workingBufferSize);
		workingBuffer->set_used(workingBufferSize);
	}
	_IRR_DEBUG_BREAK_IF(workingBuffer->size() < workingBufferSize);

	Weights.reallocate(animators);
	Weights.set_used(animators);
	for (u32 j = 0, cnt = Weights.size(); j < cnt; ++j)
	{
		Weights[j] = 0.0f;	
	}
	
	BlendingBuffers.reallocate(targetCount);
	BlendingBuffers.set_used(targetCount);

	u32 workingBufferIt = 0;

	// Set blending buffers pointers
	ISceneNodeAnimator* animatorModel = Animators[0];
	int targetTotalSize = 0;

	memset(workingBuffer->pointer(), 0, workingBuffer->size());
	for (int i = 0;
		 i < targetCount; 
		 i++, workingBufferIt += targetTotalSize)
	{
		int targetSize = getTargetSize(i);

		_IRR_DEBUG_BREAK_IF(targetSize & 0x3); // MUST be 4 bytes align

		targetTotalSize = targetSize * animators;
		BlendingBuffers[i] = workingBuffer->pointer() + workingBufferIt;
		u8* blendingTarget = (u8*)BlendingBuffers[i];
			
		const char* bindURI = animatorModel->getBindURI(i);

		animatorModel->setTarget(i, blendingTarget);
		blendingTarget += targetSize;

		const SChannel& modelTrack = animatorModel->getAnimationTrack(i);

		for(u32 i = 1, cnt = Animators.size();
			i < cnt;
			blendingTarget += targetSize, ++i)
		{
			ISceneNodeAnimator* anim = Animators[i];
			bool found = anim->setCompatibleTarget(modelTrack, blendingTarget);
			//_IRR_DEBUG_BREAK_IF(!found);
		}
	}

	_IRR_DEBUG_BREAK_IF(workingBufferIt > workingBufferSize);

	Targets.reallocate(targetCount);
	Targets.set_used(targetCount);

	NeedCompilation = false;

	if(Animators.size() != 0)
	{
		forceBind();
	}
}

void
CSceneNodeAnimatorBlender::applyAnimationValues(u32 timeMs)
{
	for (u32 i = 0, cnt = Animators.size(); i < cnt; ++i)
	{
		if (Weights[i])
		{
			Animators[i]->computeAnimationValues(timeMs);
		}
	}

	normalizeWeights();

	for (u32 animTrackIt = 0; animTrackIt < Targets.size(); ++animTrackIt)
	{
		if (Targets[animTrackIt])
		{
			const CAnimationTrackEx* animTrack
				= Animators[0]->getAnimationTrackEx(animTrackIt);
			animTrack->applyBlendedValue(BlendingBuffers[animTrackIt], 
										 Weights.pointer(),
										 Weights.size(), 
										 Targets[animTrackIt]);
		}
		else
		{
			//_IRR_DEBUG_BREAK_IF("NOT Supported");
		}
	}
}

void
CSceneNodeAnimatorBlender::computeAnimationValues(u32 timeMs)
{
	for (u32 i = 0; i < Animators.size(); ++i)
	{
		if (Weights[i])
		{
			Animators[i]->computeAnimationValues(timeMs);
		}
	}

	normalizeWeights();

	for (u32 animTrackIt = 0; animTrackIt < Targets.size(); ++animTrackIt)
	{
		if (Targets[animTrackIt])
		{
			const CAnimationTrackEx* animTrack
				= Animators[0]->getAnimationTrackEx(animTrackIt);
			animTrack->getBlendedValue(BlendingBuffers[animTrackIt], 
									   Weights.pointer(),
									   Weights.size(), 
									   Targets[animTrackIt]);
		}
		else
		{
			//_IRR_DEBUG_BREAK_IF("NOT Supported");
		}
	}
}

//! animates a scene node
void
CSceneNodeAnimatorBlender::animateNode(scene::ISceneNode* node, u32 timeMs)
{
	applyAnimationValues(timeMs);
}

} // end namespace collada
} // end namespace irr
#endif
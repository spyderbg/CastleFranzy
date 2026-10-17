#ifndef __CCHANNELBLENDER_H_INCLUDED__
#define __CCHANNELBLENDER_H_INCLUDED__

#include "IChannelAnimator.h"
#include "IAnimationTrack.h"
#include "CAnimationTrack.h"
#include "matrix4.h"
#include "ISceneNode.h"

namespace irr
{

namespace scene
{

class CChannelBlender
	: public IChannelAnimator
	, public virtual IReferenceCounted 
{
public:

	//! Update animated field(s)
	virtual void OnAnimate(s32 timeMs)
	{
		m_Animators[0].
	}

	int
	addAnimator(const float *pWeight, IChannelAnimator * pAnimator)
	{
		if(m_Animators.size() > 0)
		{
			_IRR_DEBUG_BREAK_IF(m_Animators[0]->GetBindURI() != pAnimator->GetBindURI());
		}
		else
		{
			
		}

		m_Weights.push_back(pWeight);
		m_Animators.push_back(m_Animators);

		void* target = irrnew u8[m_iTargetSize];
		m_Targets.push_back(target);
		pAnimator->Bind(target);
	}

	void
	removeAnimator(IChannelAnimator * pAnimator)
	{
		_IRR_DEBUG_BREAK_IF("Not implemented");
	}

	


protected:
	std::vector<const float *> m_Weights;
	std::vector<const IChannelAnimator *> m_Animators;
	std::vector<void *> m_Targets;
	int	m_iTargetSize;
}

}; // namespace scene

}; // namespace irr

#endif //__CCHANNELBLENDER_H_INCLUDED__

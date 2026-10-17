#ifndef __ICHANNELANIMATOR_H_INCLUDED__
#define __ICHANNELANIMATOR_H_INCLUDED__

#include "IAnimator.h"
#include "irrString.h"

namespace irr
{
namespace scene
{
	class IChannelAnimator : public IAnimator
	{
	public:
		IChannelAnimator() : pTarget(0) {}

		//! Update animated field(s)
		virtual void OnAnimate(s32 timeMs) = 0;

		//! Bind this animator to a given address
		virtual void bind(void *pTgt) 
		{ 
			pTarget = pTgt; 
		}

		//! Retrieve the object address is animated
		inline void * getTarget() const { return pTarget; }

		//! Retrieve the object URI
		virtual const core::stringc& getBindURI() const = 0;

	protected:
		//! Target pointer
		void * pTarget;
	};
};
};

#endif

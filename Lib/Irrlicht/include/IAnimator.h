#ifndef __I_ANIMATOR_H_INCLUDED__
#define __I_ANIMATOR_H_INCLUDED__

#include "IrrCompileConfig.h"
#include "irrTypes.h"

namespace irr
{
namespace scene
{
	class IAnimator
	{
		//! Update de animated field(s)
		virtual void OnAnimate(s32 timeMs) = 0;
	};
};
};

#endif
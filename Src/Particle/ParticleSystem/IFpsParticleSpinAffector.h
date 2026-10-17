// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_FPS_PARTICLE_SPIN_AFFECTOR_H_INCLUDED__
#define __I_FPS_PARTICLE_SPIN_AFFECTOR_H_INCLUDED__

#include "IFpsParticleAffector.h"

namespace irr
{

//! A particle affector which rotates the particle system.
class IFpsParticleSpinAffector : public IFpsParticleAffector
{
public:

	//! Set the min angle to spin
	virtual void setMinSpin( s32 minSpin ) = 0;

	//! Set the max angle to spin
	virtual void setMaxSpin( s32 maxSpin ) = 0;

	//! Sets the efect start time in % (calculated from particle life time)
	virtual void setStartTimePercent( f32 startTimePercent ) = 0;

	//! Sets the efect end time in % (calculated from particle life time)
	virtual void setEndTimePercent( f32 endTimePercent ) = 0;

	//! Gets the min spin angle
	virtual s32 getMinSpin() const = 0;

	//! Gets the max spin angle
	virtual s32 getMaxSpin() const = 0;

	//! Gets the efect start time in % (calculated from particle life time)
	virtual f32 getStartTimePercent() const = 0;

	//! Gets the efect end time in % (calculated from particle life time)
	virtual f32 getEndTimePercent() const = 0;


	//! Get emitter type
	virtual E_FPS_PARTICLE_AFFECTOR_TYPE getType() const { return FPS_EPAT_SPIN; }
};

}

#endif // __I_FPS_PARTICLE_SPIN_AFFECTOR_H_INCLUDED__


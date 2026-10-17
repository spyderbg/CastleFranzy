// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_FPS_PARTICLE_GRAVITY_AFFECTOR_H_INCLUDED__
#define __I_FPS_PARTICLE_GRAVITY_AFFECTOR_H_INCLUDED__

#include "IFpsParticleAffector.h"

namespace irr
{

//! A particle affector which applies gravity to particles.
class IFpsParticleGravityAffector : public IFpsParticleAffector
{
public:

	//! Sets the efect start time in % (calculated from particle life time)
	virtual void setStartTimePercent( f32 startTimePercent ) = 0;

	//! Sets the efect end time in % (calculated from particle life time)
	virtual void setEndTimePercent( f32 endTimePercent ) = 0;

	//! Set the direction and force of gravity in all 3 dimensions.
	virtual void setGravity( const core::vector3df& gravity ) = 0;

	//! Gets the efect start time in % (calculated from particle life time)
	virtual f32 getStartTimePercent() const = 0;

	//! Gets the efect end time in % (calculated from particle life time)
	virtual f32 getEndTimePercent() const = 0;

	//! Get the direction and force of gravity.
	virtual const core::vector3df& getGravity() const = 0;

	//! Get emitter type
	virtual E_FPS_PARTICLE_AFFECTOR_TYPE getType() const { return FPS_EPAT_GRAVITY; }
};

}

#endif // __I_FPS_PARTICLE_GRAVITY_AFFECTOR_H_INCLUDED__


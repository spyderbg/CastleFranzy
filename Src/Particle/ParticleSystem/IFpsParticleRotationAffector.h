// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_FPS_PARTICLE_ROTATION_AFFECTOR_H_INCLUDED__
#define __I_FPS_PARTICLE_ROTATION_AFFECTOR_H_INCLUDED__

#include "IFpsParticleAffector.h"

namespace irr
{

//! A particle affector which rotates the particle system.
class IFpsParticleRotationAffector : public IFpsParticleAffector
{
public:

	//! Set the point that particles will rotate around
	virtual void setPivotPoint( const core::vector3df& point ) = 0;

	//! Set the speed in degrees per second in all 3 dimensions
	virtual void setSpeed( const core::vector3df& speed ) = 0;

	//! Get the point that particles are attracted to
	virtual const core::vector3df& getPivotPoint() const = 0;

	//! Get the speed in degrees per second in all 3 dimensions
	virtual const core::vector3df& getSpeed() const = 0;

	//! Get emitter type
	virtual E_FPS_PARTICLE_AFFECTOR_TYPE getType() const { return FPS_EPAT_ROTATE; }
};

}
#endif // __I_FPS_PARTICLE_ROTATION_AFFECTOR_H_INCLUDED__


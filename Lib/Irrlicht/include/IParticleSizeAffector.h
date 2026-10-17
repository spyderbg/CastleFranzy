// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_PARTICLE_SIZE_AFFECTOR_H_INCLUDED__
#define __I_PARTICLE_SIZE_AFFECTOR_H_INCLUDED__

#include "IParticleAffector.h"

namespace irr
{
namespace scene
{

//! A particle affector which fades out the particles.
class IParticleSizeAffector : public IParticleAffector
{
public:

	//! Sets the target size, i.e. the size the particles will interpolate
	//! to over time.
	virtual void setTargetSize( f32 targetSize ) = 0;

	//! Sets the variation of size in percentage
	virtual void setVariation( f32 variation ) = 0;

	//! Sets the amount of time it takes for each particle to grow.
	virtual void setGrowForTime( f32 growForTime ) = 0;

	//! Sets the amount of time it takes for each particle to fade.
	virtual void setFadeForTime( f32 fadeForTime ) = 0;

	//! Gets the target size, i.e. the color the particles will interpolate
	//! to over time.
	virtual f32 getTargetSize() const = 0;

	//! Gets the variation of size in percentage
	virtual f32 getVariation()  const = 0;

	//! Gets the amount of time it takes for each particle to grow for.
	virtual f32 getGrowForTime() const = 0;

	//! Gets the amount of time it takes for each particle to fade for.
	virtual f32 getFadeForTime() const = 0;

	//! Get emitter type
	virtual E_PARTICLE_AFFECTOR_TYPE getType() const { return EPAT_SIZE; }
};

} // end namespace scene
} // end namespace irr


#endif // __I_PARTICLE_SIZE_AFFECTOR_H_INCLUDED__


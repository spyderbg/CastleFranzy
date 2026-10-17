// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_PARTICLE_SPIN_AFFECTOR_H_INCLUDED__
#define __I_PARTICLE_SPIN_AFFECTOR_H_INCLUDED__

#include "IParticleAffector.h"

namespace irr
{
namespace scene
{

//! A particle affector which rotates the particle system.
class IParticleSpinAffector : public IParticleAffector
{
public:

	//! Set the time needed to perform a full spin
	virtual void setSpinTime( f32 spinTime ) = 0;

	//! Sets the variation of time in percentage
	virtual void setVariation( f32 variation ) = 0;

	//! Gets the time needed to perform a full spin
	virtual f32 getSpinTime() const = 0;

	//! Gets the variation of time in percentage
	virtual f32 getVariation()  const = 0;


	//! Get emitter type
	virtual E_PARTICLE_AFFECTOR_TYPE getType() const { return EPAT_SPIN; }
};

} // end namespace scene
} // end namespace irr


#endif // __I_PARTICLE_SPIN_AFFECTOR_H_INCLUDED__


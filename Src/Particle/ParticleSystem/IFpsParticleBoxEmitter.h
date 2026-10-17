// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_FPS_PARTICLE_BOX_EMITTER_H_INCLUDED__
#define __I_FPS_PARTICLE_BOX_EMITTER_H_INCLUDED__

#include "IFpsParticleEmitter.h"
#include "aabbox3d.h"

namespace irr
{

//! A particle emitter which emits particles from a box shaped space
class IFpsParticleBoxEmitter : public IFpsParticleEmitter
{
public:

	//! Set the box shape
	virtual void setBox( const core::aabbox3df& box ) = 0;

	//! Get the box shape set
	virtual const core::aabbox3df& getBox() const = 0;

	//! Get emitter type
	virtual E_FPS_PARTICLE_EMITTER_TYPE getType() const { return FPS_EPET_BOX; }
};

}
#endif


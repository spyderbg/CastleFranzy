// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_FPS_PARTICLE_AFFECTOR_H_INCLUDED__
#define __I_FPS_PARTICLE_AFFECTOR_H_INCLUDED__

#include "IAttributeExchangingObject.h"
#include "SFpsParticle.h"

namespace irr
{

//! Types of billboards
enum E_FPS_PARTICLE_BILLBOARD_TYPE
{
	EPBT_CAMERA_FACING = 0,
	EPBT_UP_VECTOR_FACING,
	EPBT_COUNT
};


//! Types of built in particle affectors
enum E_FPS_PARTICLE_AFFECTOR_TYPE
{
	FPS_EPAT_NONE = 0,
	FPS_EPAT_ATTRACT,
	FPS_EPAT_FADE_OUT,
	FPS_EPAT_GRAVITY,
	FPS_EPAT_ROTATE,
	FPS_EPAT_SIZE,
	FPS_EPAT_SPIN,
	FPS_EPAT_COUNT
};

//! Names for built in particle affectors
const c8* const FpsParticleAffectorTypeNames[] =
{
	"None",
	"Attract",
	"FadeOut",
	"Gravity",
	"Rotate",
	"Size",
	"Spin",
	0
};

//! A particle affector modifies particles.
class IFpsParticleAffector : public virtual io::IAttributeExchangingObject
{
public:

	//! constructor
	IFpsParticleAffector() : Enabled(true) {}

	//! Affects an array of particles.
	//! \param now: Current time. (Same as ITimer::getTime() would return)
	//! \param particlearray: Array of particles.
	//! \param count: Amount of particles in array.
	virtual void affect(u32 lastTime, u32 now, SFpsParticle* particlearray, u32 count, float scale, const irr::core::quaternion& rotation) = 0;

	//! Sets whether or not the affector is currently enabled.
	virtual void setEnabled(bool enabled) { Enabled = enabled; }

	//! Gets whether or not the affector is currently enabled.
	virtual bool getEnabled() const { return Enabled; }

	//! Writes attributes of the object.
	//! Implement this to expose the attributes of your scene node animator for
	//! scripting languages, editors, debuggers or xml serialization purposes.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const {}

	//! Reads attributes of the object.
	//! Implement this to set the attributes of your scene node animator for
	//! scripting languages, editors, debuggers or xml deserialization purposes.
	//! \param startIndex: start index where to start reading attributes.
	//! \param in: The attributes to work with.
	//! \param options: Additional options.
	//! \return: returns last index of an attribute read by this affector
	virtual s32 deserializeAttributes(s32 startIndex, io::IAttributes* in, io::SAttributeReadWriteOptions* options=0) { return 0; }

	//! Get emitter type
	virtual E_FPS_PARTICLE_AFFECTOR_TYPE getType() const = 0;

protected:
	bool Enabled;
};

}
#endif

 
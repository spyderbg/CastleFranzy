// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_FPS_PARTICLE_ROTATION_AFFECTOR_H_INCLUDED__
#define __C_FPS_PARTICLE_ROTATION_AFFECTOR_H_INCLUDED__

#include "IFpsParticleRotationAffector.h"

namespace irr
{

//! Particle Affector for rotating particles about a point
class CFpsParticleRotationAffector : public IFpsParticleRotationAffector
{
public:

	CFpsParticleRotationAffector( const core::vector3df& speed = core::vector3df(5.0f, 5.0f, 5.0f),
		const core::vector3df& point = core::vector3df() );

	//! Affects a particle.
	virtual void affect(u32 lastTime, u32 now, SFpsParticle* particlearray, u32 count, float scale, const irr::core::quaternion& rotation);

	//! Set the point that particles will attract to
	virtual void setPivotPoint( const core::vector3df& point ) { PivotPoint = point; }

	//! Set the speed in degrees per second
	virtual void setSpeed( const core::vector3df& speed ) { Speed = speed; }

	//! Get the point that particles are attracted to
	virtual const core::vector3df& getPivotPoint() const { return PivotPoint; }

	//! Get the speed in degrees per second
	virtual const core::vector3df& getSpeed() const { return Speed; }

	//! Writes attributes of the object.
	//! Implement this to expose the attributes of your scene node animator for 
	//! scripting languages, editors, debuggers or xml serialization purposes.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const;

	//! Reads attributes of the object.
	//! Implement this to set the attributes of your scene node animator for 
	//! scripting languages, editors, debuggers or xml deserialization purposes.
	//! \param startIndex: start index where to start reading attributes.
	//! \return: returns last index of an attribute read by this affector
	virtual s32 deserializeAttributes(s32 startIndex, io::IAttributes* in, io::SAttributeReadWriteOptions* options);

private:

	core::vector3df PivotPoint;
	core::vector3df Speed;
	u32 LastTime;
};

}
#endif // __C_FPS_PARTICLE_ROTATION_AFFECTOR_H_INCLUDED__


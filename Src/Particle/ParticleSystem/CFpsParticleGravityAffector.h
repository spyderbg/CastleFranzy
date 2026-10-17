// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_FPS_PARTICLE_GRAVITY_AFFECTOR_H_INCLUDED__
#define __C_FPS_PARTICLE_GRAVITY_AFFECTOR_H_INCLUDED__

#include "IFpsParticleGravityAffector.h"
#include "SColor.h"

namespace irr
{

//! Particle Affector for affecting direction of particle
class CFpsParticleGravityAffector : public IFpsParticleGravityAffector
{
public:

	CFpsParticleGravityAffector(
		const core::vector3df& gravity = core::vector3df(0.0f,0.0f,-0.03f),
		f32 startTimePercent = 0.f, f32 endTimePercent = 1.f);

	//! Affects a particle.
	virtual void affect(u32 lastTime, u32 now, SFpsParticle* particlearray, u32 count, float scale, const irr::core::quaternion& rotation);

	//! Sets the efect start time in % (calculated from particle life time)
	virtual void setStartTimePercent( f32 startTimePercent ) { StartTimePercent = startTimePercent; }

	//! Sets the efect end time in % (calculated from particle life time)
	virtual void setEndTimePercent( f32 endTimePercent ) { EndTimePercent = endTimePercent; }

	//! Set the direction and force of gravity.
	virtual void setGravity( const core::vector3df& gravity ) { Gravity = gravity; }

	//! Gets the efect start time in % (calculated from particle life time)
	virtual f32 getStartTimePercent() const { return StartTimePercent; }

	//! Gets the efect end time in % (calculated from particle life time)
	virtual f32 getEndTimePercent() const { return EndTimePercent; }

	//! Set the direction and force of gravity.
	virtual const core::vector3df& getGravity() const { return Gravity; }

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

	core::vector3df Gravity;
	f32 StartTimePercent;
	f32 EndTimePercent;
};

}
#endif


// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_FPS_PARTICLE_SPIN_AFFECTOR_H_INCLUDED__
#define __C_FPS_PARTICLE_SPIN_AFFECTOR_H_INCLUDED__

#include "IFpsParticleSpinAffector.h"

namespace irr
{

//! Particle Affector for rotating particles about a point
class CFpsParticleSpinAffector : public IFpsParticleSpinAffector
{
public:

	CFpsParticleSpinAffector(s32 minSpin, s32 maxSpin, f32 startTimePercent, f32 endTimePercent);

	//! Affects a particle.
	virtual void affect(u32 lastTime, u32 now, SFpsParticle* particlearray, u32 count, float scale, const irr::core::quaternion& rotation);

	//! Set the min angle to spin
	virtual void setMinSpin( s32 minSpin ) { MinSpin = minSpin; }

	//! Set the max angle to spin
	virtual void setMaxSpin( s32 maxSpin ) { MaxSpin = maxSpin; }

	//! Sets the efect start time in % (calculated from particle life time)
	virtual void setStartTimePercent( f32 startTimePercent ) { StartTimePercent = startTimePercent; }

	//! Sets the efect end time in % (calculated from particle life time)
	virtual void setEndTimePercent( f32 endTimePercent ) { EndTimePercent = endTimePercent; }

	//! Gets the min spin angle
	virtual s32 getMinSpin() const { return MinSpin; }

	//! Gets the max spin angle
	virtual s32 getMaxSpin() const { return MaxSpin; }

	//! Gets the efect start time in % (calculated from particle life time)
	virtual f32 getStartTimePercent() const { return StartTimePercent; }

	//! Gets the efect end time in % (calculated from particle life time)
	virtual f32 getEndTimePercent() const { return EndTimePercent; }

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

	s32 MinSpin;
	s32 MaxSpin;
	f32 StartTimePercent;
	f32 EndTimePercent;
};

}
#endif // __C_FPS_PARTICLE_SPIN_AFFECTOR_H_INCLUDED__


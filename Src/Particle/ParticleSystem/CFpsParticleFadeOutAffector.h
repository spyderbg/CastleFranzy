// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_FPS_PARTICLE_FADE_OUT_AFFECTOR_H_INCLUDED__
#define __C_FPS_PARTICLE_FADE_OUT_AFFECTOR_H_INCLUDED__

#include "IFpsParticleFadeOutAffector.h"
#include "SColor.h"

namespace irr
{

//! Particle Affector for fading out a color
class CFpsParticleFadeOutAffector : public IFpsParticleFadeOutAffector
{
public:

	CFpsParticleFadeOutAffector(const video::SColor& targetColor, f32 startTimePercent, f32 endTimePercent);

	//! Affects a particle.
	virtual void affect(u32 lastTime, u32 now, SFpsParticle* particlearray, u32 count, float scale, const irr::core::quaternion& rotation);

	//! Sets the targetColor, i.e. the color the particles will interpolate
	//! to over time.
	virtual void setTargetColor( const video::SColor& targetColor ) { TargetColor = targetColor; }

	//! Sets the efect start time in % (calculated from particle life time)
	virtual void setStartTimePercent( f32 startTimePercent ) { StartTimePercent = startTimePercent; }

	//! Sets the efect end time in % (calculated from particle life time)
	virtual void setEndTimePercent( f32 endTimePercent ) { EndTimePercent = endTimePercent; }

	//! Sets the targetColor, i.e. the color the particles will interpolate
	//! to over time.
	virtual const video::SColor& getTargetColor() const { return TargetColor; }

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

	video::SColor TargetColor;
	f32 StartTimePercent;
	f32 EndTimePercent;
};

}
#endif


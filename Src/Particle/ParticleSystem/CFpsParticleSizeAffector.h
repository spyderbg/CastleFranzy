// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_FPS_PARTICLE_SIZE_AFFECTOR_H_INCLUDED__
#define __C_FPS_PARTICLE_SIZE_AFFECTOR_H_INCLUDED__

#include "IFpsParticleSizeAffector.h"

namespace irr
{


//! Particle Affector for fading out a color
class CFpsParticleSizeAffector : public IFpsParticleSizeAffector
{
public:

	CFpsParticleSizeAffector(f32 targetWidth, f32 targetHeight, u32 variation, f32 startTimePercent, f32 endTimePercent);

	//! Affects a particle.
	virtual void affect(u32 lastTime, u32 now, SFpsParticle* particlearray, u32 count, float scale, const irr::core::quaternion& rotation);

	//! Sets the target width, i.e. the width the particles will interpolate
	//! to over time.
	virtual void setTargetWidth( f32 targetWidth ) { TargetWidth = targetWidth; }

	//! Sets the target height, i.e. the height the particles will interpolate
	//! to over time.
	virtual void setTargetHeight( f32 targetHeight ) { TargetHeight = targetHeight; }

	//! Sets the variation of size in percentage
	virtual void setVariation( s32 variation ) { Variation = variation; }

	//! Sets the efect start time in % (calculated from particle life time)
	virtual void setStartTimePercent( f32 startTimePercent ) { StartTimePercent = startTimePercent; }

	//! Sets the efect end time in % (calculated from particle life time)
	virtual void setEndTimePercent( f32 endTimePercent ) { EndTimePercent = endTimePercent; }

	//! Gets the target width, i.e. the width the particles will interpolate
	//! to over time.
	virtual f32 getTargetWidth()  const { return TargetWidth; }

	//! Gets the target height, i.e. the height the particles will interpolate
	//! to over time.
	virtual f32 getTargetHeight()  const { return TargetHeight; }

	//! Gets the variation of size in percentage
	virtual s32 getVariation()  const { return Variation; }

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

	f32 TargetWidth;
	f32 TargetHeight;
	s32 Variation;
	f32 StartTimePercent;
	f32 EndTimePercent;
};

}
#endif


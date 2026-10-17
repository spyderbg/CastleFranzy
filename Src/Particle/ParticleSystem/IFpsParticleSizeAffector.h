// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_FPS_PARTICLE_SIZE_AFFECTOR_H_INCLUDED__
#define __I_FPS_PARTICLE_SIZE_AFFECTOR_H_INCLUDED__

#include "IFpsParticleAffector.h"

namespace irr
{

//! A particle affector which fades out the particles.
class IFpsParticleSizeAffector : public IFpsParticleAffector
{
public:

	//! Sets the target width, i.e. the width the particles will interpolate
	//! to over time.
	virtual void setTargetWidth( f32 targetWidth ) = 0;

	//! Sets the target height, i.e. the height the particles will interpolate
	//! to over time.
	virtual void setTargetHeight( f32 targetHeight ) = 0;

	//! Sets the variation of size in percentage
	virtual void setVariation( s32 variation ) = 0;

	//! Sets the efect start time in % (calculated from particle life time)
	virtual void setStartTimePercent( f32 startTimePercent ) = 0;

	//! Sets the efect end time in % (calculated from particle life time)
	virtual void setEndTimePercent( f32 endTimePercent ) = 0;

	//! Gets the target width, i.e. the width the particles will interpolate
	//! to over time.
	virtual f32 getTargetWidth() const = 0;

	//! Gets the target height, i.e. the height the particles will interpolate
	//! to over time.
	virtual f32 getTargetHeight() const = 0;

	//! Gets the variation of size in percentage
	virtual s32 getVariation()  const = 0;

	//! Gets the efect start time in % (calculated from particle life time)
	virtual f32 getStartTimePercent() const = 0;

	//! Gets the efect end time in % (calculated from particle life time)
	virtual f32 getEndTimePercent() const = 0;

	//! Get emitter type
	virtual E_FPS_PARTICLE_AFFECTOR_TYPE getType() const { return FPS_EPAT_SIZE; }
};

}
#endif // __I_FPS_PARTICLE_SIZE_AFFECTOR_H_INCLUDED__


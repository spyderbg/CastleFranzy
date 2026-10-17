// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_FPS_PARTICLE_FADE_OUT_AFFECTOR_H_INCLUDED__
#define __I_FPS_PARTICLE_FADE_OUT_AFFECTOR_H_INCLUDED__

#include "IFpsParticleAffector.h"

namespace irr
{

//! A particle affector which fades out the particles.
class IFpsParticleFadeOutAffector : public IFpsParticleAffector
{
public:

	//! Sets the targetColor, i.e. the color the particles will interpolate
	//! to over time.
	virtual void setTargetColor( const video::SColor& targetColor ) = 0;

	//! Sets the efect start time in % (calculated from particle life time)
	virtual void setStartTimePercent( f32 startTimePercent ) = 0;

	//! Sets the efect end time in % (calculated from particle life time)
	virtual void setEndTimePercent( f32 endTimePercent ) = 0;

	//! Gets the targetColor, i.e. the color the particles will interpolate
	//! to over time.
	virtual const video::SColor& getTargetColor() const = 0;

	//! Gets the efect start time in % (calculated from particle life time)
	virtual f32 getStartTimePercent() const  = 0;

	//! Gets the efect end time in % (calculated from particle life time)
	virtual f32 getEndTimePercent() const = 0;

	//! Get emitter type
	virtual E_FPS_PARTICLE_AFFECTOR_TYPE getType() const { return FPS_EPAT_FADE_OUT; }
};

}

#endif // __I_FPS_PARTICLE_FADE_OUT_AFFECTOR_H_INCLUDED__


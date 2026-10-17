// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_FPS_PARTICLE_H_INCLUDED__
#define __S_FPS_PARTICLE_H_INCLUDED__

#include "vector3d.h"
#include "SColor.h"

namespace irr
{

//! Struct for holding particle data
struct SFpsParticle
{

	//! Position of the particle
	core::vector3df pos;

	//! Old Position of the particle
	core::vector3df oldPos;

	//! Direction and speed of the particle
	core::vector3df vector;

	//! Start life time of the particle
	u32 startTime;

	//! End life time of the particle
	u32 endTime;

	//! Current color of the particle
	video::SColor color;

	//! Original color of the particle.
	/** That's the color of the particle it had when it was emitted. */
	video::SColor startColor;

	//! Original direction and speed of the particle.
	/** The direction and speed the particle had when it was emitted. */
	core::vector3df startVector;

	// spin angle of the particle in radians
	f32 spin;

	// target spin angle of the particle in radians
	f32 targetSpin;

	// initial spin angle of the particle in radians
	f32 initialSpin;

	// spin time variation
	f32 spinVariation;

	// particle width
	f32 width;

	// particle height
	f32 height;

	// particle target width delta - used for scale affector
	f32 targetWidthDelta;

	// particle target height delta  - used for scale affector
	f32 targetHeightDelta;

	// particle start width - used for scale affector
	f32 startWidth;

	// particle start height - used for scale affector
	f32 startHeight;

	// particle initial rotation
	f32 initialRotation;
};

}
#endif


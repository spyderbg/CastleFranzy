// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_FPS_PARTICLE_BOX_EMITTER_H_INCLUDED__
#define __C_FPS_PARTICLE_BOX_EMITTER_H_INCLUDED__

#include "IFpsParticleBoxEmitter.h"
#include "irrArray.h"
#include "aabbox3d.h"

namespace irr
{

//! A default box emitter
class CFpsParticleBoxEmitter : public IFpsParticleBoxEmitter
{
public:

	//! constructor
	CFpsParticleBoxEmitter(
		const core::aabbox3df& box,
		const core::vector3df& direction = core::vector3df(0.0f,0.03f,0.0f),
		u32 speedVariation = 0,
		u32 minParticlesPerSecond = 20,
		u32 maxParticlesPerSecond = 40,
		f32 particleWidth = 5.f, f32 particleHeight = 5.f, s32 sizeVariation = 0,
		video::SColor minStartColor = video::SColor(255,0,0,0),
		video::SColor maxStartColor = video::SColor(255,255,255,255),
		u32 lifeTimeMin=2000,
		u32 lifeTimeMax=4000,
		s32 maxAngleDegreesXY=0, s32 maxAngleDegreesYZ=0, s32 maxAngleDegreesXZ=0,
		s32 initialRotMin = 0, s32 initialRotMax = 0);

	//! Prepares an array with new particles to emitt into the system
	//! and returns how much new particles there are.
	virtual s32 emitt(u32 now, u32 timeSinceLastCall, SFpsParticle*& outArray, float scale, const irr::core::quaternion& rotation);

	//! Set direction the emitter emits particles.
	virtual void setDirection( const core::vector3df& newDirection ) { Direction = newDirection; }

	//! Set speed variation.
	virtual void setSpeedVariation( u32 speedVariation ) { SpeedVariation = speedVariation; }

	//! Set minimum number of particles emitted per second.
	virtual void setMinParticlesPerSecond( u32 minPPS ) { MinParticlesPerSecond = minPPS; }

	//! Set maximum number of particles emitted per second.
	virtual void setMaxParticlesPerSecond( u32 maxPPS ) { MaxParticlesPerSecond = maxPPS; }

	//! Set minimum start color.
	virtual void setMinStartColor( const video::SColor& color ) { MinStartColor = color; }

	//! Set maximum start color.
	virtual void setMaxStartColor( const video::SColor& color ) { MaxStartColor = color; }

	//! Set box from which the particles are emitted.
	virtual void setBox( const core::aabbox3df& box ) { Box = box; }

	//! Gets direction the emitter emits particles.
	virtual const core::vector3df& getDirection() const { return Direction; }

	//! Gets speed variation.
	virtual u32 getSpeedVariation() const { return SpeedVariation; }

	//! Gets minimum number of particles emitted per second.
	virtual u32 getMinParticlesPerSecond() const { return MinParticlesPerSecond; }

	//! Gets maximum number of particles emitted per second.
	virtual u32 getMaxParticlesPerSecond() const { return MaxParticlesPerSecond; }

	//! Gets particles width
	virtual f32 getParticleWidth() const { return ParticleWidth; }

	//! Gets particles width
	virtual f32 getParticleHeight() const { return ParticleHeight; }

	//! Gets particles size variation
	virtual s32 getSizeVariation() const { return SizeVariation; }

	//! Gets minimum start color.
	virtual const video::SColor& getMinStartColor() const { return MinStartColor; }

	//! Gets maximum start color.
	virtual const video::SColor& getMaxStartColor() const { return MaxStartColor; }

	//! Get box from which the particles are emitted.
	virtual const core::aabbox3df& getBox() const { return Box; }

	//! Writes attributes of the object.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const;

	//! Reads attributes of the object.
	virtual s32 deserializeAttributes(s32 startIndex, io::IAttributes* in, io::SAttributeReadWriteOptions* options);

	//! Gets min life time
	virtual u32 getMinLifeTime() const { return MinLifeTime; }

	//! Gets max life time
	virtual u32 getMaxLifeTime() const { return MaxLifeTime; }

	//| Gets max angle in degrees for XY plane
	virtual s32 getMaxAngleDegreesXY() const { return MaxAngleDegreesXY; }

	//| Gets max angle in degrees for YZ plane
	virtual s32 getMaxAngleDegreesYZ() const { return MaxAngleDegreesYZ; }

	//| Gets max angle in degrees for XZ plane
	virtual s32 getMaxAngleDegreesXZ() const { return MaxAngleDegreesXZ; }

	//| Gets initial rotation min angle
	virtual s32 getInitialRotMin() const { return InitialRotMin; }

	//| Gets initial rotation max angle
	virtual s32 getInitialRotMax() const { return InitialRotMax; }

	virtual void Reset()	{Time = 0;}

private:

	core::array<SFpsParticle> Particles;
	core::aabbox3df Box;
	core::vector3df Direction;
	u32 SpeedVariation;
	u32 MinParticlesPerSecond, MaxParticlesPerSecond;
	u32 ParticleWidth, ParticleHeight;
	s32 SizeVariation;
	video::SColor MinStartColor, MaxStartColor;
	u32 MinLifeTime, MaxLifeTime;

	u32 Time;
	u32 Emitted;
	s32 MaxAngleDegreesXY;
	s32 MaxAngleDegreesYZ;
	s32 MaxAngleDegreesXZ;
	s32 InitialRotMin;
	s32 InitialRotMax;
};

}
#endif


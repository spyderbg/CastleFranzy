// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_PARTICLE_SIZE_AFFECTOR_H_INCLUDED__
#define __C_PARTICLE_SIZE_AFFECTOR_H_INCLUDED__

#include "IParticleSizeAffector.h"

namespace irr
{
namespace scene
{

//! Particle Affector for fading out a color
class CParticleSizeAffector : public IParticleSizeAffector
{
public:

	CParticleSizeAffector(f32 targetSize, f32 variation, u32 growForTime, u32 fadeForTime);

	//! Affects a particle.
	virtual void affect(u32 now, SParticle* particlearray, u32 count);

	//! Sets the target size, i.e. the size the particles will interpolate
	//! to over time.
	virtual void setTargetSize( f32 targetSize ) { TargetSize = targetSize; }

	//! Sets the variation of size in percentage
	virtual void setVariation( f32 variation ) { Variation = variation; }

	//! Sets the amount of time it takes for each particle to grow.
	virtual void setGrowForTime( f32 growForTime ) { GrowForTime = growForTime; }

	//! Sets the amount of time it takes for each particle to fade.
	virtual void setFadeForTime( f32 fadeForTime ) { FadeForTime = fadeForTime; }

	//! Gets the target size, i.e. the color the particles will interpolate
	//! to over time.
	virtual f32 getTargetSize()  const { return TargetSize; }

	//! Gets the variation of size in percentage
	virtual f32 getVariation()  const { return Variation; }

	//! Gets the amount of time it takes for each particle to grow for.
	virtual f32 getGrowForTime() const { return GrowForTime; }

	//! Gets the amount of time it takes for each particle to fade for.
	virtual f32 getFadeForTime() const { return FadeForTime; }

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

	f32 TargetSize;
	f32 Variation;
	f32 GrowForTime;
	f32 FadeForTime;
};

} // end namespace scene
} // end namespace irr


#endif


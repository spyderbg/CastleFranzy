// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_PARTICLE_SPIN_AFFECTOR_H_INCLUDED__
#define __C_PARTICLE_SPIN_AFFECTOR_H_INCLUDED__

#include "IParticleSpinAffector.h"

namespace irr
{
namespace scene
{

//! Particle Affector for rotating particles about a point
class CParticleSpinAffector : public IParticleSpinAffector
{
public:

	CParticleSpinAffector(u32 spinTime, f32 variation);

	//! Affects a particle.
	virtual void affect(u32 now, SParticle* particlearray, u32 count);

	//! Set the time needed to perform a full spin
	virtual void setSpinTime( f32 spinTime ) { SpinTime = static_cast<u32>(spinTime); }

	//! Sets the variation of time in percentage
	virtual void setVariation( f32 variation ) { Variation = variation; }

	//! Gets the time needed to perform a full spin
	virtual f32 getSpinTime() const { return static_cast<f32>(SpinTime); }

	//! Gets the variation of time in percentage
	virtual f32 getVariation()  const { return Variation; }

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

	u32 SpinTime;
	f32 Variation;
};

} // end namespace scene
} // end namespace irr


#endif // __C_PARTICLE_SPIN_AFFECTOR_H_INCLUDED__


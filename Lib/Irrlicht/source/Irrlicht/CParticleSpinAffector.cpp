#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CParticleSpinAffector.h"
#include "IAttributes.h"
#include "irros.h"

namespace irr
{
namespace scene
{

//! constructor
CParticleSpinAffector::CParticleSpinAffector( u32 spinTime, f32 variation )
	: IParticleSpinAffector(), Variation(variation)
{
	#ifdef _DEBUG
	setDebugName("CParticleSpinAffector");
	#endif

	SpinTime = spinTime ? spinTime : u32(1000);
}


//! Affects an array of particles.
void CParticleSpinAffector::affect(u32 now, SParticle* particlearray, u32 count)
{
	if (!Enabled)
		return;

	for (u32 i=0; i<count; ++i)
	{
		if (particlearray[i].startTime == now) {
			if (Variation > 0.0f)
			{
				int variation = static_cast<int>(Variation * 100);
				particlearray[i].spinVariation = (os::Randomizer::rand() % variation) / 100.0f;
			} 
			else
			{
				particlearray[i].spinVariation = 0.0f;
			}
		}

		particlearray[i].spin = 0.0f;

		int deltaTime = now - particlearray[i].startTime;
		int spinTime = static_cast<int>(SpinTime - ((SpinTime * particlearray[i].spinVariation) / 100.0f));

		if (spinTime > 0)
		{
			int offset = deltaTime % spinTime;
			particlearray[i].spin = (offset / (float)spinTime) * core::PI * 2.0f;
		}
	}
}


//! Writes attributes of the object.
//! Implement this to expose the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml serialization purposes.
void CParticleSpinAffector::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	out->addFloat("SpinTime", static_cast<f32>(SpinTime));
	out->addFloat("Variation", Variation);
}

//! Reads attributes of the object.
//! Implement this to set the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml deserialization purposes.
//! \param startIndex: start index where to start reading attributes.
//! \return: returns last index of an attribute read by this affector
s32 CParticleSpinAffector::deserializeAttributes(s32 startIndex, io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	const char* name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "SpinTime"))
		return startIndex; // attribute not valid
	SpinTime = static_cast<u32>(in->getAttributeAsFloat(startIndex));
	++startIndex;

	name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "Variation"))
		return startIndex; // attribute not valid
	Variation = in->getAttributeAsFloat(startIndex);
	++startIndex;


	return startIndex;
}


} // end namespace scene
} // end namespace irr


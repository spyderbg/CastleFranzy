#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CParticleSizeAffector.h"
#include "IAttributes.h"
#include "irros.h"

namespace irr
{
namespace scene
{

//! constructor
CParticleSizeAffector::CParticleSizeAffector(
	f32 targetSize, f32 variation, u32 growForTime,  u32 fadeForTime)
	: IParticleSizeAffector(), TargetSize(targetSize), Variation(variation)
{

	#ifdef _DEBUG
	setDebugName("CParticleSizeAffector");
	#endif

	GrowForTime = static_cast<f32>(growForTime);
	FadeForTime = static_cast<f32>(fadeForTime);
}


//! Affects an array of particles.
void CParticleSizeAffector::affect(u32 now, SParticle* particlearray, u32 count)
{
	if (!Enabled)
		return;
	f32 d;

	for (u32 i=0; i<count; ++i)
	{
		if (particlearray[i].startTime == now) {
			if (Variation > 0.0f)
			{
				int variation = static_cast<int>(Variation * 100);
				particlearray[i].sizeVariation = (os::Randomizer::rand() % variation) / 100.0f;
			} 
			else
			{
				particlearray[i].sizeVariation = 0.0f;
			}
		}

		float size = TargetSize - ((TargetSize * particlearray[i].sizeVariation) / 100.0f);
		
		particlearray[i].size = size;

		if (GrowForTime > 0.0f && now - particlearray[i].startTime < GrowForTime)
		{
			d = (now - particlearray[i].startTime) / GrowForTime;
			particlearray[i].size = size * d;
		}
		if (FadeForTime > 0.0f && particlearray[i].endTime - now < FadeForTime)
		{
			d = (particlearray[i].endTime - now) / FadeForTime;
			particlearray[i].size = size * d;
		}
	}
}


//! Writes attributes of the object.
//! Implement this to expose the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml serialization purposes.
void CParticleSizeAffector::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	out->addFloat("TargetSize", TargetSize);
	out->addFloat("Variation", Variation);
	out->addFloat("GrowForTime", GrowForTime);
	out->addFloat("FadeForTime", FadeForTime);
}

//! Reads attributes of the object.
//! Implement this to set the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml deserialization purposes.
//! \param startIndex: start index where to start reading attributes.
//! \return: returns last index of an attribute read by this affector
s32 CParticleSizeAffector::deserializeAttributes(s32 startIndex, io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	const char* name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "TargetSize"))
		return startIndex; // attribute not valid
	TargetSize = in->getAttributeAsFloat(startIndex);
	++startIndex;

	name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "Variation"))
		return startIndex; // attribute not valid
	Variation = in->getAttributeAsFloat(startIndex);
	++startIndex;

	name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "GrowForTime"))
		return startIndex; // attribute not valid
	GrowForTime = in->getAttributeAsFloat(startIndex);
	++startIndex;

	name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "FadeForTime"))
		return startIndex; // attribute not valid
	FadeForTime = in->getAttributeAsFloat(startIndex);
	++startIndex;


	return startIndex;
}


} // end namespace scene
} // end namespace irr


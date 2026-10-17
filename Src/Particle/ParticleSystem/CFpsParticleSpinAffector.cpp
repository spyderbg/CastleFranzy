// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "stdafx.h"
#include "CFpsParticleSpinAffector.h"
#include "IAttributes.h"

namespace irr
{

//! constructor
CFpsParticleSpinAffector::CFpsParticleSpinAffector( s32 minSpin, s32 maxSpin, f32 startTimePercent, f32 endTimePercent )
	: IFpsParticleSpinAffector(), MinSpin(minSpin), MaxSpin(maxSpin), StartTimePercent(startTimePercent), EndTimePercent(endTimePercent)
{
	#ifdef _DEBUG
	setDebugName("CFpsParticleSpinAffector");
	#endif
}


//! Affects an array of particles.
void CFpsParticleSpinAffector::affect(u32 lastTime, u32 now, SFpsParticle* particlearray, u32 count, float scale, const irr::core::quaternion& rotation)
{
	if (!Enabled)
		return;

	//for (u32 i=0; i<count; ++i)
	//{
	//	if (particlearray[i].startTime == now) {
	//		if (Variation > 0.0f)
	//		{
	//			int variation = static_cast<int>(Variation * 100);
	//			particlearray[i].spinVariation = (os::Randomizer::rand() % variation) / 100.0f;
	//		} 
	//		else
	//		{
	//			particlearray[i].spinVariation = 0.0f;
	//		}
	//	}

	//	particlearray[i].spin = 0.0f;

	//	int deltaTime = now - particlearray[i].startTime;
	//	int spinTime = static_cast<int>(SpinTime - ((SpinTime * particlearray[i].spinVariation) / 100.0f));

	//	if (spinTime > 0)
	//	{
	//		int offset = deltaTime % spinTime;
	//		particlearray[i].spin = (offset / (float)spinTime) * core::PI * 2.0f;
	//	}
	//}

	float d;
	for (u32 i=0; i<count; ++i)
	{
		u32 deltaTime = particlearray[i].endTime - particlearray[i].startTime;
		u32 startTime = particlearray[i].startTime + deltaTime * StartTimePercent;
		u32 endTime = particlearray[i].startTime + deltaTime * EndTimePercent;

		if (now >= startTime && (now <= endTime || lastTime < endTime))
		{
			//this is the first time the particle gets affected by this affector
			if (now == startTime || lastTime < startTime)
			{
				particlearray[i].targetSpin = MinSpin;

				int deltaSpin = MaxSpin - MinSpin;
				if (deltaSpin != 0)
				{
					particlearray[i].targetSpin += deltaSpin > 0 ? (os::Randomizer::rand() % deltaSpin) : -(os::Randomizer::rand() % deltaSpin);
				}

				particlearray[i].targetSpin *= core::DEGTORAD;
				particlearray[i].initialSpin = particlearray[i].spin;
			}

			if (startTime == endTime)
			{
				particlearray[i].spin = particlearray[i].targetSpin;
			}
			else
			if (startTime < endTime)
			{
				u32 time = now;
				if (time > endTime) time = endTime;

				d = (float)(time - startTime) / (endTime - startTime);
				
				particlearray[i].spin = particlearray[i].initialSpin + particlearray[i].targetSpin * d;
			}
		}
	}
}


//! Writes attributes of the object.
//! Implement this to expose the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml serialization purposes.
void CFpsParticleSpinAffector::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	out->addInt("MinSpin", MinSpin);
	out->addInt("MaxSpin", MaxSpin);
	out->addInt("StartTime(%)", StartTimePercent * 100);
	out->addInt("EndTime(%)", EndTimePercent * 100);
}

//! Reads attributes of the object.
//! Implement this to set the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml deserialization purposes.
//! \param startIndex: start index where to start reading attributes.
//! \return: returns last index of an attribute read by this affector
s32 CFpsParticleSpinAffector::deserializeAttributes(s32 startIndex, io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	const char* name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "MinSpin"))
		return startIndex; // attribute not valid
	MinSpin = in->getAttributeAsInt(startIndex);
	++startIndex;

	name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "MaxSpin"))
		return startIndex; // attribute not valid
	MaxSpin = in->getAttributeAsInt(startIndex);
	++startIndex;

	name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "StartTime(%)"))
		return startIndex; // attribute not valid
	StartTimePercent = in->getAttributeAsInt(startIndex) / 100.f;
	++startIndex;

	name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "EndTime(%)"))
		return startIndex; // attribute not valid
	EndTimePercent = in->getAttributeAsInt(startIndex) / 100.f;
	++startIndex;

	return startIndex;
}

}
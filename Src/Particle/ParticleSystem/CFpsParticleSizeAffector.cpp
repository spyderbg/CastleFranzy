// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "stdafx.h"
#include "CFpsParticleSizeAffector.h"
#include "IAttributes.h"

namespace irr
{

//! constructor
CFpsParticleSizeAffector::CFpsParticleSizeAffector(f32 targetWidth, f32 targetHeight,
												   u32 variation, f32 startTimePercent, f32 endTimePercent)
	: IFpsParticleSizeAffector(), TargetWidth(targetWidth), TargetHeight(targetHeight), Variation(variation),
	StartTimePercent(startTimePercent), EndTimePercent(endTimePercent)
{

	#ifdef _DEBUG
	setDebugName("CFpsParticleSizeAffector");
	#endif
}


//! Affects an array of particles.
void CFpsParticleSizeAffector::affect(u32 lastTime, u32 now, SFpsParticle* particlearray, u32 count, float scale, const irr::core::quaternion& rotation)
{
	if (!Enabled)
		return;
	f32 d;

	float myTargetWidth  = TargetWidth  * scale;
	float myTargetHeight = TargetHeight * scale;

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
				particlearray[i].startWidth = particlearray[i].width;
				particlearray[i].startHeight = particlearray[i].height;

				if (Variation > 0)
				{
					f32 rnd = ((os::Randomizer::rand() % (Variation << 1)) - Variation) * 0.01f;
					particlearray[i].targetWidthDelta = myTargetWidth + myTargetWidth * rnd - particlearray[i].startWidth;
					particlearray[i].targetHeightDelta =  myTargetHeight + myTargetHeight * rnd - particlearray[i].startHeight;
				} 
				else
				{
					particlearray[i].targetWidthDelta = myTargetWidth - particlearray[i].startWidth;
					particlearray[i].targetHeightDelta = myTargetHeight - particlearray[i].startHeight;
				}
			}

			if (startTime == endTime)
			{
				particlearray[i].width = particlearray[i].startWidth + particlearray[i].targetWidthDelta;
				particlearray[i].height = particlearray[i].startHeight + particlearray[i].targetHeightDelta;
			}
			else
			if (startTime < endTime)
			{
				u32 time = now;
				if (time > endTime) time = endTime;

				d = (float)(time - startTime) / (endTime - startTime);
				
				particlearray[i].width = particlearray[i].startWidth + particlearray[i].targetWidthDelta * d;
				particlearray[i].height = particlearray[i].startHeight + particlearray[i].targetHeightDelta * d;
			}

			//printf("i=%d startTime %d TargetWidth %f startWidth %f width %f\n", i, particlearray[i].startTime, TargetWidth, particlearray[i].startWidth, particlearray[i].width);
		}
	}
}


//! Writes attributes of the object.
//! Implement this to expose the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml serialization purposes.
void CFpsParticleSizeAffector::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	out->addFloat("TargetWidth", TargetWidth);
	out->addFloat("TargetHeight", TargetHeight);
	out->addInt("Variation", Variation);
	out->addInt("StartTime(%)", StartTimePercent * 100);
	out->addInt("EndTime(%)", EndTimePercent * 100);
}

//! Reads attributes of the object.
//! Implement this to set the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml deserialization purposes.
//! \param startIndex: start index where to start reading attributes.
//! \return: returns last index of an attribute read by this affector
s32 CFpsParticleSizeAffector::deserializeAttributes(s32 startIndex, io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	const char* name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "TargetWidth"))
		return startIndex; // attribute not valid
	TargetWidth = in->getAttributeAsFloat(startIndex);
	++startIndex;

	name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "TargetHeight"))
		return startIndex; // attribute not valid
	TargetHeight = in->getAttributeAsFloat(startIndex);
	++startIndex;

	name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "Variation"))
		return startIndex; // attribute not valid
	Variation = in->getAttributeAsInt(startIndex);
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
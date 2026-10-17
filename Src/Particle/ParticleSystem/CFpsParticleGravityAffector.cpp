// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "stdafx.h"
#include "CFpsParticleGravityAffector.h"
#include "IAttributes.h"

namespace irr
{

//! constructor
CFpsParticleGravityAffector::CFpsParticleGravityAffector(
	const core::vector3df& gravity, f32 startTimePercent, f32 endTimePercent)
	: IFpsParticleGravityAffector(), Gravity(gravity), StartTimePercent(startTimePercent), EndTimePercent(endTimePercent)
{
	#ifdef _DEBUG
	setDebugName("CFpsParticleGravityAffector");
	#endif
}


//! Affects an array of particles.
void CFpsParticleGravityAffector::affect(u32 lastTime, u32 now, SFpsParticle* particlearray, u32 count, float scale, const irr::core::quaternion& rotation)
{
	if (!Enabled)
		return;
	f32 d;

	//for (u32 i=0; i<count; ++i)
	//{
	//	d = (now - particlearray[i].startTime) / TimeForceLost;
	//	if (d > 1.0f)
	//		d = 1.0f;
	//	if (d < 0.0f)
	//		d = 0.0f;
	//	d = 1.0f - d;

	//	particlearray[i].vector = particlearray[i].startVector.getInterpolated(Gravity, d);
	//}

	core::vector3df myGravity = rotation * (Gravity * scale);

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
				particlearray[i].startVector = particlearray[i].vector;
			}

			if (startTime == endTime)
			{
				particlearray[i].vector = myGravity;
			}
			else
			if (startTime < endTime)
			{
				u32 time = now;
				if (time > endTime) time = endTime;

				d = (float)(time - startTime) / (endTime - startTime);
				
				particlearray[i].vector = particlearray[i].startVector.getInterpolated(myGravity, 1 - d);
			}
		}
	}
}

//! Writes attributes of the object.
void CFpsParticleGravityAffector::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	out->addVector3d("Gravity", Gravity);
	out->addInt("StartTime(%)", StartTimePercent * 100);
	out->addInt("EndTime(%)", EndTimePercent * 100);
}


//! Reads attributes of the object.
s32 CFpsParticleGravityAffector::deserializeAttributes(s32 startIndex, io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	const char* name = in->getAttributeName(startIndex);

	if (!name || strcmp(name, "Gravity"))
		return startIndex; // attribute not valid

	Gravity = in->getAttributeAsVector3d(startIndex);
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
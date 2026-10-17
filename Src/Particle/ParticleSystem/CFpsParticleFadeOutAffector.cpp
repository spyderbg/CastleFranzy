// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "stdafx.h"
#include "CFpsParticleFadeOutAffector.h"
#include "IAttributes.h"

namespace irr
{

//! constructor
CFpsParticleFadeOutAffector::CFpsParticleFadeOutAffector(
	const video::SColor& targetColor, f32 startTimePercent, f32 endTimePercent)
	: IFpsParticleFadeOutAffector(), TargetColor(targetColor), StartTimePercent(startTimePercent), EndTimePercent(endTimePercent)
{

	#ifdef _DEBUG
	setDebugName("CFpsParticleFadeOutAffector");
	#endif
}


//! Affects an array of particles.
void CFpsParticleFadeOutAffector::affect(u32 lastTime, u32 now, SFpsParticle* particlearray, u32 count, float scale, const irr::core::quaternion& rotation)
{
	if (!Enabled)
		return;
	f32 d;

	//for (u32 i=0; i<count; ++i)
	//{
	//	if (particlearray[i].endTime - now < FadeOutTime2)
	//	{
	//		if (FadeOutTime2 > 0)
	//		{
	//			d = (particlearray[i].endTime - now) / FadeOutTime2;
	//			particlearray[i].color = TargetColor.getInterpolated(
	//				TargetColor2, d);
	//		}
	//	}
	//	else if (particlearray[i].endTime - now < FadeOutTime + FadeOutTime2)
	//	{
	//		d = (particlearray[i].endTime - now - FadeOutTime2) / FadeOutTime;
	//		particlearray[i].color = particlearray[i].startColor.getInterpolated(
	//			TargetColor, d);
	//	}
	//}

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
				particlearray[i].startColor = particlearray[i].color;
			}

			if (startTime == endTime)
			{
				particlearray[i].color = TargetColor;
			}
			else
			if (startTime < endTime)
			{
				u32 time = now;
				if (time > endTime) time = endTime;

				d = (float)(time - startTime) / (endTime - startTime);
				
				particlearray[i].color = particlearray[i].startColor.getInterpolated(TargetColor, 1 - d);
			}
		}
	}
}


//! Writes attributes of the object.
//! Implement this to expose the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml serialization purposes.
void CFpsParticleFadeOutAffector::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	out->addColor("TargetColor", TargetColor);
	out->addInt("StartTime(%)", StartTimePercent * 100);
	out->addInt("EndTime(%)", EndTimePercent * 100);
}

//! Reads attributes of the object.
//! Implement this to set the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml deserialization purposes.
//! \param startIndex: start index where to start reading attributes.
//! \return: returns last index of an attribute read by this affector
s32 CFpsParticleFadeOutAffector::deserializeAttributes(s32 startIndex, io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	const char* name = in->getAttributeName(startIndex);

	if (!name || strcmp(name, "TargetColor"))
		return startIndex; // attribute not valid

	TargetColor = in->getAttributeAsColor(startIndex);
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
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "stdafx.h"
#include "IAttributes.h"
#include "CFpsParticleAttractionAffector.h"

namespace irr
{

//! constructor
CFpsParticleAttractionAffector::CFpsParticleAttractionAffector(
		const core::vector3df& point, f32 speed, bool attract,
		bool affectX, bool affectY, bool affectZ )
	: Point(point), Speed(speed), AffectX(affectX), AffectY(affectY),
		AffectZ(affectZ), Attract(attract), LastTime(0)
{
	#ifdef _DEBUG
	setDebugName("CFpsParticleAttractionAffector");
	#endif
}


//! Affects an array of particles.
void CFpsParticleAttractionAffector::affect(u32 lastTime, u32 now, SFpsParticle* particlearray, u32 count, float scale, const irr::core::quaternion& rotation)
{
	if( LastTime == 0 )
	{
		LastTime = now;
		return;
	}

	f32 timeDelta = ( now - LastTime ) / 1000.0f;
	LastTime = now;

	if( !Enabled )
		return;

	core::vector3df myPoint = rotation * (Point * scale);
	float mySpeed = Speed * scale;

	for(u32 i=0; i<count; ++i)
	{
		core::vector3df direction = (myPoint - particlearray[i].pos).normalize();
		direction *= mySpeed * timeDelta;

		if( !Attract )
			direction *= -1.0f;

		if( AffectX )
			particlearray[i].pos.X += direction.X;

		if( AffectY )
			particlearray[i].pos.Y += direction.Y;

		if( AffectZ )
			particlearray[i].pos.Z += direction.Z;
	}
}

//! Writes attributes of the object.
//! Implement this to expose the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml serialization purposes.
void CFpsParticleAttractionAffector::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	out->addVector3d("Point", Point);
	out->addFloat("Speed", Speed);
	out->addBool("Attract", Attract);
	out->addBool("AffectX", AffectX);
	out->addBool("AffectY", AffectY);
	out->addBool("AffectZ", AffectZ);
}

//! Reads attributes of the object.
//! Implement this to set the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml deserialization purposes.
//! \param startIndex: start index where to start reading attributes.
//! \return: returns last index of an attribute read by this affector
s32 CFpsParticleAttractionAffector::deserializeAttributes(s32 startIndex, io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	const char* name = in->getAttributeName(startIndex);

	if (!name || strcmp(name, "Point"))
		return startIndex; // attribute not valid

	Point = in->getAttributeAsVector3d(startIndex);
	++startIndex;

	name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "Speed"))
		return startIndex; // attribute not valid

	Speed = in->getAttributeAsFloat(startIndex);
	++startIndex;

	name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "Attract"))
		return startIndex; // attribute not valid

	Attract = in->getAttributeAsBool(startIndex);
	++startIndex;

	name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "AffectX"))
		return startIndex; // attribute not valid

	AffectX = in->getAttributeAsBool(startIndex);
	++startIndex;

	name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "AffectY"))
		return startIndex; // attribute not valid

	AffectY = in->getAttributeAsBool(startIndex);
	++startIndex;

	name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "AffectZ"))
		return startIndex; // attribute not valid

	AffectZ = in->getAttributeAsBool(startIndex);
	++startIndex;

	return startIndex;
}

}
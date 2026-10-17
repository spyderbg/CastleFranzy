// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "stdafx.h"
#include "IAttributes.h"
#include "CFpsParticleRotationAffector.h"

namespace irr
{

//! constructor
CFpsParticleRotationAffector::CFpsParticleRotationAffector( const core::vector3df& speed, const core::vector3df& pivotPoint )
		: PivotPoint(pivotPoint), Speed(speed), LastTime(0)
{
	#ifdef _DEBUG
	setDebugName("CFpsParticleRotationAffector");
	#endif
}


//! Affects an array of particles.
void CFpsParticleRotationAffector::affect(u32 lastTime, u32 now, SFpsParticle* particlearray, u32 count, float scale, const irr::core::quaternion& rotation)
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

	core::vector3df myPivotPoint = rotation * (PivotPoint * scale);
	core::vector3df mySpeed = rotation * (Speed * scale);

	for(u32 i=0; i<count; ++i)
	{
		if( mySpeed.X != 0.0f )
			particlearray[i].pos.rotateYZBy( timeDelta * mySpeed.X, myPivotPoint );

		if( mySpeed.Y != 0.0f )
			particlearray[i].pos.rotateXZBy( timeDelta * mySpeed.Y, myPivotPoint );

		if( mySpeed.Z != 0.0f )
			particlearray[i].pos.rotateXYBy( timeDelta * mySpeed.Z, myPivotPoint );
	}
}

//! Writes attributes of the object.
//! Implement this to expose the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml serialization purposes.
void CFpsParticleRotationAffector::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	out->addVector3d("PivotPoint", PivotPoint);
	out->addVector3d("Speed", Speed);
}

//! Reads attributes of the object.
//! Implement this to set the attributes of your scene node animator for
//! scripting languages, editors, debuggers or xml deserialization purposes.
//! \param startIndex: start index where to start reading attributes.
//! \return: returns last index of an attribute read by this affector
s32 CFpsParticleRotationAffector::deserializeAttributes(s32 startIndex, io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	const char* name = in->getAttributeName(startIndex);

	if (!name || strcmp(name, "PivotPoint"))
		return startIndex; // attribute not valid

	PivotPoint = in->getAttributeAsVector3d(startIndex);
	++startIndex;

	name = in->getAttributeName(startIndex);
	if (!name || strcmp(name, "Speed"))
		return startIndex; // attribute not valid

	Speed = in->getAttributeAsVector3d(startIndex);
	++startIndex;

	return startIndex;
}

}

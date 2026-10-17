#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSceneNodeAnimatorRotation.h"

namespace irr
{
namespace scene
{

//! constructor
CSceneNodeAnimatorRotation::CSceneNodeAnimatorRotation(u32 time, const core::quaternion& rotation)
: Rotation(rotation), StartTime(time)
{
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorRotation");
	#endif
}


//! destructor
CSceneNodeAnimatorRotation::~CSceneNodeAnimatorRotation()
{
}



//! animates a scene node
void CSceneNodeAnimatorRotation::animateNode(ISceneNode* node, u32 timeMs)
{
	if (node) // thanks to warui for this fix
	{
		u32 diffTime = timeMs - StartTime;

		if (diffTime != 0)
		{
			core::quaternion NewRotation = node->getRotation(); 
			NewRotation = NewRotation + (Rotation * ((diffTime)/10.0f)); 
			node->setRotation(NewRotation); 
			StartTime=timeMs; 
		}
	}
}


//! Writes attributes of the scene node animator.
void CSceneNodeAnimatorRotation::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	out->addQuaternion("Rotation", Rotation);
}

//! Reads attributes of the scene node animator.
void CSceneNodeAnimatorRotation::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	Rotation = in->getAttributeAsQuaternion("Rotation");
}

ISceneNodeAnimator* CSceneNodeAnimatorRotation::createClone()
{
	CSceneNodeAnimatorRotation * newAnimator = 
		irrnew CSceneNodeAnimatorRotation(StartTime, Rotation);

	return newAnimator;
}

} // end namespace scene
} // end namespace irr


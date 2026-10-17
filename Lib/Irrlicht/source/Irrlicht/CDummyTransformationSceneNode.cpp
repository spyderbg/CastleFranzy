#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CDummyTransformationSceneNode.h"

namespace irr
{
namespace scene
{

//! constructor
CDummyTransformationSceneNode::CDummyTransformationSceneNode(s32 id)
	: IDummyTransformationSceneNode(id)
{
	#ifdef _DEBUG
	setDebugName("CDummyTransformationSceneNode");
	#endif

	setAutomaticCulling(scene::EAC_OFF);
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CDummyTransformationSceneNode::getBoundingBox() const
{
	return Box;
}



//! Returns a reference to the current relative transformation matrix.
//! This is the matrix, this scene node uses instead of scale, translation
//! and rotation.
core::matrix4& CDummyTransformationSceneNode::getRelativeTransformationMatrix()
{
	return RelativeTransformationMatrix;
}


//! Returns the relative transformation of the scene node.
/*core::matrix4 CDummyTransformationSceneNode::getRelativeTransformation() const
{
	return RelativeTransformationMatrix;
}*/

void CDummyTransformationSceneNode::setRelativeTransformationMatrix(const core::matrix4 & mat)
{
	RelativeTransformationMatrix = mat;
	setPosition(mat.getTranslation());
	setRotation(mat);
	setScale(mat.getScale());
}

/*void CDummyTransformationSceneNode::setPosition(const core::vector3df& newpos)
{
	RelativeTransformationMatrix.setTranslation(newpos);
}*/

ISceneNode* CDummyTransformationSceneNode::clone()
{
	CDummyTransformationSceneNode * newNode = 
		irrnew CDummyTransformationSceneNode( ID );

	newNode->cloneMembers(this);

	newNode->RelativeTransformationMatrix = RelativeTransformationMatrix;
	newNode->Box = Box;

	return newNode;
}

} // end namespace scene
} // end namespace irr


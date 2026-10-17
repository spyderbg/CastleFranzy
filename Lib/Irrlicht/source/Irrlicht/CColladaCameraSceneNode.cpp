#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaCameraSceneNode.h"

namespace irr
{
namespace collada
{

CCameraSceneNode::CCameraSceneNode(const CColladaDatabase& database,
								   SCamera& carema)
	: scene::CCameraSceneNode(-1)
	, IObject(database)
	, Camera(carema)
	, TargetNode(0)
{
	#ifdef _DEBUG
	setDebugName("CColladaCameraSceneNode");
	#endif
	setUID(Camera.id);
	

	switch(database.getUpVector())
	{
	case collada::SConfig::X_UP:
		setUpVector(core::vector3df(1.0f, 0.0f, 0.0f));
		break;
	case collada::SConfig::Y_UP:
		setUpVector(core::vector3df(0.0f, 1.0f, 0.0f));
		break;
	case collada::SConfig::Z_UP:
		setUpVector(core::vector3df(0.0f, 0.0f, 1.0f));
		break;
	}

	if (!Camera.orthographic)
	{
		setFOV(atan(tan(Camera.xfov * core::DEGTORAD / 2.0f) / Camera.aspectRatio) * 2.0f);
		//setFOV(Camera.xfov * core::DEGTORAD / 1.33f);//Camera.aspectRatio);
	}
	else
	{
		IsOrthogonal = true;
		setAspectRatio(Camera.aspectRatio);
		setMAG(Camera.xmag / Camera.aspectRatio);
	}

	//setNearValue(m_Camera.znear);
	//setFarValue(m_Camera.zfar);
}

CCameraSceneNode::~CCameraSceneNode()
{
	setTarget(0);
}

scene::ESCENE_NODE_TYPE
CCameraSceneNode::getType() const
{
	return scene::ESNT_COLLADA_CAMERA;
}

const char*
CCameraSceneNode::getUID() const
{
	return Camera.id;
}

void
CCameraSceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
	{
		if (TargetNode)
		{
			Target = TargetNode->getAbsolutePosition();

			updateAbsolutePosition();
			core::vector3df vect = Target - getAbsolutePosition();
			RelativeRotation = vect;		
		}
		else
		{
			getAbsoluteTransformation().transformVect(Target, core::vector3df(0,0,-100.0f));
		}
	}

	scene::CCameraSceneNode::OnRegisterSceneNode();
}

} // namespace collada
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

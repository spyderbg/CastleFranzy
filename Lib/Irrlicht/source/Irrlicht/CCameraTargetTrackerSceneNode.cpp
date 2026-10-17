#include "stdafx.h"
#include "CCameraTargetTrackerSceneNode.h"

using namespace irr;
using namespace scene;

void CCameraTargetTrackerSceneNode::render(void* /*renderData*/)
{
	if(m_pTarget)
	{
		Target = m_pTarget->getAbsolutePosition();

		updateAbsolutePosition();
		core::vector3df vect = Target - getAbsolutePosition();
		RelativeRotation = vect;		
	}

	CCameraSceneNode::render();
}
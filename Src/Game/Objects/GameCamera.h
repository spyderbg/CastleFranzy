///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Core/irrlicht.h"

class CGameCamera
{
public:
	CGameCamera(ICameraSceneNode* camera, const vector3df & position, const vector3df & target);
	virtual ~CGameCamera();

	void	setActive();
	void	update();

private:
	ICameraSceneNode * m_CameraNode;

	vector3df		m_position;
	vector3df		m_target;

};


#include "stdafx.h"

#include "Core/Application.h"
#include "GameCamera.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

CGameCamera::CGameCamera(ICameraSceneNode* camera, const vector3df & position, const vector3df & target) :
	m_CameraNode(camera),
	m_position(position),
	m_target(target)
{
#ifdef OS_IPHONE
	m_CameraNode->setAspectRatio(320.0f/480.0f); //480/320	
#else
	m_CameraNode->setAspectRatio(480.0f/320.0f); //480/320	
#endif
	m_CameraNode->setFOV(PI/4.0f);	//FOV - 45 deg
	m_CameraNode->setNearValue(1.0f);	
	m_CameraNode->setFarValue(500.0f);
	m_CameraNode->setUpVector(vector3df(0, 0, 1));
	m_CameraNode->setTarget(m_target);
	m_CameraNode->setPosition(m_position);
}

CGameCamera::~CGameCamera()
{
}

void CGameCamera::setActive()
{
	Application::GetInstance()->GetSceneManager()->setActiveCamera(m_CameraNode);
}

void CGameCamera::update()
{
}

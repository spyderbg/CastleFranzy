///////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "GameObject.h"

///////////////////////////////////////////////////////////////////////

CGameObject::CGameObject( IAttributes* attributes )
{
	m_Device = Application::GetInstance()->GetDevice();
	m_SceneManager = Application::GetInstance()->GetSceneManager();
	m_Driver = Application::GetInstance()->GetDriver();
	m_Guienv = Application::GetInstance()->GetGUIEnv();

	m_animFrozen = false;
}

CGameObject::~CGameObject()
{
}

void CGameObject::update(f32 time)
{	
}

void CGameObject::draw()
{
}

void CGameObject::setVisible(bool visible) 
{
	m_Node->setVisible(visible);
}

bool CGameObject::isVisible()
{
	return m_Node->isVisible();
}

f32 CGameObject::distanceTo(CGameObject * target)
{
	return (m_Node->getAbsolutePosition() - target->m_Node->getAbsolutePosition()).getLength();
}

f32 CGameObject::distanceToXZ(CGameObject * target)
{
	v3f diff = m_Node->getAbsolutePosition() - target->m_Node->getAbsolutePosition();
	diff.Y = 0.0f;
	return diff.getLength();
}

void CGameObject::freezeAnimation(bool freeze)
{
	if (freeze == m_animFrozen)
		return;

	m_animFrozen = freeze;

	if (!m_Node || m_Node->getAnimators().getSize() == 0)
		return;

	ITimelineController * timelineCtrl = m_Node->getAnimators().getFirstElem()->getTimelineCtrl();
	if (freeze)
	{
		m_animOldScale = timelineCtrl->getScale();
		timelineCtrl->setScale(0.0f);
	}
	else
	{
		timelineCtrl->setScale( m_animOldScale );
	}
}

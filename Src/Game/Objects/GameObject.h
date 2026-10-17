///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Core/irrlicht.h"
#include "Game/Misc.h"

///////////////////////////////////////////////////////////////////////

class CGameObject
{
public:
	CGameObject(IAttributes * attributes);
	virtual ~CGameObject();

	virtual void		update(f32 time);
	virtual void		draw();

	virtual void		setVisible(bool visible);
	virtual bool		isVisible();

	virtual void		freezeAnimation(bool freeze);

	inline ISceneNode*	getSceneNode() { return m_Node; }
	inline void			setPosition(vector3df pos) { m_Node->setPosition(pos); }
	inline vector3df	getPosition() { return m_Node->getPosition(); }
	inline vector3df	getAbsolutePosition() { return m_Node->getAbsolutePosition(); }

	f32					distanceTo(CGameObject * target);
	f32					distanceToXZ(CGameObject * target);

protected:
	ISceneNode*			m_Node;

	IrrlichtDevice*		m_Device;
	ISceneManager*		m_SceneManager;
	IVideoDriver*		m_Driver;
	IGUIEnvironment*	m_Guienv;

	bool				m_animFrozen;
	f32					m_animOldScale;
};
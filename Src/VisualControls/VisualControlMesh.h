//---------------------------------------------------------------------------------------------------------
// Copyright (C) 2009 Gameloft. All rights reserved.
//---------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
#include "IVisualControl.h"
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

class irr::scene::ISceneNode;
//common collada mesh control
//must handle reserve/free memory ( this is not right but for my case is ok, thus I have to handle it manually )
class VisualControlMesh : public IVisualControl
{
public:
	//no action
	VisualControlMesh( s32 control_id = -1 );
	~VisualControlMesh( ) ;

	void SetMesh ( irr::scene::ISceneNode *node ) { m_node = node; }

	virtual void Draw();
	virtual int	Update( float dt );
		
	virtual void	SetEnabledTE(bool is_enabled) {} 
	virtual void	SetEnabled(bool is_enabled);
	virtual void	Reset() { }

	void SetScale( float scalar ) { m_sScale = scalar; } 

	//yaw pitch roll in rad
	inline void SetRotationRad( irr::core::vector3df rot ) { m_rotation = rot; }
	//yaw pitch roll in deg
	inline void SetRotationDeg( irr::core::vector3df rot ) { m_rotation = rot * DEGTORAD; }
	
	void			SetPos( p2i pos );
	
protected:
	bool					m_enabled;
	f32						m_sScale;
	irr::scene::ISceneNode *m_node;
	irr::core::vector3df    m_position;
	irr::core::vector3df	m_rotation;

};
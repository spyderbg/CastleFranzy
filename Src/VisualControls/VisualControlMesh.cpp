#include "stdafx.h"

#include "VisualControlMesh.h"
#include "CColladaInstancedSceneNode.h"
#include "Core/irrlicht.h"


VisualControlMesh::VisualControlMesh(s32 control_id) : 
	IVisualControl( -1 , control_id , -1 )
{
	IVisualControl::SetEnabledTE(false);
	SetSelected(false);

	m_sScale = 0.0f;
	m_node = NULL;
	m_enabled = false;
}

VisualControlMesh::~VisualControlMesh() 
{
	m_node = NULL;
}

void VisualControlMesh::Draw()
{
	//handled by scene manager
	if ( !m_enabled ) 
	{
		m_node->setVisible( false );		
	}
	else 
	{
		m_node->setVisible( true );

		m_position.X = -m_pos.X;
		m_position.Y = m_pos.Y;
		m_position.Z = -10;
		
		//update 
		m_node->setPosition( m_position );	
	}	
}

void	VisualControlMesh::SetPos( p2i pos )
{
	IVisualControl::SetPos( pos );

	m_position.X = -m_pos.X;
	m_position.Y = m_pos.Y;
	m_position.Z = -10;

	if ( m_node ) 
		m_node->setPosition( m_position );	
}


int	VisualControlMesh::Update( float dt )
{
	if ( !m_enabled ) 
		return -1; 

	m_frame_counter++;
	Transition_Update();
	
	//update position from the control
	m_position.X = -m_pos.X;
	m_position.Y = m_pos.Y;
	m_position.Z = -10;
		
	//update 
	m_node->setPosition( m_position );		
	m_node->setRotation( irr::core::quaternion( m_rotation ) );			
	m_node->setScale( vector3df( m_sScale, m_sScale, m_sScale ) );	

	m_node->updateAbsolutePosition();

	if ( (!m_bVisible) ) 
		return -1;

	if ( Transition_IsGoing() ) 
		return -1;	

	return -1; 
}

void VisualControlMesh::SetEnabled(bool is_enabled) 
{
	m_enabled = is_enabled; 
	//set m_node
	GX_ASSERT( m_node != NULL );
	m_node->setVisible( is_enabled ); 
}
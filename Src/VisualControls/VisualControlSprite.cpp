#include "stdafx.h"

#include "VisualControlSprite.h"

#include "Core/irrlicht.h"

#include "Sprites/ASprite.h"
#include "Sprites/AnimationInstance.h"


VisualControlSprite::VisualControlSprite(s32 control_id) : 
	IVisualControl( -1 , control_id , -1 ), 
	m_spriteInstance( NULL )
{
	IVisualControl::SetEnabledTE(false);
	SetSelected(false);

	m_bVisible = false;
}

VisualControlSprite::~VisualControlSprite() 
{
	SAFE_DEL ( m_spriteInstance );
}

void VisualControlSprite::SetAnimation( ASprite *sprite, int animId )
{
	if ( m_spriteInstance == NULL ) 
	{
		m_spriteInstance = NEW CSpriteInstance( sprite );
		m_spriteInstance->StartAnimation( animId );
	}
	else
	{
		//can't assign new animation, if one is already assigned
		GX_ASSERT(  false ); 
	}
}

void VisualControlSprite::Draw()
{
	if ( !m_bVisible )
	{
		return;
	}
	else
	{
		if ( m_spriteInstance )
		{
			ASprite::PushClipRect( m_clip );
			m_spriteInstance->SetPosition( m_pos.X, m_pos.Y, -10 );
			m_spriteInstance->Render();
			ASprite::PopClipRect( );
		}
	}
}

int	VisualControlSprite::Update( float dt )
{
	m_frame_counter++;
	Transition_Update();

	if ( (!m_bVisible) ) 
		return -1;

	if ( Transition_IsGoing() ) 
		return -1;	
	
	if ( m_spriteInstance )
	{
		m_spriteInstance->update(dt);
		
	}

	return -1; 
}
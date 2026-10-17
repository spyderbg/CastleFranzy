#include "stdafx.h"

#include "VisualControlEditBox.h"

#include "IVisualControl.h"
#include "VisualControlText.h"

#include "Sprites/ASprite.h"
#include "Sprites/ASpriteInstance.h"
#include "Sprites/AnimationInstance.h"


#define MAX_STRING_LENGTH 16

#define CURSOR_BLINK_INTERVAL 1000

#define PASSWORD_SYMBOL L"*"
#define PASSWORD_HIDE_INTERVAL 500


VisualControlEditBox::VisualControlEditBox( EditBoxType type, int action_id, int control_id , int click_sfx  ) :
IVisualControl( action_id, control_id, click_sfx ),
m_boxType( type ), 
m_bgColor( 0xFFFFFFFF ), 
m_spriteInstance( NULL )

{
	m_textControl = NEW VisualControlText();	
}


VisualControlEditBox::~VisualControlEditBox()
{
	SAFE_DEL( m_spriteInst );
	SAFE_DEL( m_textControl );
}

void VisualControlEditBox::SetArea( p2i pos, d2i size ) 
{ 
	SetPos(pos); 
	SetSize(size); 
	m_textControl->SetArea( pos, size );
}

void VisualControlEditBox::SetArea( int x, int y, int w, int h ) 
{ 
	SetPos(p2i(x,y)); 
	SetSize(d2i(w, h)); 
	m_textControl->SetArea( x, y, w, h );
} 

void VisualControlEditBox::SetArea( r2i area ) 
{ 
	SetPos(area.getCenter()); 
	SetSize(area.getSize());
	m_textControl->SetArea( area );
}

void VisualControlEditBox::SetParametars( u32 bgColor, TCHAR *fontName, ASprite *sprite, u32 bg_anim_id )
{	
	m_bgColor = bgColor; 
	m_textControl->SetFont( stringw( fontName ) ) ;

	m_spriteInstance = NEW CSpriteInstance( sprite );
	m_spriteInstance->StartAnimation( bg_anim_id );

}

void VisualControlEditBox::SetText( stringw text )
{
	m_textControl->SetText( text ) ;	
}

void VisualControlEditBox::PushSymbol( TCHAR symbol )
{
	if ( m_textControl->GetText().size() >= MAX_STRING_LENGTH ) 
		return;

	 m_textControl->GetText()+=symbol;
}

void VisualControlEditBox::PopSymbol()
{
	if (m_textControl->GetText().size() <= 0 ) 
		return;

	m_textControl->GetText().erase( m_textControl->GetText().size() - 1 );
}

void VisualControlEditBox::Draw()
{
	rect<s32> _2dRect( m_pos.X, m_pos.Y, m_pos.X + m_size.Width, m_pos.Y + m_size.Height );	

	//draw Background 
	m_spriteInstance->SetPosition(m_pos.X, m_pos.Y, 0);
	m_spriteInstance->SetAlpha(m_transition_alpha);
	m_spriteInstance->Render();
	
	//draw text control
	m_textControl->Draw();
}

int  VisualControlEditBox::Update( float dt )
{
	m_frame_counter++;
	Transition_Update();

	if (( !m_bVisible ) ) return -1;		

	//m_spriteInst->update(dt);

	//m_sprite->UpdateAnimation(m_animations[m_curr_anim]);

	//bool isAnimDone = m_sprite->IsAnimationDone(m_animations[m_curr_anim]);
	//bool isAnimDone = m_spriteInst->IsPlaying();

	// TODO: Spyder not appropriate comment
	//if ( (isAnimDone) && (m_curr_anim == button_anim_blink) )
	{
	//	m_TE.DisableAllOtherElements(false);
	//	SetCurrentAnim( button_anim_pressed );
	//	m_is_selected = true;
	//	return m_action_id;
	}

	//if ( m_curr_anim == button_anim_blink || Transition_IsGoing() )
	//	return -1;

	//if ( m_TE.IsNowReleased() )
	//{
	//	m_TE.DisableAllOtherElements(false);
	//	m_TE.ResetInput();

	//	if (!m_is_selected && m_curr_anim == button_anim_pressed)
	//	{
	//		SetCurrentAnim( button_anim_blink, false );

	//		if ( m_click_sfx_id != -1 )
	//			__MGR_SOUNDS()->play( m_click_sfx_id );
	//	}
	//}
	//else if ( m_TE.IsActive() )
	//{
	//	if (m_TE.IsNowActivated())
	//	{
	//		if (!m_is_selected)
	//		{
	//			m_TE.DisableAllOtherElements(true);

	//			SetCurrentAnim( button_anim_pressed, false );
	//		}
	//		return button_action_gain_focus;
	//	}
	//	else if (!m_TE.IsTouchInsideElement())
	//	{
	//		if (!m_is_selected)
	//		{
	//			m_TE.DisableAllOtherElements(false);
	//			if (m_TE_resetInputOnDragOut) m_TE.ResetInput();
	//			SetCurrentAnim( button_anim_normal, false );
	//		}
	//		return button_action_lose_focus;
	//	}
	//	else if (m_TE.IsTouchInsideElement())
	//	{
	//		if (!m_is_selected)
	//		{
	//			m_TE.DisableAllOtherElements(true);
	//			SetCurrentAnim( button_anim_pressed, false );
	//		}
	//		return button_action_gain_focus;
	//	}
	//}

	return -1;
}

void VisualControlEditBox::Reset()
{
	
}
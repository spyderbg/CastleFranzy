//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#include "stdafx.h"
//#include "CGame.h"
//#include "IO/Audio/SoundManager.h"
#include "VisualControlButton.h"

#include "Menus/MenuMain.h"

#include "Sprites/ASprite.h"
#include "Sprites/ASpriteInstance.h"
#include "Sprites/AnimationInstance.h"

#include "Common/StringTable.h"
#include "Fonts/FontManager.h"

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
VisualControlButton::VisualControlButton( int action_id, int control_id, int click_sfx ) :
IVisualControl( action_id, control_id, click_sfx )
{
	m_curr_anim = button_anim_normal;
	memset(m_animations, 0, sizeof(int) * button_anim_number);

	m_TE_resetInputOnDragOut = false;
	m_disabledTextAlpha = 128;

	m_keepSelection = false;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
VisualControlButton::~VisualControlButton()
{
	SAFE_DEL( m_spriteInst );
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void VisualControlButton::Draw()
{
	if (!m_bVisible) return;

	/// Visibility check
	r2i visibleRect = r2i(0, 0, GAME_SCREEN_W, GAME_SCREEN_H);
	visibleRect.clipAgainst(GetArea());
	if (visibleRect.getWidth() == 0 || visibleRect.getHeight() == 0)
		return;

	/* for Debug TE * /
	__IRR_DRIVER()->draw2DRectangle(SColor(0xFFFF0000), GetArea());
	/**/

	//GET_LIB3D->SetClipRect(m_clip.x, m_clip.y, m_clip.w, m_clip.h);

	ASprite::PushClipRect( m_clip );

	// Draw button background / animation
	//-------------------------------
	m_spriteInst->SetPosition(m_pos.X, m_pos.Y, 0);
	m_spriteInst->SetAlpha(m_transition_alpha);
	m_spriteInst->Render();

	/// Try to find frame rect

	ASprite* sprite = m_spriteInst->GetSprite();
	s16 anim_idx = m_spriteInst->GetCurrAnimIdx();
	s32 frame_idx = sprite->GetAnimFrame( anim_idx, m_spriteInst->GetLastPlayedFrameIdx() );
	
	irr::core::stringw text1;
	irr::core::stringw text2;

	if ( m_text1_info.m_textId != -1 ) 
		text1 = _TXT_INT( m_text1_info.m_textId ) ;
	else 
		text1 = m_text1_info.m_text;

	if ( m_text2_info.m_textId != -1 ) 
		text2 = _TXT_INT( m_text2_info.m_textId ) ;
	else 
		text2 = m_text2_info.m_text;

	// Draw Button text
	//-------------------------------
	if (text1.size() > 0)
	{
		int alpha = m_transition_alpha;
		if (!m_bEnabled) alpha = (alpha * m_disabledTextAlpha) >> 8;
		CLAMP_VAL(alpha, 0, 255);

		int y_press_offset = 0;//(m_curr_anim == button_anim_pressed) ? 1 : 0;

		CFont * font = (m_curr_anim == button_anim_pressed) ? 
						__FONT(m_text1_info.m_font_pressed_id) : 
						__FONT(m_text1_info.m_font_normal_id);
				
		s32 x = m_pos.X + m_text1_info.m_text_pos.x;
		s32 y = m_pos.Y + m_text1_info.m_text_pos.y + y_press_offset;
		s32 w = m_text1_info.m_text_pos.w ? m_text1_info.m_text_pos.w : m_size.Width;

		if( frame_idx >= 0 && sprite->GetFrameRectsNum( frame_idx ) > 0 )
		{
			r2i rect;

			sprite->GetFrameRect( rect, frame_idx, 0 );
			
			//align in the frame area
			if ( m_text1_info.m_textFlags & CFont::LEFT )
				x = m_pos.X + rect.UpperLeftCorner.X ;
			else if ( m_text1_info.m_textFlags & CFont::RIGHT ) 
				x = m_pos.X +  rect.LowerRightCorner.X ;
			else
			{
				x = m_pos.X + (rect.UpperLeftCorner.X + rect.LowerRightCorner.X) / 2;				
			}

			y = m_pos.Y + (rect.UpperLeftCorner.Y + rect.LowerRightCorner.Y) / 2;


			w = rect.LowerRightCorner.X - rect.UpperLeftCorner.X;
		}

		font->DrawWrappedText(text1.c_str(), x, y, w, m_text1_info.m_textFlags, alpha);
	}


	if (text2.size() > 0)
	{
		int alpha = m_transition_alpha;
		if (!m_bEnabled) alpha = (alpha * m_disabledTextAlpha) >> 8;
		CLAMP_VAL(alpha, 0, 255);

		int y_press_offset = 0;//(m_curr_anim == button_anim_pressed) ? 1 : 0;

		CFont * font = (m_curr_anim == button_anim_pressed) ? 
						__FONT(m_text2_info.m_font_pressed_id) : 
						__FONT(m_text2_info.m_font_normal_id);
				
		s32 x = m_pos.X + m_text2_info.m_text_pos.x;
		s32 y = m_pos.Y + m_text2_info.m_text_pos.y + y_press_offset;
		s32 w = m_text2_info.m_text_pos.w ? m_text2_info.m_text_pos.w : m_size.Width;

		if( frame_idx >= 0 && sprite->GetFrameRectsNum( frame_idx ) > 1 )
		{
			r2i rect;

			sprite->GetFrameRect( rect, frame_idx, 1 );
			
			//align in the frame area
			if ( m_text2_info.m_textFlags & CFont::LEFT )
				x = m_pos.X + rect.UpperLeftCorner.X ;
			else if ( m_text2_info.m_textFlags & CFont::RIGHT ) 
				x = m_pos.X +  rect.LowerRightCorner.X ;
			else
			{
				x = m_pos.X + (rect.UpperLeftCorner.X + rect.LowerRightCorner.X) / 2;				
			}

			y = m_pos.Y + (rect.UpperLeftCorner.Y + rect.LowerRightCorner.Y) / 2;


			w = rect.LowerRightCorner.X - rect.UpperLeftCorner.X;
		}

		font->DrawWrappedText(text2.c_str(), x, y, w, m_text2_info.m_textFlags, alpha);
	}
	ASprite::PopClipRect();
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int VisualControlButton::Update( float dt )
{
	m_frame_counter++;
	Transition_Update();

	if ((!m_bVisible) || (m_spriteInst == NULL) ) return -1;

	m_spriteInst->update(dt);

	if (Transition_IsGoing())
	{
		SetCurrentAnim( m_bEnabled ? ( m_is_selected ? button_anim_pressed : button_anim_normal ) : button_anim_disabled );
		m_TE.ResetInput();
		return -1;
	}

	if (m_curr_anim == button_anim_blink)
	{
		bool isAnimDone = m_spriteInst->IsPlaying();
		if (isAnimDone)
		{
			m_TE.DisableAllOtherElements(false);
			m_TE.ResetInput();

			if (m_keepSelection)
			{
				SetCurrentAnim( button_anim_pressed );
				m_is_selected = true;
			}
			else
			{
				SetCurrentAnim( button_anim_normal );
				m_is_selected = false;
			}

			return m_action_id;
		}
		else
		{
			return -1;
		}
	}


	if ( m_TE.IsNowReleased() )
	{
		if (!m_is_selected && m_curr_anim == button_anim_pressed)
		{
			SetCurrentAnim( button_anim_blink, false );

			if ( m_click_sfx_id != -1 )
				__MGR_SOUNDS()->play( m_click_sfx_id );
		}
		else
		{
			m_TE.DisableAllOtherElements(false);
			m_TE.ResetInput();
		}
	}
	else if ( m_TE.IsActive() )
	{
		if (m_TE.IsNowActivated())
		{
			if (!m_is_selected)
			{
				m_TE.DisableAllOtherElements(true);
				SetCurrentAnim( button_anim_pressed, false );
			}
			return button_action_gain_focus;
		}
		else if (!m_TE.IsTouchInsideElement())
		{
			if (!m_is_selected)
			{
				if (m_TE_resetInputOnDragOut) 
				{
					m_TE.DisableAllOtherElements(false);
					m_TE.ResetInput();
				}
				SetCurrentAnim( button_anim_normal, false );
			}
			return button_action_lose_focus;
		}
		else if (m_TE.IsTouchInsideElement())
		{
			if (!m_is_selected)
			{
				SetCurrentAnim( button_anim_pressed, false );
			}
			return button_action_gain_focus;
		}
	}

	return -1;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void VisualControlButton::SetEnabled(bool is_enabled)
{
	SetCurrentAnim(is_enabled ? button_anim_normal : button_anim_disabled);
	IVisualControl::SetEnabled(is_enabled);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void VisualControlButton::SetSelected(bool is_selected)
{
	SetCurrentAnim( is_selected ? button_anim_pressed : button_anim_normal );
	m_is_selected = is_selected;
}


//---------------------------------------------------------------------------------------------------------
// Reset button touch only if it's in pressed state
//---------------------------------------------------------------------------------------------------------
void VisualControlButton::Reset()
{
	if( (m_TE.IsActive()) && (m_curr_anim == button_anim_pressed) )
	{
		m_TE.ResetInput();
		if ( !m_is_selected )
			SetCurrentAnim( button_anim_normal );
		m_is_selected = false;
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void VisualControlButton::SetAnimations(ASprite* sprite, int anims_num, ...)
{
	m_spriteInst = NEW CSpriteInstance( sprite );

	va_list arg_list;
	va_start( arg_list, anims_num );

	for (int i = 0; i < anims_num; i++)
	{
		m_animations[i] = va_arg(arg_list, int);
	}

	va_end(arg_list);

	// remove from here...
	SetCurrentAnim( button_anim_normal );
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
bool VisualControlButton::SetCurrentAnim(eButtonAnimations anim_type, bool loop)
{
	if ( (m_spriteInst == NULL) /*|| (m_curr_anim == anim_type)*/ ) return false;

	if ( (anim_type == button_anim_disabled) && (m_animations[button_anim_disabled] == 0) )
	{
		anim_type = button_anim_normal;
	}

	m_curr_anim = anim_type;

	if( m_spriteInst != NULL )
		m_spriteInst->StartAnimation( m_animations[m_curr_anim] );
	return true;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void VisualControlButton::SetKeepSelection(bool keep)
{
	m_keepSelection = keep;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
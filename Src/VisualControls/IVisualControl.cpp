//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#include "stdafx.h"

#include "IVisualControl.h"

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
IVisualControl::IVisualControl( int action_id, int control_id, int click_sfx ) :
	m_bEnabled(false),
	m_bEnabledTE(false)
{
	m_action_id			= action_id;
	m_control_id		= control_id;
	m_click_sfx_id		= click_sfx;
	m_value				= -1;
	m_bVisible			= true;
	m_sprite			= NULL;
	m_spriteInst		= NULL;
	m_frame_counter		= 0;
	m_transition_alpha	= 255;
	m_transition_type	= transition_none;

	m_text1_info.reset();
	m_text2_info.reset();

	SetClipRect( r2i(0, 0, OS_SCREEN_W, OS_SCREEN_H) );
	m_TE.SetTapDetectionEnabled(true);

	SetSelected(false);

	SetEnabledTE(true);
	SetEnabled(true);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
IVisualControl::~IVisualControl()
{
	SetEnabledTE(false);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IVisualControl::Transition_Start( eTransitionType type, int param1, int param2 )
{
	m_frame_counter   = 0;
	m_transition_type = type;

	switch(type)
	{
		case transition_show:
			m_transition_alpha = 0;
			m_transition_is_finished = false;
			break;

		case transition_hide:
			m_transition_alpha = 255;
			m_transition_is_finished = false;
			break;

		case transition_showLeft:
		case transition_showRight:
		{
			m_transition_orig_x  = m_pos.X;

			m_transition_off_x   = (float)param1;
			m_transition_off_max = (float)param1;
			m_transition_step    = (float)param2;
			float off = (m_transition_type == transition_showLeft) ? -m_transition_off_x : m_transition_off_x;
			MoveControl((int)(m_transition_orig_x + off), 0);
			m_transition_is_finished = false;
			break;
		}

		case transition_hideLeft:
		case transition_hideRight:
			m_transition_orig_x  = m_pos.X;

			m_transition_off_x   = 0;
			m_transition_off_max = (float)param1;
			m_transition_step    = (float)param2;
			m_transition_is_finished = false;
			break;

		case transition_stretch_show:
		case transition_stretch_hide:
			m_transition_is_finished = false;
			break;

		case transition_showWithZoom:
		case transition_hideWithZoom:
		default:
			m_transition_type = transition_none;
			break;
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IVisualControl::Transition_Update()
{
	if (m_transition_is_finished)
	{
		Transition_Finish();
		m_transition_is_finished = false;
	}

	switch (m_transition_type)
	{
		case transition_show:
		{
			m_transition_alpha += ((255 - m_transition_alpha) / 3);
			if (m_transition_alpha >= (255 - 3)) m_transition_alpha = 255;
			//CLAMP_VAL(m_transition_alpha, 0, 255);
			if (m_transition_alpha == 255) m_transition_is_finished = true;
			break;
		}

		case transition_hide:
		{
			m_transition_alpha -= (m_transition_alpha / 3);
			if (m_transition_alpha <= 55) m_transition_alpha = 0;
			if (m_transition_alpha == 0) m_transition_is_finished = true;
			//if (m_transition_alpha <= 120) m_transition_is_finished = true;
			break;
		}

		case transition_showLeft:
		case transition_showRight:
		{
			if (m_transition_off_x < 0) m_transition_off_x++;
			else 				        m_transition_off_x -= (m_transition_step);

			if (m_transition_off_x < -5) m_transition_off_x = -5;
			if (m_transition_off_x == 0)
			{
				m_transition_is_finished = true;
			}

			m_transition_alpha = (int)(((m_transition_off_max - m_transition_off_x) * 255) / m_transition_off_max);
			CLAMP_VAL(m_transition_alpha, 0, 255);

			float off = (m_transition_type == transition_showLeft) ? -m_transition_off_x : m_transition_off_x;
			MoveControl((int)(m_transition_orig_x + off), 0);
			break;
		}

		case transition_hideLeft:
		case transition_hideRight:
		{
			if (m_frame_counter <= 3) break;

			m_transition_off_x += m_transition_step;
			if (m_transition_off_x >= m_transition_off_max)
			{
				m_transition_off_x = m_transition_off_max;
				m_transition_is_finished = true;
			}

			m_transition_alpha = (int)(((m_transition_off_max - m_transition_off_x) * 255) / m_transition_off_max);
			CLAMP_VAL(m_transition_alpha, 0, 255);

			float off = (m_transition_type == transition_hideLeft) ? -m_transition_off_x : m_transition_off_x;
			MoveControl((int)(m_transition_orig_x + off), 0);
			break;
		}

		case transition_stretch_show:
			m_transition_alpha = (int)(m_frame_counter * 255) / 8 + (100 /* minimum alpha */);
			CLAMP_VAL(m_transition_alpha, 0, 255);

			if (m_frame_counter == 8)
				m_transition_is_finished = true;
			break;

		case transition_stretch_hide:
			m_transition_alpha = (int)((4 - m_frame_counter) * 255) / 4 + (100 /* minimum alpha */);
			CLAMP_VAL(m_transition_alpha, 0, 255);

			if (m_frame_counter == 4)
				m_transition_is_finished = true;
			break;
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IVisualControl::Transition_Finish()
{
	m_transition_type = transition_none;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IVisualControl::SetEnabled(bool is_enabled)
{
	m_bEnabled = is_enabled;
	//m_bEnabledTE = is_enabled;
	m_TE.SetEnabled(m_bEnabled && m_bEnabledTE);
}

//---------------------------------------------------------------------------------------------------------
// Enable/disable tap input
//---------------------------------------------------------------------------------------------------------
void IVisualControl::SetEnabledTE(bool is_enabled)
{
	m_bEnabledTE = is_enabled;
	m_TE.SetEnabled(m_bEnabled && m_bEnabledTE);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IVisualControl::SetVisible(bool is_visible, bool is_enabled)
{ 
	m_bVisible = is_visible; 
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IVisualControl::SetText1(stringw text, stringw font_normal, stringw font_pressed, const vcRect* clip, s32 text_flags)
{
	m_text1_info.m_text				= text;
	m_text1_info.m_textId			= -1;
	m_text1_info.m_textFlags		= text_flags;
	m_text1_info.m_font_normal_id	= font_normal;
	m_text1_info.m_font_pressed_id	= font_pressed;
	m_text1_info.m_text_pos			= (clip != 0) ? *clip : vcRect(0, 0, m_size.Width, m_size.Height);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IVisualControl::SetText1( int text_id , stringw font_normal, stringw font_pressed, const vcRect* clip, s32 text_flags )
{
	m_text1_info.m_textId			= text_id;
	m_text1_info.m_textFlags		= text_flags;
	m_text1_info.m_font_normal_id	= font_normal;
	m_text1_info.m_font_pressed_id	= font_pressed;
	m_text1_info.m_text_pos			= (clip != 0) ? *clip : vcRect(0, 0, m_size.Width, m_size.Height);
}
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IVisualControl::SetText2(stringw text, stringw font_normal, stringw font_pressed, const vcRect* clip, s32 text_flags)
{
	m_text2_info.m_text				= text;
	m_text2_info.m_textId			= -1;
	m_text2_info.m_textFlags		= text_flags;
	m_text2_info.m_font_normal_id	= font_normal;
	m_text2_info.m_font_pressed_id	= font_pressed;
	m_text2_info.m_text_pos			= (clip != 0) ? *clip : vcRect(0, 0, m_size.Width, m_size.Height);
}
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IVisualControl::SetText2( int text_id , stringw font_normal, stringw font_pressed, const vcRect* clip, s32 text_flags )
{
	m_text2_info.m_textId			= text_id;
	m_text2_info.m_textFlags		= text_flags;
	m_text2_info.m_font_normal_id	= font_normal;
	m_text2_info.m_font_pressed_id	= font_pressed;
	m_text2_info.m_text_pos			= (clip != 0) ? *clip : vcRect(0, 0, m_size.Width, m_size.Height);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IVisualControl::SetPos( p2i pos )
{
	m_pos = pos;
	m_TE.SetScreenCoordinates(m_pos.X - m_size.Width/2, m_pos.Y - m_size.Height/2, m_size.Width, m_size.Height);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IVisualControl::SetSize( d2i size )
{
	m_size = size;
	m_TE.SetScreenCoordinates(m_pos.X - m_size.Width/2, m_pos.Y - m_size.Height/2, m_size.Width, m_size.Height);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IVisualControl::SetClipRect( r2i rect )
{
	m_clip = rect;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#include "stdafx.h"

//#include "IO/Audio/SoundManager.h"
#include "Sprites/ASprite.h"
#include "VisualControlSwitch.h"

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
VisualControlSwitch::VisualControlSwitch( int action_id, int click_sfx ) : 
	IVisualControl( action_id, -1, click_sfx )
{
	m_is_selected  = false;
	m_value = m_is_selected; // Used in case of interrupt when drag...
	m_btn_offset_x = 0;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
VisualControlSwitch::~VisualControlSwitch()
{
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void VisualControlSwitch::Draw()
{
	if (!m_bVisible) return;

	// background
	//----------------------------
	m_sprite->PaintFrame(m_frames[switch_frame_background], m_pos.X, m_pos.Y, m_transition_alpha);

	// strings - (On / Off)
	//----------------------------
	if (m_text1_info.m_text.size() > 0)
	{
		int alpha = m_bEnabled ? m_transition_alpha : m_transition_alpha - 165;
		CLAMP_VAL(alpha, 0, 255);		
		// TODO: Spyder
		//GET_GAME->STR_DrawStringRegion(NULL, 
		//								m_text1_info.m_pack_id,
		//								m_text1_info.m_text_id,
		//								m_text1_info.m_font_normal_id,
		//								m_pos.x + m_text1_info.m_text_pos.x,
		//								m_pos.y + m_text1_info.m_text_pos.y,
		//								(m_text1_info.m_text_pos.w != 0) ? m_text1_info.m_text_pos.w : m_pos.w,
		//								(m_text1_info.m_text_pos.h != 0) ? m_text1_info.m_text_pos.h : m_pos.h,
		//								0, CGraphics::HCENTER | CGraphics::VCENTER,
		//								alpha);
	}

	if (m_text2_info.m_text.size() > 0)
	{
		int alpha = m_bEnabled ? m_transition_alpha : m_transition_alpha - 165;
		CLAMP_VAL(alpha, 0, 255);
		// TODO: Spyder
		//GET_GAME->STR_DrawStringRegion(NULL, 
		//								m_text2_info.m_pack_id,
		//								m_text2_info.m_text_id,
		//								m_text2_info.m_font_normal_id,
		//								m_pos.x + m_text2_info.m_text_pos.x,
		//								m_pos.y + m_text2_info.m_text_pos.y,
		//								(m_text2_info.m_text_pos.w != 0) ? m_text2_info.m_text_pos.w : m_pos.w,
		//								(m_text2_info.m_text_pos.h != 0) ? m_text2_info.m_text_pos.h : m_pos.h,
		//								0, CGraphics::HCENTER | CGraphics::VCENTER,
		//								alpha);
	}

	m_sprite->PaintFrame(m_frames[switch_frame_button], m_pos.X + m_btn_offset_x, m_pos.Y, m_transition_alpha);
	m_sprite->PaintFrame(m_frames[switch_frame_background_top], m_pos.X, m_pos.Y, m_transition_alpha);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int VisualControlSwitch::Update( float dt )
{
	m_frame_counter++;
	Transition_Update();

	if ( (!m_bVisible) || (Transition_IsGoing()) ) return -1;

	if ( m_TE.IsNowReleased() )
	{
		m_TE.DisableAllOtherElements(false);

		if ((!m_is_selected) && (m_btn_offset_x != 0))
			m_btn_offset_x_target = m_size.Width - kSwitch::btn_total_w;
		else if ((m_is_selected) && (m_btn_offset_x != m_size.Width - kSwitch::btn_total_w)) 
			m_btn_offset_x_target = 0;
		else
		{
			// TODO: Spyder
			//if ( m_click_sfx_id != -1 ) 
			//	SoundManager::GetInstance()->PlaySfx( m_click_sfx_id );
			m_value = m_is_selected;
			return m_action_id;
		}
	}
	else if ( m_TE.IsActive() )
	{
		if (m_TE.IsNowActivated())
		{
			m_TE.DisableAllOtherElements(true);

			int pos_x, pos_y;
			m_TE.GetTouchPos_Current(pos_x, pos_y);

			int offset = pos_x - m_pos.X;
			if (offset < (m_size.Width / 2))  m_btn_offset_x = 0;
			else                         m_btn_offset_x = m_size.Width - kSwitch::btn_total_w;

			UpdateSwitchState();
		}
		else if (m_TE.GetCurrentTouchType() == CTouchPad::Type_Drag)
		{
			int dt_x, dt_y;
			m_TE.GetTouchPos_DT(dt_x, dt_y);

			m_btn_offset_x += dt_x;
			CLAMP_VAL(m_btn_offset_x, 0, (m_size.Width - kSwitch::btn_total_w));
			UpdateSwitchState();
		}
	}
	else
	{
		if (UpdateSwitchButtonPos() == true)
		{
			// TODO: Spyder
			//if ( m_click_sfx_id != -1 ) 
			//	SoundManager::GetInstance()->PlaySfx( m_click_sfx_id );
			m_value = m_is_selected;
			return m_action_id;
		}
	}

	return -1;
}


//---------------------------------------------------------------------------------------------------------
// return true when reach target
//---------------------------------------------------------------------------------------------------------
bool VisualControlSwitch::UpdateSwitchButtonPos()
{
	if ((m_btn_offset_x_target - m_btn_offset_x) == 0) return false;

	int step = (m_size.Width - kSwitch::btn_total_w) / 2;
	m_btn_offset_x += (step * (((m_btn_offset_x_target - m_btn_offset_x) > 0) ? 1 : -1));
	CLAMP_VAL(m_btn_offset_x, 0, (m_size.Width - kSwitch::btn_total_w));

	return UpdateSwitchState();
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
bool VisualControlSwitch::UpdateSwitchState()
{
	if ( (m_btn_offset_x == 0) || (m_btn_offset_x  == (m_size.Width - kSwitch::btn_total_w)) )
	{
		m_btn_offset_x_target = m_btn_offset_x;
		m_is_selected = (m_btn_offset_x == (m_size.Width - kSwitch::btn_total_w));
		return true;
	}

	return false;	
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void VisualControlSwitch::SetSelected(bool is_selected)
{
	m_value = is_selected;
	m_is_selected = is_selected;
	m_btn_offset_x = m_btn_offset_x_target = (m_is_selected) ? m_size.Width - kSwitch::btn_total_w : 0;

}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void VisualControlSwitch::Reset()
{
	if (m_TE.IsActive())
	{
		m_TE.ResetInput();
		SetSelected(m_value ? true : false);
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void VisualControlSwitch::SetFrames(ASprite* sprite, int frames_num, ...)
{
	m_sprite = sprite;

	va_list arg_list;
	va_start( arg_list, frames_num );

	for (int i = 0; i < frames_num; i++)
	{
		m_frames[i] = va_arg(arg_list, int);
	}

	va_end(arg_list);
}
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
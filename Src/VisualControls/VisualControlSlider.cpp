//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#include "stdafx.h"

//#include "IO/Audio/SoundManager.h"

#include "Sprites/ASprite.h"

#include "VisualControlSlider.h"

#include "Game/Misc.h"

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
VisualControlSlider::VisualControlSlider( int action_id, int click_sfx ) : 
	IVisualControl( action_id, -1, click_sfx )
{
	m_min_x		= 0;
	m_max_x		= 0;
	m_offset_x	= 0;
	m_value		= 0;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
VisualControlSlider::~VisualControlSlider()
{
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void VisualControlSlider::Draw()
{
	if (!m_bVisible) return;

	m_sprite->PaintFrame(m_frames[slider_frame_empty], m_pos.X, m_pos.Y, m_transition_alpha);

	r2i fill_rect = r2i(m_pos.X + m_min_x, 0, m_pos.X + m_offset_x, 480);

	ASprite::PushClipRect( fill_rect );
	m_sprite->PaintFrame(m_frames[slider_frame_fill], m_pos.X, m_pos.Y, m_transition_alpha);
	ASprite::PopClipRect();

	//GET_LIB3D->SetClipRect(0, 0, OS_SCREEN_W, OS_SCREEN_H);
	int frame = m_is_selected ? m_frames[slider_frame_btn_glow] : m_frames[slider_frame_btn];
	if( frame >= 0 && frame < m_sprite->GetFrames() )
		m_sprite->PaintFrame( frame, m_pos.X + m_offset_x, m_pos.Y, m_transition_alpha);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int VisualControlSlider::Update( float dt )
{
	m_frame_counter++;
	Transition_Update();

	if ( (!m_bVisible) || (Transition_IsGoing()) ) return -1;

	if ( m_TE.IsNowReleased() )
	{
		m_TE.DisableAllOtherElements(false);
		m_is_selected = false;
		
		if ( m_click_sfx_id != -1 )
			__MGR_SOUNDS()->play( m_click_sfx_id );
	}
	else if ( m_TE.IsActive() )
	{
		if (m_TE.IsNowActivated())
		{
			m_TE.DisableAllOtherElements(true);
			m_is_selected = true;
		}

		int pos_x, pos_y;
		m_TE.GetTouchPos_Current(pos_x, pos_y);

		m_offset_x = pos_x - m_pos.X;
		CLAMP_VAL(m_offset_x, m_min_x, m_max_x);
		UpdatePercent();
	}

	return (m_is_selected) ? m_action_id : -1;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void VisualControlSlider::SetFrames(ASprite* sprite, int frames_num, ...)
{
	m_sprite = sprite;

	va_list arg_list;
	va_start( arg_list, frames_num );

	for (int i = 0; i < frames_num; i++)
	{
		ChangeFrame(i, va_arg(arg_list, int));
	}

	va_end(arg_list);

	SetSliderFillSize();
}

void VisualControlSlider::ChangeFrame(int frame_id, int frame_value)
{
	m_frames[frame_id] = frame_value;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void VisualControlSlider::UpdatePercent()
{
	m_value = m_offset_x * 100 / m_size.Width;
	m_value = IntRemap1(m_offset_x, m_min_x, m_max_x, 0, 100);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void VisualControlSlider::SetPercent( int percent )
{
	CLAMP(percent, 0, 100);
	m_value    = percent;
	m_offset_x = IntRemap1(percent, 0, 100, m_min_x, m_max_x);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int VisualControlSlider::GetPercent()
{
	return m_value;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void VisualControlSlider::Reset()
{
	if ( (m_TE.IsActive()) && (m_is_selected) )
	{
		m_TE.ResetInput();
		m_is_selected = false;
	}
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void VisualControlSlider::SetSliderFillSize()
{
	r2i fillRect;
	m_sprite->GetFrameRect(fillRect, m_frames[slider_frame_fill], 0);

	m_min_x = fillRect.UpperLeftCorner.X;
	m_max_x = fillRect.LowerRightCorner.X;

	SetPercent(m_value);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
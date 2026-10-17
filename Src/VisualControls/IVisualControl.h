//---------------------------------------------------------------------------------------------------------
// Copyright (C) 2009 Gameloft. All rights reserved.
//---------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#include "VisualControlCommon.h"

#include "Fonts/Font.h"

#include "IO/TouchScreen/TouchPad.h"

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

class ASprite;
class ASpriteInstance;
class CSpriteInstance;
class CTouchElement;

//---------------------------------------------------------------------------------------------------------
// Basic visual control interface
//---------------------------------------------------------------------------------------------------------
class IVisualControl
{
public:
	virtual ~IVisualControl();

	virtual void	Draw() = 0;
	virtual int		Update( float dt ) = 0;

	//-------------------------------- Transitions ---------------------------------
	virtual void	Transition_Start( eTransitionType type, int param1 = 0, int param2 = 0 );
	virtual void	Transition_Update();
	virtual void	Transition_Finish();
	inline  bool	Transition_IsGoing()              { return (m_transition_type != transition_none ); };

	void			SetText1( int text_id , stringw font_normal, stringw font_pressed, const vcRect* clip = 0, s32 text_flags = CFont::HVCENTER );
	void			SetText1( stringw text, stringw font_normal, stringw font_pressed, const vcRect* clip = 0, s32 text_flags = CFont::HVCENTER );
	void SetText1(int text_id, stringw normal, stringw pressed, const vcRect& clip, s32 flags = CFont::HVCENTER) { SetText1(text_id, normal, pressed, &clip, flags); }
	void SetText1(stringw text, stringw normal, stringw pressed, const vcRect& clip, s32 flags = CFont::HVCENTER) { SetText1(text, normal, pressed, &clip, flags); }

	void			SetText2( int text_id , stringw font_normal, stringw font_pressed, const vcRect* clip = 0, s32 text_flags = CFont::HVCENTER );
	void			SetText2( stringw text, stringw font_normal, stringw font_pressed, const vcRect* clip = 0, s32 text_flags = CFont::HVCENTER );

	virtual void	SetPos( p2i pos );
	inline p2i		GetPos() const { return m_pos; }

	void			SetSize( d2i size );
	inline d2i		GetSize() const { return m_size; }

	void			SetClipRect( r2i rect );
	inline r2i		GetClipRect() const { return m_clip; }

	inline void		SetArea( p2i pos, d2i size ) { SetPos(pos); SetSize(size); }
	inline void		SetArea( int x, int y, int w, int h ) { SetPos(p2i(x,y)); SetSize(d2i(w, h)); } 
	inline void		SetArea( r2i area ) { SetPos(area.getCenter()); SetSize(area.getSize()); }
	inline r2i		GetArea() const { return r2i(GetPos() - GetSize()/2, GetSize()); }
	inline void		MoveArea( int xOff, int yOff) { SetPos( p2i(m_pos.X + xOff, m_pos.Y + yOff) ); }

	inline void		MoveControl( d2i offset ) { SetPos(GetPos() + offset); }
	inline void		MoveControl( int x, int y ) { SetPos(GetPos() + p2i(x,y)); }

	virtual void	SetEnabledTE(bool is_enabled);
	virtual void	SetEnabled(bool is_enabled) ;
	virtual void	SetSelected(bool is_selected)     { m_is_selected = is_selected; };
	inline  bool	GetSelected()                     { return m_is_selected; };
	virtual void	Reset()                           = 0;

	void			SetVisible(bool is_visible, bool is_enabled = true);
	inline bool		GetVisible()                      { return m_bVisible; };

	inline void		SetValue(int value)               { m_value = value; };
	inline int		GetValue()                        { return m_value; };

	inline int		GetActionId()                     { return m_action_id; };
	inline int		GetControlId()                    { return m_control_id; };

	CTouchElement*	GetTouchElement()                 { return &m_TE; };

protected:
	IVisualControl( int action_id, int control_id, int click_sfx );


protected:
	//-------------------------------
	vcTextInfo			m_text1_info;				// assigned text info
	vcTextInfo			m_text2_info;				// assigned text info

	p2i					m_pos;						// control position
	d2i					m_size;						// control size, affects touch area
	r2i					m_clip;						// control clip rect

	//-------------------------------

	int					m_frame_counter;
	int					m_click_sfx_id;
	int					m_control_id;
	int					m_action_id;
	int					m_value;
	bool				m_bVisible;
	bool				m_bEnabled;
	bool				m_bEnabledTE;
	bool				m_is_selected;             // control is selected/pressed/highlighted/normal...
	unsigned int		m_flags;

	//-------------------------------
	CTouchElement		m_TE;                      // Control's touch element
	ASprite*			m_sprite;                  // Control's assigned sprite
	CSpriteInstance*	m_spriteInst;

protected:
	eTransitionType		m_transition_type;
	bool				m_transition_is_finished;
	int					m_transition_alpha;

	float				m_transition_off_x;
	float				m_transition_off_max;
	float				m_transition_step;
	int					m_transition_orig_x;
	int					m_transition_orig_y;
};
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------

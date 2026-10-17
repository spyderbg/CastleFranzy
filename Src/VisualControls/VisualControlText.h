//---------------------------------------------------------------------------------------------------------
// Copyright (C) 2009 Gameloft. All rights reserved.
//---------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
#include "IVisualControl.h"
#include "Fonts/FontManager.h"
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

class CFont; 

class VisualControlText : public IVisualControl
{
public:
	//no action
	VisualControlText( s32 control_id = -1 );
	virtual ~VisualControlText();

	virtual void	Draw();
	virtual int		Update( float dt );		

	virtual void	SetEnabledTE(bool is_enabled) {};
	virtual void	SetEnabled(bool is_enabled) {}
	virtual void	Reset() {}

	//set / get methods
	void			SetText(const stringw & text );
	void			SetText(int textId );

	stringw			GetText();

	void			SetFont( const stringw & fontName );
	void			SetRegion(int mwidth, int mheight);

	void			SetFontColor( u32 forced_color );
	void			SetAlign( s32 align );

	void			RecalculateSize();
	r2i				GetTextArea();
	
protected:
	bool			m_useForcedColor; 
	u32				m_forcedTextColor;

	stringw			m_text;
	int				m_textId;

	stringw			m_font;
	s32				m_align;
	s32				m_disabledTextAlpha;

	int				m_max_width;
	int				m_max_height;

};
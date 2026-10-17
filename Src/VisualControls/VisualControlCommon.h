//---------------------------------------------------------------------------------------------------------
// Copyright (C) 2009 Gameloft. All rights reserved.
//---------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Core/irrlicht.h"

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------
typedef enum
{
	transition_none,
	transition_finishing,

	transition_show,
	transition_hide,

	transition_showLeft,
	transition_hideLeft,

	transition_showRight,
	transition_hideRight,

	transition_showTop,
	transition_hideTop,

	transition_showWithZoom,
	transition_hideWithZoom,

	transition_stretch_show,
	transition_stretch_hide,

}eTransitionType;
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------
typedef struct tag_vcRect{
	int x;
	int y;
	int w;
	int h;	
	tag_vcRect()                               {}
	tag_vcRect( const tag_vcRect & ref )       { x=ref.x; y=ref.y; w=ref.w; h=ref.h; }
	tag_vcRect( int x, int y, int w, int h ) : x(x), y(y), w(w), h(h) {};

	void set(int x1, int y1, int w1, int h1)   { x = x1; y = y1; w = w1; h = h1; }

	bool doRectsIntersect(const tag_vcRect* rc1, const tag_vcRect* rc2)
	{
		if ( (rc1->y + rc1->h) < (rc2->y) )            return false;
		if ( (rc1->y)          > (rc2->y + rc2->h) )   return false;
		if ( (rc1->x)          > (rc2->x + rc2->w) )   return false;
		if ( (rc1->x + rc1->w) < (rc2->x) )            return false;

		return true;
	}

	void rectIntersect(tag_vcRect* rc, const tag_vcRect* rc1, const tag_vcRect* rc2)
	{
		if ( !doRectsIntersect(rc1, rc2) )
		{
			rc->set(0, 0, 0, 0);
		}
		else
		{
			rc->x = max(rc1->x, rc2->x);
			rc->y = max(rc1->y, rc2->y);
			rc->w = min(rc1->x + rc1->w, rc2->x + rc2->w) - rc->x;
			rc->h = min(rc1->y + rc1->h, rc2->y + rc2->h) - rc->y;
		}
	};

	r2i toIrrRect()
	{
		return r2i(x, y, x+w, y+h);
	}

}vcRect;
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------
typedef struct tag_vcTextInfo{
	int			m_textId;
	stringw		m_text;
	s32			m_textFlags;
	stringw		m_font_normal_id;
	stringw		m_font_pressed_id;
	vcRect		m_text_pos;               // pos and clip rect of text, related to visual control's coordinates

	void reset() 
		{ 
			m_textId = -1;
			m_text = L""; 
			m_font_normal_id = L""; 
			m_font_pressed_id = L""; 
			m_textFlags = 0; 
		}
}vcTextInfo;
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

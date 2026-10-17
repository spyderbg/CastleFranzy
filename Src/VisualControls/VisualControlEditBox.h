//---------------------------------------------------------------------------------------------------------
// Copyright (C) 2009 Gameloft. All rights reserved.
//---------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
#include "IVisualControl.h"

class VisualControlText;
class CSpriteInstance;


enum EditBoxType
{
	EditBox_Normal = 0, 
	EditBox_Password
};

//---------------------------------------------------------------------------------------------------------
// Visual control Edit box
//---------------------------------------------------------------------------------------------------------
class VisualControlEditBox : public IVisualControl
{
public:
	VisualControlEditBox( EditBoxType type, int action_id, int control_id = -1, int click_sfx = -1 );
	virtual ~VisualControlEditBox();

	void SetArea( p2i pos, d2i size );
	void SetArea( int x, int y, int w, int h );
	void SetArea( r2i area );

	//call to set parametas for the control
	void SetParametars( u32 bgColor, TCHAR *fontName, ASprite *sprite, u32 bg_anim_id );

	void SetText( TCHAR *text ) { SetText( stringw( text ) ); }
	void SetText( stringw text );
		
	void PushSymbol( TCHAR symbol );	
	void PopSymbol();

	void Draw();
	int  Update( float dt );

	void Reset();

protected:
	CSpriteInstance	  *m_spriteInstance;

	EditBoxType		   m_boxType;
	u32				   m_bgColor;

	VisualControlText *m_textControl;

};
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

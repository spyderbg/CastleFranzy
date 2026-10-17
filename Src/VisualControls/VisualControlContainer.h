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

//---------------------------------------------------------------------------------------------------------
// Visual control Edit box
//---------------------------------------------------------------------------------------------------------

class VisualControlContainer : public IVisualControl
{
public:
	VisualControlContainer( );
	virtual ~VisualControlContainer();

	void ReleaseControls();

	void SetArea( p2i pos, d2i size );
	void SetArea( int x, int y, int w, int h );
	void SetArea( r2i area );
	void SetPos( p2i pos );

	virtual void	SetEnabledTE(bool is_enabled) {} 
	virtual void	SetEnabled(bool is_enabled) ;


	/// Controls
	void	AddControl( IVisualControl *ptr );
	void	RemoveControl( IVisualControl *ptr );

	void Draw();
	int  Update( float dt );

	void Reset();

protected:

	irr::core::array< IVisualControl * >	m_controls;
	irr::core::array< p2i >					m_startPos;
};
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

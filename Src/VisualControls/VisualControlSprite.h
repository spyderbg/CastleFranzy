//---------------------------------------------------------------------------------------------------------
// Copyright (C) 2009 Gameloft. All rights reserved.
//---------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
#include "IVisualControl.h"
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

class Asprite;
class CSpriteInstance;
//simple control with sprite instance
class VisualControlSprite : public IVisualControl
{
public:
	//no action
	VisualControlSprite( s32 control_id = -1 );
	~VisualControlSprite( ) ;

	void SetAnimation( ASprite *sprite, int animId );

	virtual void Draw();
	virtual int	Update( float dt );

	virtual void	SetEnabledTE(bool is_enabled) {} 
	virtual void	SetEnabled(bool is_enabled) { }
	virtual void	Reset() { }
	
protected:

	CSpriteInstance		*m_spriteInstance;
};
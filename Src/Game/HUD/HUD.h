// ///////////////////////////////////////////////////////////////////////////
// HUD.h
//
//   This is the HUD
// ///////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Entities/IDrawable.h"
#include "Entities/IUpdatable.h"

#include "Core/irrlicht.h"

class ASprite;

#define HEART_FULL 2
#define HEART_HALF 1

class HUD: public IDrawable, public IUpdatable
{
public:
	HUD();
	~HUD();
	void			draw()				const;
	void			update(TimeStamp timestamp);
	int				getDrawingLayer()	const;


private:
	void DrawDollars() const;
	void DrawHudMainCharacter() const;
	void DrawHealth() const;
	void DrawRareItemPieces() const;
	void DrawLives()const;

private:
	
	
	ASprite* m_hudSprite;

};

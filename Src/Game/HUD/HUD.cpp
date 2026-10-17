
#include "stdafx.h"

//
//   This is the HUD
// ///////////////////////////////////////////////////////////////////////////

#include "HUD.h"

#include "Core/irrlicht.h"
#include "Core/Application.h"

#include "Sprites/ASprite.h"
#include "Sprites/ASprite.h"

#include "Common/StringTable.h"

///////////////////////////////////////////////////////////////////////////

#if 0

#define DOLLAR_POS_X 410
#define DOLLAR_POS_Y 26

#define CLOCK_POS_X 290
#define CLOCK_POS_Y	7

#define ACTIVE_PLAYER_POS_X	10
#define ACTIVE_PLAYER_POS_Y	5
#define INACTIVE_PLAYER_POS_X	70
#define INACTIVE_PLAYER_POS_Y	25

#define RARE_ITEMS_PIECES_POS_X 320
#define RARE_ITEMS_PIECES_POS_Y	5

#define LIVES_POS_X	85
#define LIVES_POS_Y 10



HUD::HUD()
{
	// we load the graphics
	m_hudSprite = new ASprite();

	//ITexture* IGItexture = Application::GetInstance()->GetDriver()->getTexture("hud.tga");
	//m_hudSprite->BuildCacheImages(IGItexture);
}

HUD::~HUD()
{ 
	delete(m_hudSprite); 
}

void HUD::draw() const
{
	m_hudSprite->PaintAFrame(SPRITE_INTERFACE_ANIM_CLOCK, 0, CLOCK_POS_X, CLOCK_POS_Y, 0,0,0);
	DrawDollars();
	DrawHudMainCharacter();
	DrawHealth();
	DrawRareItemPieces();
	DrawLives();
}

void HUD::update(TimeStamp timestamp)
{
}

int HUD::getDrawingLayer() const
{
	return IDrawable::LAYER_HUD;
}

void HUD::DrawHudMainCharacter() const
{
	int frameID = SPRITE_INTERFACE_ANIM_SHREK;
	int smallframeID = SPRITE_INTERFACE_ANIM_DONKEY_INACTIVE;
	m_hudSprite->PaintAFrame(frameID, 0, ACTIVE_PLAYER_POS_X, ACTIVE_PLAYER_POS_Y, 0, 0, 0);
	m_hudSprite->PaintAFrame(smallframeID, 0, INACTIVE_PLAYER_POS_X, INACTIVE_PLAYER_POS_Y, 0, 0, 0);
}

void HUD::DrawHealth() const
{	
	int maxHealth = 3;
	int halfHeart = 0;
	int heartPos = 0;

	int health = 3;

	int frame;
	int x = 100;
	int y;

	// The unit of the system is half-heart.
	while (halfHeart < maxHealth)
	{
		if (halfHeart < health)
		{
			if (halfHeart + 1 < health)
			{
				frame = SPRITE_INTERFACE_ANIM_HEART_FULL;
			}
			else
			{
				frame = SPRITE_INTERFACE_ANIM_HEART_HALF;
			}
		}
		else
		{
			frame = SPRITE_INTERFACE_ANIM_HEART_EMPTY;
		}

		if (heartPos < 7)
		{
			x = 100 + heartPos * 17;
			y = 5;
		}
		else
		{
			x = 108 + (heartPos - 7)  * 17;
			y = 21;
		}
		m_hudSprite->PaintAFrame(frame, 0, x, y, 0, 0, 0);
		halfHeart += 2;
		++heartPos;
	} 
}

void HUD::DrawRareItemPieces() const
{
	int rareItemPieces = 10;
	int RareItemPieces_Count = 10;
	
	m_hudSprite->PaintAFrame(SPRITE_INTERFACE_ANIM_SHREK_PARTY, 0, RARE_ITEMS_PIECES_POS_X, RARE_ITEMS_PIECES_POS_Y, 0, 0, 0);				
	for(int i = 0; i < RareItemPieces_Count; i++)
	{
		if (((rareItemPieces) & (1 << i)) != 0)
		{
			m_hudSprite->PaintAFrame(SPRITE_INTERFACE_ANIM_SHREK_PARTY + i, 0, RARE_ITEMS_PIECES_POS_X, RARE_ITEMS_PIECES_POS_Y, 0, 0, 0 );
		}
	}
}

void HUD::DrawDollars() const
{
	int dollars = 3;

	StringManager* stringMgr = Application::GetInstance()->GetStringManager();
	stringMgr->SetEscapedDigitValue(dollars, 0);
	stringMgr->DrawString(FONT_NORMAL, GLOBAL_DIGIT2, DOLLAR_POS_X, DOLLAR_POS_Y, ASprite::RIGHT | ASprite::TOP);

	stringMgr->DrawString(FONT_NORMAL, GLOBAL_HUD_X, DOLLAR_POS_X, DOLLAR_POS_Y, 0);

	m_hudSprite->PaintAFrame(SPRITE_INTERFACE_ANIM_DOLLAR, 0, 0, 0, 0,0,0);	
}

void HUD::DrawLives() const
{	
	int lives = 3;

	StringManager* stringMgr = Application::GetInstance()->GetStringManager();
	stringMgr->SetEscapedDigitValue(lives, 0);

	stringMgr->DrawString(FONT_NORMAL, GLOBAL_HUD_X, LIVES_POS_X , LIVES_POS_Y, ASprite::RIGHT | ASprite::TOP);

	stringMgr->DrawString(FONT_NORMAL, GLOBAL_DIGIT2, LIVES_POS_X , LIVES_POS_Y, ASprite::LEFT | ASprite::TOP);

	

}
#endif // 0
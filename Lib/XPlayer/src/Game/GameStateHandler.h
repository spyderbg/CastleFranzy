#ifndef __GAME_STATE_HANDLER_H__
#define __GAME_STATE_HANDLER_H__

#include "DEF.h"
#include "TouchRect.h"
#include "Image.h"

/**
 * The abstract basic class for switching game state.
 * <p>
 *
 * @version 0.0.1
 *
 */
class GameStateHandler
{
public:
	
	virtual void SetGameState( DEF::GAME_STATE_ENUM state ) = 0;
	virtual void DrawString(int posX, int posY, char *str, bool smallFont = false) = 0;
	virtual void ClearScreen() = 0;
	virtual void DrawButton(_TouchRect button) = 0;
	virtual void DrawImage(CImage *pSrc, int x, int y, int anchor) = 0;
	virtual void DrawRect(_TouchRect button) = 0;
};

#endif // __GAME_STATE_HANDLER_H__
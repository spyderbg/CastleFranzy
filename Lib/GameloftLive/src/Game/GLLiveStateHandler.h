#ifndef __GAME_STATE_HANDLER_H__
#define __GAME_STATE_HANDLER_H__

#include "GLLiveDEF.h"
#include "sprite_constants.h"
#include "TouchRect.h"
#include "Image.h"
//#include "System\MyGraphics.h"
//#include "System\GLLive_Font.h"

class GLLiveState;
class CGLLiveASprite;
class CGLLiveFont;
//class CGLLiveGraphics;
//class CGLLiveImage;

using namespace gllive;


/**
 * The abstract basic class for switching game state.
 * <p>
 *
 * @version 0.0.1
 *
 */
class GLLiveStateHandler
{
public:
	
	virtual bool PushGLLiveState( GLLiveState *pState ) = 0;
	virtual bool PopGLLiveState(bool callSwitch = true) = 0;
	virtual GLLiveState * GetCurState() = 0;
	virtual void TerminateGame() = 0;

	virtual void ClearScreen() = 0;
	virtual CGLLiveFont* GetFont(int fs) = 0;
	virtual CGLLiveGraphics* GetGraphics() = 0;
	virtual CGLLiveASprite *GetASprite(int index) = 0;

	virtual void DrawButton(_TouchRect button) = 0;
	virtual void DrawImage(CGLLiveImage *pSrc, int x, int y, int anchor) = 0;	
};

#endif // __GAME_STATE_HANDLER_H__
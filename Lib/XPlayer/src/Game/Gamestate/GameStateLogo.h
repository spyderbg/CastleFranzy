#ifndef __GAME_STATE_LOGO_H__
#define __GAME_STATE_LOGO_H__

#include "gamestate.h"

/**
 * The startup splash.
 * <p>
 *
 * @version 0.0.1
 *
 */
class GameStateLogo :
	public GameState
{

public:

	GameStateLogo( GameStateHandler * handler );

	bool HandleEvent( _TOUCH_EVENT event );

    void SwitchState();

    void Paint();

	void UpdateAnimation();

	~GameStateLogo(void);

private:

	//CGraphics * m_logo;

	int m_frameCount;

	int m_alpha;

};

#endif // __GAME_STATE_LOGO_H__
#ifndef __GAME_STATE_TEST_LOBBY_H__
#define __GAME_STATE_TEST_LOBBY_H__

#include "gamestate.h"
#include "../../include/Porting.h"
#include "InputBox.h"

class GameStateTestLobby : public GameState
{
public:
	
	GameStateTestLobby(GameStateHandler * handler);
	virtual ~GameStateTestLobby(void);
	
	bool HandleEvent( _TOUCH_EVENT event );

    void Paint();

	InputBox* m_InputBox;
	int m_selectedIndex;	

	DEF::GAME_STATE_ENUM m_state;

    void SwitchState();
	void UpdateAnimation();
};


#endif
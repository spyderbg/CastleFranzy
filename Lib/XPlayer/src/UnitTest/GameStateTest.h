#ifndef __GAME_STATE_TEST_H__
#define __GAME_STATE_TEST_H__
#include "DEF.h"

#ifdef ENABLE_UNIT_TEST

#include "gamestate.h"
#include "Porting.h"
#include "Menu.h"

#include "../../../UnitTest/MockGLXPlayerLogin.h"
#include "../../../UnitTest/MockGLXPlayerUser.h"
#include "../../../UnitTest/MockGLXPlayerUserFriend.h"
#include "../../../UnitTest/MockGLXPlayerLeaderboard.h"
#include "../../../UnitTest/MockGLXPlayerMessage.h"
#include "../../../UnitTest/MockGLXPlayerChat.h"
#include "../../../UnitTest/MockGLXPlayerYoutube.h"
#include "../../../UnitTest/MockGLXPlayerLobby.h"


class GameStateTest : public GameState, public MenuItemObserver, public TestResults
{
public:
	GameStateTest(GameStateHandler * handler);
	virtual ~GameStateTest(void);
	
	enum OnlineSubState
	{	
		TEST_PROCESS = 0,		
		TEST_MENU,		
		TEST_FINISH
	};	

	/**
	 * Based functions
	 */
	void Paint();
	bool HandleEvent( _TOUCH_EVENT event );
	void DrawCommandBar( GameStateHandler * handler, CommandBarState state, bool isDrawUpDown = true );
	void SwitchState();
	void UpdateAnimation();
	
	/**
	* TestObserver
	*
	*/	
	void OnTestStart(TestDetails const& test);
    void OnTestFailure(TestDetails const& test, char const* failure);
    void OnTestFinish(TestDetails const& test, float secondsElapsed);
	
	void TestFinishAll();
	
	/**
	 * MenuItemObserver
	 */	 
	void OnMenuItemClicked( char * menu_name );			
	
	/**
	 * Owned functions
	 */	
	void SetOnlineSubState(OnlineSubState state);	
	
	void DrawTestProcess(GameStateHandler * handler);	
	void DrawTestFinish(GameStateHandler * handler);
	
	bool HandleTestProcess( _TOUCH_EVENT event);
	bool HandleTestMenu( _TOUCH_EVENT event);
	bool HandleTestFinish( _TOUCH_EVENT event);
	
	OnlineSubState m_sub_state;
	OnlineSubState m_pre_sub_state;	
	DEF::GAME_STATE_ENUM m_state;
	
	GLXPlayerLogin* m_GLXPlayerLogin;
	
	MockGLXPlayerLogin * m_mockGLXPlayerLogin;
	MockGLXPlayerUser * m_mockGLXPlayerUser;
	MockGLXPlayerUserFriend * m_mockGLXPlayerUserFriend;
	MockGLXPlayerLeaderboard * m_mockGLXPlayerLeaderboard;
	MockGLXPlayerMessage * m_mockGLXPlayerMessage;
	MockGLXPlayerChat * m_mockGLXPlayerChat;
	MockGLXPlayerYoutube * m_mockGLXPlayerYoutube;
	MockGLXPlayerLobby * m_mockGLXPlayerLobby;
	
	Menu * m_testMenu;	
	
	int m_testSuccess;	
	float m_secondsElapsed;	
	
	int m_frame;
};

#endif // ENABLE_UNIT_TEST

#endif //__GAME_STATE_TEST_H__
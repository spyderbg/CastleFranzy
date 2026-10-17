#include "GameStateTest.h"

#ifdef ENABLE_UNIT_TEST

/**
 * Constructor and Destructor
 */
GameStateTest::GameStateTest(GameStateHandler * handler) : GameState( handler )
{
	m_testMenu = XP_API_NEW Menu(DEF::START_POS_X,DEF::START_POS_Y);
	m_testMenu->AddMenuItem( MENU_UNIT_TEST_GLXPLAYERLOGIN, this);
	m_testMenu->AddMenuItem( MENU_UNIT_TEST_GLXPLAYERUSER, this);
	m_testMenu->AddMenuItem( MENU_UNIT_TEST_GLXPLAYERUSERFRIEND, this);
	m_testMenu->AddMenuItem( MENU_UNIT_TEST_GLXPLAYERLEADERBOARD, this);
	m_testMenu->AddMenuItem( MENU_UNIT_TEST_GLXPLAYERMESSAGE, this);
	m_testMenu->AddMenuItem( MENU_UNIT_TEST_GLXPLAYERCHAT, this);
	m_testMenu->AddMenuItem( MENU_UNIT_TEST_YOUTUBE, this);
	m_testMenu->AddMenuItem( MENU_UNIT_TEST_LOBBY, this);	
	
	m_state = DEF::GAME_STATE_TEST;
	SetOnlineSubState(TEST_MENU);	
	
	m_mockGLXPlayerLogin = NULL;	
	m_mockGLXPlayerUser = NULL;
	m_mockGLXPlayerUserFriend = NULL;
	m_mockGLXPlayerLeaderboard = NULL;
	m_mockGLXPlayerMessage = NULL;
	m_mockGLXPlayerChat = NULL;
	m_mockGLXPlayerYoutube = NULL;
	m_mockGLXPlayerLobby = NULL;
	
	m_GLXPlayerLogin = NULL;
	
	m_testSuccess = 0;
	m_secondsElapsed = 0.0f;		
	m_frame = 0;
	
	//	Cheat: Create an empty output file for storing test information.
	XP_FILE_HANDLE pFile;
#ifdef OS_IPHONE
	char buffer[FILENAME_MAX];
	GetSaveFilePath(buffer, UNIT_TEST_OUTPUT_NAME);
	pFile = XP_API_FILE_OPEN( buffer, "wt+" );	
#else
	pFile = XP_API_FILE_OPEN( UNIT_TEST_OUTPUT_NAME, "wt" );
#endif
	XP_API_FILE_CLOSE(pFile);
}

GameStateTest::~GameStateTest()
{
	XP_API_DEL(m_testMenu);
	XP_API_DEL(m_mockGLXPlayerLogin);	
	XP_API_DEL(m_mockGLXPlayerUser);	
	XP_API_DEL(m_mockGLXPlayerUserFriend);
	XP_API_DEL(m_mockGLXPlayerLeaderboard);
	XP_API_DEL(m_mockGLXPlayerMessage);
	XP_API_DEL(m_mockGLXPlayerChat);
	XP_API_DEL(m_mockGLXPlayerYoutube);
	XP_API_DEL(m_mockGLXPlayerLobby);
	XP_API_DEL(m_GLXPlayerLogin);
}

/**
 * Based functions redefined
 */
void GameStateTest::Paint()
{
	switch(m_sub_state)
	{
		case TEST_PROCESS:
			m_handler->ClearScreen();
			DrawTestProcess(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK);
			break;
		case TEST_MENU:
			m_handler->ClearScreen();
			m_testMenu->Repaint(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;
		case TEST_FINISH:
			m_handler->ClearScreen();
			DrawTestFinish(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK);
			break;
			
		default:
			break;
	}	
}

bool GameStateTest::HandleEvent( _TOUCH_EVENT event )
{
	bool bHandled = false;
	switch(m_sub_state)
	{
		case TEST_PROCESS:
			bHandled = HandleTestProcess(event);
			break;
		case TEST_MENU:
			bHandled = HandleTestMenu(event);
			break;
		case TEST_FINISH:
			bHandled = HandleTestFinish(event);
			break;
			
		default:
			break;
	}
	return bHandled;
}

void GameStateTest::DrawCommandBar( GameStateHandler * handler, CommandBarState state, bool isDrawUpDown )
{
	if(isDrawUpDown == true)
	{
		handler->DrawButton(m_UpKey);
		handler->DrawButton(m_DownKey);
	}
	switch( state )
	{
	case CB_CANCEL:
	case CB_NULL_BACK:
		handler->DrawButton(m_SoftRight);
		break;
	case CB_LEAVE_NULL:
		handler->DrawButton(m_SoftLeft);
		break;
	case CB_GO_BACK:
	case CB_JOIN_BACK:
	case CB_FINISH_LEAVE:	
		handler->DrawButton(m_SoftLeft);
		handler->DrawButton(m_SoftRight);
		break;
	case CB_START_KICKOUT:
	case CB_ACCEPT_IGNORE:
		handler->DrawButton(m_SoftLeft);
		handler->DrawButton(m_SoftRight);
		handler->DrawButton(m_SoftMid);
		break;
	default:
		break;
	}

	GameState::DrawCommandBar(handler, state, isDrawUpDown);
}

void GameStateTest::UpdateAnimation( )
{		
	if(m_sub_state == TEST_PROCESS)
	{				
		if(m_GLXPlayerLogin != NULL)
		{	
			if(m_mockGLXPlayerLogin != NULL)
			{
				m_mockGLXPlayerLogin->runAllTest();			
			}
			else if(m_mockGLXPlayerUser != NULL)
			{			
				m_mockGLXPlayerUser->runAllTest();				
			}	
			else if(m_mockGLXPlayerUserFriend != NULL)
			{
				m_mockGLXPlayerUserFriend->runAllTest();
			}
			else if(m_mockGLXPlayerLeaderboard != NULL)
			{
				m_mockGLXPlayerLeaderboard->runAllTest();
			}					
			else if(m_mockGLXPlayerMessage != NULL)
			{
				m_mockGLXPlayerMessage->runAllTest();
			}					
			else if(m_mockGLXPlayerChat != NULL)
			{
				m_mockGLXPlayerChat->runAllTest();
			}					
			else if(m_mockGLXPlayerYoutube != NULL)
			{
				m_mockGLXPlayerYoutube->runAllTest();
			}					
			else if(m_mockGLXPlayerLobby != NULL)
			{
				m_mockGLXPlayerLobby->runAllTest();
			}					
		}
	}
	m_frame++;
}

void GameStateTest::SwitchState()
{
	SetGameState(m_state);
}

/**
 * MenuItemObserver
 */
void GameStateTest::OnMenuItemClicked( char * menu_name )
{
	if ( XP_API_STRCMP( menu_name, MENU_UNIT_TEST_GLXPLAYERLOGIN ) == 0 )
	{	
		XP_API_DEL(m_GLXPlayerLogin);
		m_GLXPlayerLogin = XP_API_NEW GLXPlayerLogin();		
		
		XP_API_DEL(m_mockGLXPlayerLogin);
		m_mockGLXPlayerLogin = XP_API_NEW MockGLXPlayerLogin(m_GLXPlayerLogin);	
		m_mockGLXPlayerLogin->registerResult(this);			
		
		SetOnlineSubState(TEST_PROCESS);		
	}
	else if ( XP_API_STRCMP( menu_name, MENU_UNIT_TEST_GLXPLAYERUSER ) == 0 )
	{	
		XP_API_DEL(m_GLXPlayerLogin);
		m_GLXPlayerLogin = XP_API_NEW GLXPlayerLogin();
		
		XP_API_DEL(m_mockGLXPlayerUser);
		m_mockGLXPlayerUser = XP_API_NEW MockGLXPlayerUser(m_GLXPlayerLogin);	
		m_mockGLXPlayerUser->registerResult(this);	
		
		SetOnlineSubState(TEST_PROCESS);			
	}
	else if ( XP_API_STRCMP( menu_name, MENU_UNIT_TEST_GLXPLAYERUSERFRIEND ) == 0 )
	{	
		XP_API_DEL(m_GLXPlayerLogin);
		m_GLXPlayerLogin = XP_API_NEW GLXPlayerLogin();
		
		XP_API_DEL(m_mockGLXPlayerUserFriend);
		m_mockGLXPlayerUserFriend = XP_API_NEW MockGLXPlayerUserFriend(m_GLXPlayerLogin);	
		m_mockGLXPlayerUserFriend->registerResult(this);
		
		SetOnlineSubState(TEST_PROCESS);			
	}
	else if ( XP_API_STRCMP( menu_name, MENU_UNIT_TEST_GLXPLAYERLEADERBOARD ) == 0 )
	{
		XP_API_DEL(m_GLXPlayerLogin);
		m_GLXPlayerLogin = XP_API_NEW GLXPlayerLogin();
		
		XP_API_DEL(m_mockGLXPlayerLeaderboard);
		m_mockGLXPlayerLeaderboard = XP_API_NEW MockGLXPlayerLeaderboard(m_GLXPlayerLogin);	
		m_mockGLXPlayerLeaderboard->registerResult(this);	
		
		SetOnlineSubState(TEST_PROCESS);		
	}	
	else if ( XP_API_STRCMP( menu_name, MENU_UNIT_TEST_GLXPLAYERMESSAGE ) == 0 )
	{		
		XP_API_DEL(m_GLXPlayerLogin);
		m_GLXPlayerLogin = XP_API_NEW GLXPlayerLogin();
		
		XP_API_DEL(m_mockGLXPlayerMessage);
		m_mockGLXPlayerMessage = XP_API_NEW MockGLXPlayerMessage(m_GLXPlayerLogin);	
		m_mockGLXPlayerMessage->registerResult(this);	
		
		SetOnlineSubState(TEST_PROCESS);		
	}	
	else if ( XP_API_STRCMP( menu_name, MENU_UNIT_TEST_GLXPLAYERCHAT ) == 0 )
	{		
		XP_API_DEL(m_GLXPlayerLogin);
		m_GLXPlayerLogin = XP_API_NEW GLXPlayerLogin();
		
		XP_API_DEL(m_mockGLXPlayerChat);
		m_mockGLXPlayerChat = XP_API_NEW MockGLXPlayerChat(m_GLXPlayerLogin);	
		m_mockGLXPlayerChat->registerResult(this);	
		
		SetOnlineSubState(TEST_PROCESS);		
	}	
	else if ( XP_API_STRCMP( menu_name, MENU_UNIT_TEST_YOUTUBE ) == 0 )
	{		
		XP_API_DEL(m_GLXPlayerLogin);
		m_GLXPlayerLogin = XP_API_NEW GLXPlayerLogin();
		
		XP_API_DEL(m_mockGLXPlayerYoutube);		
		m_mockGLXPlayerYoutube = XP_API_NEW MockGLXPlayerYoutube(m_GLXPlayerLogin);	
		m_mockGLXPlayerYoutube->registerResult(this);	
		
		SetOnlineSubState(TEST_PROCESS);		
	}	
	else if ( XP_API_STRCMP( menu_name, MENU_UNIT_TEST_LOBBY ) == 0 )
	{				
		XP_API_DEL(m_GLXPlayerLogin);
		m_GLXPlayerLogin = XP_API_NEW GLXPlayerLogin();
		
		XP_API_DEL(m_mockGLXPlayerLobby);		
		m_mockGLXPlayerLobby = XP_API_NEW MockGLXPlayerLobby(m_GLXPlayerLogin);	
		m_mockGLXPlayerLobby->registerResult(this);	
		
		SetOnlineSubState(TEST_PROCESS);
	}	
}


/**
 * Owned functions
 */
void GameStateTest::SetOnlineSubState( OnlineSubState state )
{	
	m_pre_sub_state = m_sub_state;	
	m_sub_state = state;
}

void GameStateTest::DrawTestProcess(GameStateHandler * handler)
{	
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	handler->DrawString(x,y,"--- Processing ---");
	y+=2*DEF::LINE_HEIGHT;
	
	char wait[] = "Please Wait ";
	char msg[32];
	int len = 0;

	len = XP_API_STRLEN(wait);
	XP_API_MEMSET(msg,0,32);
	XP_API_MEMCPY(msg,wait,len);
	
	for (int i=0; i<m_frame/2%7; i++)
	{
		msg[len++] = '.';
	}
	msg[len] = 0;
	handler->DrawString(x,y,msg);
}

void GameStateTest::DrawTestFinish(GameStateHandler * handler)
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	handler->DrawString(x,y,"--- Test Result ---");
	y+=2*DEF::LINE_HEIGHT;	
	
	char tmp[128] = {0};	
	XP_API_MEMSET(tmp, 0, 128);
	XP_API_SPRINTF(tmp, "Total: %d.", GetTotalTestCount());
	handler->DrawString(x,y,tmp);	
	y+=2*DEF::LINE_HEIGHT;	
	
	XP_API_MEMSET(tmp, 0, 128);
	XP_API_SPRINTF(tmp, "Failed: %d.", GetFailedTestCount());
	handler->DrawString(x,y,tmp);
	y+=2*DEF::LINE_HEIGHT;	
	
	XP_API_MEMSET(tmp, 0, 128);
	XP_API_SPRINTF(tmp, "Passed: %d.", m_testSuccess);
	handler->DrawString(x,y,tmp);
	y+=2*DEF::LINE_HEIGHT;		

	char tmpSec[32];
	XP_API_MEMSET(tmpSec, 0, 32);
	XP_API_FTOA(m_secondsElapsed, tmpSec, 10);
	XP_API_MEMSET(tmp, 0, 128);
	XP_API_SPRINTF(tmp, "Time Elapsed: %s s.", tmpSec);
	handler->DrawString(x,y,tmp);
}


bool GameStateTest::HandleTestProcess( _TOUCH_EVENT event)
{
	bool bHandled = false;	
	
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{		
		XP_API_DEL(m_mockGLXPlayerLogin);
		m_testSuccess = 0;
		m_secondsElapsed = 0.0f;
		ResetTestResult();
		
		SetOnlineSubState(TEST_MENU);
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateTest::HandleTestMenu( _TOUCH_EVENT event)
{
	bool bHandled = false;
	if ( m_testMenu->HandleEvent( event ) )
	{
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_state = DEF::GAME_STATE_MENU;		
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateTest::HandleTestFinish( _TOUCH_EVENT event)
{
	bool bHandled = false;	
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{		
		XP_API_DEL(m_mockGLXPlayerLogin);
		XP_API_DEL(m_mockGLXPlayerUser);
		XP_API_DEL(m_mockGLXPlayerUserFriend);
		XP_API_DEL(m_mockGLXPlayerLeaderboard);
		XP_API_DEL(m_mockGLXPlayerMessage);
		XP_API_DEL(m_mockGLXPlayerChat);
		XP_API_DEL(m_mockGLXPlayerYoutube);
		XP_API_DEL(m_mockGLXPlayerLobby);
		
		XP_API_DEL(m_GLXPlayerLogin);

		m_testSuccess = 0;
		m_secondsElapsed = 0.0f;
		ResetTestResult();		
		SetOnlineSubState(TEST_MENU);		
		bHandled = true;
	}	
	return bHandled;
}

void GameStateTest::TestFinishAll()
{	
	TestResults::TestFinishAll();
	m_testSuccess = GetTotalTestCount() - GetFailedTestCount();		

	XP_FILE_HANDLE pFile;		

#ifdef OS_IPHONE
	char buffer[FILENAME_MAX];
	GetSaveFilePath(buffer, UNIT_TEST_OUTPUT_NAME);
	pFile = XP_API_FILE_OPEN( buffer, "at+" );	
#else
	pFile = XP_API_FILE_OPEN( UNIT_TEST_OUTPUT_NAME, "at" );
#endif

	if(pFile != NULL)
	{
		signed   int nWritten;
		
		char tmp[128];
		XP_API_MEMSET(tmp, 0, 128);
		XP_API_SPRINTF(tmp, "------- Test Result -------\n");			
		nWritten = XP_API_FILE_WRITE((void*)tmp, 1, XP_API_STRLEN(tmp), pFile);	
		
		XP_API_MEMSET(tmp, 0, 128);
		XP_API_SPRINTF(tmp, "Total Test: %d.\n", GetTotalTestCount());
		nWritten = XP_API_FILE_WRITE((void*)tmp, 1, XP_API_STRLEN(tmp), pFile);	

		XP_API_MEMSET(tmp, 0, 128);
		XP_API_SPRINTF(tmp, "Test Failed: %d.\n", GetFailedTestCount());
		nWritten = XP_API_FILE_WRITE((void*)tmp, 1, XP_API_STRLEN(tmp), pFile);	
		
		
		XP_API_MEMSET(tmp, 0, 128);
		XP_API_SPRINTF(tmp, "Test Passed: %d.\n", m_testSuccess);
		nWritten = XP_API_FILE_WRITE((void*)tmp, 1, XP_API_STRLEN(tmp), pFile);		

		XP_API_MEMSET(tmp, 0, 128);
		XP_API_SPRINTF(tmp, "Failure: %d.\n", GetFailureCount());
		nWritten = XP_API_FILE_WRITE((void*)tmp, 1, XP_API_STRLEN(tmp), pFile);		

		char tmpSec[32];
		XP_API_MEMSET(tmpSec, 0, 32);
		XP_API_FTOA(m_secondsElapsed, tmpSec, 10);

		XP_API_MEMSET(tmp, 0, 128);
		XP_API_SPRINTF(tmp, "Total Time Elapsed: %s s.\n", tmpSec);
		nWritten = XP_API_FILE_WRITE((void*)tmp, 1, XP_API_STRLEN(tmp), pFile);	

		XP_API_FILE_CLOSE(pFile);
	}	
	SetOnlineSubState(TEST_FINISH);
}

void GameStateTest::OnTestStart(TestDetails const& test)
{	
	TestResults::OnTestStart(test);
	XP_FILE_HANDLE pFile;		

#ifdef OS_IPHONE
	char buffer[FILENAME_MAX];
	GetSaveFilePath(buffer, UNIT_TEST_OUTPUT_NAME);
	pFile = XP_API_FILE_OPEN( buffer, "at+" );	
#else
	pFile = XP_API_FILE_OPEN( UNIT_TEST_OUTPUT_NAME, "at" );
#endif

	if(pFile != NULL)
	{
		signed   int nWritten;
	
		char tmp[128];
		XP_API_MEMSET(tmp, 0, 128);
		XP_API_SPRINTF(tmp, "Test started...\n");			
		nWritten = XP_API_FILE_WRITE((void*)tmp, 1, XP_API_STRLEN(tmp), pFile);
		
		XP_API_MEMSET(tmp, 0, 128);
		XP_API_SPRINTF(tmp, "Suite: %s.\n", test.suiteName);
		nWritten = XP_API_FILE_WRITE((void*)tmp, 1, XP_API_STRLEN(tmp), pFile);
		
		XP_API_MEMSET(tmp, 0, 128);
		XP_API_SPRINTF(tmp, "Test Case: %s.\n",test.testName);
		nWritten = XP_API_FILE_WRITE((void*)tmp, 1, XP_API_STRLEN(tmp), pFile);		

		XP_API_FILE_CLOSE(pFile);			
	}		
}

void GameStateTest::OnTestFailure(TestDetails const& test, char const* failure)
{
	TestResults::OnTestFailure(test, failure);
	XP_FILE_HANDLE pFile;		

#ifdef OS_IPHONE
	char buffer[FILENAME_MAX];
	GetSaveFilePath(buffer, UNIT_TEST_OUTPUT_NAME);
	pFile = XP_API_FILE_OPEN( buffer, "at+" );	
#else
	pFile = XP_API_FILE_OPEN( UNIT_TEST_OUTPUT_NAME, "at" );
#endif

	if(pFile != NULL)
	{
		signed   int nWritten;
	
		char tmp[128];
		XP_API_MEMSET(tmp, 0, 128);
		XP_API_SPRINTF(tmp, "Test failed...\n");			
		nWritten = XP_API_FILE_WRITE((void*)tmp, 1, XP_API_STRLEN(tmp), pFile);	
		
		XP_API_MEMSET(tmp, 0, 128);
		XP_API_SPRINTF(tmp, "Fail message: %s.\n", failure);			
		nWritten = XP_API_FILE_WRITE((void*)tmp, 1, XP_API_STRLEN(tmp), pFile);	
		
		XP_API_FILE_CLOSE(pFile);
	}			
}

void GameStateTest::OnTestFinish(TestDetails const& test, float secondsElapsed)
{
	TestResults::OnTestFinish(test, secondsElapsed);

	XP_FILE_HANDLE pFile;		

#ifdef OS_IPHONE
	char buffer[FILENAME_MAX];
	GetSaveFilePath(buffer, UNIT_TEST_OUTPUT_NAME);
	pFile = XP_API_FILE_OPEN( buffer, "at+" );	
#else
	pFile = XP_API_FILE_OPEN( UNIT_TEST_OUTPUT_NAME, "at" );
#endif

	if(pFile != NULL)
	{
		signed   int nWritten;
	
		char tmp[128];
		XP_API_MEMSET(tmp, 0, 128);
		XP_API_SPRINTF(tmp, "Test finished...\n");			
		nWritten = XP_API_FILE_WRITE((void*)tmp, 1, XP_API_STRLEN(tmp), pFile);	
		
		char tmpSec[32] = {0};	
		XP_API_FTOA(secondsElapsed, tmpSec, 10);
		XP_API_MEMSET(tmp, 0, 128);
		XP_API_SPRINTF(tmp, "Time Elapsed: %s s.\n\n", tmpSec);
		nWritten = XP_API_FILE_WRITE((void*)tmp, 1, XP_API_STRLEN(tmp), pFile);	

		XP_API_FILE_CLOSE(pFile);
	}

	m_secondsElapsed += secondsElapsed;
}


#endif // ENABLE_UNIT_TEST
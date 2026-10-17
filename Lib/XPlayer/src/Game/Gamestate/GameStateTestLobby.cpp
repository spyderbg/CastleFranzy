#include "GameStateTestLobby.h"

#ifdef LOBBY_THREAD_TEST

extern bool g_bTestLogin;
extern bool g_bTestCreate;
extern bool g_bTestFilter;
extern bool g_bTestJoin;
extern int g_iThreadCount;
extern bool g_bRandom;

char filetime[20] = { 0 };

GameStateTestLobby::GameStateTestLobby(GameStateHandler * handler)
: GameState( handler )
{
	///@Softkey position
	m_SoftLeft.left = DEF::BUTTON_LEFT_X;
	m_SoftLeft.top = DEF::BUTTON_LEFT_Y;
	m_SoftLeft.width = DEF::BUTTON_LEFT_W;
	m_SoftLeft.height = DEF::BUTTON_LEFT_H;

	m_SoftRight.left = DEF::BUTTON_RIGHT_X;
	m_SoftRight.top = DEF::BUTTON_RIGHT_Y;
	m_SoftRight.width = DEF::BUTTON_RIGHT_W;
	m_SoftRight.height = DEF::BUTTON_RIGHT_H;
	
	m_SoftMid.left = DEF::BUTTON_MID_X;
	m_SoftMid.top = DEF::BUTTON_MID_Y;
	m_SoftMid.width = DEF::BUTTON_MID_W;
	m_SoftMid.height = DEF::BUTTON_MID_H;
	
	
	///@Input Box
	m_InputBox = XP_API_NEW InputBox(155,205);
	m_selectedIndex = 0;	

	m_state = DEF::GAME_STATE_TEST_LOBBY;
}

GameStateTestLobby::~GameStateTestLobby()
{
	XP_API_DEL( m_InputBox );
}

bool GameStateTestLobby::HandleEvent( _TOUCH_EVENT event )
{
	bool bHandled = false;

	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		m_state = DEF::GAME_STATE_MULTIPLAYER;

		char* s = NULL;
		GetCurrentSystemTime( &s );
		XP_API_STRCPY( filetime, s );
		filetime[0] = '_';
		filetime[XP_API_STRLEN(filetime)-1] = '_';

		int len = XP_API_STRLEN(filetime);
		while( len-- )
		{
			if( filetime[len] == ':' )
				filetime[len] = '_';
		}
		XP_API_DEL( s );
		m_InputBox->deleteTextInTextBox();
	}
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_state = DEF::GAME_STATE_MENU;
		m_InputBox->deleteTextInTextBox();
	}
	else if ( event.isClicked && m_SoftMid.Contain(event.position) )
	{
		if( m_selectedIndex == 0)
		{
			g_bRandom = !g_bRandom;
			if( g_bRandom )
			{
				g_bTestLogin = false;
				g_bTestCreate = false;
				g_bTestFilter = false;
				g_bTestJoin = false;
			}
			g_iThreadCount = 0;
		}
		if( !g_bRandom )
		{
			if( m_selectedIndex == 1)
			{
				g_bTestLogin = !g_bTestLogin;
				if( !g_bTestLogin )
					g_iThreadCount = 0;
			}
			else if( m_selectedIndex == 2 )
			{
				g_bTestCreate = !g_bTestCreate;
				if( g_bTestCreate )
				{
					g_bTestLogin = true;
					g_bTestFilter = false;
				}
			}
			else if( m_selectedIndex == 3 )
			{
				g_bTestFilter = !g_bTestFilter;
				if( g_bTestFilter )
				{
					g_bTestLogin = true;
					g_bTestCreate = false;
				}
			}
			else if( m_selectedIndex == 4 )
			{
				g_bTestJoin = !g_bTestJoin;
				if( g_bTestJoin )
				{
					g_bTestLogin = true;
					g_bTestFilter = true;
					g_bTestCreate = false;
				}
			}
		}
	}
	else if ( event.isClicked && m_UpKey.Contain(event.position) )
	{
		m_selectedIndex --;		
		if(m_selectedIndex < 0)
		{			
			m_selectedIndex = 5;			
		}	
		m_InputBox->deleteTextInTextBox();
		bHandled = true;
	}
	else if ( event.isClicked && m_DownKey.Contain(event.position) )
	{
		m_selectedIndex ++;
		if(m_selectedIndex > 5)
		{	
			m_selectedIndex = 0;			
		}
		m_InputBox->deleteTextInTextBox();
		bHandled = true;
	}
	else
	{
		if( m_selectedIndex == 5 && (g_bTestLogin || g_bRandom) )
		{
			bHandled = m_InputBox->updateInputBox(event);
			if ( bHandled )
			{
				char* tmp = m_InputBox->getInputBoxString();

				g_iThreadCount = XP_API_ATOI( tmp );
			}
		}
	}

	return bHandled;
}

void GameStateTestLobby::Paint()
{
	m_handler->ClearScreen();

	int x = DEF::START_POS_X;
	int y = DEF::LINE_HEIGHT;

	m_handler->DrawString(x,y,"--- Lobbt Test Menu ---");	
	y+=DEF::LINE_HEIGHT;	
	m_handler->DrawString(x,y,"--- Press select to change status ---");	
	y+=DEF::LINE_HEIGHT;	

	//draw button
	m_handler->DrawButton(m_SoftRight);
	m_handler->DrawString(m_SoftRight.left,m_SoftRight.top,"BACK");
	m_handler->DrawButton(m_SoftLeft);
	m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"GO");
	m_handler->DrawButton(m_SoftMid);
	m_handler->DrawString(m_SoftMid.left,m_SoftMid.top,"SELECT");

	m_handler->DrawButton(m_UpKey);
	m_handler->DrawString(m_UpKey.left,m_UpKey.top,"UP");
	m_handler->DrawButton(m_DownKey);
	m_handler->DrawString(m_DownKey.left,m_DownKey.top,"DOWN");

	char tmp[128] = {0};	
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Random Flow: %s", g_bRandom ? "YES" : "NO" );
	m_handler->DrawString(x,y,tmp);
	if( m_selectedIndex == 0 )
	{
		m_handler->DrawString(x - 20 , y, ">");	
	}
	y+=DEF::LINE_HEIGHT;	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Login Test: %s", g_bTestLogin ? "YES" : "NO" );
	m_handler->DrawString(x,y,tmp);
	if( m_selectedIndex == 1 )
	{
		m_handler->DrawString(x - 20 , y, ">");	
	}
	y+=DEF::LINE_HEIGHT;

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Creat Test: %s", g_bTestCreate ? "YES" : "NO" );
	m_handler->DrawString(x,y,tmp);
	if( m_selectedIndex == 2 )
	{
		m_handler->DrawString(x - 20 , y, ">");	
	}
	y+=DEF::LINE_HEIGHT;	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Filter Test: %s", g_bTestFilter ? "YES" : "NO" );
	m_handler->DrawString(x,y,tmp);
	if( m_selectedIndex == 3 )
	{
		m_handler->DrawString(x - 20 , y, ">");	
	}
	y+=DEF::LINE_HEIGHT;	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Join Test: %s", g_bTestJoin ? "YES" : "NO" );
	m_handler->DrawString(x,y,tmp);
	if( m_selectedIndex == 4 )
	{
		m_handler->DrawString(x - 20 , y, ">");	
	}
	y+=DEF::LINE_HEIGHT;	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Thread Num: %d", g_iThreadCount );
	m_handler->DrawString(x,y,tmp);
	if( m_selectedIndex == 5 )
	{
		m_handler->DrawString(x - 20 , y, ">");	
	}

	m_InputBox->showTextBox( m_handler );
}

void GameStateTestLobby::SwitchState()
{
	SetGameState(m_state);
}

void GameStateTestLobby::UpdateAnimation()
{
}
#endif
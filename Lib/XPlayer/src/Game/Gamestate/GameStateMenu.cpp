#include "stdafx.h"
#include "GameStateMenu.h"
#include "../../include/Porting.h"

#ifdef LOBBY_THREAD_TEST
bool g_bSaveLog = false;
#endif

GameStateMenu::GameStateMenu( GameStateHandler * handler ) : GameState( handler )
{
	m_menu = XP_API_NEW Menu(DEF::START_POS_X,DEF::START_POS_Y);	
//	m_menu->AddMenuItem( MENU_INSTANT_PLAY, this );	
	m_menu->AddMenuItem( MENU_MULTI_PLAYER, this );	
	m_menu->AddMenuItem( MENU_GAMELOFT_LIVE, this );	
	#ifdef MMO_CHAT
	m_menu->AddMenuItem(MENU_MMO,this);
    #endif
//	m_menu->AddMenuItem( MENU_OPTION, this );
#ifdef ENABLE_UNIT_TEST
	m_menu->AddMenuItem( MENU_UNIT_TEST, this );
#endif //ENABLE_UNIT_TEST

#ifdef LOBBY_THREAD_TEST
	m_menu->AddMenuItem( MENU_LOBBY_THREAD_SAVE_LOG, this );
#endif

	m_menu->AddMenuItem( MENU_EXIT, this );

	m_state = DEF::GAME_STATE_MENU;
}

GameStateMenu::~GameStateMenu(void)
{
	XP_API_DEL(m_menu);
}

void GameStateMenu::DrawCommandBar( GameStateHandler * handler, CommandBarState state )
{
	handler->DrawButton(m_menu->m_UpKey);
	handler->DrawButton(m_menu->m_DownKey);

	switch( state )
	{
	case CB_GO_NULL:
		handler->DrawButton(m_menu->m_SoftLeft);
		break;
	default:
		break;
	}

	GameState::DrawCommandBar(handler, state);
}

bool GameStateMenu::HandleEvent( _TOUCH_EVENT event )
{
	return m_menu->HandleEvent(event);
}

void GameStateMenu::SwitchState()
{
	SetGameState(m_state);
}

void GameStateMenu::Paint()
{
	m_handler->ClearScreen();
	m_menu->Repaint(m_handler);

#ifdef LOBBY_THREAD_TEST
	char tmp[128] = {0};	
	XP_API_MEMSET( tmp, 0, 128 );

	XP_API_SPRINTF( tmp, "SaveLog: %s", g_bSaveLog ? "YES" : "NO" );
	m_handler->DrawString( 20, m_menu->m_SoftLeft.top - 30, tmp);
#endif

	DrawCommandBar(m_handler, GameState::CB_GO_NULL);
}

void GameStateMenu::UpdateAnimation()
{
	
}

void GameStateMenu::OnMenuItemClicked( char * menu_name )
{
	if ( XP_API_STRCMP( menu_name, MENU_GAMELOFT_LIVE ) == 0 )
	{
		m_state = DEF::GAME_STATE_ONLINE;
	}	
	else if ( XP_API_STRCMP( menu_name, MENU_MULTI_PLAYER ) == 0 )
	{
#ifdef LOBBY_THREAD_TEST
		m_state = DEF::GAME_STATE_TEST_LOBBY;
#else
		m_state = DEF::GAME_STATE_MULTIPLAYER;
#endif
	}
	else if ( XP_API_STRCMP( menu_name, MENU_EXIT ) == 0 )
	{
		m_state = DEF::GAME_STATE_EXIT;
	}
	#ifdef MMO_CHAT
	else if(XP_API_STRCMP( menu_name, MENU_MMO ) == 0)
	{
	
	m_state = DEF::GAME_STATE_MMO;
	}
#endif
#ifdef ENABLE_UNIT_TEST
	else if(XP_API_STRCMP( menu_name, MENU_UNIT_TEST ) == 0)
	{
		m_state = DEF::GAME_STATE_TEST;
	}	
#endif //ENABLE_UNIT_TEST
#ifdef LOBBY_THREAD_TEST
	else if(XP_API_STRCMP( menu_name, MENU_LOBBY_THREAD_SAVE_LOG ) == 0)
	{
		g_bSaveLog = true;
	}	
#endif
}
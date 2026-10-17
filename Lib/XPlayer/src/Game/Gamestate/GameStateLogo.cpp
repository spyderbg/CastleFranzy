#include "stdafx.h"
#include "GameStateLogo.h"
#include "../../include/Porting.h"

GameStateLogo::GameStateLogo( GameStateHandler * handler ) : GameState( handler )
{
	//m_logo = NULL;
	m_frameCount = 0;
	m_alpha = 50;
}

GameStateLogo::~GameStateLogo(void)
{
	//delete m_logo;
}

bool GameStateLogo::HandleEvent( _TOUCH_EVENT event )
{
	return false;
}

void GameStateLogo::SwitchState()
{
	if ( m_frameCount < 20 )
	{
		SetGameState( DEF::GAME_STATE_LOGO );
	}
	else
	{
		SetGameState( DEF::GAME_STATE_MENU );
	}
}

/**
 * Show splash.
 *
 */
void GameStateLogo::Paint()
{
	m_handler->ClearScreen();

	m_handler->DrawString(30,120,"XPlayer Demo 0.0.6");
	
	m_frameCount++;
}

void GameStateLogo::UpdateAnimation()
{
	m_alpha += 20;
	if ( m_alpha > 255 )
	{
		m_alpha = 255;
	}
}
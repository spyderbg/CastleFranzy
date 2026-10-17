#include "stdafx.h"
#include "GameStateServerConfig.h"
#include "../../../include/Porting.h"



GameStateServerConfig::GameStateServerConfig( GameStateHandler * handler ) : GameState( handler )
{

	m_state = DEF::GAME_STATE_GET_SERVER_CONFIG;
	m_processState=GET_CONFIG_SEND;
	m_frame=0;
	m_errorCode=0;
	m_GLXPlayerSereverConfig=XP_API_NEW GLXPlayerSereverConfig();
	m_GLXPlayerSereverConfig->RegisterObserver(this);
}

GameStateServerConfig::~GameStateServerConfig(void)
{
	XP_API_DEL(m_GLXPlayerSereverConfig);
}



bool GameStateServerConfig::HandleEvent( _TOUCH_EVENT event )
{
	if(m_processState==GET_CONFIG_ERROR)
	{
		if ( event.isClicked && m_SoftRight.Contain(event.position) )
		{
			m_state = DEF::GAME_STATE_EXIT;
		}
	}

	return false;
}

void GameStateServerConfig::SwitchState()
{
	SetGameState(m_state);
}

void GameStateServerConfig::Paint()
{
	m_handler->ClearScreen();
	switch(m_processState)
	{
	case GET_CONFIG_SEND:
	case GET_CONFIG_WAIT:
	case GET_CONFIG_PROCESS:
		DrawWaiting(m_handler);
		break;
	case GET_CONFIG_ERROR:
		DrawError(m_handler);
		m_handler->DrawButton(m_SoftRight);
		GameState::DrawCommandBar(m_handler, CB_NULL_BACK,false);
		break;

	}

}

void GameStateServerConfig::UpdateAnimation()
{
	m_GLXPlayerSereverConfig->Update();
	switch(m_processState)
	{
	case GET_CONFIG_SEND:
		if(GLXPlayerSereverConfig::s_isLoadConfig==true)
		{
			m_processState=GET_CONFIG_PROCESS;
		}else
		{
			int ret=0;
			m_processState=GET_CONFIG_WAIT;
			m_GLXPlayerSereverConfig->SendGetSreverConfig(ret);
			if(ret==-1)
			{
			m_processState=GET_CONFIG_ERROR;
			}else if(ret==1)
			{
			m_processState=GET_CONFIG_PROCESS;
			}
			
		}

		break;
	case GET_CONFIG_WAIT:
		break;
	case GET_CONFIG_PROCESS:
		m_state = DEF::GAME_STATE_MENU;
		break;
	case GET_CONFIG_ERROR:
		break;

	}


}

void GameStateServerConfig::OnNetworkError()
{

	m_processState=GET_CONFIG_ERROR;
	m_errorCode = ERROR_CONNECTION;
	XP_DEBUG_OUT( "[GameStateServerConfig] OnUserNetworkError: %d\n", m_errorCode);
}

void GameStateServerConfig::OnRequestTimeout( int func_id )
{
	m_processState=GET_CONFIG_ERROR;
	m_errorCode = ERROR_REQUEST_TIME_OUT;
	XP_DEBUG_OUT( "[GameStateServerConfig] OnRequestTimeout: Function: %d - Error: %d\n", func_id, m_errorCode);

}
void GameStateServerConfig::OnRequestFailure( int func_id, int error_code )
{
	switch(func_id)
	{
	case FUNCTION_GET_WEBCONFIG:
		m_processState=GET_CONFIG_ERROR;
		m_errorCode = error_code;			
		XP_DEBUG_OUT( "[GameStateServerConfig] OnUserFunctionError: %d\n", error_code);	
		break;
	}
}
void GameStateServerConfig::OnRequestSuccess( int func_id, char * data, int data_len )
{
	switch(func_id)
	{
	case FUNCTION_GET_WEBCONFIG:
		m_processState=GET_CONFIG_PROCESS;
		break;
	}
}

void GameStateServerConfig::DrawWaiting( GameStateHandler * handler )
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

void GameStateServerConfig::DrawError( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	handler->DrawString(x,y,"--- Error ---");
	y+=2*DEF::LINE_HEIGHT;

	char msg[32];
	XP_API_MEMSET( msg, 0, 32 );
	XP_API_SPRINTF( msg, "Error code = %d", m_errorCode);

	handler->DrawString(x,y,msg);	
}
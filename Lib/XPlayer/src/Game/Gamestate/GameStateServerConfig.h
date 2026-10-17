#ifndef __GAME_STATE_SECON_H__
#define __GAME_STATE_SECON_H__

#include "gamestate.h"
#include "../../../include/Common/GLXPlayerObserver.h"
#include "../../../include/Common/GLXPlayerSereverConfig.h"

/**
* The startup splash.
* <p>
*
* @version 0.0.1
*
*/
class GameStateServerConfig :
	public GameState,GLXPlayerObserver
{

public:
	enum GetServerState{
		GET_CONFIG_SEND,
		GET_CONFIG_WAIT,
		GET_CONFIG_PROCESS,
		GET_CONFIG_ERROR,

	};
	GameStateServerConfig( GameStateHandler * handler );

	bool HandleEvent( _TOUCH_EVENT event );

	void SwitchState();

	void Paint();

	void UpdateAnimation();

	~GameStateServerConfig(void);
	void OnNetworkError();	
	void OnRequestTimeout( int func_id );
	void OnRequestFailure( int func_id, int error_code );
	void OnRequestSuccess( int func_id, char * data, int data_len );
	void DrawWaiting( GameStateHandler * handler );
	void DrawError( GameStateHandler * handler );
private:
	GLXPlayerSereverConfig* m_GLXPlayerSereverConfig;
	DEF::GAME_STATE_ENUM m_state;
	GetServerState m_processState;
	int m_frame;
	int m_errorCode;

};

#endif // __GAME_STATE_SECON_H__
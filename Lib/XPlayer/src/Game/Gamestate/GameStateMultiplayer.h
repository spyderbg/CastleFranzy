#ifndef __GAME_STATE_MULTIPLAYER_H__
#define __GAME_STATE_MULTIPLAYER_H__

#include "gamestate.h"
#include "../../include/Porting.h"

#include "../../include/User/GLXPlayerUser.h"
#include "../../include/Login/GLXPlayerLogin.h"
#include "../../include/Message/GLXPlayerMessage.h"
#include "../../include/Leaderboard/GLXPlayerLeaderboard.h"
#include "../../include/Multiplayer/Common/GLXPlayerMPBaseObserver.h"
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
#include "../../include/Multiplayer/P2P/GLXPlayerMPbyP2P.h"
#else
#include "../../include/Multiplayer/ForwardbyServer/GLXPlayerMPByServer.h"
#endif
#include "../../include/Buddy/GLXPlayerUserFriend.h"

#include "Menu.h"
#include "InputBox.h"

/*@ define the max size of session list*/
#define MAX_LIST_SIZE 5

class GameStateMultiplayer : public GameState, public MenuItemObserver, GLXPlayerMPBaseObserver, GLXPlayerObserver
{
public:
	GameStateMultiplayer(GameStateHandler * handler);
	virtual ~GameStateMultiplayer(void);
	
	enum OnlineSubState
	{	
		ONLINE_PROCESS,
		ONLINE_LOGIN,	
		ONLINE_REGISTER,
		ONLINE_RANKING,
		ONLINE_TOP_RANKING,
		ONLINE_RANKING_AROUND,
		ONLINE_TOP_FRIEND_RANKING,
		
		ONLINE_MSG_LIST,
		ONLINE_MSG_READ,
		ONLINE_MSG_INFO,

		ONLINE_MP_SESSION_BUDDY,
		ONLINE_MP_SESSION_BUDDY_PLAY,
				
		ONLINE_MP_MENU,		
		ONLINE_MP_MENU_ONLINE,		
		ONLINE_MP_MENU_LOCAL,		
		ONLINE_MP_MENU_HISCORE,		
		ONLINE_MP_PROCESS,
		ONLINE_MP_IN_SESSION,	
		ONLINE_MP_IN_SESSION_LIST,	
		ONLINE_MP_IN_GAME,	
		ONLINE_MP_END_GAME,	
		ONLINE_MP_FINISH_GAME_CONFIRM,
		ONLINE_MP_EVALUATE_PLAYER,	
		ONLINE_MP_ERROR,
		ONLINE_ERROR
	};	

	void Paint();
	bool HandleEvent( _TOUCH_EVENT event );
	void DrawCommandBar( GameStateHandler * handler, CommandBarState state, bool isDrawUpDown = true );
	void SwitchState();
	void UpdateAnimation();
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	void UpdateGame();
#endif
	void OnMenuItemClicked( char * menu_name );
	void SetOnlineSubState(OnlineSubState state);	
	
	///@Handle function
	bool HandleEventLogin( _TOUCH_EVENT event );
	bool HandleEventRegister( _TOUCH_EVENT event );
	bool HandleEventError( _TOUCH_EVENT event );
	bool HandleEventProcess( _TOUCH_EVENT event );
	bool HandleEventRanking( _TOUCH_EVENT event );
	
	bool HandleMsgList( _TOUCH_EVENT event );
	bool HandleViewMessage( _TOUCH_EVENT event );
	bool HandleMsgInfo( _TOUCH_EVENT event );	
	
	bool HandleEventSession( _TOUCH_EVENT event );
	bool HandleEventSessionList( _TOUCH_EVENT event );
	bool HandleEventInGame( _TOUCH_EVENT event );
	bool HandleEventMPMenu( _TOUCH_EVENT event);
	bool HandleEventMPMenuLocal( _TOUCH_EVENT event);
	bool HandleEventMPMenuOnline( _TOUCH_EVENT event);
	bool HandleEventMPMenuHiscore( _TOUCH_EVENT event);
	bool HandleEventMPEndGame( _TOUCH_EVENT event);
	bool HandleEventEvaluatePlayer( _TOUCH_EVENT event);
	bool HandleEventMPSessionBuddy( _TOUCH_EVENT event);
	bool HandleEventInviteToPlayGame( _TOUCH_EVENT event );
		
	///@Draw functions
	void DrawLogin( GameStateHandler * handler );
	void DrawRegister( GameStateHandler * handler );
	void DrawError(GameStateHandler * handler);
	void DrawWaiting( GameStateHandler * handler );
	void DrawRanking( GameStateHandler * handler );
	void DrawTopRanking( GameStateHandler * handler );
	void DrawRankingAround( GameStateHandler * handler );
	void DrawTopFriendRanking( GameStateHandler * handler );

	void DrawMsgList( GameStateHandler * handler );
	void DrawMessage( GameStateHandler * handler );
	void DrawMsgInfo( GameStateHandler * handler );
	
	void DrawMPError(GameStateHandler * handler);
	void DrawSession(GameStateHandler * handler);
	void DrawSessionList(GameStateHandler * handler);
	void DrawInGame(GameStateHandler * handler);	
	void DrawMPEndGame( GameStateHandler * handler );	
	void DrawEvaluatePlayer( GameStateHandler * handler );	
	void DrawMPSessionBuddy( GameStateHandler * handler );
	void DrawInviteToPlayGame( GameStateHandler * handler );
	
	///@	GLXPlayerObserver
	
	void OnNetworkError();	
	void OnRequestTimeout( int func_id );
	void OnRequestFailure( int func_id, int error_code );
	void OnRequestSuccess( int func_id, char * data, int data_len );
	
	///@ GLXPlayerMultiPlayerObserver
	
	//Error handle
	void OnMPNetworkError(int errorCode);	
	void OnMPFunctionError( char* errMsg, int error_code );
	
	//Success handle
	void OnMPuPnPTestSuccess();
	void OnMPEstablishConnectionSuccess();
	void OnMPLoginSuccess(DataPacket* currentPacket = NULL);
	void OnMPCreateSessionSuccess(DataPacket* currentPacket = NULL);
	void OnMPListSessionSuccess(DataPacket* currentPacket = NULL);
	void OnMPDisconnectSuccess();
	void OnMPJoinSessionSuccess(DataPacket* currentPacket = NULL);
	void OnMPLeaveSessionSuccess();
	void OnMPLeaveGameSuccess();
	void OnMPKickOutPlayerSuccess();
	void OnMPStartGameSuccess(DataPacket* currentPacket = NULL);
	void OnMPFinishGameSuccess();	
	
private:
	
	bool LoadProfile();
	bool SaveProfile();
	
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	void SaveData( char * name, char * data )
	{
		XP_API_DEL( m_recv_user );
		XP_API_DEL( m_recv_data );

		m_recv_user = XP_API_STRNEW( name );
		m_recv_data = XP_API_STRNEW( data );
	}
#endif

#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	GLXPlayerMPbyP2P * m_GLXPlayerMultiPlayer;
#else
	GLXPlayerMPbyServer * m_GLXPlayerMultiPlayer;
#endif
	GLXPlayerUser * m_GLXPlayerUser;
	GLXPlayerLeaderboard * m_GLXPlayerLeaderboard;
	GLXPlayerMessage* m_GLXPlayerMessage;	
	GLXPlayerUserFriend* m_GLXPlayerUserFriend;
	GLXPlayerLogin * m_GLXPlayerLogin;
	
	Menu * m_multiPlayerMenu;
	Menu * m_multiPlayerMenuOnline;
	Menu * m_multiPlayerMenuLocal;
	Menu * m_multiPlayerMenuHiscore;
	char * m_errMsg;
	
	DEF::GAME_STATE_ENUM m_state;
	OnlineSubState m_sub_state;
	OnlineSubState m_pre_sub_state;
	int m_errorCode;
	
	InputBox* m_InputBox;
	int m_selectedIndex;	
	int m_frame;
	
	///@ User informations
	char m_username[15];
	char m_password[32];
	char* m_nationData;
	char* m_email;
	bool m_emailFlag;
	
	///@Hiscore relate variable
	//Is get ranking around user or not...
	unsigned short m_isRankAround;
	
	//Score array...
	char* m_scoreArray;
	int m_score;	
	
	///@ Game data ... 
	int gameX;
	int gameY;
	int oppX;
	int oppY;
	int oldX;
	int oldY;	
	
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	char * m_recv_user;
	char * m_recv_data;

	int m_gamedata;
	int m_gamedata_old;
#endif

	int m_currentMsgType;
	int m_gameMsgCount;
	int m_userMsgCount;
	int m_friendInvitedMsgCount;
	int m_gameInvitedMsgCount;	
	int m_chatInvitedMsgCount;
	
	int m_currentSessionIndex;
	int m_numberPlayer;
	char** m_playerName;
	int* m_playerScore;
	
	bool m_isWin;
	int m_currentUserIndex;

	_TouchRect m_SoftAdd1;
	_TouchRect m_SoftAdd2;
	int m_currentFriendIndex;
	char m_invite_play_username[64];
	char m_invite_play_usernumber[64];
};

#endif //__GAME_STATE_MULTIPLAYER_H__
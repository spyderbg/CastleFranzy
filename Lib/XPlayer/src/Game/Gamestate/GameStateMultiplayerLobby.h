#ifndef __GAME_STATE_MULTIPLAYER_LOBBY_H__
#define __GAME_STATE_MULTIPLAYER_LOBBY_H__

#include "gamestate.h"
#include "../../include/Porting.h"

#include "../../include/User/GLXPlayerUser.h"
#include "../../include/Login/GLXPlayerLogin.h"
//#include "Online/Message/GLXPlayerMessage.h"
//#include "Online/Leaderboard/GLXPlayerLeaderboard.h"
#include "../../include/Multiplayer/Common/GLXPlayerMPBaseObserver.h"
#include "../../include/Buddy/GLXPlayerUserFriend.h"
#include "../../include/Multiplayer/Lobby/GLXPlayerMPLobby.h"
#include "../../include/Multiplayer/Lobby/LobbyParameterAndQuery.h"

#include "Menu.h"
#include "InputBox.h"

#include <list>
#ifdef LOBBY_THREAD_TEST
#include "stresstest/StressLog.h"
#endif
/*@ define the max size of session list*/
#define MAX_LIST_SIZE 5

#ifdef LOBBY_THREAD_TEST
const int MAX_ONLINE_MESSAGE_COUNT = 50;
const int ONINE_MESSAGE_MAX_CHARACTOR = 1024;
//char** pLogMessage = NULL;
#endif

typedef struct _LOBBY_INFO
{
	_LOBBY_INFO()
	{
		name = NULL;
		gameParam = NULL;
		nationality = NULL;
	}
	~_LOBBY_INFO()
	{
		XP_API_DEL(name);
		XP_API_DEL(gameParam);
		XP_API_DEL(nationality);
	}
	char * name;
	char * gameParam;
	BYTE   visibility;
	BYTE   locked;
	int    userNum;
	int    spectatorNum;
	char * nationality;
	BYTE   conut;
	BYTE   status;
}LOBBYINFO;

typedef struct _LOBBY
{
	_LOBBY()
	{
		name = NULL;
		gameParam = NULL;
		nationality = NULL;
	}
	~_LOBBY()
	{
		XP_API_DEL(name);
		XP_API_DEL(nationality);
		XP_API_DEL(gameParam);
	}
	int	id;
	char * name;
	char * nationality;
	char * gameParam;
	BYTE   conut;
	BYTE   status;
	BYTE   visibility;
	BYTE   locked;
	int    userNum;
	int    spectatorNum;
}LOBBY;

typedef std::list < LOBBY * >	LOBBY_LIST;

typedef struct _PLAYER
{
	_PLAYER()
	{
		name = NULL;
		nationality = NULL;
		userParameter = NULL;
	}
	~_PLAYER()
	{
		XP_API_DEL(name);
		XP_API_DEL(nationality);
		XP_API_DEL(userParameter);
	}
	char * name;
	char * nationality;
	BYTE status;
	char * userParameter;
}PLAYER;

typedef std::list < PLAYER * >	PLAYER_LIST;

class CInGame;

typedef std::list < CInGame * >	GAME_LIST;

class GameStateMultiplayerLobby : public GameState, public MenuItemObserver, GLXPlayerMPBaseObserver, GLXPlayerObserver
{
public:
#ifdef LOBBY_THREAD_TEST
	GameStateMultiplayerLobby(GameStateHandler * handler, bool bReadName, bool bTestLogin, bool bTestCreate, bool bTestFilter, bool bTestJoin, bool bTestThread);
#else
	GameStateMultiplayerLobby(GameStateHandler * handler);
#endif

	virtual ~GameStateMultiplayerLobby(void);
	
	enum OnlineSubState
	{	
		ONLINE_MP_ERROR = 0,
		ONLINE_ERROR,

		ONLINE_PROCESS,
		ONLINE_LOGIN,	
		ONLINE_REGISTER,
		//ONLINE_RANKING,
		//ONLINE_TOP_RANKING,
		//ONLINE_RANKING_AROUND,
		//ONLINE_TOP_FRIEND_RANKING,
		
		//ONLINE_MSG_LIST,
		//ONLINE_MSG_READ,
		//ONLINE_MSG_INFO,

		//ONLINE_MP_SESSION_BUDDY,
		//ONLINE_MP_SESSION_BUDDY_PLAY,
				
		ONLINE_MP_MENU,		
		//ONLINE_MP_MENU_ONLINE,		
		//ONLINE_MP_MENU_LOCAL,		
		//ONLINE_MP_MENU_HISCORE,		
		ONLINE_MP_PROCESS,
		ONLINE_MP_IN_SESSION,	
		ONLINE_MP_IN_SESSION_LIST,	
		ONLINE_MP_IN_GAME_LOADING,
		ONLINE_MP_IN_GAME,	
		ONLINE_MP_IN_GAME_LOADING_TIMEOUT,
		//ONLINE_MP_END_GAME,	
		//ONLINE_MP_FINISH_GAME_CONFIRM,
		//ONLINE_MP_EVALUATE_PLAYER,	

		//ONLINE_LOBBY_LOGIN,
		ONLINE_LOBBY_MENU,
		ONLINE_LOBBY_CREAT,
		ONLINE_LOBBY_SET_PARAMETER,
		ONLINE_LOBBY_START_FAILED,
		ONLINE_LOBBY_LIST_INFO,
		ONLINE_LOBBY_LIST,
		ONLINE_LOBBY_LIST_NAME_QUERY,
		ONLINE_LOBBY_LIST_CONDITION_QUERY,
		ONLINE_LOBBY_LIST_FRIEND_QUERY,
		ONLINE_LOBBY_JOIN
	};	

	void Paint();
	bool HandleEvent( _TOUCH_EVENT event );
	void DrawCommandBar( GameStateHandler * handler, CommandBarState state, bool isDrawUpDown = true );
	void SwitchState();
	void UpdateAnimation();
	void OnMenuItemClicked( char * menu_name );
	void SetOnlineSubState(OnlineSubState state);	
	
	///@Handle function
	bool HandleEventLogin( _TOUCH_EVENT event );
	bool HandleEventRegister( _TOUCH_EVENT event );
	bool HandleEventError( _TOUCH_EVENT event );
	bool HandleEventProcess( _TOUCH_EVENT event );
	//bool HandleEventRanking( _TOUCH_EVENT event );
	
	bool HandleMsgList( _TOUCH_EVENT event );
	bool HandleViewMessage( _TOUCH_EVENT event );
	bool HandleMsgInfo( _TOUCH_EVENT event );	
	
	bool HandleEventSession( _TOUCH_EVENT event );
	bool HandleEventSessionList( _TOUCH_EVENT event );
	bool HandleEventStartFailed( _TOUCH_EVENT event );
	bool HandleEventSetLobbyParameter( _TOUCH_EVENT event );
	bool HandleEventCreatLobby( _TOUCH_EVENT event );
	bool HandleEventInGame( _TOUCH_EVENT event );
	bool HandleEventMPMenu( _TOUCH_EVENT event);
	//bool HandleEventMPMenuLocal( _TOUCH_EVENT event);
	bool HandleEventMPMenuOnline( _TOUCH_EVENT event);
	bool HandleEventMPMenuLobby( _TOUCH_EVENT event );
	//bool HandleEventMPMenuHiscore( _TOUCH_EVENT event);
	bool HandleEventMPEndGame( _TOUCH_EVENT event);
	bool HandleEventEvaluatePlayer( _TOUCH_EVENT event);
	bool HandleEventMPSessionBuddy( _TOUCH_EVENT event);
	bool HandleEventInviteToPlayGame( _TOUCH_EVENT event );
	bool HandleEventListInfo( _TOUCH_EVENT event );
	bool HandleEventListQuery( _TOUCH_EVENT event );
	bool HandleEventListNameQuery( _TOUCH_EVENT event );
	bool HandleEventListCondotionQuery( _TOUCH_EVENT event );
	bool HandleEventListFriendQuery( _TOUCH_EVENT event );
	bool HandleEventJoinLobby( _TOUCH_EVENT event );

	///@Draw functions
	void DrawLogin( GameStateHandler * handler );
	void DrawRegister( GameStateHandler * handler );
	void DrawError(GameStateHandler * handler);
	void DrawWaiting( GameStateHandler * handler );
	//void DrawRanking( GameStateHandler * handler );
	//void DrawTopRanking( GameStateHandler * handler );
	//void DrawRankingAround( GameStateHandler * handler );
	void DrawTopFriendRanking( GameStateHandler * handler );

	void DrawMsgList( GameStateHandler * handler );
	void DrawMessage( GameStateHandler * handler );
	void DrawMsgInfo( GameStateHandler * handler );
	
	void DrawMPError(GameStateHandler * handler);
	void DrawSession(GameStateHandler * handler);
	void DrawSessionList(GameStateHandler * handler);
	void DrawetSetLobbyParameter( GameStateHandler * handler );
	void DrawetStartFailed( GameStateHandler * handler );
	void DrawetCreatLobbyParameter( GameStateHandler * handler );
	void DrawInGame(GameStateHandler * handler);	
	void DrawMPEndGame( GameStateHandler * handler );	
	void DrawEvaluatePlayer( GameStateHandler * handler );	
	void DrawMPSessionBuddy( GameStateHandler * handler );
	void DrawInviteToPlayGame( GameStateHandler * handler );

	void DrawListInfo( GameStateHandler * handler );
	void DrawListQuery( GameStateHandler * handler );
	void DrawListNameQuery( GameStateHandler * handler );
	void DrawListConditionQuery( GameStateHandler * handler );
	void DrawListFriendQuery( GameStateHandler * handler );
	void DrawJoinLobby( GameStateHandler * handler );

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
	void OnMPLoginSuccess( DataPacket* currentPacket );
	void OnMPCreateSessionSuccess( DataPacket* currentPacket );
	bool OnMPListGameParameter(DataPacket* currentPacket);
	void OnMPListSessionSuccess( DataPacket* currentPacket );
	void OnMPDisconnectSuccess();
	void OnMPJoinSessionSuccess( DataPacket* currentPacket );
	void OnMPLeaveSessionSuccess();
	void OnMPKickOutPlayerSuccess();
	void OnMPStartGameSuccess(DataPacket* currentPacket);
	void OnMPStartGameFailed( int result );	
	void OnMPFinishGameSuccess();	
	void OnMPProcessPushMessages(DataPacket* currentPacket, int messageID);	
	void OnMPSetParamSuccess( int result );	
	void OnMPRejoinLobbySuccess( DataPacket* currentPacket, bool bNeedChangeStatus );
	void OnMPGetLobbyInfo( DataPacket* currentPacket );
	void OnMPListSessionSuccessWithGameParam( DataPacket* currentPacket );
	void OnMPGetLobbyForName( DataPacket* currentPacket );
	void OnMPGetLobbyForFriend( DataPacket* currentPacket );
	void OnMPAutoMatchRequeset( int errType );
	void OnMPAutoMatchRequesetCancel();

	//void OnMPLoginSuccess(){}
	//void OnMPCreateSessionSuccess(){}
	//void OnMPListSessionSuccess(){}
	//void OnMPJoinSessionSuccess(){}
	//void OnMPLeaveGameSuccess(){}
	//void OnMPStartGameSuccess(){}

	void TransToIp( char* &name );

	char* GetUserName() { return m_username; }
	char* GetPassword() { return m_password; }

	int GetSubState() { return m_sub_state; }

private:
	
	bool LoadProfile();
	bool SaveProfile();

	void OnMPGameOver();
	
	GLXPlayerMPLobby * m_GLXPlayerMultiPlayer;
	GLXPlayerUser * m_GLXPlayerUser;
	//GLXPlayerLeaderboard * m_GLXPlayerLeaderboard;
	//GLXPlayerMessage* m_GLXPlayerMessage;	
	GLXPlayerUserFriend* m_GLXPlayerUserFriend;
	GLXPlayerLogin * m_GLXPlayerLogin;
	
	Menu * m_multiPlayerMenu;
	//Menu * m_multiPlayerMenuOnline;
	//Menu * m_multiPlayerMenuLocal;
	//Menu * m_multiPlayerMenuHiscore;
	Menu * m_multiPlayerMenuLobby;
	char * m_errMsg;
	
	DEF::GAME_STATE_ENUM m_state;
	OnlineSubState m_sub_state;
	OnlineSubState m_pre_sub_state;
	int m_errorCode;
	
	InputBox* m_InputBox;
	int m_selectedIndex;	
	int m_frame;
	
	///@ User informations
	char m_lobbyname[32];
	char m_username[32];
	char m_nationlity[8];
	char m_password[32];
	char* m_nationData;
	char* m_email;
	bool m_emailFlag;
	
	char m_uid[128];
	long m_ggi;

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

	//lobby
	bool bLobbyState;

	LOBBY_LIST m_lobby_list;
	LOBBY_LIST::iterator m_curr_lobby_it;
	UserStatus m_user_status;

	PLAYER_LIST m_player_list;
	PLAYER*		m_player_myself;
	PLAYER_LIST::iterator m_curr_player_it;

	int m_lobby_id;

	//in game
	//CInGame** m_pInGame;
	GAME_LIST m_pInGame;
	GAME_LIST::iterator m_pGameMyself;//m_pInGame[0]

	//Game Server
public:
	void InitGameServerSocket();
	void DisGameServerSocket();
	void GameServerPlay();

	GLXPlayerSocket * m_udp_socket;
	char m_send[256];
	char m_buffer[1024];

	char m_input_server[64];
	short m_tcp_port;
	short m_udp_port;

	bool m_bHasReady;

	enum GAMETYPE
	{
		GAMETYPE_NONE = 0,
		GAMETYPE_OVER
	};
	GAMETYPE m_GameType;

	//parameter
	int		m_iMaxCount;
	bool    m_bPrivateRoom;
	bool    m_bLock;
	bool    m_bTimer;
	bool    m_bColor;

	enum GAME_COLOR
	{
		BLUE = 0,
		GREEN
	};

	//>>for parameter
	bool m_bDrawGameParamChanged;
	bool m_bDrawUserParamChanged;
	char m_szParamString[256];
	int timer;
	//<<end

	int m_iUserNumber;

	LOBBYINFO*  m_LobbyInfo;

	//int m_iCountPlayer;

	int m_iStartIndexLobby;
	int m_iCountQueryLobby;
	int m_iQueryTimer;

	enum{ NAME_QUERY_MAX_CHARACTOR = 20 };
	char  m_sQueryName[NAME_QUERY_MAX_CHARACTOR];

	bool m_bLuanchedJoin;

	CLobbyParameterAndQuery*	 m_lobbyParamAndQuery;
	bool   m_bVisibilityInFriendQuery;

	enum autoMatchType
	{
		AUTO_MATCH_NONE = 0,
		AUTO_MATCH_WAITING,
		AUTO_MATCH
	};
	autoMatchType   m_autoMatchType;

	char* m_sLobbyServerAddress;

	bool m_bSameAccountLogin;

#ifdef LOBBY_THREAD_TEST
public:
	bool m_bSendConnect;
	bool m_bReadName;
	char m_sSaveOnlineLog[MAX_ONLINE_MESSAGE_COUNT][ONINE_MESSAGE_MAX_CHARACTOR];
	int m_iCurrentLogIndex;

	int m_iSendTime;
	int m_iRequestTime;

	bool SaveTestLog();
	StressLog* m_log;

	bool m_bTestLogin;
	bool m_bTestCreate;
	bool m_bTestFilter;
	bool m_bTestJoin;
	bool m_bTestThread;

#endif
};

//<<in game
enum MESSAGETYPE
{
	READY = 0,
	KEY,
	DIE,
	JOIN
};

typedef struct _Point
{
	int x;
	int y;
}Point;

typedef struct _Square
{
	Point	ps[4];
}Square;

#define MAX_SQUARE_COUNT  16
#define SQUARE_SIZE		  13

class CInGame
{
public:
	CInGame();

	~CInGame()
	{
		XP_API_DEL( m_username );
	}


	bool m_bFilledSquare[MAX_SQUARE_COUNT][MAX_SQUARE_COUNT];

	Square sq;

	char* m_username;

	enum GAMEOVERTYPE
	{
		GAMEOVERTYPE_NONE = 0,
		GAMEOVERTYPE_LOSE,
		GAMEOVERTYPE_WIN, 
		GAMEOVERTYPE_QUIT
	};

	GAMEOVERTYPE m_GameOverType;
};
//<<end

#endif //__GAME_STATE_MULTIPLAYER_LOBBY_H__
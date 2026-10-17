#ifndef __GL_XPLAYER_MP_LOBBY_H__
#define __GL_XPLAYER_MP_LOBBY_H__

#include "../../Porting.h"
#include "ConnectionLobby.h"
#include "DataPacketLobby.h"
#include "../Common/GLXPlayerMPBase.h"
#include "LobbyParameterAndQuery.h"
#include "../../Buddy/GLXPlayerUserFriend.h"

class GLXPlayerMPLobby : public GLXPlayerMPBase
{
public:
	enum LOBBY_STATUS
	{
		LOBBY_STATUS_CONNECT = 0,
		LOBBY_STATUS_LOGIN,

		LOBBY_STATUS_LEAVE,
		LOBBY_STATUS_SET_STATUS,//user status is ready or unready

		LOBBY_STATUS_CREATE,
		LOBBY_STATUS_KICK_OUT,
		LOBBY_STATUS_SET_USER_PARAM,
		LOBBY_STATUS_SET_GAME_PARAM,
		LOBBY_STATUS_LAUNCH_GAME,

		
		LOBBY_STATUS_FILTER_FOLLOW_NAME,
		LOBBY_STATUS_FILTER_FOLLOW_CONDITION,
		LOBBY_STATUS_FILTER_FOLLOW_FRIENDID,
		LOBBY_STATUS_GET_MORE_INFO,
		LOBBY_STATUS_JOIN,

		LOBBY_STATUS_REJOIN,
		LOBBY_STATUS_DISCONNECT,
		//LOBBY_STATUS_KEEP_ALIVE,
		LOBBY_STATUS_AUTOMATCH_CANCLE,

		LOBBY_STATUS_FILTER_FOLLOW_NAME_GAMEPAMAM,
		LOBBY_STATUS_FILTER_FOLLOW_CONDITION_GAMEPAMAM,
		LOBBY_STATUS_FILTER_FOLLOW_FRIENDID_GAMEPAMAM,

		LOBBY_STATUS_COUNT   //this is a normal value, when no request and has responsed, this value is set.
	};

	LOBBY_STATUS m_LobbyStatus;
	void SetLobbyStatus(LOBBY_STATUS ls) { m_LobbyStatus = ls; }
	LOBBY_STATUS GetLobbyStatus() { return m_LobbyStatus; }

public:

	/**
	* GLXPlayerMultiPlayer Constructor. 
	*/	 
	GLXPlayerMPLobby(GLXPlayerMPBaseObserver * observer, char* serverAddress = NULL);

	/**
	* GLXPlayerMultiPlayer Destructor. 
	*/
	virtual ~GLXPlayerMPLobby(void);
	
	/**
	* Function Update.
	* Called when handling network request.
	* @see #Update()
	*/
	virtual void Update();
		
	virtual void mpDisconnect();

	void mpSendDisconnect();

	virtual void mpSendEstablishConnection();

	/**
	 * Multiplayer Specific - Test if the player is in a game session on the multiplayer server.
	 * <p>
	 * @return Returns true if in session, false otherwise.
	 */
	virtual bool mpIsInSession()
	{
		return ( m_MPConnectLevel >= CONNECT_LEVEL_IN_SESSION );
	}

	virtual bool mpIsInRoom()
	{
		return ( m_MPConnectLevel >= CONNECT_LEVEL_IN_SESSION );
	}

	/**
	 * Multiplayer Specific - Test if the player is in a playing a game on the multiplayer server.
	 * <p>
	 * @return Returns true if in game, false otherwise.
	 */
	virtual bool mpIsInGame()
	{
		return ( m_MPConnectLevel >= CONNECT_LEVEL_IN_GAME );
	}

	virtual bool mpIsMaster()
	{
		//return ( m_MPConnectLevel == CONNECT_LEVEL_IN_SESSION_MASTER) ||
		//	( m_MPConnectLevel == CONNECT_LEVEL_IN_GAME_MASTER);
		return m_master;
	}

protected:
	void Init();
	bool LoadConfig();
	/**
	 * Processes incoming queue messages;
	 * return true only if the message needs further processing
	 * @return boolean
	 */
	virtual bool mpProcessIncomingMessages();

//lobby client
public:

	/**
	 * login lobby.
	 * when login to gllive, the uid is returned by gllive server.
	**/
	void mpSendLobbyLogin(long ggi, char* uid, int len, BYTE bKeepLobbyStatus);

	/**
	 * send the create the lobby room message.
	 * "name" is the gllive's name
	 * "bVisibility" and "bLock" is used to set the the lobby properties.
	**/
	void mpSendCreateLobby(char* name, BYTE bVisibility, BYTE bLock, 
							int maxCount, char*  gameParam, char* playerParam, CLobbyParameterAndQuery* clobbyParamAndQuery );

	/**
	 * send the join lobby message
	 * "user_status" is checked that the user's mode is player or spectator
	**/
	void mpSendJoinLobby(int lobby_id, BYTE user_status, char* playerParameter);

	void mpSendGetLobbyList( int userNumber, int start_num, BYTE count, 
		CLobbyParameterAndQuery* clobbyParamAndQuery, BYTE bCheckJoinable = 0 );

	void mpSendLeaveLobby(/*int lobby_id*/);

	void mpSendLunchLobbyGame();

	/**
	 * set the player status. when lunch this game, all player must be set to "ready"
	**/
	void mpSendSetPlayerStatus(BYTE status);

	enum
	{
		NORMAL_RETURN_TO_LOBBY = 0x00,
		ERROR_RETURN_TO_LOBBY  = 0x01
	};

	void mpSendRejoinLobby( int iNormal );

	void mpSendSetGameParameter( char* gameParameter, int lenParam, CLobbyParameterAndQuery* clobbyParamAndQuery );
	
	void mpSendSetUserParameter( char* userParameter, int lenParam );

	void mpSendGetLobbyInfo( int lobbyID );

	void mpSendGetLobbyForName( char* nameQuery );

	void mpSendGetLobbyForFriend( int userNumber, BYTE bVisibility, GLXPlayerUserFriend* userFriend );

	void mpSendGetLobbyListWithGameParam( int userNumber, int start_num, BYTE count, 
		CLobbyParameterAndQuery* clobbyParamAndQuery, BYTE bCheckJoinable = 0 );

	void mpSendGetLobbyForNameWithGameParam( char* nameQuery );

	void mpSendGetLobbyForFriendWithGameParam( int userNumber, BYTE bVisibility, GLXPlayerUserFriend* userFriend );

	void mpSendAutoMatchRequest(char* gameParam, char* playerParam, short playerLevel, short levelDifference);

	void mpSendCancelAutoMatchRequest();

	void mpSendKickOut(char* name);

	void mpChangeToBeMaster()
	{
		m_master = true;
	}

	bool m_master;

	ConnectionLobby * wtcpLobby;

public:

	bool GetSecondConnecting()				{ return m_bSecondConnecting;   }
	void SetSecondConnecting( bool bValue ) { m_bSecondConnecting = bValue; }

	//for next connect and when connect failed in game, if game over then return to lobby menu.
	//0 for connecting success and when connect failed can send second connect
	//-1 for reset to init status, do not need send the second connect
	enum
	{
		RESET_STATUS_TO_INIT = -1,
		SET_STATUS_TO_CONNECT_SUCCESS = 0
	};
	void SetSecConStartTime( int iValue )			{ m_lSecConStartTime = iValue; }

	long GetLaunchGameTime() { return m_lLaunchGameTime; }

	bool GetSendRejoinAfterGameOver() { return m_bSendRejoinAfterGameOver; }
	void SetSendRejoinAfterGameOver( bool bValue ) { m_bSendRejoinAfterGameOver = bValue; }

	bool GetAutoMatchStatus() { return m_bAutoMatchStatus; }
	void SetAutoMatchStatus(bool bAutoMatchStatus) { m_bAutoMatchStatus = bAutoMatchStatus; }

	void SetMPConnectLevel(BYTE connectLevel) { m_MPConnectLevel = connectLevel; }

private:
	
	bool m_bSecondConnecting;
	long m_lSecConStartTime;

	long m_lLaunchGameTime;

	bool m_bSendRejoinAfterGameOver;

	bool m_bAutoMatchStatus;

	char* m_sLobbyServerAddress;
public:

#ifdef LOBBY_THREAD_TEST
	int m_iLobbyThreadTestNum;
#endif
	
	bool m_bCheckTimeOut;
};
#endif

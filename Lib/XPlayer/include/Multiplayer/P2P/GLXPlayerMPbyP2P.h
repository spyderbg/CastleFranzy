#ifndef __GL_XPLAYER_MP_BY_P2P_H__
#define __GL_XPLAYER_MP_BY_P2P_H__

#include "../../Porting.h"
#include "../Common/Connection.h"
#include "../Common/DataPacket.h"
#include "../Common/GLXPlayerMPBase.h"
#include "../Common/XIPAddress.h"
#include "OnlineGameRoomList.h"
#include "GLXPlayerUpnpObserver.h"

///////////////////////////////////////////////////////////////////////////////
//
// Multiple Player		
//
///////////////////////////////////////////////////////////////////////////////


class GLXPlayerMPbyP2P : public GLXPlayerMPBase, public GLXPlayerUpnpObserver
{
public: // Public functions]

	enum UpnpSetMyselfState
	{
		SET_UPNP_NONE=0,
		SET_UPNP_BEGIN,
		SET_UPNP_WAIT,
		SET_UPNP_END
	};
	UpnpSetMyselfState m_upnpSetState;
	/**
	 * Multiplayer Specific - Constructor and Destructor. 
	 */
	 
	GLXPlayerMPbyP2P(GLXPlayerMPBaseObserver * observer);
	virtual ~GLXPlayerMPbyP2P(void);
	
	/**
	 * Multiplayer Specific - Update function, call when handling network request.
	 * @see #Update()
	 */
	virtual void Update();
		
	/**
	 * Multiplayer Specific - Prepare a game data package, used for in game messages.
	 * 
	 */
	virtual DataPacket* mpPrepareGameData();
	
	/**
	 * Multiplayer Specific - Send a game data package, used for in game messages.
	 * 
	 */
	void mpSendGameData( DataPacket* packet, bool bReliable = false );

	/**
	 * Multiplayer Specific - Finishes the connection with the multiplayer server.
	 */
	virtual void mpDisconnect();
			
	/**
	 * Multiplayer Specific - Send a list available games request to the server.
	 * <p>
	 * This is how it works : a request is made to the server, containing the number of sessions requested and the index of
	 * the first session. The server replies with the list, and the data is stored in internal buffers and can be accessed
	 * using the corresponding functions.
	 * The sessions are retrieved by page, using the parameters.
	 * For each session are returned the index of the session, the name, one data byte with the number of players currently in
	 * session and some binary data containing game specific session data (map, track, difficulty, etc.).
	 * <p> After the request is completed, the session list can be accessed with <code>getNameList()</code>,<code>getDataList()</code>,
	 * <code>getByteDataList()</code>,<code>getNameList()</code>, <code>getNumberOfSessions()</code> and <code>getFirstSessionIndex()</code>.
	 * @param numberOfSessions The number of session requested.
	 * @param firstSessionIndex The index of the first session requested.
	 * 
	 */
	void mpSendListRoom( BYTE count, BYTE start, DataPacket * condition );

	void mpSendListRoomByName( BYTE count, BYTE start, char * name );

	void mpSendListRoomByData( BYTE count, BYTE start, DataPacket * condition );

	void mpSendListAllRoom( BYTE count, BYTE start );
	
	/**
	 * Multiplayer Specific - Create a new game on the multiplayer server.
	 * <p>
	 * The creator of the session is called master. He can kick out a player that joins the session or start the game after
	 * enough player have joined.
	 * The created game can be seen by all the players who perform a list session request.
	 * The joining of XP_API_NEW players is signaled by a push message from the server.
	 * While being is session, the players list is available.
	 * @param sessionname The name of the session. This is identical with the player nickname.
	 * @param sessiondata Specific game data for the game session. This can include map, track, difficulty, etc.
	 * 
	 */	
	void mpSendCreateRoom( char * name, DataPacket * room_data, DataPacket * player_data,
		int min_player_count = 2, int max_player_count = 4, char * password = NULL );

	/**
	 * Multiplayer Specific - Join a game session on the multiplayer server.
	 * <p>
	 * After the game is joined, the player will wait for the session master to start the game.
	 * The players list is available while being in session.
	 * @param sessionname The name of the session to be joined.
	 * @param mysessiondata The client info send to host when a client join
	 * 
	 */
	void mpSendJoinRoom( char * name, DataPacket * player_data, char * password = NULL );
	
	/**
	 * Multiplayer Specific - Leave a game session.
	 * <p>
	 * Every player can leave a session at any time. If the player is the master of the session, the session is terminated
	 * and all the other players will receive a kick out push message.
	 * 
	 */
	void mpSendLeaveRoom();

	/**
	 * Multiplayer Specific - Kick out a player.
	 * <p>
	 * The session master can kick out a player before the game is started. The kicked out player will be removed from
	 * the session an receive a push message informing him about it.
	 * If a player that is not master tries to kick out another player, this will result in an error.
	 * @param playerName The name of the player to be kicked out.
	 * 
	 */
	virtual void mpSendKickOutPlayer(char* playerName);

	virtual void mpSendKickOutPlayer(unsigned short* playerName);

	/**
	 * Multiplayer Specific - Leave the game.
	 */
	virtual void mpSendLeaveGame();

	/**
	 * Multiplayer Specific - Start the game.
	 * <p>
	 * The start signal can be sent only by the session master, after enough players have joined the session.
	 * If another player send the start signal or the master tries to start the game when there are not enough players yet,
	 * this will result in an error response.
	 * After the game is started, the players can send in game messages to each other in whatever format they choose.
	 * 
	 */
	virtual void mpSendStartGame();
	
	/**
	 * Multiplayer Specific - Finish the game.
	 * <p>
	 * This message is send by each player after he finished playing the multiplayer game.
	 * The player will be removed from the session. The difference between finish game and leave game is that
	 * the session is not terminated when the master finished the game, but waits for all the players to finish.
	 * 
	 */
	virtual void mpSendFinishGame();

	/**
	 * Multiplayer Specific - Confirm finish the game.
	*/
	void mpSendFinishGameConfirm(bool isInConfirmStatus);

	/**
	 * Multiplayer Specific - Test if the player is in a game session on the multiplayer server.
	 * <p>
	 * @return Returns true if in session, false otherwise.
	 */
	virtual bool mpIsInSession()
	{
		if ( NULL == m_curr_room )
		{
			return false;
		}

		return m_curr_room->IsInRoom();
	}

	virtual bool mpIsInRoom()
	{
		if ( NULL == m_curr_room )
		{
			return false;
		}

		return m_curr_room->IsInRoom();
	}

	/**
	 * Multiplayer Specific - Test if the player is in a playing a game on the multiplayer server.
	 * <p>
	 * @return Returns true if in game, false otherwise.
	 */
	virtual bool mpIsInGame()
	{
		if ( NULL == m_curr_room )
		{
			return false;
		}

		return m_curr_room->IsInGame();
	}

	virtual bool mpIsMaster()
	{
		if ( NULL == m_curr_room )
		{
			return false;
		}

		return m_curr_room->AmITheHost();
	}

	COnlineGameRoomList * mpGetGameRoomList()
	{
		return m_room_list;
	}

	COnlineGameRoom * mpGetCurrentGameRoom()
	{
		return m_curr_room;
	}

	char * GetLocalIP()
	{
		return m_ip_local->getIP();
	}

	bool mpIsGameFinished()
	{
		if ( NULL == m_curr_room )
		{
			return false;
		}

		return m_curr_room->IsGameFinished();
	}

	bool mpIsGameFinishConfirm()
	{
		if ( NULL == m_curr_room )
		{
			return false;
		}

		return m_curr_room->IsGameFinishConfirm();
	}

	bool mpKeepConnectionToGameServer();

	bool mpCanStartGame();

	void OnHostLost()
	{
		m_MPConnectLevel = CONNECT_LEVEL_IN_CONNECTED;
		m_MPGameLevel = CONNECT_LEVEL_NOT_CONNECTED;
	}

	/**
	 * Test whether UPnP is enabled or disabled.
	 *
	 */
	void mpSendUPnPPortMapping();

	/**
	 * Send data to opponent directly.
	 *
	 */
	void mpSendUpdateSession();
		
	/**
	 * Check whether there are some data received from other players.
	 * If there are more than one player had sent data to you,
	 * you have to invoke mpP2PCheckGameData() method several times to received them.
	 * <p>
	 * @param ( you should alloc memory and initialize the following parameters )
	 *	name:	the player name who sent the data
	 *	buffer:	the pointer to the data
	 *	size:	the size of the data
	 * @return
	 *	true:	some data received ( you should deal with them, such as show them on screen )
	 *	false:	no data received
	 *
	 */
	bool mpCheckGameData( char * &name, DataPacket * &packet);

	bool mpIsUPnPTestDone();

	bool mpIsHost();

	bool mpIsNeedToSendFinishToServer()
	{
		return m_send_finish_to_server;
	}

	//void setNationality( char * nationality );

	void mpClearRoomData();

	void mpHandlePing();

	void mpStartGame()
	{
		m_curr_room->StartGame();
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

	// process the push messages from the server
	virtual void mpProcessPushMessages();

	void mpUpdateHostClientInteraction();

	void mpUpdateServerInteraction();

	void mpUpdateRoom();

	/**
	 * Receive data. ( Client )
	 * Receive and transfer data. ( Host )
	 *
	 */
	void mpUpdateGame();

	void mpUpdateGameFinish();

	void mpHandleRoomData();

//////////////
	
	long mpRecvPing( char ** ip, int * port, char * data, int size );

	int mpBuildSessionData( char ** buffer, COnlinePlayer * player, char * nationality_data, 
		char * sessiondata = NULL, int sessiondatalen = 0, bool password_flag = false );

	bool mpParseSessionData( int index, char * inter_ip, char * exter_ip, 
		char * inter_ping, char * exter_ping, bool bUdp = false );

//=============upnp observer===============

	virtual void OnNetworkError();

	virtual void OnRequestTimeout();

	virtual void OnUpnpRequestFailed(char *errMsg, int errCode);

	virtual void OnUpnpInitSuccess(const char* externIp);

	virtual  void OnAddPortMappingSuccess(int type,int portMapped);

	virtual  void OnRemovePortMappingSuccess();

	virtual void OnRemoveAllPortMappingSuccess();

	//virtual void OnGetExternIPSuccess();

//=============upnp observer===============
	public:
    void CloseUpnpThread();
	void UpdateUpnpSet();
	
private: //Private variables

	enum MP_P2P_COMMAND
	{
		MP_P2P_CMD_CONNECT = 0,
		MP_P2P_CMD_LOGIN,
		MP_P2P_CMD_LIST_SESSION
	};

	/**
	 * Whether UPnP can be used or not.
	 *
	 */
	bool m_upnp_enabled;
	
	/**
	 * The ip for sending and receiving ping data.
	 *
	 */
	XIPAddress * m_ip_ping;

	GLXPlayerSocket * m_socket_ping;

	bool m_upnp_tested;

	bool m_host;

	bool m_send_finish_to_server;

	MP_P2P_COMMAND m_command;

	BYTE m_MPGameLevel;

	BYTE m_old_get_list_count;

	BYTE m_old_get_list_start;

	char * m_old_create_session_name;

	char * m_old_create_session_data;

	char**  m_lBinaryDataList_Head;

	COnlineGameRoomList * m_room_list;

	COnlineGameRoom * m_curr_room;

	// the specific game data of the session the player is currently in (if any)
	char* currentsessiondata;

	/**
	 * The ip address and port of the router. 
	 *
	 */
	XIPAddress * m_ip_router;

	/**
	 * The local ip address and port.
	 *
	 */
	XIPAddress * m_ip_local;

	int m_upnp_try_times;

	bool isStartJoin;
	
	//bool isCloseUpnpThread;

	//char upnp_temp_ip[128];
};
#endif

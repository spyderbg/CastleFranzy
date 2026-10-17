#ifndef __ONLINE_GAME_ROOM_H__
#define __ONLINE_GAME_ROOM_H__

#include "OnlinePlayerList.h"

enum OnineGameRoomStatus
{
	GAME_ROOM_STATUS_NONE = 0,
	GAME_ROOM_STATUS_FOUND = 1,
	GAME_ROOM_STATUS_ROOM = 2,
	GAME_ROOM_STATUS_GAME = 3,
	GAME_ROOM_STATUS_GAME_FINISH = 4
};

/**
* Represent the game room.
*
*/
class COnlineGameRoom
{

public:

	COnlineGameRoom(void);

	~COnlineGameRoom(void);
	enum SetMyselfState
	{
		SET_NONE=0,
		SET_WAIT,
		SET_END
	};
	/**
	* The entrance to receive and send data.
	* <p>
	* The method should be invoked every frame.
	*
	*/
	void UpdateData();

	int GetIndex()
	{
		return m_index;
	}

	void SetIndex( int index )
	{
		m_index = index;
	}

	char * GetName()
	{
		return m_room_name;
	}

	void SetName( char * name )
	{	
		XP_API_DEL( m_room_name );
		m_room_name = XP_API_STRNEW( name );
	}

	DataPacket * GetData()
	{
		return m_room_data;
	}

	void SetData( DataPacket * packet )
	{
		XP_API_DEL( m_room_data );
		m_room_data = XP_API_NEW DefaultDataPacket( packet );
	}

	int GetMaxPlayerCount()
	{
		return m_max_player_count;
	}

	void SetMaxPlayerCount( int count )
	{
		m_max_player_count = count;
	}

	int GetMinPlayerCount()
	{
		return m_min_player_count;
	}

	void SetMinPlayerCount( int count )
	{
		m_min_player_count = count;
	}	

	int GetPlayerCount();

	COnlinePlayer * GetPlayer( int index );

	void SetPlayerCount( int count );

	//COnlinePlayerList * GetPlayerList()
	//{
	//	return m_player_list;
	//}

	long GetPingStartTime()
	{
		return m_ping_starttime;
	}

	long SetPingEndTime( long time )
	{
		return m_ping_endtime = time;
	}

	long GetPingTime();

	COnlinePlayer * GetHost()
	{
		if(m_player_list == NULL)
			return NULL;

		return m_host;
	}

	bool SetHost( int index );

	COnlinePlayer * GetMyself()
	{
		if(m_player_list == NULL)
			return NULL;

		return m_self;
	}	

	bool SetMyself( int index, OnlinePlayerType type );

	void SetPassword( char * password );

	bool HasPasswordSet();

	void SetPasswordFlag( bool flag )
	{
		m_room_password_flag = flag;
	}

	void SetFoundTime( long time )
	{
		m_found_time = time;
	}

	long GetFoundTime()
	{
		return m_found_time;
	}

	bool HasPassword()
	{
		return m_room_password_flag;
	}

public:

	/**
	* The game can start or not.
	*
	*/
	bool CanStart();

	bool AddPlayer( char * name, char * country, DataPacket * data, 
		char * exter_ip, char * inter_ip, int exter_tcp_port, int exter_udp_port,
		int inter_tcp_port, int inter_udp_port );

	bool AmITheHost();

	bool SendToHost( DataPacket * data );

	void HostLeave();

	//void HandlePing( GLXPlayerSocket * socket );
	long SendPing( GLXPlayerSocket * socket, XIPAddress * ip_router );

	//-----------------change the connect function to nonblock, add the connect state //by han yuan 2009.3.31
private:
	int m_joinRoomState;

public:
	enum JoinRoomState
	{
		JOINSTATE_START,
		JOINSTATE_PERPARE,
		JOINSTATE_WAITING,
		JOINSTATE_SUCCESS,
		JOINSTATE_FAILED
	};

	int GetJoinRoomState()
	{ return m_joinRoomState;}

	void SetJoinRoomState(int state)
	{ m_joinRoomState = state;}
	//------------------------------------------

	bool JoinRoom( char * name, char * country, DataPacket * data, 
		char * exter_ip, char * inter_ip, int exter_tcp_port, int exter_udp_port,
		int inter_tcp_port, int inter_udp_port, OnlinePlayerType type );

	bool IsJobDone()
	{
		return ( m_error != ERROR_PENDING );
	}

	bool IsJobSuccessed()
	{
		return ( m_error == ERROR_NONE );
	}

	int GetLastError()
	{
		return m_error;
	}

	bool IsInRoom()
	{
		return ( m_status == GAME_ROOM_STATUS_ROOM );
	}

	bool IsInGame()
	{
		return ( m_status == GAME_ROOM_STATUS_GAME );
	}

	bool IsGameFinished()
	{
		return ( m_status == GAME_ROOM_STATUS_GAME_FINISH );
	}

	bool IsGameFinishConfirm()
	{ return m_isFinishConfirm;}
	bool StartRoom();

	bool LeaveRoom();

	bool StartGame();

	bool FinishGame();
	bool FinishGameConfirm(bool isInConfirmStatus);

	bool KickoutPlayer( char * name );

	void KeepAlive();	

	bool CheckGameData( char * &name, DataPacket * &packet );

	bool SendGameData( DataPacket * game_data, bool bReliable );

	bool StartLocalRoom();
	void SetGameFinishConfirmStatus(bool flag);
	//-----------------------------------------------------------------------//
	//---------------------------------------------
	void InitSetMyself()
	{
		m_setMyselfState=SET_NONE;
	}

private:

	/**
	* Accept new connection request.
	*
	*/
	void AcceptNewConnection();

	/**
	* Receive data from other players.
	*
	*/
	void ReceiveData();

	/**
	* Receive data to other players.
	*
	*/
	void SendData();

	/**
	* Handle received data.
	*
	*/
	void HandleData();

	/**
	* Handle received data.
	*
	*/
	DataPacket * HandleAllTcpData( int index, DataPacket * currentPacket );
	DataPacket * HandleAllUdpData( DataPacket * currentPacket );
	bool HandleUdpData( DataPacket * currentPacket );
	bool HandleData( int index, DataPacket * currentPacket );
	bool HandleRequestData( int index, DataPacket * currentPacket );
	bool HandleRequestJoinRoom( int index, DataPacket * currentPacket );
	bool HandleRequestLeaveRoom( int index, DataPacket * currentPacket );
	bool HandleRequestFinishGame( int index, DataPacket * currentPacket );
	bool HandleResponseData( int index, DataPacket * currentPacket );
	bool HandleResponseJoinRoom( int index, DataPacket * currentPacket );
	bool HandleResponseLeaveRoom( int index, DataPacket * currentPacket );
	bool HandleResponseFinishGame( int index, DataPacket * currentPacket );
	bool HandlePushData( int index, DataPacket* currentPacket );
	bool HandlePushJoinRoom( int index, DataPacket* currentPacket );
	bool HandlePushLeaveRoom( int index, DataPacket* currentPacket );
	bool HandlePushKickoutRoom( int index, DataPacket* currentPacket );
	bool HandlePushStartGame( int index, DataPacket* currentPacket );
	bool HandlePushFinishGame( int index, DataPacket* currentPacket );
	bool HandlePushFinishGameConfirm( int index, DataPacket* currentPacket );
	bool HandlePushFinishGameRestart(int index, DataPacket* currentPacket);
	bool HandleGameData( int index, DataPacket* packet );
	bool HandleKeepAliveData( int index, DataPacket* packet );	

	/**
	* Keep connection and remove dead connection.
	*
	*/
	void HandleConnection();

	/**
	* New player could join the game room or not.
	*
	*/
	bool CanJoin();	

	int IsPlayerExisted( char * name );	

private:

	int m_index;

	/**
	* All the players in the game room.
	*
	*/
	COnlinePlayerList * m_player_list;

	/**
	* Myself.
	* <p>
	* The pointer has existed in player list, do not delete it.
	*
	*/
	COnlinePlayer * m_self;

	/**
	* The Host.
	* <p>
	* The pointer has existed in player list, do not delete it.
	* <p>
	* Maybe I'm the Host.
	*
	*/
	COnlinePlayer * m_host;

	/**
	* The max player amount in this room.
	*
	*/
	int m_max_player_count;

	/**
	* The min player amount in this room.
	*
	*/
	int m_min_player_count;

	/**
	* Game data network protocol between players.
	*
	*/
	OnlineProtocol m_protocol;

	/**
	* Room information.
	*
	*/
	char * m_room_name;

	/**
	* Room information.
	*
	*/
	DataPacket * m_room_data;

	int m_player_count;

	long m_ping_starttime;

	long m_ping_endtime;

	OnineGameRoomStatus m_status;

	bool m_isFinishConfirm;
	int m_error;

	char * m_room_password;

	bool m_room_password_flag;
	long m_found_time;
	///////////////////////

public :
	void OnAddPortMappingSuccess(int type,int portMapped);

private:

	SetMyselfState m_setMyselfState; 

};

#endif
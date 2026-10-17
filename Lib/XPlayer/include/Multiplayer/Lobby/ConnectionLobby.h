#ifndef __CONNECTION_LOBBY_H__
#define __CONNECTION_LOBBY_H__

#include "../../Porting.h"
#include "Memory.h"
#include "DataPacketLobby.h"
#include "../../Common/GLXPlayerSocketFactory.h"
#include "../Common/Connection.h"
#include "LobbyParameterAndQuery.h"

/**
 * The TCP connection class.
 * <p>
 * Handles all the physical communication with the server.
 * Opens, closes the sockets, sends and receives the data ...
 * <p>
 * To establish connection :
 * - call connect() to start process
 * - wait for a connection success message, then call set_connected ()
 * <p>
 * To terminate connection :
 * - call disconnect
 * <p>
 * To send requests to the server :
 * - either construct data packets and call addOutgoingPacket
 * - or use predefined primitives for standard messages ...
 * <p>
 * To get the packages received from the server :
 * - call getNextIncomingPacket
 *
 * @author World Online Team <<online-adm@gameloft.com>>
 * @author  Lu Tie Hua <tiehua.lu@gameloft.com> 
 * @version 0.0.1
 *
 */
class ConnectionLobby : public Connection
{
public:

	/**
	* Connection constructor.
	*/
	ConnectionLobby(char* address,int server_port);

	/**
	* Connection destructor.
	*/
	virtual ~ConnectionLobby();

	void dnsResolved();
	void sendData();
	void receiveData(int expected);
	void receiveDataLen();

	// utilities for sending different kind of packets according to the protocol ... 
	void sendEstablishConnectionPackage();
	void sendFinishConnectionPackage();
	void sendLoginPackage(char* nickname);
	void sendCreateSessionPackage(char* session_name,char* game_data,int length);
	void sendQuickGamePackage();


	void sendJoinSessionPackage(char* session_name);
	void sendLeaveSessionPackage();

	void sendListSessionsPackage(BYTE number_of_sessions,int session_index);

	void sendListSessionsByNamePackage(BYTE number_of_sessions,int session_index, char * name);
	void sendListSessionsByDataPackage(BYTE number_of_sessions,int session_index, char * data, int length);
	void sendListSessionsAllPackage(BYTE number_of_sessions,int session_index);

	void sendKickOutPlayerPackage(char* player_name);

	/**
	* Send the package to start game.
	*/
	void sendStartGamePackage();

	void sendFinishGamePackage();

	void sendUpdateSessionPackage(int player_number);

	void setGgi( long ggi )
	{
		m_ggi = ggi;
	}

#ifdef MP_ENABLE_TIMEOUT 
	  // the time the request is started. Used to check
	  // for timeout. Only if MP_ENABLE_TIMEOUT is defined.
	  unsigned long last_activity_time;
#endif

	// call this from time to time to avoid closing connection by the server ... 
	bool keepConnectionAlive();

	// only wait for a request response a limited time ; 
	long getTimeOfLastReqest();

	// used for the retry operation in case of network failure ... 

	// call this when sending a packet that you want retry to work on ... 
	void saveRetryData(DataPacket* packet);
	// call this to send the data you saved 
	void sendRetryData();
   
private:
	long m_ggi;

//lobby client
public:
	void sendLobbyLoginPackage(long ggi, char* uid, short len, BYTE bKeepLobbyStatus);
	void sendCreateLobbyPackage(int ggi, char* name, BYTE bVisibility, BYTE bLock, 
								int maxCount, char*  gameParam, char* playerParam, CLobbyParameterAndQuery* clobbyParamAndQuery );
	void sendJoinLobbyPackage(int lobby_id, BYTE user_status, char* playerParameter);
	void sendGetLobbyListPackage(int ggi, int userNumber, int start_num, BYTE count, 
								CLobbyParameterAndQuery* clobbyParamAndQuery, BYTE bCheckJoinable);
	void sendLeaveLobbyPackage(/*int lobby_id*/);
	void sendLunchLobbyGamePackage();
	void sendSetPlayerStatusPackage(BYTE status);
	void sendRejoinLobbyPackage( int iNormal );

	void sendSetGameParameter( char* gameParameter, short lenParam, CLobbyParameterAndQuery* clobbyParamAndQuery );
	void sendSetUserParameter( char* userParameter, short lenParam );
	void sendGetLobbyInfo( int lobbyID);
	void sendGetLobbyForName( char* nameQuery );
	void sendGetLobbyForFriend( int userNumber, BYTE bVisibility, BYTE count, char* nameArray, int len_nameArray );

	void sendAutoMatchPackage(char* gameParam, char* playerParam, short playerLevel, short levelDifference);
	void sendCancelAutoMatchPackage();

	void sendGetLobbyListPackageWithGameParam(int ggi, int userNumber, int start_num, BYTE count, 
								CLobbyParameterAndQuery* clobbyParamAndQuery, BYTE bCheckJoinable);
	void sendGetLobbyForNameWithGameParam( char* nameQuery );
	void sendGetLobbyForFriendWithGameParam( int userNumber, BYTE bVisibility, BYTE count, char* nameArray, int len_nameArray );

	void sendKickOutPackage(char* name);
};

#endif //__CONNECTION_H__

#include "../../../../include/Game_defines.h"

#ifdef ENABLE_MULTIPLAYER

#include "../../../../include/Multiplayer/Lobby/ConnectionLobby.h"

/** CONNECTION CLASS
 * This DOES NOT automatically setup the connection to the server.
 * connection.connect() MUST be called to setup the connection,
 */
ConnectionLobby::ConnectionLobby(char* address, int server_port):
Connection(address, server_port)
{
}

ConnectionLobby::~ConnectionLobby(void)
{
}

void ConnectionLobby::sendData()
{
	if (currentOutgoingPacket == NULL)	// nothing to send !!
	{
		return;
	}

	// FIX FROM LUMINES:
	// Check to see if the PPP connection has been put to sleep or closed by the
	// firmware. If it has then the nailed up TCP/IP connection has been lost
	// so disconnect. This MUST be done before sending a message since the PPP
	// connection will wake up and re-establish TCP/IP on a different IP and port
	// without notifying us.
	if (!isConnected())
	{
		// If the PPP connection is not open so trigger a disconnect.
		XP_DEBUG_OUT( "sendData: PPP not open, disconnect." );
		disconnect();
		return;
	}

	// send two sync packets ..
	//sbuf[0] = (char) currentOutgoingPacket->getMessageLen();
	//XP_API_MEMCPY( sbuf+1, currentOutgoingPacket->getMessageBody(), currentOutgoingPacket->getMessageLen() );

	int iResult = m_socket->Select( GLXPlayerSocket::XSOCKET_SELECT_WRITE );

	if ( iResult < 0 )
	{
		XP_DEBUG_OUT( "sendData: select error." );
		return;
	}

	iResult = m_socket->Send( (char *)currentOutgoingPacket->getMessageBody(), 
		currentOutgoingPacket->getMessageLen() );

	if ( iResult < 0 )
	{
		disconnect();
		return;
	}


	time_of_last_sending = XP_API_GET_TIME();

	// go to next packet ... 
	DataPacket* packet = currentOutgoingPacket;

	// move to next 
	currentOutgoingPacket = currentOutgoingPacket->getNextPacket();

	// delete old, NULL check is done at beginning of function.
	delete packet;


	if (currentOutgoingPacket != NULL)	// still have packets .. 
	{
		sendData();
	}
	else
	{
		// no more things in the list .. we can leave now ..
		lastOutgoingPacket = NULL;
	}
}

void ConnectionLobby::receiveDataLen()
{
	//don't think you have a stack that big on an average phone
	//byte buffer[MAX_MESSAGE_SIZE];
	char *buffer = rbuf;

	XP_API_MEMSET(buffer, 0, 128);

	int iResult = m_socket->Select( GLXPlayerSocket::XSOCKET_SELECT_READ );

	if ( iResult < 0 )
	{
		return;
	}

	if ( m_socket->IsReadyForRW() )
	{
		iResult = m_socket->Recv( (char *)buffer, 2 );

		if (iResult == -1)
		{
			return;
		}
		else if(iResult != 2)
		{
			disconnect();
		}
		else
		{
			//int rExpected = (BYTE) buffer[0];
			buffer[2] = '\0';
			int rExpected = 0;
			rExpected |= (buffer[0] & 0xFF) << 8;
			rExpected |= (buffer[1] & 0xFF);
			receiveData( rExpected );
		}
	}
}

void ConnectionLobby::receiveData(int expectedLen)
{
	if(expectedLen > 128)
	{
		int ttt = 0;
	}
	//int nRead;

	//don't think you have a stack that big on an average phone
	//byte buffer[MAX_MESSAGE_SIZE];
	char *buffer = rbuf;

	int iResult = m_socket->Select( GLXPlayerSocket::XSOCKET_SELECT_READ );

	if ( iResult < 0 )
	{
		return;
	}

	if ( m_socket->IsReadyForRW() )
	{
		iResult = m_socket->Recv( (char *)buffer, expectedLen );

		if (iResult == -1)
		{
			return;
		}
		else if(iResult == 1)
		{
			disconnect();
		}
		else
		{
			if( iResult != expectedLen )
			{
				int ttt = 0;
			}

			// IMPORTANT:
			// The game code MUST free this when it processes the message.
			// Otherwise a memory leak will occur.
			DataPacket* packet = XP_API_NEW DataPacketLobby();
			packet->setMessageBody( (BYTE*)buffer ,expectedLen );
			addIncomingPacket(packet);

			// reload .. 
			XP_API_MEMSET( buffer, 0, 128 );
			//receiveDataLen();//??
			///continue;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// utilities for sending different kind of packets ... 

// in order to establish an udp connection with the server , we need to send 
// two packets containing a connection identifier ; if the server 
// matches the packages , the connection is established 
void ConnectionLobby:: sendEstablishConnectionPackage()
{
	int connection_id = XP_API_GET_RANDOM( NULL );

	DataPacket* packet = XP_API_NEW DataPacketLobby();
	packet->InitMessage( CMSG_CONNECT );
	packet->addInt( connection_id );
	packet->packMessage();

   addOutgoingPacket(packet);
   time_of_last_request = XP_API_GET_TIME();

   // Reset the last activity time so that we don't include the time to open
   // the PPP connection and socket in the timeout.
#ifdef MP_ENABLE_TIMEOUT 
	last_activity_time = XP_API_GET_TIME();
#endif //MP_ENABLE_TIMEOUT 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConnectionLobby:: sendFinishConnectionPackage()
{
	int connection_id = XP_API_GET_RANDOM( NULL );
	
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_DISCONNECT );
	packet->addInt( connection_id );
	packet->packMessage();

	addOutgoingPacket(packet);
	time_of_last_request = XP_API_GET_TIME();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConnectionLobby::sendLoginPackage(char* nickname)
{
   DataPacket* login_packet = XP_API_NEW DataPacketLobby();

   login_packet->addByte(BASIC_MESSAGE_TYPE_GAME);
   login_packet->addByte(GAME_MESSAGE_REQUEST);
   login_packet->addByte(REQUEST_MESSAGE_LOGIN);
   XP_DEBUG_OUT("ConnectionLobby::sendLoginPackage()\nuser=[%s]\n", nickname);

   login_packet->addShortLenString(nickname,XP_API_STRLEN(nickname));

   login_packet->packMessage();

   saveRetryData(login_packet);
   addOutgoingPacket(login_packet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// the game data is optional .. if none , send NULL 
void ConnectionLobby:: sendCreateSessionPackage(char* session_name,char* game_data, int length)
{
   DataPacket* packet = XP_API_NEW DataPacketLobby();

   packet->addByte(BASIC_MESSAGE_TYPE_GAME);
   packet->addByte(GAME_MESSAGE_REQUEST);
   packet->addByte(REQUEST_MESSAGE_CREATE_SESSION);
   packet->addShortLenString(session_name, XP_API_STRLEN(session_name));

   if (game_data == NULL)
   {
      game_data = " ";
   }
   packet->addShortLenString(game_data, length);

   packet->packMessage();

   saveRetryData(packet);
   addOutgoingPacket(packet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConnectionLobby:: sendQuickGamePackage()
{
   DataPacket* packet = XP_API_NEW DataPacketLobby();

   packet->addByte(BASIC_MESSAGE_TYPE_GAME);
   packet->addByte(GAME_MESSAGE_REQUEST);
   packet->addByte(REQUEST_MESSAGE_GET_QUICK_SESSION);

   packet->packMessage();

   saveRetryData(packet);
   addOutgoingPacket(packet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConnectionLobby:: sendJoinSessionPackage(char* session_name)
{
   DataPacket* packet = XP_API_NEW DataPacketLobby();

   packet->addByte(BASIC_MESSAGE_TYPE_GAME);
   packet->addByte(GAME_MESSAGE_REQUEST);
   packet->addByte(REQUEST_MESSAGE_JOIN_SESSION);
   packet->addShortLenString(session_name,XP_API_STRLEN(session_name));

   packet->packMessage();

   saveRetryData(packet);
   addOutgoingPacket(packet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConnectionLobby:: sendLeaveSessionPackage()
{
   DataPacket* packet = XP_API_NEW DataPacketLobby();

   packet->addByte(BASIC_MESSAGE_TYPE_GAME);
   packet->addByte(GAME_MESSAGE_REQUEST);
   packet->addByte(REQUEST_MESSAGE_LEAVE_SESSION);

   packet->packMessage();

   saveRetryData(packet);
   addOutgoingPacket(packet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConnectionLobby:: sendListSessionsPackage(BYTE number_of_sessions,int session_index)
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->addByte(BASIC_MESSAGE_TYPE_GAME);
	packet->addByte(GAME_MESSAGE_REQUEST);
	packet->addByte(REQUEST_MESSAGE_LIST_SESSIONS);
	packet->addByte(number_of_sessions);
	packet->addInt(session_index);

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

void ConnectionLobby::sendListSessionsAllPackage(BYTE number_of_sessions,int session_index)
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->addByte(BASIC_MESSAGE_TYPE_GAME);
	packet->addByte(GAME_MESSAGE_REQUEST);
	packet->addByte(REQUEST_MESSAGE_LIST_SESSIONS);
	packet->addByte(number_of_sessions);
	packet->addInt(session_index);
	packet->addByte(REQUEST_QUERY_ALL);

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

void ConnectionLobby::sendListSessionsByNamePackage(BYTE number_of_sessions,int session_index, char * name)
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->addByte(BASIC_MESSAGE_TYPE_GAME);
	packet->addByte(GAME_MESSAGE_REQUEST);
	packet->addByte(REQUEST_MESSAGE_LIST_SESSIONS);
	packet->addByte(number_of_sessions);
	packet->addInt(session_index);
	packet->addByte(REQUEST_QUERY_BY_NAME);
	packet->addShortLenString(name,XP_API_STRLEN(name));

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

void ConnectionLobby::sendListSessionsByDataPackage(BYTE number_of_sessions,int session_index, char * data, int length)
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->addByte(BASIC_MESSAGE_TYPE_GAME);
	packet->addByte(GAME_MESSAGE_REQUEST);
	packet->addByte(REQUEST_MESSAGE_LIST_SESSIONS);
	packet->addByte(number_of_sessions);
	packet->addInt(session_index);
	packet->addByte(REQUEST_QUERY_BY_DATA);
	packet->addShortLenString(data,length);

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

/**
 * Send the package to start game.
 * Data Format: *grs
 */
void ConnectionLobby::sendStartGamePackage()
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->addByte( BASIC_MESSAGE_TYPE_GAME );
	packet->addByte( GAME_MESSAGE_REQUEST );
	packet->addByte( REQUEST_MESSAGE_START_GAME );
	packet->packMessage();
	saveRetryData( packet );
	addOutgoingPacket( packet );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConnectionLobby:: sendFinishGamePackage()
{
   DataPacket* packet = XP_API_NEW DataPacketLobby();

   packet->addByte(BASIC_MESSAGE_TYPE_GAME);
   packet->addByte(GAME_MESSAGE_REQUEST);
   packet->addByte(REQUEST_MESSAGE_FINISH_GAME);

   packet->packMessage();

   saveRetryData(packet);
   addOutgoingPacket(packet);
}

void ConnectionLobby::sendUpdateSessionPackage( int player_number )
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->addByte(BASIC_MESSAGE_TYPE_GAME);
	packet->addByte(GAME_MESSAGE_REQUEST);
	packet->addByte(REQUEST_MESSAGE_UPDATE_SESSION);
	packet->addByte(player_number);

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConnectionLobby:: sendKickOutPlayerPackage(char * player_name)
{
   if (player_name == NULL)
   {
      return;
   }

   DataPacket* packet = XP_API_NEW DataPacketLobby();

   packet->addByte(BASIC_MESSAGE_TYPE_GAME);
   packet->addByte(GAME_MESSAGE_REQUEST);
   packet->addByte(REQUEST_MESSAGE_KICK_OUT_PLAYER);
   packet->addShortLenString(player_name,XP_API_STRLEN(player_name));

   packet->packMessage();

   saveRetryData(packet);
   addOutgoingPacket(packet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ConnectionLobby::keepConnectionAlive()
{
   // check the time of last sending ...
	long current_time = XP_API_GET_TIME();

	if (( current_time - time_of_last_sending) > KEEP_ALIVE_TIMEOUT )
	{
      // time_of_last_sending is set in sendData(), connectCallbk() and
      // sendEstablishConnectionPackageOnReceive(). This ensures that the keep
      // alive is only sent when necessary.
//      XP_DEBUG_OUT("TX keep alive msg\n");

		DataPacket* packet = XP_API_NEW DataPacketLobby();

//		packet->addByte(BASIC_MESSAGE_TYPE_GAME);
//		packet->addByte(GAME_MESSAGE_KEEP_ALIVE);
//		packet->addInt( CMSG_KEEP_ALIVE );
		packet->InitMessage( CMSG_KEEP_ALIVE );

		packet->packMessage();

		addOutgoingPacket(packet);

		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConnectionLobby::saveRetryData(DataPacket* packet)
{
   if (packet == NULL)
   {
      return;
   }

   cleanRetryData();

   retryPacket = XP_API_NEW DataPacketLobby();

   retryPacket->setMessageBody(packet->getMessageBody(),packet->getMessageLen());

   time_of_last_request = XP_API_GET_TIME();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConnectionLobby::sendRetryData()
{
   if (retryPacket == NULL)
   {
      return;
   }

   // Create a XP_API_NEW packet. The current one has to be saved for further retries.
   // The only way to delete the retry packet is to use cleanRetryData().
   DataPacket* packet = XP_API_NEW DataPacketLobby();

   packet->setMessageBody(retryPacket->getMessageBody(),retryPacket->getMessageLen());

   addOutgoingPacket(packet);

   time_of_last_request = XP_API_GET_TIME();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//lobby client
void ConnectionLobby:: sendLobbyLoginPackage(long ggi, char* uid, short len, BYTE bKeepLobbyStatus)
{
   DataPacket* packet = XP_API_NEW DataPacketLobby();

   packet->InitMessage( CMSG_LOGIN );
   packet->addInt( ggi );
   packet->addShortLenString(uid, len);
   packet->addByte(bKeepLobbyStatus);
   packet->packMessage();

   saveRetryData(packet);
   addOutgoingPacket(packet);
}

void ConnectionLobby:: sendCreateLobbyPackage(int ggi, char* name, BYTE bVisibility, BYTE bLock, 
											  int maxCount, char*  gameParam, char* playerParam, CLobbyParameterAndQuery* clobbyParamAndQuery)
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_CREATE_LOBBY );
//	packet->addInt(ggi);
	packet->addShortLenString(name, XP_API_STRLEN(name));
	packet->addByte(bVisibility);
	packet->addByte(bLock);
	packet->addShort(maxCount);
	packet->addBlob(gameParam, XP_API_STRLEN(gameParam));
	packet->addBlob(playerParam, XP_API_STRLEN(playerParam));

	if( clobbyParamAndQuery != NULL )
	{
		char* lobbyParamAndQuery = NULL;
		int length = 0;
		lobbyParamAndQuery = clobbyParamAndQuery->PackParameter( length );

		if( lobbyParamAndQuery != NULL )
		{
			//short len =  XP_API_STRLEN( lobbyParamAndQuery );
			packet->addQueryString( lobbyParamAndQuery, length );
		}

		XP_API_DEL( lobbyParamAndQuery );
	}

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

void ConnectionLobby:: sendJoinLobbyPackage(int lobby_id, BYTE user_status, char* playerParameter)
{
   DataPacket* packet = XP_API_NEW DataPacketLobby();

   packet->InitMessage(CMSG_JOIN_LOBBY);
   packet->addInt(lobby_id);
   packet->addByte(user_status);
   packet->addBlob(playerParameter, XP_API_STRLEN( playerParameter ));
   packet->packMessage();

   saveRetryData(packet);
   addOutgoingPacket(packet);
}

void ConnectionLobby:: sendGetLobbyListPackage(int ggi, int userNumber, int start_num, BYTE count,
											   CLobbyParameterAndQuery* clobbyParamAndQuery, BYTE bCheckJoinable)
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_QUERY_LOBBY );
//	packet->addInt(ggi);
	packet->addInt(userNumber);
	packet->addByte(count);
	packet->addInt(start_num);

	if( clobbyParamAndQuery != NULL )
	{
		char* lobbyParamAndQuery = NULL;
		int length = 0;
		lobbyParamAndQuery = clobbyParamAndQuery->PackParameter( length );

		if( lobbyParamAndQuery != NULL )
		{
			//short len =  XP_API_STRLEN( lobbyParamAndQuery );
			packet->addQueryString( lobbyParamAndQuery, length );
		}

		XP_API_DEL( lobbyParamAndQuery );
	}
	else
	{
		packet->addByte(0);
	}

	packet->addByte(bCheckJoinable);

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

void ConnectionLobby:: sendLeaveLobbyPackage(/*int lobby_id*/)
{
   DataPacket* packet = XP_API_NEW DataPacketLobby();

   packet->InitMessage( CMSG_LEAVE_LOBBY );
   //packet->addInt(lobby_id);
   packet->packMessage();

   saveRetryData(packet);
   addOutgoingPacket(packet);
}

void ConnectionLobby::sendLunchLobbyGamePackage()
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_START_GAME );
	packet->packMessage();

	saveRetryData( packet );
	addOutgoingPacket( packet );
}

void ConnectionLobby::sendSetPlayerStatusPackage(BYTE status)
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_SET_USER_STATUS );
	packet->addByte(status);
	packet->packMessage();

	saveRetryData( packet );
	addOutgoingPacket( packet );
}

void ConnectionLobby::sendRejoinLobbyPackage( int iNormal )
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_REJOIN_GAME );
	packet->addInt( iNormal );
	packet->packMessage();

	saveRetryData( packet );
	addOutgoingPacket( packet );
}

void ConnectionLobby::sendSetGameParameter( char* gameParameter, short lenParam, CLobbyParameterAndQuery* clobbyParamAndQuery )
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_SET_GAME_PARAMETER );
	packet->addBlob( gameParameter, lenParam );

	if( clobbyParamAndQuery != NULL )
	{
		char* lobbyParamAndQuery = NULL;
		int length = 0;
		lobbyParamAndQuery = clobbyParamAndQuery->PackParameter( length );

		if( lobbyParamAndQuery != NULL )
		{
			//short len =  XP_API_STRLEN( lobbyParamAndQuery );
			packet->addQueryString( lobbyParamAndQuery, length );
		}

		XP_API_DEL( lobbyParamAndQuery );
	}

	packet->packMessage();

	saveRetryData( packet );
	addOutgoingPacket( packet );
}

void ConnectionLobby::sendSetUserParameter( char* userParameter, short lenParam )
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_SET_USER_PARAMETER );
	packet->addBlob( userParameter, lenParam );
	packet->packMessage();

	saveRetryData( packet );
	addOutgoingPacket( packet );
}

void ConnectionLobby::sendGetLobbyInfo( int lobbyID)
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_GET_LOBBY_INFOR );
	packet->addInt(lobbyID);
	packet->packMessage();

	saveRetryData( packet );
	addOutgoingPacket( packet );
}

void ConnectionLobby::sendGetLobbyForName( char* nameQuery )
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_QUERY_LOBBY_BY_FRIENDID );
	packet->addInt(0);
	packet->addByte(0);
	packet->addInt(0);
	packet->addBlob( nameQuery, XP_API_STRLEN(nameQuery) );

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

void ConnectionLobby::sendGetLobbyForFriend( int userNumber, BYTE bVisibility, BYTE count, char* nameArray, int len_nameArray )
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_QUERY_LOBBY_OF_FRIENDS );
	packet->addInt(userNumber);
	packet->addByte(count);
	packet->addInt(0);
	packet->addByte(bVisibility);
	packet->addByte(count);
	packet->addQueryString( nameArray, len_nameArray );

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);

}

void ConnectionLobby::sendAutoMatchPackage(char* gameParam, char* playerParam, short playerLevel, short levelDifference)
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_AUTOMATCH_USER );
	packet->addBlob(gameParam, XP_API_STRLEN(gameParam));
	packet->addShort(playerLevel);
	packet->addShort(levelDifference);	
	packet->addBlob(playerParam, XP_API_STRLEN(playerParam));

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

void ConnectionLobby::sendCancelAutoMatchPackage()
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_AUTOMATCH_USER_CANCLE );

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}


void ConnectionLobby::sendKickOutPackage(char* name)
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_KICK_OUT_PLAYER );
	packet->addShortLenString(name, XP_API_STRLEN(name));

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

void ConnectionLobby::sendGetLobbyListPackageWithGameParam(int ggi, int userNumber, int start_num, BYTE count, 
										  CLobbyParameterAndQuery* clobbyParamAndQuery, BYTE bCheckJoinable)
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_QUERY_LOBBY_GAMEPAMAM );
//	packet->addInt(ggi);
	packet->addInt(userNumber);
	packet->addByte(count);
	packet->addInt(start_num);

	if( clobbyParamAndQuery != NULL )
	{
		char* lobbyParamAndQuery = NULL;
		int length = 0;
		lobbyParamAndQuery = clobbyParamAndQuery->PackParameter( length );

		if( lobbyParamAndQuery != NULL )
		{
			//short len =  XP_API_STRLEN( lobbyParamAndQuery );
			packet->addQueryString( lobbyParamAndQuery, length );
		}

		XP_API_DEL( lobbyParamAndQuery );
	}
	else
	{
		packet->addByte(0);
	}

	packet->addByte(bCheckJoinable);

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

void ConnectionLobby::sendGetLobbyForNameWithGameParam( char* nameQuery )
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_QUERY_LOBBY_BY_FRIENDID_GAMEPARAM );
	packet->addInt(0);
	packet->addByte(0);
	packet->addInt(0);
	packet->addBlob( nameQuery, XP_API_STRLEN(nameQuery) );

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

void ConnectionLobby::sendGetLobbyForFriendWithGameParam( int userNumber, BYTE bVisibility, BYTE count, char* nameArray, int len_nameArray )
{
	DataPacket* packet = XP_API_NEW DataPacketLobby();

	packet->InitMessage( CMSG_QUERY_LOBBY_OF_FRIENDS_GAMEPARAM );
	packet->addInt(userNumber);
	packet->addByte(count);
	packet->addInt(0);
	packet->addByte(bVisibility);
	packet->addByte(count);
	packet->addQueryString( nameArray, len_nameArray );

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

#endif //ENABLE_MULTIPLAYER

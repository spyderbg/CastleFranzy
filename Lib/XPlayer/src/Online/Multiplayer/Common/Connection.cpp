#include "../../../../include/Game_defines.h"

#ifdef ENABLE_MULTIPLAYER

#include "../../../../include/Multiplayer/Common/Connection.h"

/** CONNECTION CLASS
 * This DOES NOT automatically setup the connection to the server.
 * connection.connect() MUST be called to setup the connection,
 */
Connection::Connection(char* address, int server_port)
{
	// store the server connection info .. 
	if (address != NULL)
	{
		the_server_address = XP_API_STRNEW( address );
	}
	else
	{
		the_server_address = NULL;
	}

	the_server_port = server_port;

	// Not connected.
	connected = false;

	currentOutgoingPacket	=	NULL;
	lastOutgoingPacket		=	NULL;

	currentIncomingPacket	=	NULL;
	lastIncomingPacket		=	NULL;

	retryPacket				=	NULL;

	XP_API_MEMSET(rbuf, 0, sizeof(rbuf));
	XP_API_MEMSET(sbuf, 0, sizeof(rbuf));

	time_of_last_sending	=	0;
	time_of_last_request	=	0;

#ifdef MP_ENABLE_TIMEOUT 
	last_activity_time   = 0;
#endif //MP_ENABLE_TIMEOUT 

	m_socket = GLXPlayerSocketFactory::GetSocket( the_server_address, the_server_port, NULL );

#ifdef MP_ENABLE_P2P

	m_soc_udp = NULL;

#endif // MP_ENABLE_P2P

	m_name = NULL;

	m_connectState = CONNECT_STATE_START;
}

Connection::~Connection(void)
{
	disconnect();

	delete( the_server_address );
	delete( m_socket );

#ifdef MP_ENABLE_P2P

	XP_API_DEL( m_soc_udp );

#endif // MP_ENABLE_P2P

	delete( m_name );
	m_name = NULL;

}

void Connection::setName( char * name )
{
	XP_API_DEL( m_name );
	m_name = XP_API_STRNEW( name );
}

/**
 * In order to establish an udp connection with the server,
 * we need to send two packets containing a connection identifier.
 * If the server matches the packages,the connection is established.
 *
 */
bool Connection::connect()
{
	if(m_connectState == CONNECT_STATE_START)
	{
		if ( NULL == the_server_address || NULL == the_server_port )
		{
			// cannot connect if we don't know where !!!
			m_connectState = CONNECT_STATE_FAILED;
			return false;
		}

		//initialize a socket
		if ( !m_socket->CreateSocket() )
		{
			m_connectState = CONNECT_STATE_FAILED;
			return false;
		}

		m_connectState = CONNECT_STATE_WAITING;
		return false;
	}
	else if(m_connectState == CONNECT_STATE_WAITING)
	{
		if ( !m_socket->ConnectByName( the_server_address ) )
		{
			if(m_socket->GetNonBlockConnectState() == GLXPlayerSocket::NONBLOCK_CONNECT_STATE_WAITING)
			{
				return false;
			}
			else if(m_socket->GetNonBlockConnectState() == GLXPlayerSocket::NONBLOCK_CONNECT_STATE_START)
			{
				return false;
			}
			else
			{
				m_connectState = CONNECT_STATE_FAILED;
				m_socket->SetNonBlockConnectState(GLXPlayerSocket::NONBLOCK_CONNECT_STATE_START);
				return false;
			}
		}
		else
		{
			connected = true;

			// we are connected, send the session establish connection package 
			sendEstablishConnectionPackage();

			// start the receiving operation ... 
			receiveDataLen();

			//    connected = true;
			time_of_last_sending = XP_API_GET_TIME();

			m_connectState = CONNECT_STATE_SUCCESS;
			m_socket->SetNonBlockConnectState(GLXPlayerSocket::NONBLOCK_CONNECT_STATE_START);

			return true;
		}
	}
	else
	{
		return false;
	}
}

void Connection::disconnect()
{
	connected = false;

	if ( m_socket != NULL )
	{
		m_socket->CloseSocket();
		delete( m_socket );
		m_socket = NULL;
	}

   // Delete the retry packets.
   cleanRetryData();

   DataPacket* temp = NULL;

   // Delete all the incoming packets.
   while (currentIncomingPacket)
   {
      temp = currentIncomingPacket;
      currentIncomingPacket = currentIncomingPacket->getNextPacket();
      delete temp;
   }
   currentIncomingPacket = NULL;
   lastIncomingPacket    = NULL;

   // Delete all the outgoing packets.
   while (currentOutgoingPacket)
   {
      temp = currentOutgoingPacket;
      currentOutgoingPacket = currentOutgoingPacket->getNextPacket();
      delete temp;
   }
   currentOutgoingPacket = NULL;
   lastOutgoingPacket    = NULL;

   // Reset the buffers.
   XP_API_MEMSET(rbuf, 0, sizeof(rbuf));
   XP_API_MEMSET(sbuf, 0, sizeof(rbuf));

   // Reset the times.
   time_of_last_sending = 0;
   time_of_last_request = 0;

#ifdef MP_ENABLE_TIMEOUT 
	last_activity_time   = 0;
#endif //MP_ENABLE_TIMEOUT
}

void Connection::sendData()
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
	sbuf[0] = (char) currentOutgoingPacket->getMessageLen();
	XP_API_MEMCPY( sbuf+1, currentOutgoingPacket->getMessageBody(), currentOutgoingPacket->getMessageLen() );

	int iResult = m_socket->Select( GLXPlayerSocket::XSOCKET_SELECT_WRITE );

	if ( iResult < 0 )
	{
		XP_DEBUG_OUT( "sendData: select error." );
		return;
	}

	iResult = m_socket->Send( (char*)sbuf, currentOutgoingPacket->getMessageLen()+1 );

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

void Connection::receiveDataLen()
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
		iResult = m_socket->Recv( (char *)buffer, 1 );

		if (iResult == -1)
		{
			return;
		}
		else if(iResult != 1)
		{
			disconnect();
		}
		else
		{
			int rExpected = (BYTE) buffer[0];
			XP_API_MEMSET( buffer, 0, 128 );
			receiveData( rExpected );
		}
	}
}

void Connection::receiveData(int expectedLen)
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
			DataPacket* packet = XP_API_NEW DefaultDataPacket();
			packet->setMessageBody( (BYTE*)buffer ,expectedLen );
			addIncomingPacket(packet);

			// reload .. 
			XP_API_MEMSET( buffer, 0, 128 );
			//receiveDataLen();//??
			///continue;
		}
	}
}

void Connection::addOutgoingPacket(DataPacket* packet)
{
   if (packet != NULL)
   {
      if (!connected)
      {
//         XP_DEBUG_OUT("addOutgoingPacket: not connected, packet deleted");
         XP_API_DEL(packet);
         return;
      }
      if (currentOutgoingPacket == NULL)
      {
         // nothing in the list. 
         currentOutgoingPacket = packet;
         lastOutgoingPacket = packet;
         sendData();
      }
      else
      {
         if (lastOutgoingPacket != NULL)
         {
            // put it at the end. 
            lastOutgoingPacket->setNextPacket(packet);
            lastOutgoingPacket = packet;
         }
         else
         {
            // this should not happen. When currentPacket is not null,
            // last packet should be not null either.
            XP_DEBUG_OUT("addOutgoingPacket: Error!");
         }
      }
   }
}

void Connection::addIncomingPacket(DataPacket* packet)
{
   if (packet != NULL)
   {
      if (currentIncomingPacket == NULL)
      {
         // nothing in the list. 
         currentIncomingPacket = packet;
         lastIncomingPacket = packet;        
      }
      else
      {
         if (lastIncomingPacket != NULL)
         {
            // put it at the end. 
            lastIncomingPacket->setNextPacket(packet);
            lastIncomingPacket = packet;
         }
         else
         {
            // this should not happen. When currentPacket is not null,
            // last packet should be not null either.
            XP_DEBUG_OUT("addIncomingPacket: Error!");
         }
      }
   }
}

/**
 * IMPORTANT:
 * After the message is processed, it MUST be deleted!!! 
 * Otherwise a memory leak will occur!
 */
DataPacket* Connection::getNextIncomingPacket()
{
   if (currentIncomingPacket == NULL)
   {
      // no incoming packets.
      return NULL;
   }

   DataPacket* packet = currentIncomingPacket;

   currentIncomingPacket = currentIncomingPacket->getNextPacket();
   if (lastIncomingPacket == packet)
   {
      // this will be actually NULL since 
      // lastIncomingPacket == packet == (old) currentIncomingPacket happens only if 
      // there is only one packet in the list
      lastIncomingPacket = currentIncomingPacket; 
   }

   // remove the link .. this package will be deleted by the caller ... 
   packet->setNextPacket(NULL);
   return packet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The caller MUST trigger the appropriate disconnect if the connection
// is lost.
bool Connection::isConnected()
{
   return connected;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Connection::set_connected(bool conn)
{
   connected = conn;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// utilities for sending different kind of packets ... 

// in order to establish an udp connection with the server , we need to send 
// two packets containing a connection identifier ; if the server 
// matches the packages , the connection is established 
void Connection:: sendEstablishConnectionPackage()
{
	// get the connection id - random number
	int connection_id = 0;

	connection_id = XP_API_GET_RANDOM( m_name );

	XP_DEBUG_OUT( "EstablishConnection random = %d\n", connection_id );

	DataPacket* packet = NULL;

#ifdef ENABLE_DUAL_TCP
   the_connection_id = connection_id;
   sendEstablishConnectionPackageOnReceive();
#else

   packet = XP_API_NEW DefaultDataPacket();

   packet->addByte( BASIC_MESSAGE_TYPE_CONNECT );
   packet->addByte( CONNECTION_MESSAGE_TYPE_INIT_WRITE );
   packet->addInt( connection_id );
   packet->packMessage();

   addOutgoingPacket(packet);

#endif

   packet = XP_API_NEW DefaultDataPacket();

   packet->addByte(BASIC_MESSAGE_TYPE_CONNECT);
   packet->addByte(CONNECTION_MESSAGE_TYPE_INIT_READ);
   packet->addInt(connection_id);

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

void Connection:: sendFinishConnectionPackage()
{
   DataPacket* packet = XP_API_NEW DefaultDataPacket();

   packet->addByte(BASIC_MESSAGE_TYPE_CONNECT);
   packet->addByte(CONNECTION_MESSAGE_TYPE_DISCONECT);

   packet->packMessage();

   addOutgoingPacket(packet);
   time_of_last_request = XP_API_GET_TIME();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Connection::sendLoginPackage(char* nickname)
{
   DataPacket* login_packet = XP_API_NEW DefaultDataPacket();

   login_packet->addByte(BASIC_MESSAGE_TYPE_GAME);
   login_packet->addByte(GAME_MESSAGE_REQUEST);
   login_packet->addByte(REQUEST_MESSAGE_LOGIN);
   XP_DEBUG_OUT("Connection::sendLoginPackage()\nuser=[%s]\n", nickname);
   login_packet->addByteLenString(nickname,XP_API_STRLEN(nickname));

   login_packet->packMessage();

   saveRetryData(login_packet);
   addOutgoingPacket(login_packet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// the game data is optional .. if none , send NULL 
void Connection:: sendCreateSessionPackage(char* session_name,char* game_data, int length)
{
   DataPacket* packet = XP_API_NEW DefaultDataPacket();

   packet->addByte(BASIC_MESSAGE_TYPE_GAME);
   packet->addByte(GAME_MESSAGE_REQUEST);
   packet->addByte(REQUEST_MESSAGE_CREATE_SESSION);
   packet->addByteLenString(session_name, XP_API_STRLEN(session_name));

   if (game_data == NULL)
   {
      game_data = " ";
   }
   packet->addByteLenString(game_data, length);

   packet->packMessage();

   saveRetryData(packet);
   addOutgoingPacket(packet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Connection:: sendQuickGamePackage()
{
   DataPacket* packet = XP_API_NEW DefaultDataPacket();

   packet->addByte(BASIC_MESSAGE_TYPE_GAME);
   packet->addByte(GAME_MESSAGE_REQUEST);
   packet->addByte(REQUEST_MESSAGE_GET_QUICK_SESSION);

   packet->packMessage();

   saveRetryData(packet);
   addOutgoingPacket(packet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Connection:: sendJoinSessionPackage(char* session_name)
{
   DataPacket* packet = XP_API_NEW DefaultDataPacket();

   packet->addByte(BASIC_MESSAGE_TYPE_GAME);
   packet->addByte(GAME_MESSAGE_REQUEST);
   packet->addByte(REQUEST_MESSAGE_JOIN_SESSION);
   packet->addByteLenString(session_name,XP_API_STRLEN(session_name));

   packet->packMessage();

   saveRetryData(packet);
   addOutgoingPacket(packet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Connection:: sendLeaveSessionPackage()
{
   DataPacket* packet = XP_API_NEW DefaultDataPacket();

   packet->addByte(BASIC_MESSAGE_TYPE_GAME);
   packet->addByte(GAME_MESSAGE_REQUEST);
   packet->addByte(REQUEST_MESSAGE_LEAVE_SESSION);

   packet->packMessage();

   saveRetryData(packet);
   addOutgoingPacket(packet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Connection:: sendListSessionsPackage(BYTE number_of_sessions,int session_index)
{
	DataPacket* packet = XP_API_NEW DefaultDataPacket();

	packet->addByte(BASIC_MESSAGE_TYPE_GAME);
	packet->addByte(GAME_MESSAGE_REQUEST);
	packet->addByte(REQUEST_MESSAGE_LIST_SESSIONS);
	packet->addByte(number_of_sessions);
	packet->addInt(session_index);

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

void Connection::sendListSessionsAllPackage(BYTE number_of_sessions,int session_index)
{
	DataPacket* packet = XP_API_NEW DefaultDataPacket();

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

void Connection::sendListSessionsByNamePackage(BYTE number_of_sessions,int session_index, char * name)
{
	DataPacket* packet = XP_API_NEW DefaultDataPacket();

	packet->addByte(BASIC_MESSAGE_TYPE_GAME);
	packet->addByte(GAME_MESSAGE_REQUEST);
	packet->addByte(REQUEST_MESSAGE_LIST_SESSIONS);
	packet->addByte(number_of_sessions);
	packet->addInt(session_index);
	packet->addByte(REQUEST_QUERY_BY_NAME);
	packet->addByteLenString(name,XP_API_STRLEN(name));

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

void Connection::sendListSessionsByDataPackage(BYTE number_of_sessions,int session_index, char * data, int length)
{
	DataPacket* packet = XP_API_NEW DefaultDataPacket();

	packet->addByte(BASIC_MESSAGE_TYPE_GAME);
	packet->addByte(GAME_MESSAGE_REQUEST);
	packet->addByte(REQUEST_MESSAGE_LIST_SESSIONS);
	packet->addByte(number_of_sessions);
	packet->addInt(session_index);
	packet->addByte(REQUEST_QUERY_BY_DATA);
	packet->addByteLenString(data,length);

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

/**
 * Send the package to start game.
 * Data Format: *grs
 */
void Connection::sendStartGamePackage()
{
	DataPacket* packet = XP_API_NEW DefaultDataPacket();

	packet->addByte( BASIC_MESSAGE_TYPE_GAME );
	packet->addByte( GAME_MESSAGE_REQUEST );
	packet->addByte( REQUEST_MESSAGE_START_GAME );
	packet->packMessage();
	saveRetryData( packet );
	addOutgoingPacket( packet );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Connection:: sendFinishGamePackage()
{
   DataPacket* packet = XP_API_NEW DefaultDataPacket();

   packet->addByte(BASIC_MESSAGE_TYPE_GAME);
   packet->addByte(GAME_MESSAGE_REQUEST);
   packet->addByte(REQUEST_MESSAGE_FINISH_GAME);

   packet->packMessage();

   saveRetryData(packet);
   addOutgoingPacket(packet);
}

void Connection::sendUpdateSessionPackage( int player_number )
{
	DataPacket* packet = XP_API_NEW DefaultDataPacket();

	packet->addByte(BASIC_MESSAGE_TYPE_GAME);
	packet->addByte(GAME_MESSAGE_REQUEST);
	packet->addByte(REQUEST_MESSAGE_UPDATE_SESSION);
	packet->addByte(player_number);

	packet->packMessage();

	saveRetryData(packet);
	addOutgoingPacket(packet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Connection:: sendKickOutPlayerPackage(char * player_name)
{
   if (player_name == NULL)
   {
      return;
   }

   DataPacket* packet = XP_API_NEW DefaultDataPacket();

   packet->addByte(BASIC_MESSAGE_TYPE_GAME);
   packet->addByte(GAME_MESSAGE_REQUEST);
   packet->addByte(REQUEST_MESSAGE_KICK_OUT_PLAYER);
   packet->addByteLenString(player_name,XP_API_STRLEN(player_name));

   packet->packMessage();

   saveRetryData(packet);
   addOutgoingPacket(packet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Connection::keepConnectionAlive()
{
   // check the time of last sending ...
	long current_time = XP_API_GET_TIME();

	if (( current_time - time_of_last_sending) > KEEP_ALIVE_TIMEOUT )
	{
      // time_of_last_sending is set in sendData(), connectCallbk() and
      // sendEstablishConnectionPackageOnReceive(). This ensures that the keep
      // alive is only sent when necessary.
//      XP_DEBUG_OUT("TX keep alive msg\n");

		DataPacket* packet = XP_API_NEW DefaultDataPacket();

		packet->addByte(BASIC_MESSAGE_TYPE_GAME);
		packet->addByte(GAME_MESSAGE_KEEP_ALIVE);

		packet->packMessage();

		addOutgoingPacket(packet);

		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Connection::saveRetryData(DataPacket* packet)
{
   if (packet == NULL)
   {
      return;
   }

   cleanRetryData();

   retryPacket = XP_API_NEW DefaultDataPacket();

   retryPacket->setMessageBody(packet->getMessageBody(),packet->getMessageLen());

   time_of_last_request = XP_API_GET_TIME();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Connection::sendRetryData()
{
   if (retryPacket == NULL)
   {
      return;
   }

   // Create a XP_API_NEW packet. The current one has to be saved for further retries.
   // The only way to delete the retry packet is to use cleanRetryData().
   DataPacket* packet = XP_API_NEW DefaultDataPacket();

   packet->setMessageBody(retryPacket->getMessageBody(),retryPacket->getMessageLen());

   addOutgoingPacket(packet);

   time_of_last_request = XP_API_GET_TIME();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Connection::cleanRetryData()
{
   if (retryPacket != NULL)
   {
      delete retryPacket;
      retryPacket = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long Connection::getTimeOfLastReqest()
{
   return time_of_last_request;
}

#endif //ENABLE_MULTIPLAYER
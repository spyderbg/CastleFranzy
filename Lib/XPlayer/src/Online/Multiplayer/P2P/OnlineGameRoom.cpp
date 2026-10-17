#include "../../../../include/Multiplayer/P2P/OnlineGameRoom.h"
#include "../../../../include/Multiplayer/Common/DataBuilder.h"

#ifdef ENABLE_MULTIPLAYER

COnlineGameRoom::COnlineGameRoom(void)
{
	m_player_list = XP_API_NEW COnlinePlayerList();
	m_self = NULL;
	m_host = NULL;
	m_protocol = ONLINE_PROTOCOL_UDP;
	m_room_name = NULL;
	m_room_data = NULL;
	m_max_player_count = MP_P2P_MAX_OPPONENT_COUNT;
	m_player_count = 0;
	m_ping_starttime = 0;
	m_ping_endtime = 0;
	m_status = GAME_ROOM_STATUS_FOUND;
	m_error = ERROR_NONE;
	m_room_password = NULL;
	m_isFinishConfirm = false;

	m_joinRoomState = JOINSTATE_START;
	m_setMyselfState=SET_NONE;
}

COnlineGameRoom::~COnlineGameRoom(void)
{
	m_self = NULL;
	m_host = NULL;
	XP_API_DEL( m_player_list );
	XP_API_DEL( m_room_name );
	XP_API_DEL( m_room_data );
	XP_API_DEL( m_room_password );
}

int COnlineGameRoom::GetPlayerCount()
{
	if ( NULL == m_self )
	{
		// I have not joined the game room.
		return m_player_count;
	}
	else
	{
		return m_player_list->GetPlayerCount();
	}
}

COnlinePlayer * COnlineGameRoom::GetPlayer( int index )
{
	return m_player_list->GetPlayer( index );
}

void COnlineGameRoom::SetPlayerCount( int count )
{
	m_player_count = count;
}

bool COnlineGameRoom::SetHost( int index )
{
	if ( index >= m_player_list->Count() )
	{
		return false;
	}

	m_host = m_player_list->Get( index );
	m_host->SetJoined( true );
	return true;
}

bool COnlineGameRoom::SetMyself( int index, OnlinePlayerType type )
{
	switch(m_setMyselfState)
	{
	case SET_NONE:
		if ( index >= m_player_list->Count() )
		{
			m_setMyselfState=SET_NONE;
			return true;
		}
		m_self = m_player_list->Get( index );
		m_setMyselfState=SET_WAIT;
		m_self->InitUpnpInitState();
		return false;
	case SET_WAIT:
		if(m_self->InitSocket( type ))
          m_setMyselfState=SET_END;
		return false;
	case SET_END:
		m_self->SetAlive( true );
		m_self->SetConnected( true );
		m_setMyselfState=SET_NONE;
		return true;
		

	}
	return false;
}

void COnlineGameRoom::UpdateData()
{
	if ( AmITheHost() )
	{
		// Accept connection request from client.
		AcceptNewConnection();
	}

	// Receive data from client.
	ReceiveData();

	// Handel data
	HandleData();
	
	// Send data
	SendData();

	KeepAlive();

	// Remove dead connection
	HandleConnection();	
}

bool COnlineGameRoom::AddPlayer( char * name, char * country, DataPacket * data, 
	char * exter_ip, char * inter_ip, int exter_tcp_port, int exter_udp_port,
		int inter_tcp_port, int inter_udp_port )
{
	if ( m_player_list->Count() >= m_max_player_count )
	{
		return false;
	}

	return ( m_player_list->AddPlayer( name, country, data, exter_ip, inter_ip, exter_tcp_port, 
		exter_udp_port, inter_tcp_port, inter_udp_port ) != NULL );
}

void COnlineGameRoom::AcceptNewConnection()
{
	GLXPlayerSocket * socket = m_self->Accept();
	if ( NULL != socket )
	{
		// Add player into list.
		XP_DEBUG_OUT( "COnlineGameRoom::AcceptNewConnection()\n" );
		COnlinePlayer * player = m_player_list->AddPlayer(NULL,NULL,NULL,NULL,NULL,0,0,0,0);		
		if ( NULL == player )
		{
			XP_DEBUG_OUT( "NULL == player\n" );
			XP_API_DEL( socket );
		}
		else
		{
			XP_DEBUG_OUT( "SetTcpSocket\n" );			
			// Set tcp socket.
			player->SetTcpSocket( socket );
			// Wait for join session package
			player->SetConnected( true );
			player->SetJoined( false );
		}
	}	
}

void COnlineGameRoom::ReceiveData()
{
	if ( AmITheHost() )
	{
		// Receive data from TCP connection
		for ( int i=0; i<m_player_list->Count(); i++ )
		{
			COnlinePlayer * player = m_player_list->Get( i );
			if ( player == m_self )
			{
				continue;
			}

			player->ReceiveTCPData();
			m_self->ReceiveUDPData( player );
		}
	}
	else
	{
		if(m_self != NULL)
		{
			m_self->ReceiveData();
		}
	}
}

void COnlineGameRoom::SendData()
{
	if ( AmITheHost() )
	{
		for ( int i=0; i<m_player_list->Count(); i++ )
		{
			COnlinePlayer * player = m_player_list->Get( i );
			if ( player == m_self )
			{
				continue;
			}

			player->SendAllTCPData();
			m_self->SendAllUDPData( player );
		}
	}
	else
	{
		m_self->SendAllData( m_host );
	}
}

void COnlineGameRoom::HandleData()
{
	if ( AmITheHost() )
	{
		if ( IsInRoom() )
		{
			m_self->HandlePingData();
		}

		for ( int i=0; i<m_player_list->Count(); i++ )
		{
			COnlinePlayer * player = m_player_list->Get( i );
			if ( player == m_self )
			{
				continue;
			}

			DataPacket * packet = HandleAllTcpData( i, player->GetIncomingPacket() );
			player->SetIncomingPacket( packet );
		}

		DataPacket * packet = HandleAllUdpData( m_self->GetIncomingPacket() );
		m_self->SetIncomingPacket( packet );
	}
	else
	{
		DataPacket * packet = HandleAllTcpData( -1, m_self->GetIncomingPacket() );
		m_self->SetIncomingPacket( packet );
		packet = HandleAllUdpData( m_self->GetIncomingPacket() );
		m_self->SetIncomingPacket( packet );
	}
}

DataPacket * COnlineGameRoom::HandleAllTcpData( int index, DataPacket * currentPacket )
{
	DataPacket * previous = NULL;
	DataPacket * header = NULL;

	while( NULL != currentPacket )
	{
		DataPacket * packet = currentPacket;

		if ( packet->getDataType() == ONLINE_PROTOCOL_TCP && HandleData( index, packet ) )
		{
			if ( NULL == previous )
			{
				currentPacket = currentPacket->getNextPacket();
			}
			else
			{
				currentPacket = currentPacket->getNextPacket();
				previous->setNextPacket( currentPacket );
			}
			XP_API_DEL( packet );
			packet = NULL;
		}
		else
		{
			if ( NULL == header )
			{
				header = currentPacket;
			}
			previous = currentPacket;
			currentPacket = currentPacket->getNextPacket();
		}
	}

	return header;
}

DataPacket * COnlineGameRoom::HandleAllUdpData( DataPacket * currentPacket )
{
	DataPacket * previous = NULL;
	DataPacket * header = NULL;

	while( NULL != currentPacket )
	{
		DataPacket * packet = currentPacket;

		if ( packet->getDataType() == ONLINE_PROTOCOL_UDP && HandleUdpData( packet ) )
		{
			if ( NULL == previous )
			{
				currentPacket = currentPacket->getNextPacket();
			}
			else
			{
				currentPacket = currentPacket->getNextPacket();
				previous->setNextPacket( currentPacket );
			}
			XP_API_DEL( packet );
			packet = NULL;
		}
		else
		{
			if ( NULL == header )
			{
				header = currentPacket;
			}
			previous = currentPacket;
			currentPacket = currentPacket->getNextPacket();
		}
	}

	return header;
}

bool COnlineGameRoom::HandleUdpData( DataPacket * currentPacket )
{
	int index= -1;

	for ( int i=0; i<m_player_list->Count(); i++ )
	{
		COnlinePlayer * player = m_player_list->Get( i );
		if ( NULL != currentPacket->getIP() && currentPacket->getPort() > 0 )
		{
			if ( XP_API_STRCMP( player->GetExternalIP(), currentPacket->getIP() ) == 0 &&
				 player->GetExternalUdpPort() == currentPacket->getPort() )
			{
				index = i;
				break;
			}

			if ( XP_API_STRCMP( player->GetInternalIP(), currentPacket->getIP() ) == 0 &&
				 player->GetInternalUdpPort() == currentPacket->getPort() )
			{
				index = i;
				break;
			}
		}
	}

	if ( index < 0 )
	{
		return true;
	}

	return HandleData( index, currentPacket );
}

bool COnlineGameRoom::HandleData( int index, DataPacket * currentPacket )
{
	bool bReturn = false;
	BYTE type = 0;
	BYTE subtype = 0;	

	if ( NULL == currentPacket )
	{
		return false;
	}

	if ( !currentPacket->getByte( type ) )
	{
		XP_DEBUG_OUT( "HandleData() - get type error!\n" );
		return true;
	}

	if ( BASIC_MESSAGE_TYPE_GAME != type )
	{
		XP_DEBUG_OUT( "HandleData() - type error!\n" );
		return true;
	}

	if ( !currentPacket->getByte( subtype ) )
	{
		XP_DEBUG_OUT( "HandleData() - get subtype error!\n" );
		return true;
	}

	switch ( subtype )
	{
		case GAME_MESSAGE_REQUEST:
		{
			bReturn = HandleRequestData( index, currentPacket );
		}  
		break;

		case GAME_MESSAGE_RESPONSE:
		{
			bReturn = HandleResponseData( index, currentPacket );
		}
		break;

		case GAME_MESSAGE_PUSH:
		{
			bReturn = HandlePushData( index, currentPacket );
		}
		break;
		
		case GAME_MESSAGE_IN_GAME:
		case GAME_MESSAGE_IN_GAME_RELIABLE:
		{
			bReturn = HandleGameData( index, currentPacket );
		}               
		break;
		
		case GAME_MESSAGE_KEEP_ALIVE:
		{
			bReturn = HandleKeepAliveData( index, currentPacket );
		}
        break;

        default:
		{
			XP_DEBUG_OUT( "XPeerList::HandleData() - subtype error!\n" );
			bReturn = true;
		}
		break;
	}

	return bReturn;
}

bool COnlineGameRoom::HandleRequestData( int index, DataPacket * currentPacket )
{
	BYTE cmdtype = 0;

	if ( !currentPacket->getByte( cmdtype ) )
	{
		return true;
	}

	switch( cmdtype )
	{
		// Join Session "~grj~luth~10.15.0.171|219.142.114.185|56800|56800|56810|CN"
		case REQUEST_MESSAGE_JOIN_SESSION:
		{
			HandleRequestJoinRoom( index, currentPacket );
		}
		break;

		case REQUEST_MESSAGE_LEAVE_SESSION:
		{
			HandleRequestLeaveRoom( index, currentPacket );
		}
		break;

		case REQUEST_MESSAGE_FINISH_GAME:
		{
			HandleRequestFinishGame( index, currentPacket );
		}
		break;

		default:
		{
			;
		}
		break;
	}

	return true;
}

bool COnlineGameRoom::HandlePushData( int index, DataPacket* currentPacket )
{
	BYTE cmdtype = 0;

	if ( !currentPacket->getByte( cmdtype ) )
	{
		return true;
	}

	switch( cmdtype )
	{
		// ~gpj^~name
		case REQUEST_MESSAGE_JOIN_SESSION:
		{
			HandlePushJoinRoom( index, currentPacket );
		}
		break;

		case PUSH_MESSAGE_LEAVE_SESSION:
		{
			HandlePushLeaveRoom( index, currentPacket );
		}
		break;

		case PUSH_MESSAGE_KICK_OUT:
		{
			HandlePushKickoutRoom( index, currentPacket );
		}
		break;

		case PUSH_MESSAGE_START_GAME:
		{
			HandlePushStartGame( index, currentPacket );
		}
		break;

		case PUSH_MESSAGE_FINISH_GAME:
		{
			HandlePushFinishGame( index, currentPacket );
		}
		break;

		case PUSH_MESSAGE_FINISH_GAME_CONFIRM:
		{
			HandlePushFinishGameConfirm( index, currentPacket );
		}
		break;

		case PUSH_MESSAGE_FINISH_GAME_RESTART:
		{
			HandlePushFinishGameRestart( index, currentPacket );
		}
		break;

		default:
		{
			;
		}
		break;
	}

	return true;
}

bool COnlineGameRoom::HandlePushJoinRoom( int index, DataPacket* currentPacket )
{
	char * name = NULL;
	BYTE len = 0;
	char * country = NULL;
	DataPacket * packet = NULL;

	currentPacket->getByteLenString( name, len );

	if ( IsPlayerExisted( name ) >= 0 )
	{
		// Existed player
		XP_API_DEL( name );
		return false;
	}	

	currentPacket->getByteLenString( country, len );
	currentPacket->getPacket( packet );

	AddPlayer( name, country, packet, NULL, NULL, 0, 0, 0, 0 );
	
	COnlinePlayer * player = m_player_list->GetLastPlayer();

	player->SetAlive( true );
	player->SetConnected( false );
	player->SetJoined( true );
	
	XP_API_DEL( country );
	XP_API_DEL( packet );

	return true;
}

bool COnlineGameRoom::HandlePushLeaveRoom( int index, DataPacket* currentPacket )
{
	char * name = NULL;
	BYTE len = 0;

	currentPacket->getByteLenString( name, len );

	m_player_list->RemovePlayer( name );

	XP_API_DEL( name );
	name = NULL;

	return true;
}

bool COnlineGameRoom::HandlePushKickoutRoom( int index, DataPacket* currentPacket )
{
	BYTE reason = -1;
	currentPacket->getByte( reason );

	if ( ERROR_KICK_OUT_KICKED == reason )
	{
		char * name = NULL;
		BYTE len = 0;
		currentPacket->getByteLenString( name, len );

		if ( XP_API_STRCMP( name, m_self->GetName() ) == 0 )
		{
			m_self->SetAlive( false );
			m_status = GAME_ROOM_STATUS_FOUND;
		}
		else
		{
			m_player_list->RemovePlayer( name );
			m_status = GAME_ROOM_STATUS_ROOM;
		}

		XP_API_DEL( name );
		name = NULL;
	}
	else if ( ERROR_KICK_OUT_NO_SESSION == reason )
	{
		m_self->SetAlive( false );
		m_status = GAME_ROOM_STATUS_FOUND;
	}

	return true;
}

bool COnlineGameRoom::HandlePushStartGame( int index, DataPacket* currentPacket )
{
	m_status = GAME_ROOM_STATUS_GAME;
	return true;
}

bool COnlineGameRoom::HandlePushFinishGame( int index, DataPacket* currentPacket )
{
	m_status = GAME_ROOM_STATUS_GAME_FINISH;
	return true;
}

bool COnlineGameRoom::HandlePushFinishGameConfirm( int index, DataPacket* currentPacket )
{
	m_isFinishConfirm = true;
	return true;
}

bool COnlineGameRoom::HandlePushFinishGameRestart(int index, DataPacket* currentPacket)
{
	m_isFinishConfirm = false;
	return true;
}

bool COnlineGameRoom::HandleGameData( int index, DataPacket* packet )
{
	if ( AmITheHost() )
	{
		for ( int i=0; i<m_player_list->Count(); i++ )
		{
			if ( i == index )
			{
				continue;
			}

			COnlinePlayer * player = m_player_list->Get( i );

			if ( player == m_self )
			{
				continue;
			}

			XP_DEBUG_OUT( "==== Host Transfer Data ====\n" );

			DataPacket * data = XP_API_NEW DefaultDataPacket( packet );
			DataPacket * data_send = XP_API_NEW DefaultDataPacket( packet );

			BYTE head_data = 0;

			data->getByte( head_data );
			data->getByte( head_data );
			
			if ( head_data == GAME_MESSAGE_IN_GAME_RELIABLE )
			{
				XP_DEBUG_OUT( "GAME_MESSAGE_IN_GAME_RELIABLE\n" );
				data_send->setDataType( DATA_PACKET_TYPE_GAME_RELIABLE );
				player->AddOutputData( data_send );
			}
			else if ( head_data == GAME_MESSAGE_IN_GAME )
			{
				XP_DEBUG_OUT( "GAME_MESSAGE_IN_GAME\n" );
				data_send->setDataType( DATA_PACKET_TYPE_GAME );
				player->AddOutputData( data_send );
			}
			else
			{
				XP_DEBUG_OUT( "Invalid Data Type!!!\n" );
			}

			XP_API_DEL( data );
			XP_API_DEL( data_send );
		}
	}
	
	return false;
}

bool COnlineGameRoom::HandleKeepAliveData( int index, DataPacket* packet )
{
	return true;
}

bool COnlineGameRoom::HandleRequestJoinRoom( int index, DataPacket * currentPacket )
{
	// The room is full or not
	if ( !CanJoin() )
	{
		// Send response message
		DataPacket * package = XP_API_NEW DefaultDataPacket();
		DataBuilder::BuildRequestError( package, REQUEST_MESSAGE_JOIN_SESSION );
		package->addByte( ERROR_JOIN_SESSION_TOO_MANY_PLAYERS );
		package->packMessage();
		m_player_list->Get(index)->AddOutputData( package );
		m_player_list->Get(index)->SetAlive( false );
		XP_API_DEL( package );
		return true;
	}

	char *data_name = NULL;
	BYTE data_name_len = 0;

	// Save user nick name
	currentPacket->getByteLenString( data_name, data_name_len );

	// Get header information
	int local_ip = 0;
	int router_ip = 0;
	short local_tcp_port = 0;
	short local_udp_port = 0;
	short router_tcp_port = 0;
	short router_udp_port = 0;
	char * nationality = NULL;
	BYTE nationlity_len = 0;
	char * password = NULL;
	BYTE password_len = 0;

	if( !currentPacket->getInt( local_ip ) )
	{
		return false;
	}

	if( !currentPacket->getShort( local_tcp_port ) )
	{
		return false;		
	}

	if ( !currentPacket->getShort( local_udp_port ) )
	{
		return false;	
	}

	if( !currentPacket->getInt( router_ip ) )
	{
		return false;	
	}

	if( !currentPacket->getShort( router_tcp_port ) )
	{
		return false;	
	}

	if( !currentPacket->getShort( router_udp_port ) )
	{
		return false;	
	}

	if( !currentPacket->getByteLenString( nationality, nationlity_len ) )
	{
		return false;
	}

	if( !currentPacket->getByteLenString( password, password_len ) )
	{
		return false;
	}

	// Get player data
	DataPacket * packet_player = NULL;
	currentPacket->getPacket( packet_player );

	char ip_internal[32] = { 0 };
	char ip_external[32] = { 0 };
	
	XP_API_IP_INT2STR( local_ip, ip_internal );
	XP_API_IP_INT2STR( router_ip, ip_external );
	int port_local_tcp = (unsigned short)local_tcp_port;
	int port_local_udp = (unsigned short)local_udp_port;
	int port_router_tcp = (unsigned short)router_tcp_port;
	int port_router_udp = (unsigned short)router_udp_port;

	// Judge room password
	if ( NULL != m_room_password )
	{
		if ( XP_API_STRCMP( password, m_room_password ) != 0 )
		{
			XP_API_DEL( data_name );
			XP_API_DEL( password );
			XP_API_DEL( nationality );
			XP_API_DEL( packet_player );
			// Send response message
			DataPacket * package = XP_API_NEW DefaultDataPacket();
			DataBuilder::BuildRequestError( package, REQUEST_MESSAGE_JOIN_SESSION );
			package->addByte( ERROR_JOIN_SESSION_WRONG_PASSWORD );
			package->packMessage();
			m_player_list->Get(index)->AddOutputData( package );
			m_player_list->Get(index)->SetAlive( false );
			XP_API_DEL( package );
			return true;
		}
	}

	m_player_list->Get(index)->SetName( data_name );
	XP_API_DEL( data_name );

	COnlinePlayer * player = m_player_list->Get(index);
	player->SetNationality( nationality );
	player->SetJoined( true );
	player->SetConnected( true );
	player->SetAlive( true );

	char ip_local[32] = { 0 };
	char ip_remote[32] = { 0 };

	XP_API_MEMSET( ip_local, 0, 32 );
	XP_API_MEMSET( ip_remote, 0, 32 );
	XP_API_SPRINTF( ip_local, "%s:%d", ip_internal, port_local_tcp );
	XP_API_SPRINTF( ip_remote, "%s:%d", ip_external, port_router_tcp );
	player->SetExternalTcpAddress( ip_remote );				
	player->SetInternalTcpAddress( ip_local );

	XP_API_MEMSET( ip_local, 0, 32 );
	XP_API_MEMSET( ip_remote, 0, 32 );
	XP_API_SPRINTF( ip_local, "%s:%d", ip_internal, port_local_udp );
	XP_API_SPRINTF( ip_remote, "%s:%d", ip_external, port_router_udp );
	player->SetExternalUdpAddress( ip_remote );
	player->SetInternalUdpAddress( ip_local );

	player->SetData( packet_player );
	XP_API_DEL( packet_player );

	// Send push message to other users
	for (int i=0; i<m_player_list->Count(); i++)
	{
		if (i == index)
		{
			continue;
		}

		COnlinePlayer * player = m_player_list->Get(i);

		if ( player == m_self )
		{
			continue;
		}

		COnlinePlayer * player_new = m_player_list->Get(index);

		DataPacket * package = XP_API_NEW DefaultDataPacket();
		DataBuilder::BuildPushMessage( package, PUSH_MESSAGE_JOIN_SESSION );		
		package->addByteLenString( player_new->GetName(), XP_API_STRLEN( player_new->GetName() ) );
		package->addByteLenString( nationality, XP_API_STRLEN( nationality ) );
		package->addPacket( player_new->GetData() );
		package->packMessage();
		player->AddOutputData( package );
		XP_API_DEL( package );
	}

	// Send response message
	DataPacket * package = XP_API_NEW DefaultDataPacket();
	DataBuilder::BuildRequestSuccess( package, REQUEST_MESSAGE_JOIN_SESSION );
	package->addByte( m_player_list->Count() );
	for( int i=0; i<m_player_list->Count(); i++ )
	{
		COnlinePlayer * player = m_player_list->Get(i);
		package->addByteLenString( player->GetName(), XP_API_STRLEN( player->GetName() ) );
		if ( index == i )
		{
			package->addByteLenString( nationality, XP_API_STRLEN( nationality ) );
		}
		else
		{
			package->addByteLenString( player->GetNationality(), XP_API_STRLEN( player->GetNationality() ) );
		}
		package->addPacket( player->GetData() );
	}
	package->packMessage();
	m_player_list->Get(index)->AddOutputData( package );
	XP_API_DEL( package );

	XP_API_DEL( nationality );
	XP_API_DEL( password );

	return true;
}

bool COnlineGameRoom::HandleRequestLeaveRoom( int index, DataPacket * currentPacket )
{
	// Send push message to other users
	for (int i=0; i<m_player_list->Count(); i++)
	{
		if ( i == index )
		{
			continue;
		}

		COnlinePlayer * player = m_player_list->Get(i);

		if ( player == m_self )
		{
			continue;
		}

		DataPacket * package = XP_API_NEW DefaultDataPacket();
		DataBuilder::BuildPushMessage( package, PUSH_MESSAGE_LEAVE_SESSION );
		package->addByteLenString( m_player_list->Get(index)->GetName(), XP_API_STRLEN( m_player_list->Get(index)->GetName() ) );
		package->packMessage();
		player->AddOutputData( package );
		XP_API_DEL( package );
	}

	// Send response message
	DataPacket * package = XP_API_NEW DefaultDataPacket();
	DataBuilder::BuildRequestSuccess( package, REQUEST_MESSAGE_LEAVE_SESSION );
	package->packMessage();
	m_player_list->Get(index)->AddOutputData( package );
	XP_API_DEL( package );

	// The connection will be removed after this frame
	m_player_list->Get(index)->SetAlive( false );

	return true;
}

bool COnlineGameRoom::HandleRequestFinishGame( int index, DataPacket * currentPacket )
{
	// Send push message to other users
	for (int i=0; i<m_player_list->Count(); i++)
	{
		if ( i == index )
		{
			continue;
		}

		COnlinePlayer * player = m_player_list->Get( i );

		if ( player == m_self )
		{
			continue;
		}

		DataPacket * package = XP_API_NEW DefaultDataPacket();
		DataBuilder::BuildPushMessage( package, PUSH_MESSAGE_FINISH_GAME );
		package->packMessage();
		player->AddOutputData( package );
		XP_API_DEL( package );
		package = NULL;
	}

	// Send response message
	DataPacket * package = XP_API_NEW DefaultDataPacket();
	DataBuilder::BuildRequestSuccess( package, REQUEST_MESSAGE_FINISH_GAME );
	package->packMessage();
	m_player_list->Get(index)->AddOutputData( package );
	XP_API_DEL( package );
	package = NULL;

	m_status = GAME_ROOM_STATUS_GAME_FINISH;

	return true;
}

bool COnlineGameRoom::HandleResponseData( int index, DataPacket * currentPacket )
{
	BYTE cmdtype = 0;

	if ( !currentPacket->getByte( cmdtype ) )
	{
		return true;
	}

	switch( cmdtype )
	{
		case REQUEST_MESSAGE_JOIN_SESSION:
		{
			HandleResponseJoinRoom( index, currentPacket );
		}
		break;

		case REQUEST_MESSAGE_LEAVE_SESSION:
		{
			HandleResponseLeaveRoom( index, currentPacket );
		}
		break;

		case REQUEST_MESSAGE_FINISH_GAME:
		{
			HandleResponseFinishGame( index, currentPacket );
		}
		break;

		default:
		{
			;
		}
		break;
	}

	return true;
}

bool COnlineGameRoom::HandleResponseJoinRoom( int index, DataPacket * currentPacket )
{
	BYTE result = 0;
	if ( !currentPacket->getByte( result ) )
	{
		XP_DEBUG_OUT("Msg error\n");
		m_error = ERROR_BAD_RESPONSE;
		return true;
	}

	if ( result == RESPONSE_MESSAGE_SUCCESS )
	{
		BYTE count = 0;
		currentPacket->getByte( count );

		if ( count <= 0 )
		{
			XP_DEBUG_OUT("Msg error\n");
			return true;
		}

		for ( int i=0; i<count; i++ )
		{
			char * name = NULL;
			BYTE len = 0;
			char * buffer = NULL;
			BYTE buffer_len = 0;
			int index_player = -1;
			DataPacket * packet = NULL;

			currentPacket->getByteLenString( name, len );
			index_player = IsPlayerExisted( name );
			currentPacket->getByteLenString( buffer, buffer_len );
			currentPacket->getPacket( packet );

			if ( index_player >= 0 )
			{
				COnlinePlayer * player = m_player_list->Get( index_player );
				if ( player != m_self )
				{
					player->SetNationality( buffer );
					player->SetData( packet );
				}
			}
			else
			{
				AddPlayer( name, buffer, packet, NULL, NULL, 0, 0, 0, 0 );
	
				COnlinePlayer * player_new = m_player_list->GetLastPlayer();

				player_new->SetAlive( true );
				player_new->SetConnected( false );
				player_new->SetJoined( true );
			}

			XP_API_DEL( name );
			XP_API_DEL( buffer );
			XP_API_DEL( packet );
		}
		
		m_host->SetAlive( true );
		m_host->SetJoined( true );
		m_host->SetConnected( false );
		m_self->SetJoined( true );
		m_status = GAME_ROOM_STATUS_ROOM;
		m_error = ERROR_NONE;
	}
	else
	{
		BYTE error_code = 0;
		if ( !currentPacket->getByte( error_code ) )
		{
			XP_DEBUG_OUT("Msg error\n");
			m_error = ERROR_BAD_RESPONSE;
			return true;
		}

		if ( error_code == ERROR_JOIN_SESSION_TOO_MANY_PLAYERS )
		{
			m_error = ERROR_MP_JOIN_ROOM_FULL;
		}
		else if ( error_code == ERROR_JOIN_SESSION_WRONG_PASSWORD )
		{
			m_error = ERROR_MP_JOIN_ROOM_PASSWORD;
		}
		else
		{
			m_error = ERROR_JOIN_GAME;
		}
	}
	
	return true;
}

bool COnlineGameRoom::HandleResponseLeaveRoom( int index, DataPacket * currentPacket )
{
	BYTE result = 0;
	if ( !currentPacket->getByte(result) )
	{
		XP_DEBUG_OUT("Msg error\n");
		m_error = ERROR_BAD_RESPONSE;
		return true;
	}

	if ( result == RESPONSE_MESSAGE_SUCCESS )
	{
		m_status = GAME_ROOM_STATUS_FOUND;
		m_error = ERROR_NONE;
	}
	else
	{
		m_error = ERROR_REQUEST_FAILED;
	}

	return true;
}

bool COnlineGameRoom::HandleResponseFinishGame( int index, DataPacket * currentPacket )
{
	m_status = GAME_ROOM_STATUS_GAME_FINISH;
	return true;
}

bool COnlineGameRoom::CanJoin()
{
	return ( m_player_list->GetPlayerCount() < m_max_player_count );
}

bool COnlineGameRoom::CanStart()
{
	if ( NULL == m_player_list || NULL == m_host || NULL == m_self )
	{
		return false;
	}

	if ( !AmITheHost() )
	{
		return false;
	}

	return ( m_player_list->GetPlayerCount() >= m_min_player_count );
}

void COnlineGameRoom::HandleConnection()
{
	if ( AmITheHost() )
	{
		// Check dead connection.
		for( int i=0; i<m_player_list->Count(); i++ )
		{
			COnlinePlayer * player = m_player_list->Get( i );
			if ( player == m_self )
			{
				continue;
			}

			if ( player->IsAlive() )
			{
				if ( !player->CheckAlive() )
				{
					player->SetAlive( false );
				}
			}
		}	

		// Remove dead connection
		bool handled = true;
		while( handled )
		{
			handled = false;
			for ( int i=0; i<m_player_list->Count(); i++ )
			{
				COnlinePlayer * player = m_player_list->Get( i );
				if ( player == m_self )
				{
					continue;
				}
				if ( !player->IsAlive() )
				{
					m_player_list->RemovePlayer( i );
					handled = true;
					break;
				}
			}
		}
	}
	else
	{
		if ( m_self->IsAlive() )
		{
			if ( !m_self->CheckAlive() )
			{
				m_self->SetAlive( false );
			}
		}

		if ( !m_self->IsAlive() )
		{
			XP_API_DEL( m_player_list );
			m_self = NULL;
			m_host = NULL;
			m_status = GAME_ROOM_STATUS_FOUND;
		}
	}

	if ( IsInGame() )
	{
		if ( NULL != m_player_list && m_player_list->Count() <= 1 )
		{
			XP_API_DEL( m_player_list );
			m_self = NULL;
			m_host = NULL;
			m_status = GAME_ROOM_STATUS_FOUND;
		}
	}
}

bool COnlineGameRoom::StartRoom()
{
	m_status = GAME_ROOM_STATUS_ROOM;
	m_host->SetAlive( true );
	m_host->SetConnected( true );
	m_host->SetJoined( true );
	return m_host->StartListen( m_max_player_count );
}

bool COnlineGameRoom::AmITheHost()
{
	if ( NULL == m_host || NULL == m_self )
	{
		return false;
	}

	return ( m_host == m_self );
}

bool COnlineGameRoom::SendToHost( DataPacket * data )
{
	bool bSuccessful = false;

	if ( data->IsSendByTcp() )
	{
		m_self->AddOutputData( data );
		bSuccessful = m_self->SendAllTCPData();		
	}

	if ( data->IsSendByUdp() )
	{
		m_self->AddOutputData( data );
		bSuccessful = m_self->SendAllUDPData( m_host );	
	}

	return bSuccessful;	
}

void COnlineGameRoom::HostLeave()
{
	m_status = GAME_ROOM_STATUS_FOUND;

	// Send push message to other users
	for (int i=0; i<m_player_list->Count(); i++)
	{
		COnlinePlayer * player = m_player_list->Get( i );
		if ( player == m_self )
		{
			continue;
		}

		if ( player->IsAlive() )
		{
			DataPacket * package = XP_API_NEW DefaultDataPacket();
			package->setDataType( DATA_PACKET_TYPE_SESSION );
			DataBuilder::BuildPushMessage( package, PUSH_MESSAGE_KICK_OUT );			
			package->addByte( ERROR_KICK_OUT_NO_SESSION );			
			package->packMessage();
			player->AddOutputData( package );
			player->SetAlive( false );
			XP_API_DEL( package );
		}
	}

	SendData();
	HandleConnection();

	return;	
}

//void COnlineGameRoom::HandlePing( GLXPlayerSocket * socket )
//{
//	if ( 0 == m_ping_starttime )
//	{
//		m_ping_starttime = SendPing( socket );
//	}
//	
//	//if ( 0 == m_ping_endtime )
//	//{
//	//	m_ping_endtime = ReceivePing( socket );
//	//}
//}

long COnlineGameRoom::SendPing( GLXPlayerSocket * socket, XIPAddress * ip_router )
{
	if ( socket->Select( GLXPlayerSocket::XSOCKET_SELECT_WRITE ) <= 0)
	{
		return 0;
	}

	char buffer[32] = { 0 };
	long timestamp = XP_API_GET_TIME();
	XP_API_SPRINTF( buffer, "%c|%s|%d", BASIC_MESSAGE_TYPE_PING, m_host->GetName(), timestamp );

	int count = 0;

	if ( XP_API_STRCMP( ip_router->getIP(), m_host->GetExternalIP() ) == 0 )
	{
		count = socket->SendTo( buffer, XP_API_STRLEN( buffer ), m_host->GetInternalIP(),
			m_host->GetInternalUdpPort() );
	}
	else
	{		
		count = socket->SendTo( buffer, XP_API_STRLEN( buffer ), m_host->GetExternalIP(),
			m_host->GetExternalUdpPort() );			
	}

	if ( count > 0 )
	{
		m_ping_starttime = timestamp;	
	}
	else
	{
		m_ping_starttime = -1;
	}

	return m_ping_starttime;
}

//long COnlineGameRoom::ReceivePing( GLXPlayerSocket * socket )
//{
//	
//}

long COnlineGameRoom::GetPingTime()
{
	if ( 0 == m_ping_starttime || 0 == m_ping_endtime )
	{
		return 0;
	}

	if ( m_ping_starttime < 0 || m_ping_endtime < 0 )
	{
		return -1;
	}

	return ( m_ping_endtime - m_ping_starttime );
}

bool COnlineGameRoom::JoinRoom( char * name, char * country, DataPacket * data, 
	char * exter_ip, char * inter_ip, int exter_tcp_port, int exter_udp_port,
	int inter_tcp_port, int inter_udp_port, OnlinePlayerType type )
{
	if(m_joinRoomState == JOINSTATE_START)
	{
		//if ( !AddPlayer( name, country, data, exter_ip, inter_ip, exter_tcp_port, exter_udp_port,
		//	inter_tcp_port, inter_udp_port ) )
		//{
		//	m_joinRoomState = JOINSTATE_FAILED;
		//	return false;
		//}

	
		InitSetMyself();
		m_joinRoomState = JOINSTATE_PERPARE;
		return false;
	}
	else if(m_joinRoomState==JOINSTATE_PERPARE)
	{
		if(	SetMyself( m_player_list->Count() - 1, type ))
		{
			m_joinRoomState = JOINSTATE_WAITING;
		}
	
		return false;
	}
	else if(m_joinRoomState == JOINSTATE_WAITING)
	{
		if ( !m_self->Connect( m_host ) )
		{
			if(m_self->GetConnectState() == GLXPlayerSocket::NONBLOCK_CONNECT_STATE_WAITING)
			{
				return false;
			}
			else
			{
				m_host->SetConnected( false );
				m_joinRoomState = JOINSTATE_FAILED;
				m_self->SetConnectState(GLXPlayerSocket::NONBLOCK_CONNECT_STATE_START);
				return false;
			}
		}
		else
		{
			m_host->SetConnected( true );

			XP_DEBUG_OUT( "[[[[ Join Room = %s %d %d %s %d %d %s ]]]]\n", 
				m_self->GetInternalIP(), m_self->GetInternalTcpPort(), m_self->GetInternalUdpPort(),
				m_self->GetExternalIP(), m_self->GetExternalTcpPort(), m_self->GetExternalUdpPort(), country );

			// Prepare packet data
			DataPacket* packet = XP_API_NEW DefaultDataPacket();
			DataBuilder::BuildGameMessage( packet, REQUEST_MESSAGE_JOIN_SESSION );
			packet->setDataType( DATA_PACKET_TYPE_SESSION );
			packet->addByteLenString( name, XP_API_STRLEN( name ) );
			packet->addInt( XP_API_IP_STR2INT( m_self->GetInternalIP() ) );
			packet->addShort( m_self->GetInternalTcpPort() );
			packet->addShort( m_self->GetInternalUdpPort() );
			packet->addInt( XP_API_IP_STR2INT( m_self->GetExternalIP() ) );
			packet->addShort( m_self->GetExternalTcpPort() );
			packet->addShort( m_self->GetExternalUdpPort() );
			packet->addByteLenString( country, XP_API_STRLEN( country ) );
			if ( NULL != m_room_password )
			{
				packet->addByteLenString( m_room_password, XP_API_STRLEN( m_room_password ) );
			}
			else
			{
				packet->addByteLenString( " ", 1 );
			}
			//packet->addPacket( data );
			packet->addPacket( m_self->GetData() );
			packet->packMessage();

			// Send join session package
			m_self->AddOutputData( packet );
			XP_API_DEL( packet );

			if ( !m_self->SendAllTCPData() )
			{
				XP_DEBUG_OUT( "Write failed!!!!!\n" );
				XP_API_DEL( packet );
				packet = NULL;
				m_joinRoomState = JOINSTATE_FAILED;
				m_self->SetConnectState(GLXPlayerSocket::NONBLOCK_CONNECT_STATE_START);
				return false;
			}

			m_error = ERROR_PENDING;

			m_joinRoomState = JOINSTATE_SUCCESS;
			m_self->SetConnectState(GLXPlayerSocket::NONBLOCK_CONNECT_STATE_START);

			return true;
		}
	}
	return true;
}

int COnlineGameRoom::IsPlayerExisted( char * name )
{
	if ( NULL == name )
	{
		return -1;
	}

	if ( NULL == m_player_list )
	{
		return -1;
	}

	return m_player_list->GetPlayerIndex( name );
}

bool COnlineGameRoom::KickoutPlayer( char * name )
{
	int index = IsPlayerExisted( name );

	if ( index < 0 )
	{
		return false;
	}

	for ( int i=0; i<m_player_list->Count(); i++ )
	{
		COnlinePlayer * player = m_player_list->Get( i );

		if ( player == m_host )
		{
			continue;
		}

		// Send kickout push message
		DataPacket * package = XP_API_NEW DefaultDataPacket();
		DataBuilder::BuildPushMessage( package, PUSH_MESSAGE_KICK_OUT );
		package->addByte( ERROR_KICK_OUT_KICKED );
		package->addByteLenString( name, XP_API_STRLEN( name ) );
		package->packMessage();
		player->AddOutputData( package );
		XP_API_DEL( package );

		if ( index == i )
		{
			player->SetAlive( false );
		}
	}

	// Send data
	SendData();

	// Remove dead connection
	HandleConnection();

	return true;
}

void COnlineGameRoom::KeepAlive()
{
	if ( AmITheHost() )
	{
		for ( int i=0; i<m_player_list->Count(); i++ )
		{
			COnlinePlayer * player = m_player_list->Get( i );

			if ( player == m_self )
			{
				continue;
			}

			player->KeepAlive();
		}
	}
	else
	{
		m_self->KeepAlive();
	}
}

bool COnlineGameRoom::LeaveRoom()
{
	if ( AmITheHost() )
	{
		return false;
	}

	DataPacket* packet = XP_API_NEW DefaultDataPacket();
	DataBuilder::BuildGameMessage( packet, REQUEST_MESSAGE_LEAVE_SESSION );
	packet->packMessage();
	SendToHost( packet );
	XP_API_DEL( packet );
	packet = NULL;
	return true;
}

bool COnlineGameRoom::StartGame()
{
	if ( !AmITheHost() )
	{
		return false;
	}

	m_status = GAME_ROOM_STATUS_GAME;

	// Send push message to other users
	for (int i=0; i<m_player_list->Count(); i++)
	{
		COnlinePlayer * player = m_player_list->Get( i );
		if ( player == m_self )
		{
			continue;
		}

		DataPacket * package = XP_API_NEW DefaultDataPacket();
		DataBuilder::BuildPushMessage( package, PUSH_MESSAGE_START_GAME );
		package->packMessage();
		player->AddOutputData( package );
		XP_API_DEL( package );
	}

	SendData();

	return true;
}

bool COnlineGameRoom::FinishGame()
{
	if ( AmITheHost() )
	{
		m_status = GAME_ROOM_STATUS_GAME_FINISH;

		// Send push message to other users
		for (int i=0; i<m_player_list->Count(); i++)
		{
			COnlinePlayer * player = m_player_list->Get( i );
			if ( player == m_self )
			{
				continue;
			}

			DataPacket * package = XP_API_NEW DefaultDataPacket();
			DataBuilder::BuildPushMessage( package, PUSH_MESSAGE_FINISH_GAME );
			package->packMessage();
			player->AddOutputData( package );
			XP_API_DEL( package );
		}

		SendData();
	}
	else
	{
		DataPacket * package = XP_API_NEW DefaultDataPacket();
		DataBuilder::BuildGameMessage( package, REQUEST_MESSAGE_FINISH_GAME );
		package->packMessage();
		SendToHost( package );
		XP_API_DEL( package );
		package = NULL;
	}

	return true;
}

bool COnlineGameRoom::FinishGameConfirm(bool isInConfirmStatus)
{
	if ( AmITheHost() )
	{
		// Send push message to other users
		for (int i=0; i<m_player_list->Count(); i++)
		{
			COnlinePlayer * player = m_player_list->Get( i );
			if ( player == m_self )
			{
				continue;
			}

			DataPacket * package = XP_API_NEW DefaultDataPacket();
			DataBuilder::BuildPushMessage( package, isInConfirmStatus? PUSH_MESSAGE_FINISH_GAME_CONFIRM: PUSH_MESSAGE_FINISH_GAME_RESTART );
			package->packMessage();
			player->AddOutputData( package );
			XP_API_DEL( package );
		}

		SendData();
	}

	return true;
}

bool COnlineGameRoom::CheckGameData( char * &name, DataPacket * &packet )
{
	if ( AmITheHost() )
	{
		for ( int i=0; i<m_player_list->Count(); i++ )
		{
			COnlinePlayer * player = m_player_list->Get( i );

			if ( player == m_self )
			{
				continue;
			}

			if ( player->CheckGameData( name, packet ) )
			{
				return true;
			}
		}

		return m_self->CheckGameData( name, packet );
	}
	else
	{
		return m_self->CheckGameData( name, packet );
	}

	return false;
}

bool COnlineGameRoom::SendGameData( DataPacket * game_data, bool bReliable )
{
	DataPacket * package = XP_API_NEW DefaultDataPacket();
	DataBuilder::BuildGameDataMessage( package, m_self->GetName(), game_data, bReliable );
	package->packMessage();

	if ( AmITheHost() )
	{
		// Send message to other users
		for (int i=0; i<m_player_list->Count(); i++)
		{
			COnlinePlayer * player = m_player_list->Get( i );
			if ( player == m_self )
			{
				continue;
			}
			
			if ( package->IsSendByTcp() )
			{
				player->AddOutputData( package );
			}
		}

		if ( package->IsSendByUdp() )
		{
			m_self->AddOutputData( package );
		}

		SendData();
	}
	else
	{
		SendToHost( package );
	}

	XP_API_DEL( package );
	package = NULL;
	
	return true;
}

void COnlineGameRoom::SetPassword( char * password )
{
	if ( NULL != password && XP_API_STRLEN( password ) > 0 )
	{
		XP_API_DEL( m_room_password );
		m_room_password = XP_API_STRNEW( password );
	}
}


bool COnlineGameRoom::HasPasswordSet( )
{
return (m_room_password==NULL);
}
bool COnlineGameRoom::StartLocalRoom()
{
	m_status = GAME_ROOM_STATUS_ROOM;
	m_host->SetAlive( true );
	m_host->SetConnected( true );
	m_host->SetJoined( true );
	return m_host->StartLocalRoom( m_max_player_count );
}

void COnlineGameRoom::SetGameFinishConfirmStatus(bool flag)
{
	m_isFinishConfirm = flag;
}

void COnlineGameRoom::OnAddPortMappingSuccess(int type, int portMapped)
{
	m_self->OnAddPortMappingSuccess(type,portMapped);

}


#endif
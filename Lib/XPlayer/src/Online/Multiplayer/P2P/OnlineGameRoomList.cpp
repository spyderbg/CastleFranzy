#include "../../../../include/Multiplayer/P2P/OnlineGameRoomList.h"

#ifdef ENABLE_MULTIPLAYER

#define ONLINE_GAME_ROOM_ONCE		10

COnlineGameRoomList::COnlineGameRoomList(void)
{
	m_list = XP_API_NEW COnlineGameRoom * [ONLINE_GAME_ROOM_ONCE];
	m_count = 0;
}

COnlineGameRoomList::~COnlineGameRoomList(void)
{
	for ( int i=0; i<m_count; i++ )
	{
		XP_API_DEL( m_list[i] );
		m_list[i] = NULL;
	}

	XP_API_DEL( m_list );
}

COnlineGameRoomList::COnlineGameRoomList( DataPacket * currentPacket )
{
	m_list = NULL;
	m_count = 0;

	BYTE room_count = 0;
	currentPacket->getByte( room_count );

	if ( room_count <= 0 )
	{
		return;
	}

	int index = 0;
	currentPacket->getInt( index );

	m_count = room_count;
	m_list = XP_API_NEW COnlineGameRoom * [ONLINE_GAME_ROOM_ONCE];

	for ( int i=0; i<m_count; i++ )
	{
		char * buffer = NULL;
		BYTE len = 0;
		BYTE player_count = 0;
		
		m_list[i] = XP_API_NEW COnlineGameRoom();
		m_list[i]->SetIndex( i );

		// Name
		currentPacket->getByteLenString( buffer, len );
		m_list[i]->SetName( buffer );
		XP_API_DEL( buffer );

		// The player number of the room
		currentPacket->getByte( player_count );
		m_list[i]->SetPlayerCount( player_count );

		XP_API_DEL( buffer );
	}

	for ( int i=0; i<m_count; i++ )
	{
		if( !SaveRoomData( currentPacket, i ) )
		{
			XP_DEBUG_OUT( "COnlineGameRoomList construct error!!!\n" );
			break;
		}
	}
}

bool COnlineGameRoomList::SaveRoomData( DataPacket * currentPacket, int index )
{
	char * buffer = NULL;
	BYTE len = 0;

	if( !currentPacket->getByteLenString( buffer, len ) )
	{
		return false;
	}

	DefaultDataPacket * packet = XP_API_NEW DefaultDataPacket();
	packet->setMessageBody( (BYTE *)buffer, len );
	XP_API_DEL( buffer );

	int local_ip = 0;
	int router_ip = 0;
	short local_tcp_port = 0;
	short local_udp_port = 0;
	short router_tcp_port = 0;
	short router_udp_port = 0;
	char * nationality = NULL;
	BYTE nationlity_len = 0;
	BYTE password = 0;
	char * room_data = NULL;
	BYTE room_data_len = 0;

	if( !packet->getInt( local_ip ) )
	{
		XP_API_DEL( packet );
		return false;
	}

	if( !packet->getShort( local_tcp_port ) )
	{
		XP_API_DEL( packet );
		return false;		
	}

	if ( !packet->getShort( local_udp_port ) )
	{
		XP_API_DEL( packet );
		return false;	
	}

	if( !packet->getInt( router_ip ) )
	{
		XP_API_DEL( packet );
		return false;	
	}

	if( !packet->getShort( router_tcp_port ) )
	{
		XP_API_DEL( packet );
		return false;	
	}

	if( !packet->getShort( router_udp_port ) )
	{
		XP_API_DEL( packet );
		return false;	
	}

	if( !packet->getByteLenString( nationality, nationlity_len ) )
	{
		XP_API_DEL( packet );
		return false;
	}

	if( !packet->getByte( password ) )
	{
		XP_API_DEL( packet );
		return false;		
	}

	packet->getByteLenString( room_data, room_data_len );
	XP_API_DEL( packet );

	char ip_internal[32] = { 0 };
	char ip_external[32] = { 0 };
	
	XP_API_IP_INT2STR( local_ip, ip_internal );
	XP_API_IP_INT2STR( router_ip, ip_external );
	unsigned short port_local_tcp = local_tcp_port;
	unsigned short port_local_udp = local_udp_port;
	unsigned short port_router_tcp = router_tcp_port;
	unsigned short port_router_udp = router_udp_port;

	if ( NULL == room_data || 0 == room_data_len )
	{
		m_list[index]->SetData( NULL );
	}
	else
	{
		DefaultDataPacket * room_data_packet = XP_API_NEW DefaultDataPacket();
		room_data_packet->setMessageBody( (BYTE *)room_data, room_data_len );
		m_list[index]->SetData( room_data_packet );
		XP_API_DEL( room_data );
		XP_API_DEL( room_data_packet );
	}

	m_list[index]->SetPasswordFlag( password == 0x1 );

	if ( !m_list[index]->AddPlayer( m_list[index]->GetName(), nationality, NULL, 
		ip_external, ip_internal, 
		port_router_tcp, port_router_udp, port_local_tcp, port_local_udp ) )
	{
		XP_API_DEL( nationality );
		return false;
	}

	XP_API_DEL( nationality );
	m_list[index]->SetHost( 0 );
	return true;
}

COnlineGameRoom * COnlineGameRoomList::Get( int index )
{
	if ( index >= m_count )
	{
		return NULL;
	}

	return m_list[index];	
}

COnlineGameRoom * COnlineGameRoomList::Get( char * name )
{
	if ( 0 == m_count )
	{
		return NULL;
	}
	
	for (int i=0; i<m_count; i++)
	{
		if ( XP_API_STRCMP( name, m_list[i]->GetName() ) == 0 )
		{
			return m_list[i];
		}
	}
	
	return NULL;
}

bool COnlineGameRoomList::ClearAllLostRoom( long time )
{
	bool bHasLost = ClearOneLostRoom( time );
	while( bHasLost )
	{
		bHasLost = ClearOneLostRoom( time );
	}
	return true;
}

bool COnlineGameRoomList::ClearOneLostRoom( long time )
{
	int index = 0;
	long curr_time = XP_API_GET_TIME();
	
	if ( m_count == 0 )
	{
		return false;
	}
	
	for (index=0; index<m_count; index++)
	{
		if ( ( curr_time - m_list[index]->GetFoundTime() ) >= time )
		{
			break;
		}
	}
	
	if ( index == m_count )
	{
		return false;
	}
	
	XP_API_DEL( m_list[index] );
	for ( int i=index; i<m_count-1; i++ )
	{
		m_list[i] = m_list[i+1];
	}
	m_list[m_count-1] = NULL;
	m_count--;
	
	return true;
}
bool COnlineGameRoomList::AddRoom(DataPacket * currentPacket, long time)
{
	if(m_count >=ONLINE_GAME_ROOM_ONCE)
	{
		return false;
	}

	char * buffer = NULL;
	BYTE len = 0;
	BYTE player_count = 0;

	// Name
	currentPacket->getByteLenString( buffer, len );
	
	COnlineGameRoom * room = Get( buffer );
	if ( NULL != room )
	{
		XP_API_DEL( buffer );
		room->SetFoundTime(time);
		XP_DEBUG_OUT("AddRoom: Existed!!!");
		return false;
	}
	
	m_list[m_count] = XP_API_NEW COnlineGameRoom();
	m_list[m_count]->SetIndex(m_count);
	m_list[m_count]->SetFoundTime(time);
	m_list[m_count]->SetName( buffer );
	XP_API_DEL( buffer );

	// The player number of the room
	currentPacket->getByte( player_count );
	m_list[m_count]->SetPlayerCount( player_count );

	SaveRoomData( currentPacket, m_count );

	m_count++;

	return true;
}

#endif
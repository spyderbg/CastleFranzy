#include "../../../../include/Multiplayer/P2P/OnlinePlayerList.h"

#ifdef ENABLE_MULTIPLAYER

#define ONLINE_PLAYER_LIST_ONCE		10

COnlinePlayerList::COnlinePlayerList(void)
{
	m_list = XP_API_NEW COnlinePlayer * [ONLINE_PLAYER_LIST_ONCE];
	m_count = 0;
}

COnlinePlayerList::~COnlinePlayerList(void)
{
	for ( int i=0; i<m_count; i++ )
	{
		XP_API_DEL( m_list[i] );
		m_list[i] = NULL;
	}

	XP_API_DEL( m_list );
}

COnlinePlayer * COnlinePlayerList::AddPlayer( char * name, char * country, DataPacket * data, 
	char * exter_ip, char * inter_ip, int exter_tcp_port, int exter_udp_port,
	int inter_tcp_port, int inter_udp_port )
{	
	if ( m_count == ONLINE_PLAYER_LIST_ONCE )
	{
		return NULL;
	}

	m_list[m_count] = XP_API_NEW COnlinePlayer();
	m_list[m_count]->SetIndex( m_count );

	if ( NULL != name )
	{
		m_list[m_count]->SetName( name );
	}

	if ( NULL != country )
	{
		m_list[m_count]->SetNationality( country );
	}

	if ( NULL != data )
	{
		m_list[m_count]->SetData( data );
	}

	if ( NULL != exter_ip )
	{
		char buffer[64] = { 0 };

		XP_API_MEMSET( buffer, 0, 64 );
		XP_API_SPRINTF( buffer, "%s:%d", exter_ip, exter_tcp_port );
		m_list[m_count]->SetExternalTcpAddress( buffer );

		XP_API_MEMSET( buffer, 0, 64 );
		XP_API_SPRINTF( buffer, "%s:%d", exter_ip, exter_udp_port );
		m_list[m_count]->SetExternalUdpAddress( buffer );
	}

	if ( NULL != inter_ip )
	{
		char buffer[64] = { 0 };

		XP_API_MEMSET( buffer, 0, 64 );
		XP_API_SPRINTF( buffer, "%s:%d", inter_ip, inter_tcp_port );
		m_list[m_count]->SetInternalTcpAddress( buffer );

		XP_API_MEMSET( buffer, 0, 64 );
		XP_API_SPRINTF( buffer, "%s:%d", inter_ip, inter_udp_port );
		m_list[m_count]->SetInternalUdpAddress( buffer );
	}

	m_count++;
	return m_list[m_count-1];
}

COnlinePlayer * COnlinePlayerList::Get( int index )
{
	if ( index >= m_count )
	{
		return NULL;
	}

	return m_list[index];
}

bool COnlinePlayerList::RemovePlayer( int index )
{
	if ( index >= m_count )
	{
		return false;
	}

	XP_API_DEL( m_list[index] );
	m_list[index] = NULL;
	for ( int i=index; i<m_count-1; i++ )
	{
		m_list[i] = m_list[i+1];
	}
	m_list[m_count-1] = NULL;
	m_count--;
	return true;
}

bool COnlinePlayerList::RemovePlayer( char * name )
{
	int index = GetPlayerIndex( name );

	if ( index < 0 )
	{
		return false;
	}

	RemovePlayer( index );

	return true;
}

int COnlinePlayerList::GetPlayerIndex( char * name )
{
	if ( m_count == 0 )
	{
		return -1;
	}

	for ( int i=0; i<m_count; i++ )
	{
		COnlinePlayer * player = m_list[i];
		if ( NULL != player->GetName() && XP_API_STRCMP( player->GetName(), name ) == 0 )
		{
			return i;
		}
	}

	return -1;
	
}

COnlinePlayer * COnlinePlayerList::GetLastPlayer()
{
	if ( m_count == 0 )
	{
		return NULL;
	}

	return m_list[m_count-1];
}

int COnlinePlayerList::GetPlayerCount()
{
	int count = 0;

	for ( int i=0; i<m_count; i++ )
	{
		COnlinePlayer * player = m_list[i];
		if ( NULL != player && player->IsAlive() && player->IsJoined() )
		{
			count++;
		}
	}

	return count;
}

COnlinePlayer * COnlinePlayerList::GetPlayer( int index )
{
	int count = 0;

	for ( int i=0; i<m_count; i++ )
	{
		COnlinePlayer * player = m_list[i];
		if ( NULL != player && player->IsAlive() && player->IsJoined() )
		{
			count++;
		}

		if ( count > index )
		{
			return player;
		}
	}

	return NULL;	
}

#endif
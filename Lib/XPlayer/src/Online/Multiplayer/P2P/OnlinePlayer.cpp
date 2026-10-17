#include "../../../../include/Multiplayer/P2P/OnlinePlayer.h"
#include "../../../../include/Game_defines.h"
#include "../../../../include/Multiplayer/Common/DataBuilder.h"
#include "upnp/PortMapper.h"

#ifdef ENABLE_MULTIPLAYER

COnlinePlayer::COnlinePlayer(void)
{
	m_index = 0;
	m_name = NULL;
	m_nationality = NULL;
	m_data = NULL;
	m_external_tcp_address = NULL;
	m_internal_tcp_address = NULL;
	m_external_udp_address = NULL;
	m_internal_udp_address = NULL;
	m_last_send_time = 0;
	m_last_recv_time = 0;
	m_tcp_socket = NULL;
	m_udp_socket = NULL;
	m_outcoming_packet = NULL;
	m_incoming_packet = NULL;
	m_bAlive = true;
	m_bConnected = false;
	m_bJoined = false;
	m_protocol = ONLINE_PROTOCOL_NIL;
	XP_API_MEMSET( m_tcp_buffer, 0, ONLINE_MAX_PACKET_SIZE );
	m_tcp_position = 0;
	XP_API_MEMSET( m_udp_buffer, 0, ONLINE_MAX_PACKET_SIZE );
	m_udp_position = 0;
	m_upnpInitState=UPNP_INIT_TCPNONE;
	m_tcpIport=0;
	m_udpIport=0;
}

COnlinePlayer::~COnlinePlayer(void)
{
	XP_API_DEL( m_name );
	XP_API_DEL( m_nationality );
	XP_API_DEL( m_data );
	XP_API_DEL( m_external_tcp_address );
	XP_API_DEL( m_internal_tcp_address );
	XP_API_DEL( m_external_udp_address );
	XP_API_DEL( m_internal_udp_address );
	XP_API_DEL( m_tcp_socket );
	XP_API_DEL( m_udp_socket );
	XP_API_DEL( m_outcoming_packet );
	RemoveIncomingPacket();
	m_incoming_packet = NULL;
}

char * COnlinePlayer::GetName()
{
	return m_name;
}

void COnlinePlayer::SetName( char * name )
{
	XP_API_DEL( m_name );
	m_name = XP_API_STRNEW( name );
}

char * COnlinePlayer::GetNationality()
{
	return m_nationality;
}

void COnlinePlayer::SetNationality( char * name )
{
	XP_API_DEL( m_nationality );
	m_nationality = XP_API_STRNEW( name );
}

bool COnlinePlayer::SetExternalTcpAddress( char * ip_data )
{
	if ( NULL == ip_data || XP_API_STRLEN( ip_data ) == 0 )
	{
		return false;
	}

	XP_API_DEL( m_external_tcp_address );
	m_external_tcp_address = XP_API_NEW XIPAddress( ip_data );
	return ( m_external_tcp_address != NULL );
}

bool COnlinePlayer::SetExternalUdpAddress( char * ip_data )
{
	if ( NULL == ip_data || XP_API_STRLEN( ip_data ) == 0 )
	{
		return false;
	}

	XP_API_DEL( m_external_udp_address );
	m_external_udp_address = XP_API_NEW XIPAddress( ip_data );
	return ( m_external_udp_address != NULL );
}

char * COnlinePlayer::GetExternalIP()
{
	if ( NULL == m_external_tcp_address )
	{
		return NULL;
	}

	return m_external_tcp_address->getIP();
}

int COnlinePlayer::GetExternalTcpPort()
{
	if ( NULL == m_external_tcp_address )
	{
		return 0;
	}

	return m_external_tcp_address->getTcpPort();
}

int COnlinePlayer::GetExternalUdpPort()
{
	if ( NULL == m_external_udp_address )
	{
		return 0;
	}

	return m_external_udp_address->getUdpPort();
}

bool COnlinePlayer::SetInternalTcpAddress( char * ip_data )
{
	if ( NULL == ip_data || XP_API_STRLEN( ip_data ) == 0 )
	{
		return false;
	}

	XP_API_DEL( m_internal_tcp_address );
	m_internal_tcp_address = XP_API_NEW XIPAddress( ip_data );
	return ( m_internal_tcp_address != NULL );
}

bool COnlinePlayer::SetInternalUdpAddress( char * ip_data )
{
	if ( NULL == ip_data || XP_API_STRLEN( ip_data ) == 0 )
	{
		return false;
	}

	XP_API_DEL( m_internal_udp_address );
	m_internal_udp_address = XP_API_NEW XIPAddress( ip_data );
	return ( m_internal_udp_address != NULL );
}

char * COnlinePlayer::GetInternalIP()
{
	if ( NULL == m_internal_tcp_address )
	{
		return NULL;
	}

	return m_internal_tcp_address->getIP();
}

int COnlinePlayer::GetInternalTcpPort()
{
	if ( NULL == m_internal_tcp_address )
	{
		return 0;
	}

	return m_internal_tcp_address->getTcpPort();
}

int COnlinePlayer::GetInternalUdpPort()
{
	if ( NULL == m_internal_udp_address )
	{
		return 0;
	}

	return m_internal_udp_address->getUdpPort();
}

void COnlinePlayer::SetTcpSocket( GLXPlayerSocket * socket )
{
	XP_API_DEL( m_tcp_socket );
	m_tcp_socket = socket;
}

void COnlinePlayer::RemoveIncomingPacket()
{
	DataPacket * temp = NULL;
	while( NULL != m_incoming_packet )
	{
		temp = m_incoming_packet;
		// move to next
		m_incoming_packet = m_incoming_packet->getNextPacket();
		// delete current packet
		XP_API_DEL( temp );
		temp = NULL;
	}
}

void COnlinePlayer::RemoveOutcomingPacket()
{
	DataPacket * temp = NULL;
	while( NULL != m_outcoming_packet )
	{
		temp = m_outcoming_packet;
		// move to next
		m_outcoming_packet = m_outcoming_packet->getNextPacket();
		// delete current packet
		XP_API_DEL( temp );
		temp = NULL;
	}
}

bool COnlinePlayer::ReceiveData()
{
	// If receive data from tcp connection succesfully, do not receive data from udp.
	// Next frame will try to receive data from udp.
	ReceiveTCPData();
	ReceiveUDPData();
	return true;
}

int COnlinePlayer::AddIncomingDataPacket( char * buffer, int length, OnlineProtocol protocol,
										 char * ip, int port )
{
	int position = 0;

	DataPacket * packet = XP_API_NEW DefaultDataPacket();
	packet->setDataType( protocol );
	position = packet->String2Packet( buffer, length );

	if ( position < 0 )
	{
		XP_API_DEL( packet );
		return -1;
	}

	if ( protocol == ONLINE_PROTOCOL_UDP )
	{
		packet->setIP( ip );
		packet->setPort( port );
	}

	// Construct the data to DataPacket.
	if ( NULL == m_incoming_packet )
	{
		m_incoming_packet = packet;
	}
	else
	{
		DataPacket * tail = m_incoming_packet;
		while( NULL != tail->getNextPacket() )
		{
			tail = tail->getNextPacket();
		}
		tail->setNextPacket( packet );
	}

	return position;
}

int COnlinePlayer::AddIncomingData( char * buffer, int length, OnlineProtocol protocol,
								   char * ip, int port )
{
	int totalpos = 0;
	int position = AddIncomingDataPacket( buffer, length, protocol, ip, port );

	while( position > 0 )
	{
		totalpos += position;

		if ( totalpos >= length )
		{
			break;
		}

		position = AddIncomingDataPacket( buffer+totalpos, length-totalpos, protocol, ip , port );
	}

	return totalpos;
}

bool COnlinePlayer::ReceiveTCPData()
{
	if ( NULL == m_tcp_socket )
	{
		return false;
	}

	if ( m_tcp_socket->Select( GLXPlayerSocket::XSOCKET_SELECT_READ ) <= 0 )
	{
		return false;
	}

	int size = m_tcp_socket->Recv( m_tcp_buffer+m_tcp_position, ONLINE_MAX_PACKET_SIZE - m_tcp_position );

	if ( size < 0 )
	{
		return false;
	}

	if ( size == 0 )
	{
		SetAlive( false );
		return false;
	}

	m_last_recv_time = XP_API_GET_TIME();

	int length = m_tcp_position + size;
	int pos = AddIncomingData( m_tcp_buffer, length, ONLINE_PROTOCOL_TCP );

	if ( pos < length )
	{
		XP_API_MEMCPY( m_tcp_buffer, m_tcp_buffer + pos, length - pos );
		m_tcp_position = length - pos;
		XP_API_MEMSET( m_tcp_buffer+m_tcp_position, 0, ONLINE_MAX_PACKET_SIZE-m_tcp_position );
	}
	else
	{
		XP_API_MEMSET( m_tcp_buffer, 0, ONLINE_MAX_PACKET_SIZE );
		m_tcp_position = 0;
	}

	return true;
}

bool COnlinePlayer::ReceiveUDPData( COnlinePlayer * sender )
{
	if ( NULL == m_udp_socket )
	{
		return false;	
	}

	if ( m_udp_socket->Select( GLXPlayerSocket::XSOCKET_SELECT_READ ) <= 0 )
	{
		return false;
	}

	char * ip = NULL;
	int port = 0;

	if ( XP_API_STRCMP( GetExternalIP(), sender->GetExternalIP() ) == 0 )
	{
		ip = XP_API_STRNEW( sender->GetInternalIP() );
		port = sender->GetInternalUdpPort();
	}
	else
	{
		ip = XP_API_STRNEW( sender->GetExternalIP() );
		port = sender->GetExternalUdpPort();		
	}

	int size = m_udp_socket->RecvFrom( m_udp_buffer+m_udp_position, ONLINE_MAX_PACKET_SIZE-m_udp_position, ip, port );

	if ( size <= 0 )
	{
		XP_API_DEL( ip );
		return false;
	}

	m_last_recv_time = XP_API_GET_TIME();

	int length = m_udp_position + size;
	int pos = AddIncomingData( m_udp_buffer, length, ONLINE_PROTOCOL_UDP, ip, port );

	if ( pos < length )
	{
		XP_API_MEMCPY( m_udp_buffer, m_udp_buffer + pos, length - pos );
		m_udp_position = length - pos;
		XP_API_MEMSET( m_udp_buffer+m_udp_position, 0, ONLINE_MAX_PACKET_SIZE-m_udp_position );
	}
	else
	{
		XP_API_MEMSET( m_udp_buffer, 0, ONLINE_MAX_PACKET_SIZE );
		m_udp_position = 0;
	}

	XP_API_DEL( ip );

	return true;	
}

bool COnlinePlayer::ReceiveUDPData()
{
	if ( NULL == m_udp_socket )
	{
		return false;
	}

	if ( m_udp_socket->Select( GLXPlayerSocket::XSOCKET_SELECT_READ ) <= 0 )
	{
		return false;
	}

	char * ip = NULL;
	int port;
	int size = m_udp_socket->RecvFromUnkownIP( m_udp_buffer+m_udp_position, ONLINE_MAX_PACKET_SIZE-m_udp_position, &ip, &port );

	if ( size <= 0 )
	{
		XP_API_DEL( ip );
		return false;
	}

	m_last_recv_time = XP_API_GET_TIME();

	int length = m_udp_position + size;
	int pos = AddIncomingData( m_udp_buffer, length, ONLINE_PROTOCOL_UDP, ip, port );

	if ( pos < length )
	{
		XP_API_MEMCPY( m_udp_buffer, m_udp_buffer + pos, length - pos );
		m_udp_position = length - pos;
		XP_API_MEMSET( m_udp_buffer+m_udp_position, 0, ONLINE_MAX_PACKET_SIZE-m_udp_position );
	}
	else
	{
		XP_API_MEMSET( m_udp_buffer, 0, ONLINE_MAX_PACKET_SIZE );
		m_udp_position = 0;
	}

	XP_API_DEL( ip );

	return true;
}

GLXPlayerSocket * COnlinePlayer::Accept()
{
	if ( NULL == m_tcp_socket )
	{
		return NULL;
	}

	int iResult = m_tcp_socket->Select( GLXPlayerSocket::XSOCKET_SELECT_READ );

	if ( iResult <= 0 )
	{
		return NULL;
	}

	XP_DEBUG_OUT( "COnlinePlayer::Accept() \n" );

	return m_tcp_socket->Accept();
}

bool COnlinePlayer::AddOutputData( DataPacket * packet )
{
	if ( NULL == m_outcoming_packet )
	{
		m_outcoming_packet = XP_API_NEW DefaultDataPacket( packet );
	}
	else
	{
		XP_DEBUG_OUT( "COnlinePlayer::AddOutputData() ====================================================================== \n" );
		DataPacket * tail = m_outcoming_packet;
		while( NULL != tail->getNextPacket() )
		{
			tail = tail->getNextPacket();
		}
		DataPacket * packet_new = XP_API_NEW DefaultDataPacket( packet );
		tail->setNextPacket( packet_new );
	}

	return true;
}

bool COnlinePlayer::SendAllData( COnlinePlayer * receiver )
{
	if ( NULL == m_outcoming_packet )
	{
		return false;
	}

	while( NULL != m_outcoming_packet )
	{
		DataPacket * packet = m_outcoming_packet;
		if ( packet->IsSendByTcp() )
		{
			SendTCPDataPacket( packet );
		}
		else if ( packet->IsSendByUdp() )
		{
			SendUDPDataPacket( receiver, packet );
		}
		else
		{
			XP_DEBUG_OUT( "************************* Unknown Data Type *************************\n" );
		}
		m_outcoming_packet = m_outcoming_packet->getNextPacket();
		XP_API_DEL( packet );
		packet = NULL;
	}

	return true;
}

bool COnlinePlayer::SendAllTCPData()
{
	if ( NULL == m_outcoming_packet )
	{
		return false;
	}

	if ( NULL == m_tcp_socket )
	{
		return false;
	}

	if ( !m_tcp_socket->Select( GLXPlayerSocket::XSOCKET_SELECT_WRITE ) )
	{
		XP_DEBUG_OUT( "************************* SendAllTCPData Failed: Select = 0 *************************\n" );
		return false;
	}

	DataPacket * previous = NULL;
	DataPacket * header = NULL;

	while( NULL != m_outcoming_packet )
	{
		DataPacket * packet = m_outcoming_packet;
		if ( packet->IsSendByTcp() )
		{
			SendTCPDataPacket( packet );
			if ( NULL == previous )
			{
				m_outcoming_packet = m_outcoming_packet->getNextPacket();
			}
			else
			{
				m_outcoming_packet = m_outcoming_packet->getNextPacket();
				previous->setNextPacket( m_outcoming_packet );
			}
			XP_API_DEL( packet );
			packet = NULL;
		}
		else
		{
			if ( NULL == header )
			{
				header = m_outcoming_packet;
			}
			previous = m_outcoming_packet;
			m_outcoming_packet = m_outcoming_packet->getNextPacket();			
		}
	}

	m_outcoming_packet = header;

	return true;
}

bool COnlinePlayer::SendAllUDPData( COnlinePlayer * receiver )
{
	if ( NULL == m_outcoming_packet )
	{
		return false;
	}

	if ( NULL == m_udp_socket )
	{
		return false;
	}

	if ( !m_udp_socket->Select( GLXPlayerSocket::XSOCKET_SELECT_WRITE ) )
	{
		XP_DEBUG_OUT( "************************* SendAllUDPData Failed: Select = 0 *************************\n" );
		return false;
	}

	DataPacket * previous = NULL;
	DataPacket * header = NULL;

	while( NULL != m_outcoming_packet )
	{
		DataPacket * packet = m_outcoming_packet;
		if ( packet->IsSendByUdp() )
		{
			SendUDPDataPacket( receiver, packet );
			if ( NULL == previous )
			{
				m_outcoming_packet = m_outcoming_packet->getNextPacket();
			}
			else
			{
				m_outcoming_packet = m_outcoming_packet->getNextPacket();
				previous->setNextPacket( m_outcoming_packet );
			}
			XP_API_DEL( packet );
			packet = NULL;
		}
		else
		{
			if ( NULL == header )
			{
				header = m_outcoming_packet;
			}
			previous = m_outcoming_packet;
			m_outcoming_packet = m_outcoming_packet->getNextPacket();			
		}
	}

	m_outcoming_packet = header;

	return true;
}

int COnlinePlayer::SendTCPDataPacket( DataPacket * packet )
{
	char * buffer = XP_API_NEW char[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	int len = 0;

	packet->Packet2String( buffer, len );

	int size = m_tcp_socket->Send( buffer, len );

	if ( size > 0 )
	{
		m_last_send_time = XP_API_GET_TIME();
	}

	XP_API_DEL( buffer );

	if ( size < 0 )
	{
		SetAlive( false );
	}

	return size;
}

int COnlinePlayer::SendUDPDataPacket( COnlinePlayer * receiver, DataPacket * packet )
{
	if ( NULL == receiver )
	{
		return 0;
	}

	char * buffer = XP_API_NEW char[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	int len = 0;

	packet->Packet2String( buffer, len );

	int size = 0;

	if ( NULL == GetExternalIP() || NULL == receiver->GetExternalIP() )
	{
		size = m_udp_socket->SendTo( buffer, len, receiver->GetInternalIP(), receiver->GetInternalUdpPort() );
	}
	else
	{
		if ( XP_API_STRCMP( GetExternalIP(), receiver->GetExternalIP() ) == 0 )
		{
			size = m_udp_socket->SendTo( buffer, len, receiver->GetInternalIP(), receiver->GetInternalUdpPort() );
		}
		else
		{
			size = m_udp_socket->SendTo( buffer, len, receiver->GetExternalIP(), receiver->GetExternalUdpPort() );
		}
	}

	if ( size > 0 )
	{
		m_last_send_time = XP_API_GET_TIME();
	}

	XP_API_DEL( buffer );

	return size;	
}

bool COnlinePlayer::CheckAlive()
{
#ifdef MP_P2P_DEBUG_DO_NOT_CHECK_ALIVE
	return true;
#else

	if ( !IsAlive() || !IsConnected() || !IsJoined() )
	{
		return true;
	}

	long current_time = XP_API_GET_TIME();

	if ( (current_time - m_last_recv_time) > MP_KEEP_ALIVE_TIMEOUT )
	{
		return false;
	}
	else
	{
		return true;
	}
#endif

}

bool COnlinePlayer::KeepAlive()
{
	if ( !IsAlive() || !IsConnected() || !IsJoined() )
	{
		return false;
	}

	long current_time = XP_API_GET_TIME();

	if ( (current_time - m_last_send_time) > MP_KEEP_ALIVE_SENDTIME  )
	{
		DataPacket* packet = XP_API_NEW DefaultDataPacket();
		DataBuilder::BuildKeepAliveMessage( packet );
		packet->packMessage();
		AddOutputData( packet );
		XP_API_DEL( packet );
		SendAllData( NULL );
	}

	return true;
}

bool COnlinePlayer::InitSocket( OnlinePlayerType type )
{

	switch(m_upnpInitState)
	{
	case UPNP_INIT_TCPNONE:
		{

			m_upnpInitState=InitTcpSocket(type);

		}
		return false;
	case UPNP_INIT_TCPSEND:
		{
			m_upnpInitState=UPNP_INIT_TCPWAIT;
			PortMapper::GetInstance()->AddPortMapping( PortMapper::PORT_MAPPING_TCP,
				m_internal_tcp_address->getIP(), m_internal_tcp_address->getTcpPort() );
		}
		return false;
	case UPNP_INIT_TCPWAIT:


		return false;
	case UPNP_INIT_TCPEND:
		{
			m_external_tcp_address->setTcpPort( m_tcpIport );
			m_upnpInitState=UPNP_INIT_UDPNONE;
		}
		return false;
	case UPNP_INIT_UDPNONE:
		{
			m_upnpInitState=InitUdpSocket(type);
		}
		return false;
	case UPNP_INIT_UDPSEND:
		{
			m_upnpInitState=UPNP_INIT_UDPWAIT;
			PortMapper::GetInstance()->AddPortMapping( PortMapper::PORT_MAPPING_UDP,
				m_internal_udp_address->getIP(), m_internal_udp_address->getUdpPort() );
		}
		break;
	case UPNP_INIT_UDPWAIT:
		return false;
	case UPNP_INIT_UDPEND:
		{
			m_external_tcp_address->setTcpPort( m_udpIport );
			m_upnpInitState=UPNP_INIT_ALLEND;
		}
		return false;
	case UPNP_INIT_ALLEND:
		return true;
	default:
		return false;
	}

	return false;
}

COnlinePlayer::InitUpnpState COnlinePlayer::InitTcpSocket( OnlinePlayerType type )
{

	if ( NULL == m_tcp_socket )
	{
		m_tcp_socket = GLXPlayerSocketFactory::GetSocket();
		m_tcp_socket->CreateTcpSocket();

		if ( type == ONLINE_PLAYER_HOST )
		{
			m_tcpIport = m_tcp_socket->Bind( GetInternalIP(), GetInternalTcpPort() );
			m_internal_tcp_address->setTcpPort( m_tcpIport );
			//iPort = PortMapper::GetInstance()->AddPortMapping( PortMapper::PORT_MAPPING_TCP,
			//	m_internal_tcp_address->getIP(), m_internal_tcp_address->getTcpPort() );
			//m_external_tcp_address->setTcpPort( iPort );
			return UPNP_INIT_TCPSEND;
		}
		else if ( type == ONLINE_PLAYER_CLINET || type == ONLINE_PLAYER_LOCAL_CLINET )
		{
			m_internal_tcp_address->setTcpPort( 0 );
			m_external_tcp_address->setTcpPort( 0 );
		}
		else if ( type == ONLINE_PLAYER_LOCAL_HOST )
		{
			int iPort = m_tcp_socket->Bind( GetInternalIP(), GetInternalTcpPort() );
			m_internal_tcp_address->setTcpPort( iPort );
			m_external_tcp_address->setTcpPort( iPort );
		}
	}

	return UPNP_INIT_UDPNONE;
}

COnlinePlayer::InitUpnpState COnlinePlayer::InitUdpSocket( OnlinePlayerType type )
{
	if ( NULL == m_udp_socket )
	{
		m_udp_socket = GLXPlayerSocketFactory::GetSocket();
		m_udp_socket->CreateUdpSocket();

		if ( type == ONLINE_PLAYER_HOST || type == ONLINE_PLAYER_CLINET )
		{
			m_udpIport = m_udp_socket->Bind( GetInternalIP(), GetInternalUdpPort() );
			m_internal_udp_address->setUdpPort( m_udpIport );

			//int iExternalPort = PortMapper::GetInstance()->AddPortMapping( PortMapper::PORT_MAPPING_UDP,
			//	m_internal_udp_address->getIP(), m_internal_udp_address->getUdpPort() );
			//	m_external_udp_address->setUdpPort( iExternalPort );

			return UPNP_INIT_UDPSEND;
		}
		else if ( type == ONLINE_PLAYER_LOCAL_HOST || type == ONLINE_PLAYER_LOCAL_CLINET)
		{
			int iPort = m_udp_socket->Bind( GetInternalIP(), GetInternalUdpPort() );
			m_internal_udp_address->setUdpPort( iPort );
			m_external_udp_address->setUdpPort( iPort );
		}
	}

	return UPNP_INIT_ALLEND;
}

bool COnlinePlayer::StartListen( int count )
{
	m_tcp_socket->Listen( count );
	return true;
}

bool COnlinePlayer::StartLocalRoom( int count )
{
	// Te be implemented
	m_tcp_socket->Listen( count );
	return true;
}

bool COnlinePlayer::Connect( COnlinePlayer * listener )
{
	if ( NULL == GetExternalIP() || XP_API_STRCMP( GetExternalIP(), listener->GetExternalIP() ) == 0 )
	{
		return m_tcp_socket->ConnectTo( listener->GetInternalIP(), listener->GetInternalTcpPort() );
	}
	else
	{
		return m_tcp_socket->ConnectTo( listener->GetExternalIP(), listener->GetExternalTcpPort() );
	}	
}

bool COnlinePlayer::CheckGameData( char * &name, DataPacket * &packet )
{
	if ( NULL == m_incoming_packet )
	{
		return false;
	}

	DataPacket * data = XP_API_NEW DefaultDataPacket( m_incoming_packet );
	BYTE header = 0;
	BYTE len = 0;
	data->getByte( header );
	data->getByte( header );
	data->getByteLenString( name, len );
	data->getPacket( packet );
	XP_API_DEL( data );
	data = NULL;

	data = m_incoming_packet;
	m_incoming_packet = m_incoming_packet->getNextPacket();
	XP_API_DEL( data );
	data = NULL;

	return true;
}

void COnlinePlayer::HandlePingData()
{
	// Handle ping package
	if ( NULL == m_udp_socket )
	{
		return;
	}

	int iResult = m_udp_socket->Select( GLXPlayerSocket::XSOCKET_SELECT_READ );
	if ( iResult > 0 )
	{
		char buffer[128] = { 0 };
		char * ip = NULL;
		int port;
		int iCount = m_udp_socket->RecvFromUnkownIP( buffer, sizeof( buffer ), &ip, &port );
		if ( iCount > 0 )
		{
			m_udp_socket->SendTo( buffer, XP_API_STRLEN( buffer ), ip, port );
		}
		XP_API_DEL( ip );
	}	
}


void COnlinePlayer::OnAddPortMappingSuccess(int type,int portMapped)
{

	if( PortMapper::PORT_MAPPING_TCP==type)
	{
		m_tcpIport=portMapped;
		m_upnpInitState=UPNP_INIT_TCPEND;

	}else
	{
		m_udpIport=portMapped;
			m_upnpInitState=UPNP_INIT_UDPEND;
	}

}
#endif
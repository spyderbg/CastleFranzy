#include "../../../../include/Multiplayer/Common/XIPAddress.h"

XIPAddress::XIPAddress( char * data )
{
	if ( NULL != data )
	{
		char * sep = XP_API_STRSTR( data, ":" );

		if ( NULL != sep )
		{
			int len_ip = sep - data;
			int len_port = XP_API_STRLEN( data ) - ( len_ip + 1 );

			m_ip = XP_API_NEW char[len_ip+1];
			XP_API_MEMSET( m_ip, 0, len_ip+1 );
			XP_API_MEMCPY( m_ip, data, len_ip );

			char * temp = XP_API_NEW char[len_port+1];
			XP_API_MEMSET( temp, 0, len_port+1 );
			XP_API_MEMCPY( temp, sep+1, len_port );

			m_port_tcp = XP_API_ATOI( temp );			

			XP_API_DEL( temp );

			m_port_udp = m_port_tcp;
		}
		else
		{
			m_ip = NULL;
			m_port_tcp = 0;
			m_port_udp = 0;
			m_port_broadcast = 0;
		}
	}
	else
	{
		m_ip = NULL;
		m_port_tcp = 0;
		m_port_udp = 0;
		m_port_broadcast = 0;
	}
}

XIPAddress::~XIPAddress(void)
{
	XP_API_DEL( m_ip );
}

XIPAddress::XIPAddress( XIPAddress * addr )
{
	m_ip = XP_API_STRNEW( addr->getIP() );
	m_port_tcp = addr->getTcpPort();
	m_port_udp = addr->getUdpPort();
	m_port_broadcast = addr->getBroadcastPort();
}

char * XIPAddress::getIP()
{
	return m_ip;
}

void XIPAddress::setIP(const char * ip)
{
	if ( NULL != m_ip )
	{
		XP_API_DEL( m_ip );
		m_ip = NULL;
	}

	m_ip = XP_API_STRNEW( ip );
}

int XIPAddress::getTcpPort()
{
	return m_port_tcp;
}

void XIPAddress::setTcpPort(int port)
{
	m_port_tcp = port;
}

int XIPAddress::getUdpPort()
{
	return m_port_udp;
}

void XIPAddress::setUdpPort(int port)
{
	m_port_udp = port;
}

int XIPAddress::getBroadcastPort()
{
	return m_port_broadcast;
}

void XIPAddress::setBroadcastPort(int port)
{
	m_port_broadcast = port;
}

// 10.0.0.0        -   10.255.255.255  (10/8 prefix)
// 172.16.0.0      -   172.31.255.255  (172.16/12 prefix)
// 192.168.0.0     -   192.168.255.255 (192.168/16 prefix)
bool XIPAddress::IsPrivateAddress()
{
	char ip_a[] = "10.";
	char ip_b[] = "172.";
	char ip_c[] = "192.168.";

	if ( XP_API_STRNCMP( m_ip, ip_a, XP_API_STRLEN( ip_a ) ) == 0 )
	{
		return true;
	}

	if ( XP_API_STRNCMP( m_ip, ip_b, XP_API_STRLEN( ip_b ) ) == 0 )
	{
		char ip[8] = { 0 };

		XP_API_MEMSET( ip, 0, 8 );
		XP_API_PARSE_DATA( m_ip, ip, 1, '.' );

		int ip_value = XP_API_ATOI( ip );

		if ( ip_value >= 16 && ip_value <= 31 )
		{
			return true;
		}		
	}

	if ( XP_API_STRNCMP( m_ip, ip_c, XP_API_STRLEN( ip_c ) ) == 0 )
	{
		return true;
	}

	return false;	
}